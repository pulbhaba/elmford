#include "core/world/pathfinding.hpp"

#include <functional>
#include <map>
#include <queue>
#include <utility>

namespace fantasy::core {

PathResult find_path(const World& world, LocationId start, LocationId goal) {
    PathResult result;
    if (world.find_location(start) == nullptr || world.find_location(goal) == nullptr) {
        return result;
    }
    if (start == goal) {
        result.status = PathStatus::same_location;
        result.path = {start};
        return result;
    }

    // Dijkstra from the goal. Connections are undirected, so distance-to-goal
    // equals distance-from-goal, and the path is then rebuilt forward from the
    // start by always taking the lowest-ID neighbor that stays on a shortest
    // route. The queue orders by (cost, ID) so settling order is stable.
    using Entry = std::pair<TravelCost, LocationId>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<>> queue;
    std::map<LocationId, TravelCost> dist;
    dist[goal] = 0;
    queue.emplace(0, goal);
    while (!queue.empty()) {
        const auto [d, node] = queue.top();
        queue.pop();
        if (d > dist[node]) {
            continue;
        }
        if (node == start) {
            break;
        }
        for (const auto& [next, cost] : world.neighbors(node)) {
            const TravelCost candidate = d + cost;
            const auto it = dist.find(next);
            if (it == dist.end() || candidate < it->second) {
                dist[next] = candidate;
                queue.emplace(candidate, next);
            }
        }
    }

    const auto start_it = dist.find(start);
    if (start_it == dist.end()) {
        result.status = PathStatus::unreachable;
        return result;
    }

    result.status = PathStatus::found;
    result.cost = start_it->second;
    LocationId current = start;
    result.path.push_back(current);
    while (current != goal) {
        const TravelCost remaining = dist.at(current);
        for (const auto& [next, cost] : world.neighbors(current)) {
            const auto it = dist.find(next);
            // Only nodes settled before the search stopped have final
            // distances; any node on a shortest route to start is settled.
            if (it != dist.end() && cost + it->second == remaining) {
                current = next;
                break;
            }
        }
        result.path.push_back(current);
    }
    return result;
}

} // namespace fantasy::core
