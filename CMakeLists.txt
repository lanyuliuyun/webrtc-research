cmake_minimum_required(VERSION 3.9)

project(webrtc-research)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)

set(HIKOPUS_ROOT "D:/01.project/ezrtc/v1.5-dev/dep/opus_codec")

if (MSVC)
    add_compile_options(
        /utf-8
        /MT
        /DWIN32_LEAN_AND_MEAN
        /D_CRT_SECURE_NO_WARNINGS
        /DWEBRTC_WIN=1
        /DNOMINMAX
        /std:c++17
        /DWEBRTC_USE_BUILTIN_OPUS=1
    )
elseif (ANDROID)
    add_compile_options(
        /DWEBRTC_ANDROID=1
        /DWEBRTC_POSIX=1
        /DWEBRTC_HAS_NEON=1
        /DWEBRTC_USE_BUILTIN_OPUS=1
    )
elseif (APPLE)
    add_compile_options(
        /DWEBRTC_POSIX=1
        /DWEBRTC_MAC=1
        /DWEBRTC_HAS_NEON=1
        /DWEBRTC_USE_BUILTIN_OPUS=1
    )
elseif (UNIX)
    add_compile_options(
        /DWEBRTC_LINUX=1
        /DWEBRTC_POSIX=1
        /DWEBRTC_USE_BUILTIN_OPUS=1
    )
endif()

include_directories(
    ${HIKOPUS_ROOT}/include
)

link_directories(
    ${HIKOPUS_ROOT}/lib
)

add_subdirectory(neteq)
add_subdirectory(neteq_test)
