#include "core/config/simulation_config.hpp"

#include <utility>

namespace fantasy::core {

std::chrono::nanoseconds cadence_of(const UpdateCadences& cadences, SimulationSystem system) {
    switch (system) {
    case SimulationSystem::needs:
        return cadences.needs;
    case SimulationSystem::activity:
        return cadences.activity;
    case SimulationSystem::lifecycle:
        return cadences.lifecycle;
    case SimulationSystem::reporting:
        return cadences.reporting;
    }
    return std::chrono::nanoseconds::zero();
}

bool is_due(const SimulationConfig& config, SimulationSystem system, std::uint64_t tick) {
    const auto step = config.fixed_step.count();
    const auto cadence = cadence_of(config.cadences, system).count();
    if (step <= 0 || cadence <= 0 || cadence % step != 0) {
        return false;
    }
    return tick % static_cast<std::uint64_t>(cadence / step) == 0;
}

std::vector<std::string> validate(const SimulationConfig& config) {
    std::vector<std::string> errors;
    using std::chrono::nanoseconds;

    if (config.ticks == 0) {
        errors.emplace_back("ticks must be positive");
    }
    const bool step_positive = config.fixed_step > nanoseconds::zero();
    if (!step_positive) {
        errors.emplace_back("fixed step must be positive");
    }
    if (config.world_day_duration <= config.world_day_duration.zero()) {
        errors.emplace_back("world-day duration must be positive");
    } else if (step_positive &&
               std::chrono::duration_cast<nanoseconds>(config.world_day_duration) % config.fixed_step !=
                   nanoseconds::zero()) {
        errors.emplace_back("fixed step of " + std::to_string(config.fixed_step.count()) +
                            " ns must evenly divide the world-day duration");
    }
    if (config.real_time_per_world_day <= config.real_time_per_world_day.zero()) {
        errors.emplace_back("real-time duration per world day must be positive");
    }
    const std::pair<const char*, SimulationSystem> systems[] = {
        {"needs", SimulationSystem::needs},
        {"activity", SimulationSystem::activity},
        {"lifecycle", SimulationSystem::lifecycle},
        {"reporting", SimulationSystem::reporting},
    };
    for (const auto& [name, system] : systems) {
        const auto cadence = cadence_of(config.cadences, system);
        if (cadence <= nanoseconds::zero()) {
            errors.emplace_back(std::string{name} + " cadence must be positive");
        } else if (step_positive && cadence % config.fixed_step != nanoseconds::zero()) {
            errors.emplace_back(std::string{name} + " cadence must be a multiple of the fixed step");
        }
    }
    return errors;
}

} // namespace fantasy::core
