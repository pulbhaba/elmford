#pragma once

#include "core/simulation/stable_id.hpp"
#include "core/world/world.hpp"

#include <vector>

namespace fantasy::core {

enum class PathStatus {
    found,            // `path` and `cost` describe a shortest route
    same_location,    // start equals goal: path is {start}, cost is 0
    unreachable,      // both locations exist but no route connects them
    unknown_location, // start or goal is invalid or not in the world
};

struct PathResult {
    PathStatus status = PathStatus::unknown_location;
    std::vector<LocationId> path; // start..goal inclusive; empty unless found/same_location
    TravelCost cost = 0;          // total world-seconds; 0 unless found
};

// Shortest path over the world's undirected location graph (decision 0005).
// Dijkstra with integer world-second costs; no floating point.
//
// Tie-break rule: among all minimum-cost paths, the result is the one whose
// sequence of location IDs, read from start to goal, is lexicographically
// smallest (at every step the lowest-ID neighbor that stays on a shortest
// route is taken). The result depends only on the world state and the query.
[[nodiscard]] PathResult find_path(const World& world, LocationId start, LocationId goal);

} // namespace fantasy::core
