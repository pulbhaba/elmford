#include "core/world/world_overview.hpp"

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace fantasy::core;

TEST_CASE("world overview is stable and lists the Aveloria parts", "[world][cli]") {
    const std::string first = describe_world(build_aveloria_world());
    REQUIRE(first == describe_world(build_aveloria_world()));
    REQUIRE(first.find("\"Aveloria\"") != std::string::npos);
    REQUIRE(first.find("\"Marale\"") != std::string::npos);
    REQUIRE(first.find("\"Brelin\"") != std::string::npos);
    REQUIRE(first.find("Connections") != std::string::npos);
}
