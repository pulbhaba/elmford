#include "core/time/simulation_clock.hpp"

#include <limits>
#include <stdexcept>

namespace fantasy::core {

SimulationClock::SimulationClock(Duration fixed_step) : fixed_step_(fixed_step) {
    if (fixed_step_.count() <= 0) {
        throw std::invalid_argument("simulation fixed step must be positive");
    }
}

SimulationClock::Duration SimulationClock::fixed_step() const noexcept {
    return fixed_step_;
}

SimulationClock::Duration SimulationClock::current_time() const noexcept {
    return current_time_;
}

std::uint64_t SimulationClock::tick_index() const noexcept {
    return tick_index_;
}

void SimulationClock::advance() {
    if (tick_index_ == std::numeric_limits<std::uint64_t>::max()) {
        throw std::overflow_error("simulation tick index overflow");
    }
    if (fixed_step_.count() > std::numeric_limits<Duration::rep>::max() - current_time_.count()) {
        throw std::overflow_error("simulation time overflow");
    }

    current_time_ += fixed_step_;
    ++tick_index_;
}

} // namespace fantasy::core
