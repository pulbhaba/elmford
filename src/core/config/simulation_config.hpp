#pragma once

#include <chrono>
#include <cstdint>

namespace fantasy::core {

struct SimulationConfig {
    std::uint64_t seed{0};
    std::uint64_t ticks{1000};
    std::chrono::nanoseconds fixed_step{std::chrono::seconds{1}}; // one world-second
    std::chrono::hours world_day_duration{24};
    std::chrono::hours real_time_per_world_day{4};
};

inline constexpr SimulationConfig default_simulation_config{};

} // namespace fantasy::core
