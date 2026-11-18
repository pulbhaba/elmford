#pragma once

#include <compare>
#include <cstdint>
#include <functional>
#include <limits>
#include <stdexcept>

namespace fantasy::core {

// A stable identifier whose tag makes different ID domains non-interchangeable.
// The maximum value is reserved as invalid; default-constructed IDs are invalid.
template <typename Tag>
class StableId {
public:
    using value_type = std::uint64_t;
    static constexpr value_type invalid_value = std::numeric_limits<value_type>::max();

    constexpr StableId() noexcept = default;

    [[nodiscard]] static constexpr StableId from_value(value_type value) noexcept {
        return StableId{value, ConstructionToken{}};
    }

    [[nodiscard]] constexpr value_type value() const noexcept { return value_; }
    [[nodiscard]] constexpr bool is_valid() const noexcept { return value_ != invalid_value; }

    friend constexpr bool operator==(StableId, StableId) noexcept = default;
    friend constexpr auto operator<=>(StableId, StableId) noexcept = default;

private:
    struct ConstructionToken {};

    constexpr StableId(value_type value, ConstructionToken) noexcept : value_{value} {}

    value_type value_{invalid_value};
};

struct RegionIdTag final {};
struct SettlementIdTag final {};
struct LocationIdTag final {};

using RegionId = StableId<RegionIdTag>;
using SettlementId = StableId<SettlementIdTag>;
using LocationId = StableId<LocationIdTag>;

// Allocates monotonically increasing IDs in call order. The allocator starts
// at zero by default and throws before it could issue the invalid sentinel.
template <typename Id>
class StableIdAllocator {
public:
    using value_type = typename Id::value_type;

    explicit constexpr StableIdAllocator(value_type first_value = 0) noexcept
        : next_value_{first_value} {}

    [[nodiscard]] Id allocate() {
        if (next_value_ == Id::invalid_value) {
            throw std::overflow_error("stable ID allocator exhausted");
        }

        const auto value = next_value_;
        ++next_value_;
        return Id::from_value(value);
    }

private:
    value_type next_value_;
};

} // namespace fantasy::core

namespace std {

template <typename Tag>
struct hash<fantasy::core::StableId<Tag>> {
    [[nodiscard]] size_t operator()(fantasy::core::StableId<Tag> id) const noexcept {
        return hash<std::uint64_t>{}(id.value());
    }
};

} // namespace std
