#pragma once

#include <chrono>
#include <cstdint>

namespace fantasy::core {

class SimulationClock {
public:
    using Duration = std::chrono::nanoseconds;

    explicit SimulationClock(Duration fixed_step = std::chrono::milliseconds{1});

    [[nodiscard]] Duration fixed_step() const noexcept;
    [[nodiscard]] Duration current_time() const noexcept;
    [[nodiscard]] std::uint64_t tick_index() const noexcept;

    void advance();

private:
    Duration fixed_step_;
    Duration current_time_{0};
    std::uint64_t tick_index_{0};
};

} // namespace fantasy::core
