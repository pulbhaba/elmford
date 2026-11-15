#include "core/simulation/simulation_runner.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <cstdint>
#include <stdexcept>

using namespace std::chrono_literals;

TEST_CASE("simulation runner performs one update for each step") {
    fantasy::core::SimulationRunner runner{123, 5ms};
    std::uint64_t checksum = 0;
    std::uint64_t updates = 0;

    const auto stats = runner.run(4, [&checksum, &updates](const auto& clock, auto& random) {
        ++updates;
        REQUIRE(clock.tick_index() == updates);
        checksum += random.next_u64();
    });

    REQUIRE(runner.seed() == 123);
    REQUIRE(updates == 4);
    REQUIRE(checksum != 0);
    REQUIRE(stats.steps_executed == 4);
    REQUIRE(stats.final_simulation_time == 20ms);
    REQUIRE(runner.clock().tick_index() == 4);
}

TEST_CASE("simulation runner supports a zero-step run") {
    fantasy::core::SimulationRunner runner{0};
    std::uint64_t updates = 0;

    const auto stats = runner.run(0, [&updates](const auto&, auto&) { ++updates; });

    REQUIRE(updates == 0);
    REQUIRE(stats.steps_executed == 0);
    REQUIRE(stats.final_simulation_time == 0ns);
}

TEST_CASE("simulation runner rejects a missing update callback") {
    fantasy::core::SimulationRunner runner{0};

    REQUIRE_THROWS_AS(runner.run(1, {}), std::invalid_argument);
}
