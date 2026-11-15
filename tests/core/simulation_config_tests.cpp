#include "core/config/simulation_config.hpp"
#include "core/simulation/simulation_runner.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>

using namespace std::chrono_literals;

TEST_CASE("default simulation configuration defines world-day pacing") {
    const auto& config = fantasy::core::default_simulation_config;

    REQUIRE(config.seed == 0);
    REQUIRE(config.ticks == 1000);
    REQUIRE(config.fixed_step == 1ms);
    REQUIRE(config.world_day_duration == 24h);
    REQUIRE(config.real_time_per_world_day == 4h);
    REQUIRE(config.world_day_duration / config.real_time_per_world_day == 6);
}

TEST_CASE("simulation runner accepts committed simulation configuration") {
    const fantasy::core::SimulationConfig config{
        .seed = 123,
        .ticks = 17,
        .fixed_step = 5ms,
    };
    const fantasy::core::SimulationRunner runner{config};

    REQUIRE(runner.seed() == 123);
    REQUIRE(runner.clock().fixed_step() == 5ms);
}
