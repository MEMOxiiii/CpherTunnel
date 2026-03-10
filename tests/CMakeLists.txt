cmake_minimum_required(VERSION 3.16)

# Simple test executable
add_executable(bedrock_protocol_tests
    test_main.cpp
)

target_include_directories(bedrock_protocol_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(bedrock_protocol_tests PUBLIC cxx_std_17)

if(WIN32)
    target_link_libraries(bedrock_protocol_tests PRIVATE ws2_32)
endif()
