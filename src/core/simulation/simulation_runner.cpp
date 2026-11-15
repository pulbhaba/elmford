#include "core/simulation/simulation_runner.hpp"

#include <chrono>
#include <stdexcept>

namespace fantasy::core {

SimulationRunner::SimulationRunner(std::uint64_t seed, SimulationClock::Duration fixed_step)
    : seed_(seed), clock_(fixed_step), random_(seed) {}

const SimulationClock& SimulationRunner::clock() const noexcept {
    return clock_;
}

RandomStream& SimulationRunner::random() noexcept {
    return random_;
}

std::uint64_t SimulationRunner::seed() const noexcept {
    return seed_;
}

SimulationStats SimulationRunner::run(std::uint64_t steps, const Update& update) {
    if (!update) {
        throw std::invalid_argument("simulation update callback must be set");
    }

    const auto start = std::chrono::steady_clock::now();
    for (std::uint64_t step = 0; step < steps; ++step) {
        clock_.advance();
        update(clock_, random_);
    }
    const auto finish = std::chrono::steady_clock::now();

    return SimulationStats{
        .steps_executed = steps,
        .final_simulation_time = clock_.current_time(),
        .wall_time = finish - start,
    };
}

} // namespace fantasy::core
