#pragma once

#include "core/simulation/stable_id.hpp"
#include "core/world/world.hpp"

#include <vector>

namespace fantasy::core {

// The compiled-in Aveloria example world (plan 0003, task #33) together with
// the IDs of its parts. World does not yet record which settlement a location
// belongs to, so the builder reports that grouping here.
//
// Names are literal strings following decision 0009 (region Aveloria, villages
// Marale and Brelin); location names are authored content, listed in the
// README. IDs are fixed constants, so every build is identical.
struct AveloriaWorld {
    World world;

    RegionId region;
    SettlementId marale;
    SettlementId brelin;

    // Locations of each village, in ascending ID order: homes, fields, market.
    std::vector<LocationId> marale_locations;
    std::vector<LocationId> brelin_locations;

    // The road location between the two villages (belongs to neither).
    LocationId road;
};

// Builds the Aveloria example world. Pure and deterministic: no seed, clock,
// or I/O is involved, and every call returns an identical world.
[[nodiscard]] AveloriaWorld build_aveloria_world();

} // namespace fantasy::core
