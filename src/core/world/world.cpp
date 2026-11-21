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

AddResult World::add_location(RegionId region, LocationId id, PlaceName name) {
    if (!id.is_valid()) return AddResult::invalid_id;
    const auto parent = regions_.find(region);
    if (parent == regions_.end()) return AddResult::unknown_region;
    if (name_is_empty(name)) return AddResult::empty_name;
    if (location_regions_.contains(id)) return AddResult::duplicate_id;
    parent->second.locations_.emplace(id, Location{id, std::move(name)});
    location_regions_.emplace(id, region);
    return AddResult::added;
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

RegionId World::region_of(SettlementId id) const noexcept {
    const auto it = settlement_regions_.find(id);
    return it == settlement_regions_.end() ? RegionId{} : it->second;
}

RegionId World::region_of(LocationId id) const noexcept {
    const auto it = location_regions_.find(id);
    return it == location_regions_.end() ? RegionId{} : it->second;
}

} // namespace fantasy::core
