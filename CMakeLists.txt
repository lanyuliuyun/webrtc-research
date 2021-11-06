cmake_minimum_required(VERSION 3.9)

project(webrtc-research)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)

set(WEBRTC_ROOT "D:/webrtc/agora_mirror/webrtc/src")
set(ABSEIL_ROOT "D:/webrtc/agora_mirror/webrtc/src/third_party/abseil-cpp")

if (MSVC)
    add_compile_options(
        /utf-8
        /DWIN32_LEAN_AND_MEAN
        /DWEBRTC_WIN=1
        /DNOMINMAX
        /std:c++17
    )
endif()

include_directories(
    ${ABSEIL_ROOT}
    ${PROJECT_SOURCE_DIR}
)

add_subdirectory(neteq)
