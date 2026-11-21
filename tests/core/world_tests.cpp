#include "core/world/world.hpp"

#include <catch2/catch_test_macros.hpp>

#include <vector>

using namespace fantasy::core;

namespace {
RegionId R(std::uint64_t v) { return RegionId::from_value(v); }
SettlementId S(std::uint64_t v) { return SettlementId::from_value(v); }
LocationId L(std::uint64_t v) { return LocationId::from_value(v); }
} // namespace

TEST_CASE("a new world is empty") {
    World world;
    CHECK(world.regions().empty());
    CHECK(world.find_region(R(0)) == nullptr);
    CHECK(world.find_settlement(S(0)) == nullptr);
    CHECK(world.find_location(L(0)) == nullptr);
}

TEST_CASE("regions own settlements and locations, found by ID") {
    World world;
    REQUIRE(world.add_region(R(1), "Aveloria") == AddResult::added);
    REQUIRE(world.add_settlement(R(1), S(10), "Marale") == AddResult::added);
    REQUIRE(world.add_location(R(1), L(20), "Market") == AddResult::added);

    const Region* region = world.find_region(R(1));
    REQUIRE(region != nullptr);
    CHECK(region->id() == R(1));
    CHECK(region->name() == "Aveloria");
    CHECK(region->settlements().size() == 1);
    CHECK(region->locations().size() == 1);

    const Settlement* settlement = world.find_settlement(S(10));
    REQUIRE(settlement != nullptr);
    CHECK(settlement->name == "Marale");
    CHECK(world.region_of(S(10)) == R(1));

    const Location* location = world.find_location(L(20));
    REQUIRE(location != nullptr);
    CHECK(location->name == "Market");
    CHECK(world.region_of(L(20)) == R(1));

    CHECK(world.find_settlement(S(11)) == nullptr);
    CHECK_FALSE(world.region_of(S(11)).is_valid());
    CHECK_FALSE(world.region_of(L(21)).is_valid());
}

TEST_CASE("places may be unnamed and are identified by ID alone") {
    World world;
    REQUIRE(world.add_region(R(0)) == AddResult::added);
    REQUIRE(world.add_settlement(R(0), S(0)) == AddResult::added);
    REQUIRE(world.add_location(R(0), L(0)) == AddResult::added);
    CHECK_FALSE(world.find_region(R(0))->name().has_value());
    CHECK_FALSE(world.find_settlement(S(0))->name.has_value());
    CHECK_FALSE(world.find_location(L(0))->name.has_value());
}

TEST_CASE("iteration is by ascending ID regardless of insertion order") {
    World world;
    for (auto v : {5, 1, 3}) REQUIRE(world.add_region(R(v)) == AddResult::added);
    for (auto v : {9, 2, 7}) REQUIRE(world.add_settlement(R(3), S(v)) == AddResult::added);
    for (auto v : {8, 4, 6}) REQUIRE(world.add_location(R(3), L(v)) == AddResult::added);

    std::vector<std::uint64_t> regions, settlements, locations;
    for (const auto& [id, region] : world.regions()) regions.push_back(id.value());
    for (const auto& [id, s] : world.find_region(R(3))->settlements()) settlements.push_back(id.value());
    for (const auto& [id, l] : world.find_region(R(3))->locations()) locations.push_back(id.value());

    CHECK(regions == std::vector<std::uint64_t>{1, 3, 5});
    CHECK(settlements == std::vector<std::uint64_t>{2, 7, 9});
    CHECK(locations == std::vector<std::uint64_t>{4, 6, 8});
}

TEST_CASE("entities with unknown parents are rejected and change nothing") {
    World world;
    CHECK(world.add_settlement(R(1), S(1)) == AddResult::unknown_region);
    CHECK(world.add_location(R(1), L(1)) == AddResult::unknown_region);
    CHECK(world.find_settlement(S(1)) == nullptr);
    CHECK(world.find_location(L(1)) == nullptr);
    CHECK(world.regions().empty());
}

TEST_CASE("duplicate, invalid, and empty-name additions are rejected") {
    World world;
    REQUIRE(world.add_region(R(1), "Aveloria") == AddResult::added);
    REQUIRE(world.add_region(R(2)) == AddResult::added);
    REQUIRE(world.add_settlement(R(1), S(1), "Marale") == AddResult::added);
    REQUIRE(world.add_location(R(1), L(1)) == AddResult::added);

    CHECK(world.add_region(R(1), "Other") == AddResult::duplicate_id);
    CHECK(world.find_region(R(1))->name() == "Aveloria");
    // IDs are unique world-wide, not per region.
    CHECK(world.add_settlement(R(2), S(1)) == AddResult::duplicate_id);
    CHECK(world.add_location(R(2), L(1)) == AddResult::duplicate_id);
    CHECK(world.region_of(S(1)) == R(1));
    CHECK(world.find_region(R(2))->settlements().empty());

    CHECK(world.add_region(RegionId{}) == AddResult::invalid_id);
    CHECK(world.add_settlement(R(1), SettlementId{}) == AddResult::invalid_id);
    CHECK(world.add_location(R(1), LocationId{}) == AddResult::invalid_id);

    CHECK(world.add_region(R(3), "") == AddResult::empty_name);
    CHECK(world.add_settlement(R(1), S(2), "") == AddResult::empty_name);
    CHECK(world.add_location(R(1), L(2), "") == AddResult::empty_name);
    CHECK(world.find_region(R(3)) == nullptr);
    CHECK(world.find_settlement(S(2)) == nullptr);
    CHECK(world.find_location(L(2)) == nullptr);
}
