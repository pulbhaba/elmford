#pragma once

#include "core/config/simulation_config.hpp"
#include "core/random/random_stream.hpp"
#include "core/time/simulation_clock.hpp"

#include <array>
#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

namespace fantasy::core {

// Tick phases in their fixed execution order. This enum is the single
// definition of the order; advancing time follows the last phase and is done
// by the runner, not by a registered system.
enum class TickPhase { commands, needs, activities, resolution, lifecycle };

inline constexpr std::array<TickPhase, 5> tick_phase_order{
    TickPhase::commands,
    TickPhase::needs,
    TickPhase::activities,
    TickPhase::resolution,
    TickPhase::lifecycle,
};

class TickScheduler {
public:
    using System = std::function<void(const SimulationClock&, RandomStream&)>;

    // Registers a system in a phase. With a cadence key the system runs only
    // on ticks where is_due(config, key, tick) holds; without one it runs on
    // every tick. Systems in a phase run in registration order.
    void register_system(TickPhase phase, System system,
                         std::optional<SimulationSystem> cadence = std::nullopt);

    // Runs every due system for the zero-based tick, phase by phase.
    void run_tick(const SimulationConfig& config, std::uint64_t tick,
                  const SimulationClock& clock, RandomStream& random) const;

    [[nodiscard]] std::size_t system_count() const noexcept;

private:
    struct Entry {
        System system;
        std::optional<SimulationSystem> cadence;
    };
    std::array<std::vector<Entry>, tick_phase_order.size()> phases_;
};

} // namespace fantasy::core
