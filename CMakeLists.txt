cmake_minimum_required(VERSION 3.25)

project(fantasy_simulation VERSION 0.1.0 LANGUAGES CXX)

include(CTest)
option(FANTASY_SIM_BUILD_TESTS "Build the Catch2 test suite" ${BUILD_TESTING})

# Infrastructure: clock, random streams, and logging. Never links simulation.
add_library(fantasy_sim_infrastructure
    src/core/time/simulation_clock.cpp
    src/core/random/random_stream.cpp
    src/core/logging/logger.cpp
)
add_library(fantasy_sim::infrastructure ALIAS fantasy_sim_infrastructure)
target_include_directories(fantasy_sim_infrastructure PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_compile_features(fantasy_sim_infrastructure PUBLIC cxx_std_20)

# Simulation: domain code. Links infrastructure one-way.
add_library(fantasy_sim_simulation
    src/core/config/simulation_config.cpp
    src/core/simulation/simulation_runner.cpp
    src/core/simulation/tick_scheduler.cpp
    src/core/time/world_calendar.cpp
    src/core/world/world.cpp
)
add_library(fantasy_sim::simulation ALIAS fantasy_sim_simulation)
target_link_libraries(fantasy_sim_simulation PUBLIC fantasy_sim::infrastructure)

# Compatibility alias: existing consumers keep linking fantasy_sim::core.
add_library(fantasy_sim_core INTERFACE)
add_library(fantasy_sim::core ALIAS fantasy_sim_core)
target_link_libraries(fantasy_sim_core INTERFACE fantasy_sim::simulation)

add_executable(fantasy_sim src/application/main.cpp)
target_link_libraries(fantasy_sim PRIVATE fantasy_sim::core)

if(MSVC)
    target_compile_options(fantasy_sim_infrastructure PRIVATE /W4 /permissive-)
    target_compile_options(fantasy_sim_simulation PRIVATE /W4 /permissive-)
    target_compile_options(fantasy_sim PRIVATE /W4 /permissive-)
else()
    target_compile_options(fantasy_sim_infrastructure PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(fantasy_sim_simulation PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(fantasy_sim PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(FANTASY_SIM_BUILD_TESTS)
    include(FetchContent)
    set(CATCH_BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(CATCH_INSTALL_DOCS OFF CACHE BOOL "" FORCE)
    set(CATCH_INSTALL_EXTRAS OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        Catch2
        URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.7.1.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(fantasy_sim_tests
        tests/core/simulation_clock_tests.cpp
        tests/core/random_stream_tests.cpp
        tests/core/simulation_runner_tests.cpp
        tests/core/simulation_config_tests.cpp
        tests/core/tick_scheduler_tests.cpp
        tests/core/world_calendar_tests.cpp
        tests/core/golden_determinism_tests.cpp
        tests/core/logger_tests.cpp
        tests/core/stable_id_tests.cpp
        tests/core/world_tests.cpp
    )
    target_link_libraries(fantasy_sim_tests PRIVATE fantasy_sim::core Catch2::Catch2WithMain)
    if(MSVC)
        target_compile_options(fantasy_sim_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(fantasy_sim_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    include(Catch)
    catch_discover_tests(fantasy_sim_tests)
endif()
