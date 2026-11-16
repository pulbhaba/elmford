#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace fantasy::core {

struct SimulationConfig {
    std::uint64_t seed{0};
    std::uint64_t ticks{1000};
    std::chrono::nanoseconds fixed_step{std::chrono::seconds{1}}; // one world-second
    std::chrono::hours world_day_duration{24};
    std::chrono::hours real_time_per_world_day{4};
};

// Returns one readable message per violated rule; an empty list means valid.
// Rules: ticks > 0, fixed_step > 0, world_day_duration > 0 and an exact
// multiple of fixed_step, real_time_per_world_day > 0.
std::vector<std::string> validate(const SimulationConfig& config);

inline constexpr SimulationConfig default_simulation_config{};

} // namespace fantasy::core
