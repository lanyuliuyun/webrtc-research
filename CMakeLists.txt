cmake_minimum_required(VERSION 3.9)

project(webrtc-research)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)

set(WEBRTC_ROOT "E:/04.OpenSource/webrtc_agora")
set(ABSEIL_ROOT "E:/04.OpenSource/abseil-cpp-master")
set(LIBOPUS_ROOT "D:/01.project/ezrtc/v1.5-dev/dep/opus-1.3.1")

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
endif()

include_directories(
    ${ABSEIL_ROOT}
    ${PROJECT_SOURCE_DIR}
    ${LIBOPUS_ROOT}/include
)

link_directories(
    ${LIBOPUS_ROOT}/lib
)

add_subdirectory(neteq)
