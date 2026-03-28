cmake_minimum_required(VERSION 3.16)
project(CpherTunnel VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ─── Options ──────────────────────────────────────────────────────
option(cphertunnel_BUILD_TESTS "Build unit tests" ON)
option(cphertunnel_BUILD_EXAMPLES "Build examples" ON)

# ─── Library (header-only) ────────────────────────────────────────
add_library(cphertunnel INTERFACE)
target_include_directories(cphertunnel INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(cphertunnel INTERFACE cxx_std_17)

# ─── Platform threading ──────────────────────────────────────────
if(UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(cphertunnel INTERFACE Threads::Threads)
endif()

if(WIN32)
    target_link_libraries(cphertunnel INTERFACE ws2_32)
endif()

# ─── Tests ────────────────────────────────────────────────────────
if(cphertunnel_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ─── Examples ─────────────────────────────────────────────────────
if(cphertunnel_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# ─── Install ──────────────────────────────────────────────────────
install(DIRECTORY include/ DESTINATION include)
install(TARGETS cphertunnel EXPORT CpherTunnelTargets)
install(EXPORT CpherTunnelTargets
    FILE CpherTunnelConfig.cmake
    NAMESPACE CpherTunnel::
    DESTINATION lib/cmake/CpherTunnel
)
