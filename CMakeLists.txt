cmake_minimum_required(VERSION 3.16)
project(BedrockProtocol VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ─── Options ──────────────────────────────────────────────────────
option(BEDROCK_PROTOCOL_BUILD_TESTS "Build unit tests" ON)
option(BEDROCK_PROTOCOL_BUILD_EXAMPLES "Build examples" ON)

# ─── Library (header-only) ────────────────────────────────────────
add_library(bedrock_protocol INTERFACE)
target_include_directories(bedrock_protocol INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(bedrock_protocol INTERFACE cxx_std_17)

# ─── Platform threading ──────────────────────────────────────────
if(UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(bedrock_protocol INTERFACE Threads::Threads)
endif()

if(WIN32)
    target_link_libraries(bedrock_protocol INTERFACE ws2_32)
endif()

# ─── Tests ────────────────────────────────────────────────────────
if(BEDROCK_PROTOCOL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ─── Examples ─────────────────────────────────────────────────────
if(BEDROCK_PROTOCOL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# ─── Install ──────────────────────────────────────────────────────
install(DIRECTORY include/ DESTINATION include)
install(TARGETS bedrock_protocol EXPORT BedrockProtocolTargets)
install(EXPORT BedrockProtocolTargets
    FILE BedrockProtocolConfig.cmake
    NAMESPACE BedrockProtocol::
    DESTINATION lib/cmake/BedrockProtocol
)
