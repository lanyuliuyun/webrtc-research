
#include "neteq.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/rtp_headers.h"
#include "api/array_view.h"
#include "api/audio/audio_frame.h"

#include "opus.h"
#include <windows.h>

#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include <bitset>

class PacketLost
{
public:
    PacketLost()
        : lost_percent_(0)
        , packet_intv_(100)
        , lost_bits_()
    {
        srand((unsigned)time(NULL));
    }
    ~PacketLost() {}

    void Set(int percent)
    {
        lost_percent_ = percent;
        UpdateLostBitmap();
    }

    bool Run()
    {
        bool lost = false;
        packet_intv_--;
        lost = lost_bits_[packet_intv_];

        if (packet_intv_ == 0)
        {
            packet_intv_ = 100;
            UpdateLostBitmap();
        }

        return lost;
    }

private:
    void UpdateLostBitmap()
    {
        lost_bits_.reset();
        for (int i = 0; i < lost_percent_; ++i)
        {
            int idx = rand() % 100;
            lost_bits_.set(idx);
        }
    }

    int lost_percent_;
    int packet_intv_;
    std::bitset<100> lost_bits_;
};

OpusEncoder* create_encoder()
{
    int opus_error;
    OpusEncoder *encoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_AUDIO, &opus_error);
    opus_encoder_ctl(encoder, OPUS_SET_COMPLEXITY(8));
    opus_encoder_ctl(encoder, OPUS_SET_BITRATE(64000));
    opus_encoder_ctl(encoder, OPUS_SET_VBR(1));
    //opus_encoder_ctl(encoder, OPUS_SET_VBR_CONSTRAINT(1));
    opus_encoder_ctl(encoder, OPUS_SET_FORCE_CHANNELS(1));
    //opus_encoder_ctl(encoder, OPUS_SET_MAX_BANDWIDTH(OPUS_BANDWIDTH_FULLBAND));
    opus_encoder_ctl(encoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_MUSIC));
    opus_encoder_ctl(encoder, OPUS_SET_INBAND_FEC(0));
    //opus_encoder_ctl(encoder, OPUS_SET_PACKET_LOSS_PERC(40));
    //opus_encoder_ctl(encoder, OPUS_SET_DTX(0));
    //opus_encoder_ctl(encoder, OPUS_SET_LSB_DEPTH(24));
    opus_encoder_ctl(encoder, OPUS_SET_EXPERT_FRAME_DURATION(OPUS_FRAMESIZE_20_MS));
    //opus_encoder_ctl(encoder, OPUS_SET_PREDICTION_DISABLED(0));

    return encoder;
}

int main(int argc, char *argv[])
{
    webrtc::NetEq::Config config;
    config.sample_rate_hz = 48000;

    auto decoder_factory = webrtc::CreateBuiltinAudioDecoderFactory();

    webrtc::NetEq *neteq = webrtc::NetEq::Create(config, decoder_factory);

    webrtc::SdpAudioFormat::Parameters empty_param;
    webrtc::SdpAudioFormat opus_audio_format("opus", 48000, 2, empty_param);
    neteq->RegisterPayloadType(105, opus_audio_format);
    
    PacketLost lost;
    lost.Set(10);

    OpusEncoder *encoder = create_encoder();
    
    uint16_t pcm_frames[20 * 48];
    uint8_t opus_packet[20 * 48];

    uint32_t rtp_ts = 20;
    uint16_t rtp_sn = 0;

    FILE *pcm_in_fp = fopen(argv[1], "rb");
    FILE *pcm_out_fp = fopen(argv[2], "wb");

    int next_rtp_op_ts = GetTickCount();
    int next_pcm_op_ts = GetTickCount() + 10;
    while (!feof(pcm_in_fp))
    {
        int now = (int)GetTickCount();
        int next_op_ts = next_rtp_op_ts < next_pcm_op_ts ? next_rtp_op_ts : next_pcm_op_ts;
        int ts_delta = next_op_ts - now;
        if (ts_delta > 0) { Sleep(ts_delta); }

        now = (int)GetTickCount();
        if (next_rtp_op_ts <= now)
        {
            int read_ret = fread(pcm_frames, 2, (20*48), pcm_in_fp);
            if (read_ret < (20*48)) { break; }

            if (!lost.Run())
            {
                opus_int32 enc_ret = opus_encode(encoder, (opus_int16*)pcm_frames, (20*48), opus_packet, (20*48));
                if (enc_ret > 0)
                {
                    webrtc::RTPHeader rtp_head;
                    rtp_head.markerBit = true;
                    rtp_head.payloadType = 105;
                    rtp_head.payload_type_frequency = 480000;
                    rtp_head.sequenceNumber = rtp_sn;
                    rtp_head.timestamp = rtp_ts;
                    rtp_head.ssrc = 0x11223344;

                    rtc::ArrayView<const uint8_t> rtp_play_load(opus_packet, enc_ret);

                    int neteq_ret = neteq->InsertPacket(rtp_head, rtp_play_load, next_rtp_op_ts);
                }
            }

            rtp_ts += 960;
            rtp_sn++;

            next_rtp_op_ts += 20;
        }

        if (next_pcm_op_ts <= now)
        {
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

            next_pcm_op_ts += 10;
        }
    }

    fclose(pcm_in_fp);
    fclose(pcm_out_fp);

    opus_encoder_destroy(encoder);

    delete neteq;

    return 0;
}