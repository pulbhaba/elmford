#pragma once

#include "core/simulation/stable_id.hpp"

#include <cstdint>
#include <map>
#include <optional>
#include <string>

namespace fantasy::core {

// Outcome of adding an entity to a World. Anything other than `added` leaves
// the world unchanged.
enum class AddResult {
    added,
    invalid_id,     // the supplied ID is the invalid sentinel
    duplicate_id,   // an entity of that type already has this ID
    unknown_region, // the parent region does not exist
    empty_name,     // a name was supplied but is empty
};

// Outcome of connecting two locations. Anything other than `connected` leaves
// the world unchanged.
enum class ConnectResult {
    connected,
    unknown_location,  // an endpoint is invalid or not in the world
    self_connection,   // both endpoints are the same location
    duplicate,         // the two locations are already connected
    non_positive_cost, // the travel cost is zero or negative
};

// Travel cost of a connection, in world-seconds (decision 0003): the time an
// inhabitant needs to walk between the two locations. Always positive.
using TravelCost = std::int64_t;

// Names are optional: an unnamed place is identified by its ID alone
// (decision 0009). A supplied name must be non-empty.
using PlaceName = std::optional<std::string>;

struct Settlement {
    SettlementId id;
    PlaceName name;
};

struct Location {
    LocationId id;
    PlaceName name;
};

// A region owns its settlements and locations. Iteration is by ascending ID.
class Region {
public:
    [[nodiscard]] RegionId id() const noexcept { return id_; }
    [[nodiscard]] const PlaceName& name() const noexcept { return name_; }

    [[nodiscard]] const std::map<SettlementId, Settlement>& settlements() const noexcept {
        return settlements_;
    }
    [[nodiscard]] const std::map<LocationId, Location>& locations() const noexcept {
        return locations_;
    }

private:
    friend class World;

    Region(RegionId id, PlaceName name) : id_{id}, name_{std::move(name)} {}

    RegionId id_;
    PlaceName name_;
    std::map<SettlementId, Settlement> settlements_;
    std::map<LocationId, Location> locations_;
};

// Domain state for a world: regions containing settlements and locations.
// Holds no presentation data. Entities are never removed or moved, so
// pointers returned by find_* stay valid for the life of the world.
//
// Lookups: find_* return nullptr when the ID is unknown or invalid.
// Ordering: regions(), and each region's settlements() and locations(), are
// ordered by ascending ID value (std::map ordering of the typed ID), never by
// insertion order. IDs of each type are unique across the whole world, so a
// settlement or location ID resolves without naming its region.
class World {
public:
    AddResult add_region(RegionId id, PlaceName name = std::nullopt);
    AddResult add_settlement(RegionId region, SettlementId id, PlaceName name = std::nullopt);
    AddResult add_location(RegionId region, LocationId id, PlaceName name = std::nullopt);

    [[nodiscard]] const Region* find_region(RegionId id) const noexcept;
    [[nodiscard]] const Settlement* find_settlement(SettlementId id) const noexcept;
    [[nodiscard]] const Location* find_location(LocationId id) const noexcept;

    // Connects two existing locations (in any regions) with a positive travel
    // cost in world-seconds. A connection is undirected: it is traversable
    // both ways at the same cost, and connecting (a, b) then (b, a) is a
    // duplicate. The navigation graph is owned by the world.
    ConnectResult connect(LocationId a, LocationId b, TravelCost cost);

    [[nodiscard]] bool are_connected(LocationId a, LocationId b) const noexcept;

    // Travel cost in world-seconds between two connected locations, or
    // nullopt when they are not connected.
    [[nodiscard]] std::optional<TravelCost> travel_cost(LocationId a, LocationId b) const noexcept;

    // Neighbors of a location with their travel costs, ordered by ascending
    // destination ID. Empty for an unknown or unconnected location.
    [[nodiscard]] const std::map<LocationId, TravelCost>& neighbors(LocationId id) const noexcept;

    // Owning region of a settlement or location; invalid ID when unknown.
    [[nodiscard]] RegionId region_of(SettlementId id) const noexcept;
    [[nodiscard]] RegionId region_of(LocationId id) const noexcept;

    [[nodiscard]] const std::map<RegionId, Region>& regions() const noexcept { return regions_; }

private:
    std::map<RegionId, Region> regions_;
    std::map<SettlementId, RegionId> settlement_regions_;
    std::map<LocationId, RegionId> location_regions_;
    std::map<LocationId, std::map<LocationId, TravelCost>> connections_;
};

} // namespace fantasy::core
