#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace fantasy::core {

// Domain systems that run on a configurable cadence.
enum class SimulationSystem { needs, activity, lifecycle, reporting };

// Per-system update cadences; each must be a positive multiple of the fixed step.
struct UpdateCadences {
    std::chrono::nanoseconds needs{std::chrono::minutes{1}};
    std::chrono::nanoseconds activity{std::chrono::minutes{1}};
    std::chrono::nanoseconds lifecycle{std::chrono::hours{1}};
    std::chrono::nanoseconds reporting{std::chrono::hours{1}};
};

struct SimulationConfig {
    std::uint64_t seed{0};
    std::uint64_t ticks{1000};
    std::chrono::nanoseconds fixed_step{std::chrono::seconds{1}}; // one world-second
    std::chrono::hours world_day_duration{24};
    std::chrono::hours real_time_per_world_day{4};
    UpdateCadences cadences{};
};

// Returns one readable message per violated rule; an empty list means valid.
// Rules: ticks > 0, fixed_step > 0, world_day_duration > 0 and an exact
// multiple of fixed_step, real_time_per_world_day > 0, and every cadence a
// positive multiple of fixed_step.
std::vector<std::string> validate(const SimulationConfig& config);

// Returns the cadence of a system.
std::chrono::nanoseconds cadence_of(const UpdateCadences& cadences, SimulationSystem system);

// True when the system is due at the zero-based tick (tick 0 is always due).
// Integer math only; returns false for an invalid step or cadence.
bool is_due(const SimulationConfig& config, SimulationSystem system, std::uint64_t tick);

inline constexpr SimulationConfig default_simulation_config{};

} // namespace fantasy::core
