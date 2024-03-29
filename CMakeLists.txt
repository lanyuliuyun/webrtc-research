cmake_minimum_required(VERSION 3.9)

project(webrtc-research)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)

set(LIBOPUS_ROOT "E:/04.OpenSource/opus-1.3.1")

if (MSVC)
    add_compile_options(
        /utf-8
        /DWIN32_LEAN_AND_MEAN
        /D_CRT_SECURE_NO_WARNINGS
        /DWEBRTC_WIN=1
        /DNOMINMAX
        /DWEBRTC_USE_BUILTIN_OPUS=1
        /DUSE_CUSTOM_LIBOPUS_CODEC=1
    )
elseif (ANDROID)
    add_compile_options(
        -DWEBRTC_ANDROID=1
        -DWEBRTC_POSIX=1
        -DWEBRTC_HAS_NEON=1
        -DWEBRTC_USE_BUILTIN_OPUS=1
    )
elseif (APPLE)
    add_compile_options(
        -DWEBRTC_POSIX=1
        -DWEBRTC_MAC=1
        -DWEBRTC_HAS_NEON=1
        -DWEBRTC_USE_BUILTIN_OPUS=1
    )
elseif (UNIX)
    add_compile_options(
        -DWEBRTC_LINUX=1
        -DWEBRTC_POSIX=1
        -DWEBRTC_USE_BUILTIN_OPUS=1
    )
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/common_base
    ${LIBOPUS_ROOT}/include
)

link_directories(
    ${LIBOPUS_ROOT}/build
)

add_subdirectory(common_base)
add_subdirectory(common_audio)

add_subdirectory(neteq)
#add_subdirectory(neteq_test)

add_subdirectory(aec3)
