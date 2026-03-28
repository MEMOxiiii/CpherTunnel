cmake_minimum_required(VERSION 3.16)

# Simple test executable
add_executable(cphertunnel_tests
    test_main_new.cpp
)

target_include_directories(cphertunnel_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(cphertunnel_tests PUBLIC cxx_std_17)

if(WIN32)
    target_link_libraries(cphertunnel_tests PRIVATE ws2_32)
endif()
