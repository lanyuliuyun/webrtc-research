include_directories(
    ${PROJECT_SOURCE_DIR}/neteq/include
    ${PROJECT_SOURCE_DIR}/neteq
    ${PROJECT_SOURCE_DIR}/neteq/abseil-cpp
)

add_executable(neteq_test neteq_test.cpp)
target_link_libraries(neteq_test
    neteq
    opus
    Winmm
)