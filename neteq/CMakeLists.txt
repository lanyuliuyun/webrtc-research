
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(api_SRCS
    api/rtp_headers.cc
    api/audio/audio_frame.cc
    api/audio_codecs/audio_codec_pair_id.cc
    api/audio_codecs/audio_decoder.cc
    api/audio_codecs/audio_format.cc
    api/audio_codecs/builtin_audio_decoder_factory.cc
    api/audio_codecs/opus/audio_decoder_opus.cc
)

set(cng_SRCS
    modules/audio_coding/codecs/cng/webrtc_cng.cc
)

set(codec_opus_SRCS
    modules/audio_coding/codecs/opus/audio_coder_opus_common.cc
    modules/audio_coding/codecs/opus/audio_decoder_opus.cc
    modules/audio_coding/codecs/opus/opus_interface.c
)

set(neteq_SRCS
    accelerate.cc
    audio_multi_vector.cc
    audio_vector.cc
    background_noise.cc
    buffer_level_filter.cc
    comfort_noise.cc
    cross_correlation.cc
    decision_logic.cc
    decoder_database.cc
    delay_manager.cc
    delay_peak_detector.cc
    dsp_helper.cc
    dtmf_buffer.cc
    dtmf_tone_generator.cc
    expand.cc
    expand_uma_logger.cc
    histogram.cc
    merge.cc
    nack_tracker.cc
    neteq.cc
    neteq_impl.cc
    normal.cc
    packet.cc
    packet_buffer.cc
    post_decode_vad.cc
    preemptive_expand.cc
    random_vector.cc
    red_payload_splitter.cc
    statistics_calculator.cc
    sync_buffer.cc
    tick_timer.cc
    time_stretch.cc
    timestamp_scaler.cc
)

add_library(neteq 
    ${neteq_SRCS}
    ${api_SRCS}
    ${cng_SRCS}
    ${codec_opus_SRCS})
