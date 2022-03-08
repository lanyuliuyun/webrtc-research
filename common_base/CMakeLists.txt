include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(abseil_SRCS
    absl/strings/match.cc
    absl/base/internal/throw_delegate.cc
    absl/strings/internal/memutil.cc
    absl/strings/ascii.cc
    absl/types/bad_optional_access.cc
    absl/types/optional.cc
)

#rtc_base/strings/audio_format_to_string.cc
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
    rtc_base/strings/string_builder.cc
    rtc_base/synchronization/sequence_checker.cc
    rtc_base/synchronization/yield_policy.cc
)

set(system_wrappers_SRCS
    system_wrappers/source/cpu_features.cc
    system_wrappers/source/field_trial.cc
    system_wrappers/source/metrics.cc
)

set(api_SRCS
    api/task_queue/task_queue_base.cc
)

add_library(webrtc_common_base
    ${abseil_SRCS}
    ${rtc_base_SRCS}
    ${system_wrappers_SRCS}
    ${api_SRCS}
)
