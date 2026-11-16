#include "core/config/simulation_config.hpp"
#include "core/time/simulation_clock.hpp"
#include "core/time/world_calendar.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <stdexcept>

using namespace std::chrono_literals;
using fantasy::core::WorldTime;
using fantasy::core::world_time_from;

TEST_CASE("world time at zero is the start of day zero") {
    const auto t = world_time_from(0ns, 24h);
    REQUIRE(t.day_index == 0);
    REQUIRE(t.hour == 0);
    REQUIRE(t.minute == 0);
    REQUIRE(t.time_of_day == 0ns);
}

TEST_CASE("world time handles day boundaries exactly") {
    const auto last = world_time_from(24h - 1ns, 24h);
    REQUIRE(last.day_index == 0);
    REQUIRE(last.hour == 23);
    REQUIRE(last.minute == 59);
    REQUIRE(last.time_of_day == 24h - 1ns);

    const auto next = world_time_from(24h, 24h);
    REQUIRE(next.day_index == 1);
    REQUIRE(next.hour == 0);
    REQUIRE(next.minute == 0);
    REQUIRE(next.time_of_day == 0ns);
}

TEST_CASE("world time floors hours and minutes at boundaries") {
    REQUIRE(world_time_from(59s, 24h).minute == 0);
    REQUIRE(world_time_from(60s, 24h).minute == 1);
    REQUIRE(world_time_from(1h - 1ns, 24h).hour == 0);
    const auto t = world_time_from(1h, 24h);
    REQUIRE(t.hour == 1);
    REQUIRE(t.minute == 0);
}

TEST_CASE("world time follows default pacing and fixed steps") {
    const fantasy::core::SimulationConfig config{};
    fantasy::core::SimulationClock clock{config.fixed_step};
    // 1 world-second per tick: 3661 ticks is 01:01:01 on day 0.
    for (int i = 0; i < 3661; ++i) {
        clock.advance();
    }
    const auto t = world_time_from(clock.current_time(), config.world_day_duration);
    REQUIRE(t == WorldTime{0, 1, 1, 3661s});

    for (int i = 0; i < 86400 - 3661; ++i) {
        clock.advance();
    }
    const auto next = world_time_from(clock.current_time(), config.world_day_duration);
    REQUIRE(next == WorldTime{1, 0, 0, 0ns});
}

TEST_CASE("world time scales with a non-default world day duration") {
    // A 12h world day: each world-hour is 30 minutes of simulation time.
    const auto half = world_time_from(6h, 12h);
    REQUIRE(half.day_index == 0);
    REQUIRE(half.hour == 12);
    REQUIRE(half.minute == 0);

    const auto t = world_time_from(12h + 45min, 12h);
    REQUIRE(t.day_index == 1);
    REQUIRE(t.hour == 1);
    REQUIRE(t.minute == 30);
    REQUIRE(t.time_of_day == 45min);

    const auto day3 = world_time_from(36h, 12h);
    REQUIRE(day3.day_index == 3);
    REQUIRE(day3.hour == 0);
}

TEST_CASE("world time rejects invalid inputs") {
    REQUIRE_THROWS_AS(world_time_from(0ns, 0ns), std::invalid_argument);
    REQUIRE_THROWS_AS(world_time_from(0ns, -1h), std::invalid_argument);
    REQUIRE_THROWS_AS(world_time_from(-1ns, 24h), std::invalid_argument);
    REQUIRE_THROWS_AS(world_time_from(0ns, std::chrono::nanoseconds::max()),
                      std::invalid_argument);
}
