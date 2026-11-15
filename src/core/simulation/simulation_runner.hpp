#pragma once

#include "core/config/simulation_config.hpp"
#include "core/random/random_stream.hpp"
#include "core/time/simulation_clock.hpp"

#include <chrono>
#include <cstdint>
#include <functional>

namespace fantasy::core {

struct SimulationStats {
    std::uint64_t steps_executed;
    SimulationClock::Duration final_simulation_time;
    std::chrono::steady_clock::duration wall_time;
};

class SimulationRunner {
public:
    using Update = std::function<void(const SimulationClock&, RandomStream&)>;

    explicit SimulationRunner(SimulationConfig config);
    explicit SimulationRunner(
        std::uint64_t seed,
        SimulationClock::Duration fixed_step = std::chrono::milliseconds{1});

    [[nodiscard]] const SimulationClock& clock() const noexcept;
    [[nodiscard]] RandomStream& random() noexcept;
    [[nodiscard]] std::uint64_t seed() const noexcept;

    SimulationStats run(std::uint64_t steps, const Update& update);

private:
    std::uint64_t seed_;
    SimulationClock clock_;
    RandomStream random_;
};

} // namespace fantasy::core
