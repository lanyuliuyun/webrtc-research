
WEBRTC_ROOT = /mnt/d/webrtc

CROSS_COMPILE ?= 
CXX = $(CROSS_COMPILE)g++

rtp_receiver: encode.c  h264_rtp_packer.c  rtp_receiver.cc
	$(CXX) $^ -o $@ \
        -DWEBRTC_LINUX -DWEBRTC_POSIX -std=c++11 -pthread \
        -I$(WEBRTC_ROOT) \
        -L$(WEBRTC_ROOT)/lib -lwebrtc_rtp_rtcp -lwebrtc_common_video -lwebrtc_base -lwebrtc_system_wrappers -levent -lx264
