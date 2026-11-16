#include "core/simulation/tick_scheduler.hpp"

#include <stdexcept>
#include <utility>

namespace fantasy::core {

void TickScheduler::register_system(TickPhase phase, System system,
                                    std::optional<SimulationSystem> cadence) {
    if (!system) {
        throw std::invalid_argument("tick system must be set");
    }
    phases_[static_cast<std::size_t>(phase)].push_back(Entry{std::move(system), cadence});
}

void TickScheduler::run_tick(const SimulationConfig& config, std::uint64_t tick,
                             const SimulationClock& clock, RandomStream& random) const {
    for (const auto phase : tick_phase_order) {
        for (const auto& entry : phases_[static_cast<std::size_t>(phase)]) {
            if (!entry.cadence || is_due(config, *entry.cadence, tick)) {
                entry.system(clock, random);
            }
        }
    }
}

std::size_t TickScheduler::system_count() const noexcept {
    std::size_t count = 0;
    for (const auto& phase : phases_) {
        count += phase.size();
    }
    return count;
}

} // namespace fantasy::core
