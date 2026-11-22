#include "core/world/aveloria.hpp"

#include <cstdint>

namespace fantasy::core {

namespace {
LocationId loc(std::uint64_t value) { return LocationId::from_value(value); }
} // namespace

AveloriaWorld build_aveloria_world() {
    AveloriaWorld a;

    a.region = RegionId::from_value(1);
    a.marale = SettlementId::from_value(1);
    a.brelin = SettlementId::from_value(2);

    World& w = a.world;
    w.add_region(a.region, "Aveloria");
    w.add_settlement(a.region, a.marale, "Marale");
    w.add_settlement(a.region, a.brelin, "Brelin");

    // Marale: 1 homes, 2 fields, 3 market. Road: 4. Brelin: 5 homes, 6 fields, 7 market.
    const LocationId marale_homes = loc(1);
    const LocationId marale_fields = loc(2);
    const LocationId marale_market = loc(3);
    a.road = loc(4);
    const LocationId brelin_homes = loc(5);
    const LocationId brelin_fields = loc(6);
    const LocationId brelin_market = loc(7);

    w.add_location(a.region, marale_homes, "Marale Homes", a.marale);
    w.add_location(a.region, marale_fields, "Marale Fields", a.marale);
    w.add_location(a.region, marale_market, "Marale Market", a.marale);
    w.add_location(a.region, a.road, "Aveloria Road");
    w.add_location(a.region, brelin_homes, "Brelin Homes", a.brelin);
    w.add_location(a.region, brelin_fields, "Brelin Fields", a.brelin);
    w.add_location(a.region, brelin_market, "Brelin Market", a.brelin);

    // Travel costs are walking time in world-seconds (decision 0003).
    w.connect(marale_homes, marale_market, 300);
    w.connect(marale_homes, marale_fields, 600);
    w.connect(marale_fields, marale_market, 900);
    w.connect(marale_market, a.road, 1200);
    w.connect(a.road, brelin_market, 1500);
    w.connect(brelin_market, brelin_homes, 360);
    w.connect(brelin_homes, brelin_fields, 540);
    w.connect(brelin_fields, brelin_market, 780);

    return a;
}

} // namespace fantasy::core
