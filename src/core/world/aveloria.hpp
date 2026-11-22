#pragma once

#include "core/simulation/stable_id.hpp"
#include "core/world/world.hpp"

namespace fantasy::core {

// The compiled-in Aveloria example world (plan 0003, task #33) together with
// the IDs of its parts. Settlement membership of locations is recorded in the
// World (World::locations_of / settlement_of).
//
// Names are literal strings following decision 0009 (region Aveloria, villages
// Marale and Brelin); location names are authored content, listed in the
// README. IDs are fixed constants, so every build is identical.
struct AveloriaWorld {
    World world;

    RegionId region;
    SettlementId marale;
    SettlementId brelin;

    // The road location between the two villages (belongs to no settlement).
    LocationId road;
};

// Builds the Aveloria example world. Pure and deterministic: no seed, clock,
// or I/O is involved, and every call returns an identical world.
[[nodiscard]] AveloriaWorld build_aveloria_world();

} // namespace fantasy::core
