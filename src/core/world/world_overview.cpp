#include "core/world/world_overview.hpp"

#include <sstream>

namespace fantasy::core {

namespace {

template <typename Id>
std::string label(const char* kind, Id id, const PlaceName& name) {
    std::ostringstream out;
    out << kind << ' ' << id.value();
    if (name) {
        out << " \"" << *name << '"';
    }
    return out.str();
}

} // namespace

std::string describe_world(const World& world) {
    std::ostringstream out;

    for (const auto& [region_id, region] : world.regions()) {
        out << label("Region", region_id, region.name()) << '\n';

        for (const auto& [settlement_id, settlement] : region.settlements()) {
            out << "  " << label("Settlement", settlement_id, settlement.name) << '\n';
            for (const LocationId id : world.locations_of(settlement_id)) {
                out << "    " << label("Location", id, world.find_location(id)->name) << '\n';
            }
        }

        bool header = false;
        for (const auto& [location_id, location] : region.locations()) {
            if (location.settlement) {
                continue;
            }
            if (!header) {
                out << "  Outside settlements\n";
                header = true;
            }
            out << "    " << label("Location", location_id, location.name) << '\n';
        }

        out << "  Connections\n";
        for (const auto& [from, location] : region.locations()) {
            for (const auto& [to, cost] : world.neighbors(from)) {
                if (from < to) {
                    out << "    " << from.value() << " <-> " << to.value() << " : " << cost
                        << " world-seconds\n";
                }
            }
        }
    }
    return out.str();
}

} // namespace fantasy::core
