
WEBRTC_ROOT = /mnt/d/webrtc

CROSS_COMPILE ?= 
CXX = $(CROSS_COMPILE)g++

nack_module: nack_module.cc
	$(CXX) $^ -o $@ \
        -DWEBRTC_LINUX -std=c++11 -pthread \
        -I$(WEBRTC_ROOT) \
        -L$(WEBRTC_ROOT)/lib -lwebrtc_video_coding -lwebrtc_base -lwebrtc_system_wrappers
