#include "core/world/pathfinding.hpp"

#include <catch2/catch_test_macros.hpp>

#include <vector>

using namespace fantasy::core;

namespace {
LocationId L(std::uint64_t v) { return LocationId::from_value(v); }

World make_world(std::uint64_t locations) {
    World world;
    world.add_region(RegionId::from_value(1));
    for (std::uint64_t i = 1; i <= locations; ++i) {
        world.add_location(RegionId::from_value(1), L(i));
    }
    return world;
}

using Path = std::vector<LocationId>;
} // namespace

TEST_CASE("shortest path prefers lower total cost over fewer hops") {
    World world = make_world(4);
    world.connect(L(1), L(4), 100);
    world.connect(L(1), L(2), 10);
    world.connect(L(2), L(3), 10);
    world.connect(L(3), L(4), 10);

    const PathResult result = find_path(world, L(1), L(4));
    CHECK(result.status == PathStatus::found);
    CHECK(result.cost == 30);
    CHECK(result.path == Path{L(1), L(2), L(3), L(4)});

    // Connections are undirected: the reverse query has the same cost.
    const PathResult back = find_path(world, L(4), L(1));
    CHECK(back.cost == 30);
    CHECK(back.path == Path{L(4), L(3), L(2), L(1)});
}

TEST_CASE("equal-cost alternatives resolve to the lexicographically lowest ID path") {
    World world = make_world(5);
    // 1 -> {2,3,4} -> 5, all total cost 20.
    for (std::uint64_t mid : {4u, 2u, 3u}) {
        world.connect(L(1), L(mid), 10);
        world.connect(L(mid), L(5), 10);
    }
    const PathResult result = find_path(world, L(1), L(5));
    CHECK(result.status == PathStatus::found);
    CHECK(result.cost == 20);
    CHECK(result.path == Path{L(1), L(2), L(5)});
}

TEST_CASE("tie-break is independent of connection insertion order") {
    World a = make_world(4);
    a.connect(L(1), L(2), 5);
    a.connect(L(2), L(4), 5);
    a.connect(L(1), L(3), 5);
    a.connect(L(3), L(4), 5);

    World b = make_world(4);
    b.connect(L(3), L(4), 5);
    b.connect(L(1), L(3), 5);
    b.connect(L(4), L(2), 5);
    b.connect(L(2), L(1), 5);

    CHECK(find_path(a, L(1), L(4)).path == Path{L(1), L(2), L(4)});
    CHECK(find_path(b, L(1), L(4)).path == Path{L(1), L(2), L(4)});
}

TEST_CASE("same-location query returns an explicit zero-cost result") {
    World world = make_world(2);
    const PathResult result = find_path(world, L(1), L(1));
    CHECK(result.status == PathStatus::same_location);
    CHECK(result.cost == 0);
    CHECK(result.path == Path{L(1)});
}

TEST_CASE("unreachable and unknown locations return explicit results") {
    World world = make_world(4);
    world.connect(L(1), L(2), 5);
    world.connect(L(3), L(4), 5);

    const PathResult unreachable = find_path(world, L(1), L(4));
    CHECK(unreachable.status == PathStatus::unreachable);
    CHECK(unreachable.path.empty());
    CHECK(unreachable.cost == 0);

    CHECK(find_path(world, L(1), L(99)).status == PathStatus::unknown_location);
    CHECK(find_path(world, L(99), L(1)).status == PathStatus::unknown_location);
    CHECK(find_path(world, LocationId{}, L(1)).status == PathStatus::unknown_location);
}

TEST_CASE("repeated identical queries return identical results") {
    World world = make_world(6);
    world.connect(L(1), L(2), 3);
    world.connect(L(1), L(3), 3);
    world.connect(L(2), L(4), 3);
    world.connect(L(3), L(4), 3);
    world.connect(L(4), L(5), 2);
    world.connect(L(4), L(6), 2);
    world.connect(L(5), L(6), 1);

    const PathResult first = find_path(world, L(1), L(6));
    for (int i = 0; i < 20; ++i) {
        const PathResult again = find_path(world, L(1), L(6));
        CHECK(again.status == first.status);
        CHECK(again.cost == first.cost);
        CHECK(again.path == first.path);
    }
    CHECK(first.cost == 8);
    CHECK(first.path == Path{L(1), L(2), L(4), L(6)});
}
