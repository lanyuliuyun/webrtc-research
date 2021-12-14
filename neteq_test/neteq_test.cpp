
#include "neteq.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/rtp_headers.h"
#include "api/array_view.h"
#include "api/audio/audio_frame.h"

#include "hik_opus.h"

#include <stdio.h>
#include <inttypes.h>

#ifdef WIN32
	#include <winsock2.h>			/* for htons()/htonl() */
#else
	#include <arpa/inet.h>			/* for htons()/htonl() */
#endif

extern "C" void *aligned_malloc(unsigned int size, unsigned int alignment);
extern "C" void aligned_free(void *p);

int main(int argc, char *argv[])
{
    webrtc::NetEq::Config config;
    config.sample_rate_hz = 48000;

    auto decoder_factory = webrtc::CreateBuiltinAudioDecoderFactory();

    webrtc::NetEq *neteq = webrtc::NetEq::Create(config, decoder_factory);

    webrtc::SdpAudioFormat::Parameters empty_param;
    webrtc::SdpAudioFormat opus_audio_format("opus", 48000, 2, empty_param);
    neteq->RegisterPayloadType(105, opus_audio_format);

    void *enc_handle;
    {
        AUDIOENC_PARAM enc_param;
        memset(&enc_param, 0, sizeof(enc_param));
        enc_param.sample_rate = 48000;
        enc_param.num_channels = 1;
        enc_param.bitrate = 64000;

        MEM_TAB enc_mem;
        memset(&enc_mem, 0, sizeof(enc_mem));
        HIK_OPUSENC_GetMemSize(&enc_param, &enc_mem);
        enc_mem.base = aligned_malloc(enc_mem.size, enc_mem.alignment);

        HRESULT hik_ret = HIK_OPUSENC_Create(&enc_param, &enc_mem, &enc_handle);
    }

    uint16_t pcm_frames[20 * 48];
    uint8_t opus_packet[20 * 48];

    uint32_t rtp_ts = 20;
    uint16_t rtp_sn = 0;
    uint32_t recv_ts = 20;

    FILE *pcm_in_fp = fopen(argv[1], "rb");
    FILE *pcm_out_fp = fopen(argv[2], "wb");

    while (!feof(pcm_in_fp))
    {
        int read_ret = fread(pcm_frames, 1, sizeof(pcm_frames), pcm_in_fp);
        if (read_ret < sizeof(pcm_frames)) { break; }

        AUDIOENC_PROCESS_PARAM enc_buf;
        memset(&enc_buf, 0, sizeof(enc_buf));
        enc_buf.reserved[0] = 20 * 48;
        enc_buf.in_buf = (U08*)pcm_frames;
        enc_buf.out_buf = opus_packet;
        HRESULT hik_ret = HIK_OPUSENC_Encode(enc_handle, &enc_buf);

        webrtc::RTPHeader rtp_head;
        rtp_head.markerBit = true;
        rtp_head.payloadType = 105;
        rtp_head.payload_type_frequency = 480000;
        rtp_head.sequenceNumber = rtp_sn;
        rtp_head.timestamp = rtp_ts;
        rtp_head.ssrc = 0x11223344;

        rtc::ArrayView<const uint8_t> rtp_play_load(enc_buf.out_buf, enc_buf.out_frame_size);

        int neteq_ret = neteq->InsertPacket(rtp_head, rtp_play_load, recv_ts);

        webrtc::AudioFrame audio;
        bool audio_mute;
        int ret = neteq->GetAudio(&audio, &audio_mute);
        if (ret == webrtc::NetEq::kOK)
        {
            fwrite(audio.data(), 2, (10*48), pcm_out_fp);
        }
        else
        {
            printf("first GetAudio() failed, sn: %u\n", rtp_sn);
        }
        Sleep(10);

        ret = neteq->GetAudio(&audio, &audio_mute);
        if (ret == webrtc::NetEq::kOK)
        {
            fwrite(audio.data(), 2, (10*48), pcm_out_fp);
        }
        else
        {
            printf("second GetAudio() failed, sn: %u\n", rtp_sn);
        }

        Sleep(10);

        rtp_ts += 960;
        rtp_sn++;
        recv_ts += 20;
    }

    fclose(pcm_in_fp);
    fclose(pcm_out_fp);

    delete neteq;

    return 0;
}