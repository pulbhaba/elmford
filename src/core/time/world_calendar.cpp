#include "core/time/world_calendar.hpp"

#include <limits>
#include <stdexcept>

namespace fantasy::core {

namespace {
constexpr std::int64_t minutes_per_day = 24 * 60;
}

WorldTime world_time_from(std::chrono::nanoseconds simulation_time,
                          std::chrono::nanoseconds world_day_duration) {
    const std::int64_t day = world_day_duration.count();
    if (day <= 0 || day > std::numeric_limits<std::int64_t>::max() / minutes_per_day) {
        throw std::invalid_argument("world day duration must be positive and bounded");
    }
    const std::int64_t now = simulation_time.count();
    if (now < 0) {
        throw std::invalid_argument("simulation time must not be negative");
    }

    const std::int64_t time_of_day = now % day;
    const std::int64_t minute_of_day = time_of_day * minutes_per_day / day;

    WorldTime result;
    result.day_index = static_cast<std::uint64_t>(now / day);
    result.hour = static_cast<std::uint8_t>(minute_of_day / 60);
    result.minute = static_cast<std::uint8_t>(minute_of_day % 60);
    result.time_of_day = std::chrono::nanoseconds{time_of_day};
    return result;
}

} // namespace fantasy::core
