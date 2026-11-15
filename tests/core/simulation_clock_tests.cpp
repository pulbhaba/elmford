#include "core/time/simulation_clock.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <limits>
#include <stdexcept>

using namespace std::chrono_literals;

TEST_CASE("simulation clock starts at zero") {
    const fantasy::core::SimulationClock clock{10ms};

    REQUIRE(clock.current_time() == 0ns);
    REQUIRE(clock.fixed_step() == 10ms);
    REQUIRE(clock.tick_index() == 0);
}

TEST_CASE("simulation clock advances by its fixed step") {
    fantasy::core::SimulationClock clock{250us};

    clock.advance();
    REQUIRE(clock.current_time() == 250us);
    REQUIRE(clock.tick_index() == 1);

    clock.advance();
    clock.advance();
    REQUIRE(clock.current_time() == 750us);
    REQUIRE(clock.tick_index() == 3);
}

TEST_CASE("simulation clock rejects non-positive fixed steps") {
    REQUIRE_THROWS_AS(fantasy::core::SimulationClock{0ns}, std::invalid_argument);
    REQUIRE_THROWS_AS(fantasy::core::SimulationClock{-1ns}, std::invalid_argument);
}

TEST_CASE("simulation clock detects elapsed time overflow") {
    fantasy::core::SimulationClock clock{fantasy::core::SimulationClock::Duration::max()};

    clock.advance();
    REQUIRE(clock.tick_index() == 1);
    REQUIRE_THROWS_AS(clock.advance(), std::overflow_error);
}
