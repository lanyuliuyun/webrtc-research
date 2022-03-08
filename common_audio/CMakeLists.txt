
set(signal_processing_SRCS
    signal_processing/auto_correlation.c
    signal_processing/auto_corr_to_refl_coef.c
    signal_processing/complex_fft.c
    signal_processing/copy_set_operations.c
    signal_processing/division_operations.c
    signal_processing/dot_product_with_scale.cc
    signal_processing/energy.c
    signal_processing/filter_ar.c
    signal_processing/filter_ma_fast_q12.c
    signal_processing/get_hanning_window.c
    signal_processing/get_scaling_square.c
    signal_processing/ilbc_specific_functions.c
    signal_processing/levinson_durbin.c
    signal_processing/lpc_to_refl_coef.c
    signal_processing/randomization_functions.c
    signal_processing/real_fft.c
    signal_processing/refl_coef_to_lpc.c
    signal_processing/resample.c
    signal_processing/resample_48khz.c
    signal_processing/resample_by_2.c
    signal_processing/resample_by_2_internal.c
    signal_processing/resample_fractional.c
    signal_processing/splitting_filter.c
    signal_processing/spl_init.c
    signal_processing/spl_inl.c
    signal_processing/spl_sqrt.c
    signal_processing/sqrt_of_one_minus_x_squared.c
    signal_processing/vector_scaling_operations.c
)
if (ANDROID OR APPLE)
list(APPEND signal_processing_SRCS
    signal_processing/complex_bit_reverse_arm.S
    signal_processing/cross_correlation_neon.c
    signal_processing/downsample_fast_neon.c
    signal_processing/filter_ar_fast_q12_armv7.S
    signal_processing/min_max_operations_neon.c
)
elseif (MSVC OR UNIX)
list(APPEND signal_processing_SRCS
    signal_processing/complex_bit_reverse.c
    signal_processing/cross_correlation.c
    signal_processing/downsample_fast.c
    signal_processing/filter_ar_fast_q12.c
    signal_processing/min_max_operations.c
)
endif()

if (ANDROID OR APPLE)
set(spl_sqrt_floor_SRCS
    third_party/spl_sqrt_floor/spl_sqrt_floor_arm.S
)
elseif (MSVC OR UNIX)
set(spl_sqrt_floor_SRCS
    third_party/spl_sqrt_floor/spl_sqrt_floor.c
)
endif()

set(vad_SRCS
    vad/vad.cc
    vad/vad_core.c
    vad/vad_filterbank.c
    vad/vad_gmm.c
    vad/vad_sp.c
    vad/webrtc_vad.c
)

add_library(webrtc_common_audio
    ${signal_processing_SRCS}
    ${spl_sqrt_floor_SRCS}
    ${vad_SRCS}
)