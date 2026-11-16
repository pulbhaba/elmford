#include "core/config/simulation_config.hpp"
#include "core/simulation/simulation_runner.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <string>

using namespace std::chrono_literals;

TEST_CASE("default simulation configuration defines world-day pacing") {
    const auto& config = fantasy::core::default_simulation_config;

    REQUIRE(config.seed == 0);
    REQUIRE(config.ticks == 1000);
    REQUIRE(config.fixed_step == 1s);
    REQUIRE(config.world_day_duration % config.fixed_step == 0s);
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

namespace {

using fantasy::core::SimulationConfig;
using fantasy::core::validate;

bool mentions(const SimulationConfig& config, const std::string& text) {
    const auto errors = validate(config);
    return errors.size() == 1 && errors.front().find(text) != std::string::npos;
}

} // namespace

TEST_CASE("default simulation configuration validates") {
    REQUIRE(validate(fantasy::core::default_simulation_config).empty());
}

TEST_CASE("validation rejects each invalid rule with a readable message") {
    SimulationConfig config;

    config = {};
    config.ticks = 0;
    REQUIRE(mentions(config, "ticks"));

    config = {};
    config.fixed_step = 0ns;
    REQUIRE(mentions(config, "fixed step"));

    config = {};
    config.fixed_step = -1s;
    REQUIRE(mentions(config, "fixed step"));

    config = {};
    config.fixed_step = 7ns;
    REQUIRE(mentions(config, "divide"));

    config = {};
    config.world_day_duration = 0h;
    REQUIRE(mentions(config, "world-day duration"));

    config = {};
    config.real_time_per_world_day = 0h;
    REQUIRE(mentions(config, "real-time"));
}

TEST_CASE("validation reports every violated rule") {
    SimulationConfig config;
    config.ticks = 0;
    config.fixed_step = 0ns;
    config.real_time_per_world_day = -1h;
    REQUIRE(validate(config).size() == 3);
}
