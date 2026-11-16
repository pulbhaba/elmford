#include "core/config/simulation_config.hpp"

namespace fantasy::core {

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
    return errors;
}

} // namespace fantasy::core
