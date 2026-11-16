// Golden-value tests. Values are same-build reproducibility checks (decision
// 0004); they are not a cross-platform promise. Only integer state is hashed.
#include "core/config/simulation_config.hpp"
#include "core/random/random_stream.hpp"
#include "core/simulation/simulation_runner.hpp"
#include "core/simulation/tick_scheduler.hpp"
#include "core/time/world_calendar.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <cstdint>

using namespace std::chrono_literals;
using fantasy::core::SimulationSystem;
using fantasy::core::TickPhase;

namespace {

// FNV-1a over 64-bit words, fed byte by byte (little-endian order).
class Checksum {
public:
    void add(std::uint64_t value) noexcept {
        for (int shift = 0; shift < 64; shift += 8) {
            hash_ ^= (value >> shift) & 0xffU;
            hash_ *= 1099511628211ULL;
        }
    }
    [[nodiscard]] std::uint64_t value() const noexcept { return hash_; }

private:
    std::uint64_t hash_{14695981039346656037ULL};
};

constexpr std::uint64_t kSeed = 20240607;

std::uint64_t scheduler_checksum() {
    fantasy::core::SimulationConfig config;
    config.fixed_step = 1s;
    config.cadences.needs = 2s;
    config.cadences.activity = 3s;
    config.cadences.lifecycle = 5s;
    fantasy::core::SimulationRunner runner{config};
    Checksum sum;
    auto& scheduler = runner.scheduler();
    const auto system_stream = [&](SimulationSystem system, std::uint64_t entity) {
        return fantasy::core::derive_stream(kSeed, static_cast<std::uint64_t>(system), entity);
    };
    scheduler.register_system(TickPhase::lifecycle, [&](const auto& clock, auto&) {
        auto stream = system_stream(SimulationSystem::lifecycle, clock.tick_index());
        sum.add(1);
        sum.add(clock.tick_index());
        sum.add(stream.next_u64());
    }, SimulationSystem::lifecycle);
    scheduler.register_system(TickPhase::needs, [&](const auto& clock, auto&) {
        auto stream = system_stream(SimulationSystem::needs, 7);
        sum.add(2);
        sum.add(clock.tick_index());
        sum.add(static_cast<std::uint64_t>(stream.uniform_int(0, 999)));
    }, SimulationSystem::needs);
    scheduler.register_system(TickPhase::activities, [&](const auto& clock, auto&) {
        sum.add(3);
        sum.add(clock.tick_index());
    }, SimulationSystem::activity);
    scheduler.register_system(TickPhase::commands, [&](const auto& clock, auto&) {
        sum.add(4);
        sum.add(clock.tick_index());
    });
    runner.run(60, [&](const auto& clock, auto& random) {
        sum.add(5);
        sum.add(static_cast<std::uint64_t>(clock.current_time().count()));
        sum.add(random.next_u64());
    });
    return sum.value();
}

} // namespace

TEST_CASE("golden: derived stream values for fixed seeds") {
    REQUIRE(fantasy::core::derive_stream_id(kSeed, 1) == 0xccf4da35de16fcd9ULL);
    REQUIRE(fantasy::core::derive_stream_id(kSeed, 1, 42) == 0xe06d215c53b3f747ULL);
    auto stream = fantasy::core::derive_stream(kSeed, 1, 42);
    REQUIRE(stream.next_u64() == 0x307fb9caa0c04d09ULL);
    REQUIRE(stream.next_u64() == 0xd0d56a2735acffaaULL);
    REQUIRE(stream.uniform_int(-100, 100) == 11);
}

TEST_CASE("golden: runner random stream for a fixed seed") {
    fantasy::core::RandomStream stream{kSeed};
    Checksum sum;
    for (int index = 0; index < 100; ++index) sum.add(stream.next_u64());
    REQUIRE(sum.value() == 0xe3830e23f7fd7cbfULL);
}

TEST_CASE("golden: scheduler and cadence gating checksum") {
    REQUIRE(scheduler_checksum() == 0x545ded018d2b9d46ULL);
    REQUIRE(scheduler_checksum() == scheduler_checksum());
}

TEST_CASE("golden: calendar time from simulation time") {
    const auto day = 24h;
    const auto time = fantasy::core::world_time_from(day * 3 + 5h + 7min + 9s, day);
    REQUIRE(time.day_index == 3);
    REQUIRE(time.hour == 5);
    REQUIRE(time.minute == 7);
    REQUIRE(time.time_of_day == 5h + 7min + 9s);

    Checksum sum;
    for (std::int64_t second = 0; second < 3 * 86400; second += 977) {
        const auto world = fantasy::core::world_time_from(std::chrono::seconds{second}, day);
        sum.add(world.day_index);
        sum.add(world.hour);
        sum.add(world.minute);
        sum.add(static_cast<std::uint64_t>(world.time_of_day.count()));
    }
    REQUIRE(sum.value() == 0xcfec8b8072442a5fULL);
}
