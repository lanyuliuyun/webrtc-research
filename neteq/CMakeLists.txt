
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(rtc_base_SRCS
    rtc_base/checks.cc
    rtc_base/critical_section.cc
    rtc_base/event.cc
    rtc_base/event_tracer.cc
    rtc_base/logging.cc
    rtc_base/platform_thread.cc
    rtc_base/platform_thread_types.cc
    rtc_base/string_encode.cc
    rtc_base/string_to_number.cc
    rtc_base/string_utils.cc
    rtc_base/time_utils.cc
    rtc_base/zero_memory.cc
    rtc_base/strings/audio_format_to_string.cc
    rtc_base/strings/string_builder.cc
    rtc_base/synchronization/sequence_checker.cc
    rtc_base/synchronization/yield_policy.cc
)

set(api_SRCS
    api/rtp_headers.cc
    api/audio/audio_frame.cc
    api/audio_codecs/audio_codec_pair_id.cc
    api/audio_codecs/audio_decoder.cc
    api/audio_codecs/audio_encoder.cc
    api/audio_codecs/audio_format.cc
    api/audio_codecs/builtin_audio_decoder_factory.cc
    api/audio_codecs/opus/audio_decoder_multi_channel_opus.cc
    api/audio_codecs/opus/audio_decoder_opus.cc
    api/task_queue/task_queue_base.cc
)

set(signal_processing_SRCS
    common_audio/signal_processing/auto_correlation.c
    common_audio/signal_processing/auto_corr_to_refl_coef.c
    common_audio/signal_processing/complex_fft.c
    common_audio/signal_processing/copy_set_operations.c
    common_audio/signal_processing/division_operations.c
    common_audio/signal_processing/dot_product_with_scale.cc
    common_audio/signal_processing/energy.c
    common_audio/signal_processing/filter_ar.c
    common_audio/signal_processing/filter_ma_fast_q12.c
    common_audio/signal_processing/get_hanning_window.c
    common_audio/signal_processing/get_scaling_square.c
    common_audio/signal_processing/ilbc_specific_functions.c
    common_audio/signal_processing/levinson_durbin.c
    common_audio/signal_processing/lpc_to_refl_coef.c
    common_audio/signal_processing/randomization_functions.c
    common_audio/signal_processing/real_fft.c
    common_audio/signal_processing/refl_coef_to_lpc.c
    common_audio/signal_processing/resample.c
    common_audio/signal_processing/resample_48khz.c
    common_audio/signal_processing/resample_by_2.c
    common_audio/signal_processing/resample_by_2_internal.c
    common_audio/signal_processing/resample_fractional.c
    common_audio/signal_processing/splitting_filter.c
    common_audio/signal_processing/spl_init.c
    common_audio/signal_processing/spl_inl.c
    common_audio/signal_processing/spl_sqrt.c
    common_audio/signal_processing/sqrt_of_one_minus_x_squared.c
    common_audio/signal_processing/vector_scaling_operations.c
)
if (MSVC)
list(APPEND signal_processing_SRCS
    common_audio/signal_processing/complex_bit_reverse.c
    common_audio/signal_processing/cross_correlation.c
    common_audio/signal_processing/downsample_fast.c
    common_audio/signal_processing/filter_ar_fast_q12.c
    common_audio/signal_processing/min_max_operations.c
)
elseif (ANDROID OR APPLE)
list(APPEND signal_processing_SRCS
    common_audio/signal_processing/complex_bit_reverse_arm.S
    common_audio/signal_processing/cross_correlation_neon.c
    common_audio/signal_processing/downsample_fast_neon.c
    common_audio/signal_processing/filter_ar_fast_q12_armv7.S
    common_audio/signal_processing/min_max_operations_neon.c
)
endif()

if (MSVC)
set(spl_sqrt_floor_SRCS
    common_audio/third_party/spl_sqrt_floor/spl_sqrt_floor.c
)
elseif (ANDROID OR APPLE)
set(spl_sqrt_floor_SRCS
    common_audio/third_party/spl_sqrt_floor/spl_sqrt_floor_arm.S
)
endif()

set(vad_SRCS
    common_audio/vad/vad.cc
    common_audio/vad/vad_core.c
    common_audio/vad/vad_filterbank.c
    common_audio/vad/vad_gmm.c
    common_audio/vad/vad_sp.c
    common_audio/vad/webrtc_vad.c
)

set(cng_SRCS
    modules/audio_coding/codecs/cng/audio_encoder_cng.cc
    modules/audio_coding/codecs/cng/webrtc_cng.cc
)

set(codec_opus_SRCS
    modules/audio_coding/codecs/opus/audio_coder_opus_common.cc
    modules/audio_coding/codecs/opus/audio_decoder_multi_channel_opus_impl.cc
    modules/audio_coding/codecs/opus/audio_decoder_opus.cc
    modules/audio_coding/codecs/opus/opus_interface.c
)

set(system_wrappers_SRCS
    system_wrappers/source/cpu_features.cc
    system_wrappers/source/field_trial.cc
    system_wrappers/source/metrics.cc
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

add_library(rtc_base ${rtc_base_SRCS})
add_library(api ${api_SRCS})
add_library(signal_processing ${signal_processing_SRCS})
add_library(spl_sqrt_floor ${spl_sqrt_floor_SRCS})
add_library(vad ${vad_SRCS})
add_library(cng ${cng_SRCS})
add_library(codec_opus ${codec_opus_SRCS})
add_library(system_wrappers ${system_wrappers_SRCS})

add_library(neteq SHARED 
    ${neteq_SRCS}
    ${rtc_base_SRCS}
    ${api_SRCS}
    ${signal_processing_SRCS}
    ${spl_sqrt_floor_SRCS}
    ${vad_SRCS}
    ${cng_SRCS}
    ${codec_opus_SRCS}
    ${system_wrappers_SRCS})
target_link_libraries(neteq
    Winmm
    opus
)
