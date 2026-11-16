#include "core/simulation/simulation_runner.hpp"

#include <chrono>
#include <stdexcept>

namespace fantasy::core {

SimulationRunner::SimulationRunner(SimulationConfig config)
    : config_(config), seed_(config.seed), clock_(config.fixed_step), random_(config.seed) {}

SimulationRunner::SimulationRunner(std::uint64_t seed, SimulationClock::Duration fixed_step)
    : SimulationRunner(SimulationConfig{.seed = seed, .fixed_step = fixed_step}) {}

const SimulationClock& SimulationRunner::clock() const noexcept {
    return clock_;
}

RandomStream& SimulationRunner::random() noexcept {
    return random_;
}

TickScheduler& SimulationRunner::scheduler() noexcept {
    return scheduler_;
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
        // Phases run for the zero-based tick, then time advances.
        scheduler_.run_tick(config_, clock_.tick_index(), clock_, random_);
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
