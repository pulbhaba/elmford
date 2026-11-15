#pragma once

#include <cstdint>

namespace fantasy::core {

class RandomStream {
public:
    explicit RandomStream(std::uint64_t seed, std::uint64_t stream_id = 0) noexcept;

    [[nodiscard]] std::uint64_t next_u64() noexcept;
    [[nodiscard]] std::int64_t uniform_int(std::int64_t minimum, std::int64_t maximum);
    [[nodiscard]] double uniform_real(double minimum, double maximum);

private:
    static std::uint64_t mix(std::uint64_t value) noexcept;

    std::uint64_t state_;
};

} // namespace fantasy::core
