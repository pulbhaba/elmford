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
    config.cadences = {.needs = 7ns, .activity = 7ns, .lifecycle = 7ns, .reporting = 7ns};
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

TEST_CASE("default cadences are positive multiples of the default step") {
    const auto& config = fantasy::core::default_simulation_config;
    REQUIRE(config.cadences.needs == 1min);
    REQUIRE(config.cadences.activity == 1min);
    REQUIRE(config.cadences.lifecycle == 1h);
    REQUIRE(config.cadences.reporting == 1h);
}

TEST_CASE("validation rejects cadences that are not positive multiples of the step") {
    SimulationConfig config;
    config.cadences.needs = 0s;
    REQUIRE(mentions(config, "needs cadence must be positive"));

    config = {};
    config.cadences.activity = -1s;
    REQUIRE(mentions(config, "activity cadence must be positive"));

    config = {};
    config.fixed_step = 7s;
    config.world_day_duration = 24h * 7;
    config.cadences = {.needs = 14s, .activity = 14s, .lifecycle = 14s, .reporting = 15s};
    REQUIRE(mentions(config, "reporting cadence must be a multiple"));
}

TEST_CASE("is_due uses integer tick math at cadence boundaries") {
    using fantasy::core::SimulationSystem;
    using fantasy::core::is_due;
    SimulationConfig config;
    config.cadences.needs = 3s;

    REQUIRE(is_due(config, SimulationSystem::needs, 0));
    REQUIRE_FALSE(is_due(config, SimulationSystem::needs, 1));
    REQUIRE_FALSE(is_due(config, SimulationSystem::needs, 2));
    REQUIRE(is_due(config, SimulationSystem::needs, 3));
    REQUIRE(is_due(config, SimulationSystem::needs, 6));
    REQUIRE_FALSE(is_due(config, SimulationSystem::lifecycle, 3599));
    REQUIRE(is_due(config, SimulationSystem::lifecycle, 3600));

    config.cadences.reporting = 0s;
    REQUIRE_FALSE(is_due(config, SimulationSystem::reporting, 0));
}
