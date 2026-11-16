#include "core/simulation/simulation_runner.hpp"
#include "core/simulation/tick_scheduler.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

using namespace std::chrono_literals;
using fantasy::core::SimulationSystem;
using fantasy::core::TickPhase;

namespace {

fantasy::core::SimulationConfig test_config() {
    fantasy::core::SimulationConfig config;
    config.fixed_step = 1s;
    config.cadences.needs = 2s;
    config.cadences.lifecycle = 4s;
    return config;
}

std::vector<std::string> run_ticks(std::uint64_t ticks) {
    const auto config = test_config();
    fantasy::core::SimulationRunner runner{config};
    std::vector<std::string> log;
    auto& scheduler = runner.scheduler();
    // Registered out of phase order on purpose.
    scheduler.register_system(TickPhase::lifecycle,
        [&log](const auto& clock, auto&) { log.push_back("life" + std::to_string(clock.tick_index())); },
        SimulationSystem::lifecycle);
    scheduler.register_system(TickPhase::needs,
        [&log](const auto&, auto&) { log.push_back("needsA"); }, SimulationSystem::needs);
    scheduler.register_system(TickPhase::needs,
        [&log](const auto&, auto&) { log.push_back("needsB"); }, SimulationSystem::needs);
    scheduler.register_system(TickPhase::commands, [&log](const auto&, auto&) { log.push_back("cmd"); });
    scheduler.register_system(TickPhase::resolution, [&log](const auto&, auto&) { log.push_back("res"); });
    scheduler.register_system(TickPhase::activities, [&log](const auto&, auto&) { log.push_back("act"); });
    runner.run(ticks, [&log](const auto&, auto&) { log.push_back("|"); });
    return log;
}

} // namespace

TEST_CASE("scheduler runs phases in fixed order and registration order within a phase") {
    const auto log = run_ticks(1);
    const std::vector<std::string> expected{
        "cmd", "needsA", "needsB", "act", "res", "life0", "|"};
    REQUIRE(log == expected);
}

TEST_CASE("scheduler gates systems by cadence") {
    const auto log = run_ticks(5);
    int needs = 0;
    int life = 0;
    int cmd = 0;
    for (const auto& entry : log) {
        needs += entry == "needsA";
        life += entry == "life0" || entry == "life4";
        cmd += entry == "cmd";
    }
    REQUIRE(needs == 3); // ticks 0, 2, 4
    REQUIRE(life == 2);  // ticks 0, 4
    REQUIRE(cmd == 5);   // no cadence: every tick
}

TEST_CASE("scheduler output is repeatable") {
    REQUIRE(run_ticks(10) == run_ticks(10));
}

TEST_CASE("runner without registered systems behaves as before") {
    fantasy::core::SimulationRunner runner{123, 5ms};
    REQUIRE(runner.scheduler().system_count() == 0);
    std::uint64_t updates = 0;
    runner.run(3, [&updates](const auto&, auto&) { ++updates; });
    REQUIRE(updates == 3);
    REQUIRE(runner.clock().current_time() == 15ms);
}
