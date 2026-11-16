#pragma once

#include <chrono>
#include <cstdint>

namespace fantasy::core {

// Calendar position derived from simulation time. All fields use integer math.
//
// Units:
//   day_index    whole world days elapsed since time zero (day 0 is the first).
//   hour         0..23, the world-hour within the day.
//   minute       0..59, the world-minute within the world-hour.
//   time_of_day  simulation nanoseconds elapsed since the day began,
//                in [0, world_day_duration).
//
// A world day always has 24 world-hours of 60 world-minutes. The configured
// world-day duration sets how much simulation time that is, so with a non-24h
// duration the world-hour and world-minute scale proportionally.
//
// Rounding: hour and minute are floored, so a boundary instant belongs to the
// later unit. minute = floor(time_of_day * 1440 / world_day_duration), split
// into hour = minute_of_day / 60 and minute = minute_of_day % 60.
struct WorldTime {
    std::uint64_t day_index{0};
    std::uint8_t hour{0};
    std::uint8_t minute{0};
    std::chrono::nanoseconds time_of_day{0};

    friend bool operator==(const WorldTime&, const WorldTime&) = default;
};

// Derives WorldTime from a clock reading. Throws std::invalid_argument for a
// non-positive world-day duration, one too large for exact integer math, or
// negative simulation time. Calendar rules stop at the 24-hour day.
[[nodiscard]] WorldTime world_time_from(std::chrono::nanoseconds simulation_time,
                                        std::chrono::nanoseconds world_day_duration);

} // namespace fantasy::core
