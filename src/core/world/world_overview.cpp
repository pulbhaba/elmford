#include "core/world/world_overview.hpp"

#include <algorithm>
#include <set>
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

std::string describe_world(const AveloriaWorld& aveloria) {
    const World& world = aveloria.world;
    std::ostringstream out;

    for (const auto& [region_id, region] : world.regions()) {
        out << label("Region", region_id, region.name()) << '\n';

        std::set<LocationId> grouped;
        for (const auto& [settlement_id, settlement] : region.settlements()) {
            out << "  " << label("Settlement", settlement_id, settlement.name) << '\n';
            const auto& ids = settlement_id == aveloria.marale ? aveloria.marale_locations
                              : settlement_id == aveloria.brelin ? aveloria.brelin_locations
                                                                 : std::vector<LocationId>{};
            std::vector<LocationId> sorted{ids};
            std::sort(sorted.begin(), sorted.end());
            for (const LocationId id : sorted) {
                grouped.insert(id);
                if (const Location* location = world.find_location(id)) {
                    out << "    " << label("Location", id, location->name) << '\n';
                }
            }
        }

        bool header = false;
        for (const auto& [location_id, location] : region.locations()) {
            if (grouped.count(location_id) != 0) {
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
