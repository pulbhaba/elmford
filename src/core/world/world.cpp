#include "core/world/world.hpp"

#include <utility>

namespace fantasy::core {

namespace {

bool name_is_empty(const PlaceName& name) {
    return name.has_value() && name->empty();
}

} // namespace

AddResult World::add_region(RegionId id, PlaceName name) {
    if (!id.is_valid()) return AddResult::invalid_id;
    if (name_is_empty(name)) return AddResult::empty_name;
    if (regions_.contains(id)) return AddResult::duplicate_id;
    regions_.emplace(id, Region{id, std::move(name)});
    return AddResult::added;
}

AddResult World::add_settlement(RegionId region, SettlementId id, PlaceName name) {
    if (!id.is_valid()) return AddResult::invalid_id;
    const auto parent = regions_.find(region);
    if (parent == regions_.end()) return AddResult::unknown_region;
    if (name_is_empty(name)) return AddResult::empty_name;
    if (settlement_regions_.contains(id)) return AddResult::duplicate_id;
    parent->second.settlements_.emplace(id, Settlement{id, std::move(name)});
    settlement_regions_.emplace(id, region);
    return AddResult::added;
}

AddResult World::add_location(RegionId region, LocationId id, PlaceName name,
                              std::optional<SettlementId> settlement) {
    if (!id.is_valid()) return AddResult::invalid_id;
    const auto parent = regions_.find(region);
    if (parent == regions_.end()) return AddResult::unknown_region;
    if (name_is_empty(name)) return AddResult::empty_name;
    if (location_regions_.contains(id)) return AddResult::duplicate_id;
    if (settlement) {
        const auto owner = settlement_regions_.find(*settlement);
        if (owner == settlement_regions_.end()) return AddResult::unknown_settlement;
        if (owner->second != region) return AddResult::settlement_region_mismatch;
    }
    parent->second.locations_.emplace(id, Location{id, std::move(name), settlement});
    location_regions_.emplace(id, region);
    if (settlement) settlement_locations_[*settlement].insert(id);
    return AddResult::added;
}

ConnectResult World::connect(LocationId a, LocationId b, TravelCost cost) {
    if (!location_regions_.contains(a) || !location_regions_.contains(b)) {
        return ConnectResult::unknown_location;
    }
    if (a == b) return ConnectResult::self_connection;
    if (cost <= 0) return ConnectResult::non_positive_cost;
    if (are_connected(a, b)) return ConnectResult::duplicate;
    connections_[a].emplace(b, cost);
    connections_[b].emplace(a, cost);
    return ConnectResult::connected;
}

bool World::are_connected(LocationId a, LocationId b) const noexcept {
    return travel_cost(a, b).has_value();
}

std::optional<TravelCost> World::travel_cost(LocationId a, LocationId b) const noexcept {
    const auto from = connections_.find(a);
    if (from == connections_.end()) return std::nullopt;
    const auto to = from->second.find(b);
    if (to == from->second.end()) return std::nullopt;
    return to->second;
}

const std::map<LocationId, TravelCost>& World::neighbors(LocationId id) const noexcept {
    static const std::map<LocationId, TravelCost> none;
    const auto it = connections_.find(id);
    return it == connections_.end() ? none : it->second;
}

const Region* World::find_region(RegionId id) const noexcept {
    const auto it = regions_.find(id);
    return it == regions_.end() ? nullptr : &it->second;
}

const Settlement* World::find_settlement(SettlementId id) const noexcept {
    const auto owner = settlement_regions_.find(id);
    if (owner == settlement_regions_.end()) return nullptr;
    const auto& settlements = regions_.at(owner->second).settlements();
    return &settlements.at(id);
}

const Location* World::find_location(LocationId id) const noexcept {
    const auto owner = location_regions_.find(id);
    if (owner == location_regions_.end()) return nullptr;
    const auto& locations = regions_.at(owner->second).locations();
    return &locations.at(id);
}

std::optional<SettlementId> World::settlement_of(LocationId id) const noexcept {
    const Location* location = find_location(id);
    return location ? location->settlement : std::nullopt;
}

const std::set<LocationId>& World::locations_of(SettlementId id) const noexcept {
    static const std::set<LocationId> none;
    const auto it = settlement_locations_.find(id);
    return it == settlement_locations_.end() ? none : it->second;
}

RegionId World::region_of(SettlementId id) const noexcept {
    const auto it = settlement_regions_.find(id);
    return it == settlement_regions_.end() ? RegionId{} : it->second;
}

RegionId World::region_of(LocationId id) const noexcept {
    const auto it = location_regions_.find(id);
    return it == location_regions_.end() ? RegionId{} : it->second;
}

} // namespace fantasy::core
