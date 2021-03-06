
#include <webrtc/modules/rtp_rtcp/include/rtp_receiver.h>   // for webrtc::RtpReceiver
#include <webrtc/modules/rtp_rtcp/include/rtp_payload_registry.h>   // for webrtc::RTPPayloadRegistry
#include <webrtc/common_types.h>   // for webrtc::VideoCodec
#include <webrtc/modules/rtp_rtcp/include/rtp_header_parser.h>   // for webrtc::RtpHeaderParser

//#include <webrtc/modules/rtp_rtcp/source/forward_error_correction.h>   // for webrtc::ForwardErrorCorrection
//#include <webrtc/modules/rtp_rtcp/source/ulpfec_generator.h>   // for webrtc::UlpfecGenerator
#include <webrtc/modules/rtp_rtcp/include/flexfec_sender.h>   // for webrtc::FlexfecSender
#include <webrtc/modules/rtp_rtcp/include/flexfec_receiver.h>   // for webrtc::FlexfecReceiver

using namespace webrtc;

#include "encode.h"
#include "h264_rtp_packer.h"

#include <memory>
#include <functional>
using namespace std;

#include <stdio.h>

typedef function<void(const uint8_t* rtp_packet, size_t rtp_packet_len)> RtpPacketSink;

class RtpStreamGenerator
{
  public:
    RtpStreamGenerator(const char* es_file, RtpPacketSink rtp_packet_sink)
        : encoder_(NULL)
        , image_(NULL)
        , ts_ms_(1800)
        , rtp_packer_(NULL)
        , nalu_fp_(NULL)
        , rtp_packet_sink_(rtp_packet_sink)
        , fec_generator_()
    {
        nalu_fp_ = fopen(es_file, "wb");

        encoder_ = avc_encoder_new(640, 480, 15, 256000, RtpStreamGenerator::on_nalu_data, this);
        image_ = i420_image_alloc(640, 480);
        avc_encoder_init(encoder_);
        rtp_packer_ = h264_rtp_packer_new(96, 1, 12345, 1000, RtpStreamGenerator::on_rtp_packet, this);

        vector<RtpExtension> empty_rtp_extensions;
        fec_generator_.reset(new FlexfecSender(97, 12346, 12345, empty_rtp_extensions, Clock::GetRealTimeClock()));

        FecProtectionParams fec_param;
        fec_param.fec_rate = 16;
        // fec group size
        fec_param.max_fec_frames = 15;
        fec_param.fec_mask_type = kFecMaskBursty;
        fec_generator_->SetFecParameters(fec_param);

        return;
    }

    ~RtpStreamGenerator()
    {
        i420_image_free(image_);
        h264_rtp_packer_destroy(rtp_packer_);
        avc_encoder_destroy(encoder_);
        fclose(nalu_fp_);

        return;
    }

    void run(int frame_count)
    {
        int i = 0;
        for (; i < frame_count; ++i)
        {
            fill_image_i420(image_, i);
            avc_encoder_encode(encoder_, image_, i, 0);
        }
        avc_encoder_encode(encoder_, NULL, 0, 0);

        return;
    }

    void handleNLAU(uint8_t* nalu, unsigned len, int is_last)
    {
        fwrite(nalu, 1, len, nalu_fp_);
        h264_rtp_packer_pack(rtp_packer_, (unsigned char *)nalu, len, ts_ms_, is_last);
        ts_ms_ += 3600;

        return;
    }

    void handleRtpPacket(const unsigned char *rtp_packet, unsigned len)
    {
        rtp_packet_sink_((uint8_t*)rtp_packet, (size_t)len);

        RtpPacketToSend rtp_packet_tosend(nullptr);
        rtp_packet_tosend.Parse(rtp_packet, len);

        fec_generator_->AddRtpPacketAndGenerateFec(rtp_packet_tosend);
        if (fec_generator_->FecAvailable())
        {
            vector<unique_ptr<RtpPacketToSend>> packets = fec_generator_->GetFecPackets();
            for (auto& packet : packets)
            {
                rtp_packet_sink_(packet->data(), packet->size());
            }
        }

        return;
    }
        
  private:
    static void on_nalu_data(uint8_t* nalu, unsigned len, int is_last, void* userdata)
    {
        RtpStreamGenerator* thiz = (RtpStreamGenerator*)userdata;
        thiz->handleNLAU(nalu, len, is_last);
    }
    
    static void on_rtp_packet(const unsigned char *rtp_packet, unsigned len, void *userdata)
    {
        RtpStreamGenerator* thiz = (RtpStreamGenerator*)userdata;
        thiz->handleRtpPacket(rtp_packet, len);
    }

    avc_encoder_t* encoder_;
    i420_image_t *image_;

    unsigned ts_ms_;

    h264_rtp_packer_t* rtp_packer_;
    FILE *nalu_fp_;

    RtpPacketSink rtp_packet_sink_;

    unique_ptr<FlexfecSender> fec_generator_;
};


class RtpHandler : public RtpData, public RtpFeedback, public RecoveredPacketReceiver
{
  public:
    RtpHandler(const char* file)
    {
        fp_ = fopen(file, "wb");

        VideoCodec video_codec;
        strncpy(video_codec.plName, "h264", 4);
        video_codec.codecType = kVideoCodecH264;
        video_codec.plType = 96;
        video_codec.H264()->profile = H264::kProfileHigh;
        rtp_payload_registry_.RegisterReceivePayload(video_codec);

        strncpy(video_codec.plName, "red", 3);
        video_codec.codecType = kVideoCodecRED;
        video_codec.plType = 97;
        rtp_payload_registry_.RegisterReceivePayload(video_codec);

        strncpy(video_codec.plName, "ulpfec", 6);
        video_codec.codecType = kVideoCodecULPFEC;
        video_codec.plType = 98;
        rtp_payload_registry_.RegisterReceivePayload(video_codec);

        // ======================================================================

        rtp_receiver_.reset(RtpReceiver::CreateVideoReceiver(Clock::GetRealTimeClock(), this, this, &rtp_payload_registry_));
        rtp_head_parser_.reset(RtpHeaderParser::Create());

        fec_receiver_.reset(new FlexfecReceiver(12346, 12345, this));
    }

    virtual ~RtpHandler()
    {
        fclose(fp_);
    }

    void receiveRtp(const uint8_t* rtp_packet, size_t rtp_packet_len)
    {
        RtpPacketReceived received_packet;
        if (!received_packet.Parse(rtp_packet, rtp_packet_len))
        {
            return;
        }

        fec_receiver_->OnRtpPacket(received_packet);

        // check if it's normal rtp
        if (received_packet.Ssrc() != 12345)
        {
            return;
        }

        received_packet.set_payload_type_frequency(90000);
        RTPHeader rtp_head;
        received_packet.GetHeader(&rtp_head);
        processMediaRtp(rtp_head, rtp_packet, rtp_packet_len);

        return;
    }

    // webrtc::RtpData

    virtual 
    int32_t OnReceivedPayloadData(const uint8_t* payload_data, size_t payload_size, const WebRtcRTPHeader* rtp_header)
    {
        fwrite(payload_data, 1, payload_size, fp_);

        return 0;
    }

    // webrtc::RtpData::OnRecoveredPacket()
    // webrtc::RecoveredPacketReceiver::OnRecoveredPacket()
    virtual 
    bool OnRecoveredPacket(const uint8_t* packet, size_t packet_length)
    {
        receiveRtp(packet, packet_length);

        return true;
    }

    // webrtc::RtpFeedback 

    virtual
    int32_t OnInitializeDecoder(int8_t payload_type, const char payload_name[RTP_PAYLOAD_NAME_SIZE], int frequency, size_t channels, uint32_t rate)
    {
        return 0;
    }

    virtual
    void OnIncomingSSRCChanged(uint32_t ssrc)
    {
        return;
    }

    virtual
    void OnIncomingCSRCChanged(uint32_t csrc, bool added)
    {
        return;
    }

  private:
    void processMediaRtp(const RTPHeader& rtp_head, const uint8_t* rtp_packet, size_t rtp_packet_len)
    {
        const uint8_t* payload = rtp_packet + rtp_head.headerLength;
        size_t payload_len = rtp_packet_len - rtp_head.headerLength;
        PayloadUnion payload_spec;
        if (rtp_payload_registry_.GetPayloadSpecifics(rtp_head.payloadType, &payload_spec))
        {
            rtp_receiver_->IncomingRtpPacket(rtp_head, payload, payload_len, payload_spec, true);
        }

        return;
    }    

  private:
    FILE* fp_;

    RTPPayloadRegistry rtp_payload_registry_;
    unique_ptr<RtpReceiver> rtp_receiver_;
    unique_ptr<RtpHeaderParser> rtp_head_parser_;

    unique_ptr<FlexfecReceiver> fec_receiver_;
};


int main(int argc, char *argv[])
{
    RtpHandler rtp_handler("receive.h264");

    RtpPacketSink receive_rtp = bind(&RtpHandler::receiveRtp, &rtp_handler, placeholders::_1, placeholders::_2);
    RtpStreamGenerator rtp_stream_generator("nalu.h264", receive_rtp);

    rtp_stream_generator.run(30);
    
    return 0;
}
