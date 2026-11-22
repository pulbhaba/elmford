#pragma once

#include "core/world/world.hpp"

#include <string>

namespace fantasy::core {

// Plain-text overview of a world (plan 0003, task #34):
// regions, settlements with their locations, locations outside any settlement,
// and connections, using the world's recorded settlement membership. Order is by ascending ID throughout, so the text is
// identical on every call. Pure: no I/O, clock, or randomness.
[[nodiscard]] std::string describe_world(const World& world);

} // namespace fantasy::core
