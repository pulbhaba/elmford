#pragma once

#include <cstdint>
#include <optional>

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

// Derives a stream identifier from (world seed, system id, optional entity id).
// Mixing: each input is folded in with a SplitMix64 finalizer round and a
// distinct odd constant, in the fixed order seed, system, entity-tag, entity, so
// (system, entity) pairs are not interchangeable and "no entity" differs from
// entity 0. The result depends only on its inputs, never on call order or on
// which other systems or entities exist. System ids must be stable values
// (for example static_cast of a SimulationSystem enumerator).
[[nodiscard]] std::uint64_t derive_stream_id(std::uint64_t world_seed, std::uint64_t system_id,
                                             std::optional<std::uint64_t> entity_id = std::nullopt) noexcept;

// Domain contract for randomness: systems and entities draw from their own
// derived stream, not from a shared one.
[[nodiscard]] RandomStream derive_stream(std::uint64_t world_seed, std::uint64_t system_id,
                                         std::optional<std::uint64_t> entity_id = std::nullopt) noexcept;

} // namespace fantasy::core
