#include "core/logging/logger.hpp"
#include "core/time/simulation_clock.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <sstream>

using namespace std::chrono_literals;

TEST_CASE("logger writes levels and an optional simulation timestamp") {
    std::ostringstream output;
    fantasy::core::SimulationClock clock{2ms};
    fantasy::core::Logger logger{output, &clock};

    logger.info("started");
    clock.advance();
    logger.warning("slow update");
    logger.error("failed");

    REQUIRE(output.str() == "[INFO] [t=0ns] started\n"
                            "[WARNING] [t=2000000ns] slow update\n"
                            "[ERROR] [t=2000000ns] failed\n");
}

TEST_CASE("logger can write without a simulation timestamp") {
    std::ostringstream output;
    fantasy::core::Logger logger{output};

    logger.info("ready");

    REQUIRE(output.str() == "[INFO] ready\n");
}
