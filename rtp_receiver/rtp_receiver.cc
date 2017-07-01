
#include <webrtc/modules/rtp_rtcp/include/rtp_receiver.h>
#include <webrtc/modules/rtp_rtcp/include/rtp_payload_registry.h>
#include <webrtc/common_types.h>   // for VideoCodec
#include <webrtc/modules/rtp_rtcp/include/rtp_header_parser.h>   // for webrtc::RtpHeaderParser
using namespace webrtc;

#include <memory>
#include <functional>
using namespace std;

#include <stdio.h>

class RtpHandler : public RtpData, public RtpFeedback
{
public:
RtpHandler(const char* file)
{
	fp_ = ::fopen(file, "wb");
}
virtual ~RtpHandler()
{
	::fclose(fp_);
}

// webrtc::RtpData

virtual 
int32_t OnReceivedPayloadData(const uint8_t* payload_data, size_t payload_size, const WebRtcRTPHeader* rtp_header)
{
	::fwrite(payload_data, 1, payload_size, fp_);
	
	return 0;
}

virtual 
bool OnRecoveredPacket(const uint8_t* packet, size_t packet_length)
{
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
	FILE* fp_;

};

/************************************************************/

#include "encode.h"
#include "h264_rtp_packer.h"

function<void(uint8_t* rtp_packet, size_t rtp_packet_len)> rtp_packet_sink;

h264_rtp_packer_t* rtp_packer;
FILE *nalu_fp;

void on_nalu_data(void* nalu, unsigned len, int is_last, void* userdata)
{
	static unsigned ts = 1800;
	
	(void)userdata;
	
	::fwrite(nalu, 1, len, nalu_fp);
	
	h264_rtp_packer_pack(rtp_packer, (unsigned char *)nalu, len, ts, is_last);
	
	ts += 3600;
	
    return;
}

void rtp_sink(const unsigned char *rtp_packet, unsigned len, void *userdata)
{
	(void)userdata;
	
	rtp_packet_sink((uint8_t*)rtp_packet, (size_t)len);
	
	return;
}

int main(int argc, char *argv[])
{
	Clock* clock = Clock::GetRealTimeClock();

	RTPPayloadRegistry rtp_payload_registry;

	unique_ptr<RtpHandler> rtp_handler;
	unique_ptr<RtpReceiver> rtp_receiver;
	unique_ptr<RtpHeaderParser> rtp_head_parser;

	{
		VideoCodec video_codec;
		strncpy(video_codec.plName, "h264", 4);
		video_codec.codecType = kVideoCodecH264;
		video_codec.plType = 96;
		video_codec.H264()->profile = H264::kProfileHigh;
		rtp_payload_registry.RegisterReceivePayload(video_codec);

		strncpy(video_codec.plName, "red", 3);
		video_codec.codecType = kVideoCodecRED;
		video_codec.plType = 97;
		rtp_payload_registry.RegisterReceivePayload(video_codec);
		
		strncpy(video_codec.plName, "ulpfec", 6);
		video_codec.codecType = kVideoCodecULPFEC;
		video_codec.plType = 98;
		rtp_payload_registry.RegisterReceivePayload(video_codec);
	}
	
	rtp_handler.reset(new RtpHandler("receive.h264"));
	rtp_receiver.reset(RtpReceiver::CreateVideoReceiver(clock, rtp_handler.get(), rtp_handler.get(), &rtp_payload_registry));
	
	rtp_head_parser.reset(RtpHeaderParser::Create());
	
	auto receive_rtp = [&rtp_head_parser, &rtp_payload_registry, &rtp_receiver](uint8_t* rtp_packet, size_t rtp_packet_len) {
		RTPHeader rtp_head;
		if (!rtp_head_parser->Parse(rtp_packet, rtp_packet_len, &rtp_head))
		{
			return;
		}

		rtp_head.payload_type_frequency = 90000;

		uint8_t* payload = rtp_packet + rtp_head.headerLength;
		size_t payload_len = rtp_packet_len - rtp_head.headerLength;
		PayloadUnion payload_spec;
		if (rtp_payload_registry.GetPayloadSpecifics(rtp_head.payloadType, &payload_spec))
		{
			rtp_receiver->IncomingRtpPacket(rtp_head, payload, payload_len, payload_spec, true);
		}
	};

	rtp_packet_sink = receive_rtp;
	
	{
		nalu_fp = ::fopen("nalu.h264", "wb");
		
		avc_encoder_t* encoder = avc_encoder_new(640, 480, 15, 256000, on_nalu_data, NULL);
		i420_image_t *image = i420_image_alloc(640, 480);
		
		avc_encoder_init(encoder);
		
		rtp_packer = h264_rtp_packer_new(96, 1, 12346, 1000, rtp_sink, NULL);
		
		int i = 0;
		for (; i < 30; ++i)
		{
			fill_image_i420(image, i);
			avc_encoder_encode(encoder, image, i, 0);
		}
		avc_encoder_encode(encoder, NULL, 0, 0);

		i420_image_free(image);
		h264_rtp_packer_destroy(rtp_packer);
		avc_encoder_destroy(encoder);
		
		::fclose(nalu_fp);
	}
	
	return 0;
}
