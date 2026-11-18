#include "core/simulation/stable_id.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>

using fantasy::core::LocationId;
using fantasy::core::RegionId;
using fantasy::core::SettlementId;
using fantasy::core::StableIdAllocator;

static_assert(!std::is_same_v<RegionId, SettlementId>);
static_assert(!std::is_convertible_v<RegionId, SettlementId>);
static_assert(!std::is_convertible_v<std::uint64_t, RegionId>);

TEST_CASE("stable IDs default to the documented invalid value") {
    constexpr RegionId invalid{};
    constexpr auto valid = RegionId::from_value(0);

    STATIC_REQUIRE(RegionId::invalid_value == std::numeric_limits<std::uint64_t>::max());
    STATIC_REQUIRE(invalid.value() == RegionId::invalid_value);
    STATIC_REQUIRE_FALSE(invalid.is_valid());
    STATIC_REQUIRE(valid.is_valid());
}

TEST_CASE("stable IDs order by their numeric value") {
    const auto first = RegionId::from_value(3);
    const auto second = RegionId::from_value(7);

    REQUIRE(first < second);
    REQUIRE(second > first);
    REQUIRE(first == RegionId::from_value(3));
}

TEST_CASE("stable IDs work as unordered container keys") {
    std::unordered_set<LocationId> locations;

    locations.insert(LocationId::from_value(8));
    locations.insert(LocationId::from_value(12));
    locations.insert(LocationId::from_value(8));

    REQUIRE(locations.size() == 2);
    REQUIRE(locations.contains(LocationId::from_value(8)));
    REQUIRE_FALSE(locations.contains(LocationId::from_value(9)));
}

TEST_CASE("stable ID allocators issue a deterministic sequence per ID type") {
    StableIdAllocator<RegionId> first_allocator;
    StableIdAllocator<RegionId> same_sequence_allocator;
    StableIdAllocator<SettlementId> settlement_allocator;

    REQUIRE(first_allocator.allocate() == RegionId::from_value(0));
    REQUIRE(first_allocator.allocate() == RegionId::from_value(1));
    REQUIRE(first_allocator.allocate() == RegionId::from_value(2));
    REQUIRE(same_sequence_allocator.allocate() == RegionId::from_value(0));
    REQUIRE(settlement_allocator.allocate() == SettlementId::from_value(0));
}

TEST_CASE("stable ID allocator never emits the invalid sentinel") {
    StableIdAllocator<RegionId> allocator{RegionId::invalid_value - 1};

    REQUIRE(allocator.allocate() == RegionId::from_value(RegionId::invalid_value - 1));
    REQUIRE_THROWS_AS(allocator.allocate(), std::overflow_error);
}
