#include "core/world/aveloria.hpp"
#include "core/world/pathfinding.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace fantasy::core;

namespace {
bool same_world(const AveloriaWorld& x, const AveloriaWorld& y) {
    if (x.world.regions().size() != y.world.regions().size()) return false;
    auto rx = x.world.regions().begin();
    auto ry = y.world.regions().begin();
    for (; rx != x.world.regions().end(); ++rx, ++ry) {
        const Region& a = rx->second;
        const Region& b = ry->second;
        if (a.id() != b.id() || a.name() != b.name()) return false;
        if (a.settlements().size() != b.settlements().size()) return false;
        if (a.locations().size() != b.locations().size()) return false;
        for (auto i = a.settlements().begin(), j = b.settlements().begin();
             i != a.settlements().end(); ++i, ++j) {
            if (i->first != j->first || i->second.name != j->second.name) return false;
        }
        for (auto i = a.locations().begin(), j = b.locations().begin(); i != a.locations().end();
             ++i, ++j) {
            if (i->first != j->first || i->second.name != j->second.name) return false;
            if (x.world.neighbors(i->first) != y.world.neighbors(j->first)) return false;
        }
    }
    return true;
}
} // namespace

TEST_CASE("Aveloria has the named region and villages") {
    const AveloriaWorld a = build_aveloria_world();
    const Region* region = a.world.find_region(a.region);
    REQUIRE(region != nullptr);
    CHECK(region->name() == "Aveloria");
    CHECK(region->settlements().size() == 2);
    REQUIRE(a.world.find_settlement(a.marale) != nullptr);
    REQUIRE(a.world.find_settlement(a.brelin) != nullptr);
    CHECK(a.world.find_settlement(a.marale)->name == "Marale");
    CHECK(a.world.find_settlement(a.brelin)->name == "Brelin");
    CHECK(a.world.region_of(a.marale) == a.region);
    CHECK(a.world.region_of(a.brelin) == a.region);
}

TEST_CASE("each village has multiple connected locations") {
    const AveloriaWorld a = build_aveloria_world();
    for (const SettlementId village : {a.marale, a.brelin}) {
        const auto* group = &a.world.locations_of(village);
        REQUIRE(group->size() > 1);
        for (LocationId id : *group) {
            CHECK(a.world.settlement_of(id) == village);
            const Location* location = a.world.find_location(id);
            REQUIRE(location != nullptr);
            CHECK(location->name.has_value());
            CHECK(a.world.region_of(id) == a.region);
        }
        // Every location of the village reaches the first one without leaving it.
        for (LocationId id : *group) {
            const PathResult r = find_path(a.world, *group->begin(), id);
            REQUIRE((r.status == PathStatus::found || r.status == PathStatus::same_location));
            for (LocationId step : r.path) {
                CHECK(a.world.find_location(step)->name.has_value());
            }
        }
    }
}

TEST_CASE("settlement membership is recorded in the world; the road is unaffiliated") {
    const AveloriaWorld a = build_aveloria_world();
    CHECK(a.world.locations_of(a.marale).size() == 3);
    CHECK(a.world.locations_of(a.brelin).size() == 3);
    CHECK(a.world.settlement_of(a.road) == std::nullopt);
    for (LocationId id : a.world.locations_of(a.marale)) {
        CHECK(a.world.find_location(id)->name->rfind("Marale", 0) == 0);
    }
    for (LocationId id : a.world.locations_of(a.brelin)) {
        CHECK(a.world.find_location(id)->name->rfind("Brelin", 0) == 0);
    }
}

TEST_CASE("the villages are connected to each other by the road") {
    const AveloriaWorld a = build_aveloria_world();
    REQUIRE(a.world.find_location(a.road) != nullptr);
    CHECK(a.world.are_connected(*a.world.locations_of(a.marale).rbegin(), a.road));
    CHECK(a.world.are_connected(a.road, *a.world.locations_of(a.brelin).rbegin()));
}

TEST_CASE("a path exists between a location in each settlement") {
    const AveloriaWorld a = build_aveloria_world();
    const LocationId from = *a.world.locations_of(a.marale).begin();
    const LocationId to = *a.world.locations_of(a.brelin).begin();

    const PathResult r = find_path(a.world, from, to);
    REQUIRE(r.status == PathStatus::found);
    CHECK(r.path.front() == from);
    CHECK(r.path.back() == to);
    CHECK(r.cost > 0);

    bool via_road = false;
    for (LocationId id : r.path) via_road = via_road || id == a.road;
    CHECK(via_road);

    const PathResult back = find_path(a.world, to, from);
    REQUIRE(back.status == PathStatus::found);
    CHECK(back.cost == r.cost);
}

TEST_CASE("the builder returns the same world every call") {
    const AveloriaWorld first = build_aveloria_world();
    const AveloriaWorld second = build_aveloria_world();
    CHECK(same_world(first, second));
    CHECK(first.world.locations_of(first.marale) == second.world.locations_of(second.marale));
    CHECK(first.world.locations_of(first.brelin) == second.world.locations_of(second.brelin));
    CHECK(first.road == second.road);
    CHECK(find_path(first.world, *first.world.locations_of(first.marale).begin(), *first.world.locations_of(first.brelin).begin())
              .path ==
          find_path(second.world, *second.world.locations_of(second.marale).begin(), *second.world.locations_of(second.brelin).begin())
              .path);
}
