#include "core/random/random_stream.hpp"

#include <bit>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace fantasy::core {
namespace {

constexpr std::uint64_t sign_bit = std::uint64_t{1} << 63;
constexpr std::uint64_t increment = 0x9E3779B97F4A7C15ULL;

} // namespace

RandomStream::RandomStream(std::uint64_t seed, std::uint64_t stream_id) noexcept
    : state_(mix(seed ^ mix(stream_id))) {}

std::uint64_t RandomStream::mix(std::uint64_t value) noexcept {
    value = (value ^ (value >> 30)) * 0xBF58476D1CE4E5B9ULL;
    value = (value ^ (value >> 27)) * 0x94D049BB133111EBULL;
    return value ^ (value >> 31);
}

std::uint64_t RandomStream::next_u64() noexcept {
    state_ += increment;
    return mix(state_);
}

std::int64_t RandomStream::uniform_int(std::int64_t minimum, std::int64_t maximum) {
    if (minimum > maximum) {
        throw std::invalid_argument("random integer minimum must not exceed maximum");
    }

    const auto ordered_minimum = std::bit_cast<std::uint64_t>(minimum) ^ sign_bit;
    const auto ordered_maximum = std::bit_cast<std::uint64_t>(maximum) ^ sign_bit;
    const std::uint64_t span = ordered_maximum - ordered_minimum + 1;

    std::uint64_t offset;
    if (span == 0) {
        // The requested interval covers the entire int64_t domain.
        offset = next_u64();
    } else {
        const std::uint64_t rejection_threshold = (std::uint64_t{0} - span) % span;
        std::uint64_t value;
        do {
            value = next_u64();
        } while (value < rejection_threshold);
        offset = value % span;
    }

    const auto ordered_result = ordered_minimum + offset;
    return std::bit_cast<std::int64_t>(ordered_result ^ sign_bit);
}

double RandomStream::uniform_real(double minimum, double maximum) {
    if (!std::isfinite(minimum) || !std::isfinite(maximum) || minimum >= maximum) {
        throw std::invalid_argument("random real bounds must be finite and minimum must be less than maximum");
    }

    constexpr double unit_range = 1.0 / 9007199254740992.0; // 2^53
    const auto mantissa = next_u64() >> 11;
    const double unit_value = static_cast<double>(mantissa) * unit_range;
    const double result = std::lerp(minimum, maximum, unit_value);
    return result < maximum ? result : std::nextafter(maximum, minimum);
}

} // namespace fantasy::core
