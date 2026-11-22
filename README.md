# Fantasy Simulation

A small, headless C++20 foundation for a deterministic fantasy world
simulation. It currently provides only a fixed-step clock, deterministic random
streams, a minimal simulation runner, structured logging, and a command-line
executable.

## Requirements

- CMake 3.25 or newer
- A C++20 compiler
- Network access on the first test-enabled configure so CMake can download the
  pinned Catch2 3.7.1 test dependency

The runtime library and executable use only the C++ standard library. Catch2 is
used for tests and can be disabled for an offline executable-only build with
`-DFANTASY_SIM_BUILD_TESTS=OFF`.

## Build and test

From the repository root:

```sh
cmake --preset default
cmake --build --preset default
ctest --preset default
```

For a release build, replace `default` with `release` in each command.

On Windows, run the same commands from a shell where CMake and a C++20 compiler
are available (for example, a Visual Studio developer shell). The presets use
CMake's default generator, so they work with Make, Ninja, or Visual Studio.

To configure without downloading Catch2:

```sh
cmake --preset runtime
cmake --build --preset runtime
```

## Run

```sh
./build/fantasy_sim --seed 12345 --ticks 1000
```

The arguments are optional. The defaults are seed `0` and `1000` ticks. Each
tick advances simulation time by one world-second (1,000,000,000 ns); use
`--step-ns 1000000` for 1 ms steps. The output includes the seed, executed
step count, final simulation time, wall-clock execution duration, update count,
and a deterministic checksum from one random draw per update. The duration can
vary between runs; simulation results and checksum remain stable for identical
inputs.

Use `--describe-world` to print the Aveloria example world (its region,
settlements with their member locations, unaffiliated locations, and
connections, grouped from the world's recorded membership) in a stable order and exit; the
output is identical on every run, and the simulation arguments are ignored:

```sh
./build/fantasy_sim --describe-world
```

Use `fantasy_sim --help` to show the supported arguments. Invalid arguments
produce an error on standard error and a non-zero exit code.

## Simulation configuration

The committed defaults are in
[`src/core/config/simulation_config.hpp`](src/core/config/simulation_config.hpp).
They include the seed, tick count, fixed simulation step, world-day duration,
and real-time duration for a world day. The default world day is 24 world-hours
per four hours of real-time play, or 6x world-time progression. The application
can use these separate durations to pace fixed simulation steps. The current
headless CLI is a batch runner and still computes requested ticks as quickly as
possible; `--step-ns` changes the simulated duration of each tick, not its
wall-clock runtime.

`fantasy::core::validate(config)` returns a list of readable error messages
(empty when valid). It requires a positive tick count, a positive fixed step, a
positive world-day duration that is an exact multiple of the fixed step, and a
positive real-time duration per world day. The CLI validates before running,
prints each error to standard error, and exits with code 2; for example,
`--step-ns 7` is rejected because 7 ns does not divide a 24-hour world day.

## CMake targets

- `fantasy_sim::infrastructure` holds the clock, random streams, and logging.
  It never links the simulation target.
- `fantasy_sim::simulation` holds domain code (currently the runner) and links
  infrastructure one-way.
- `fantasy_sim::core` is a compatibility alias that resolves to simulation.
- No target links SDL or any graphics dependency.

## Core interfaces

- `StableId<Tag>` provides distinct `RegionId`, `SettlementId`, and `LocationId`
  types. IDs compare and hash by their `uint64_t` value; the maximum value is
  reserved for invalid IDs, including default-constructed IDs.
- `StableIdAllocator<Id>` allocates IDs from zero in call order. Each ID type
  has its own allocator; allocation throws `std::overflow_error` rather than
  returning the invalid sentinel when the value range is exhausted.
- `World` (simulation target, `core/world/world.hpp`) owns regions; each
  region owns its settlements and locations. Lookup is by typed ID
  (`find_*` returns `nullptr` when unknown) and iteration is by ascending ID,
  not insertion order. IDs are unique per type across the world. `add_*`
  returns an `AddResult` and changes nothing unless it is `added`; rejections
  are invalid ID, duplicate ID, unknown parent region, and empty name. A
  location may optionally belong to one settlement of its own region (roads and
  wilderness stay unaffiliated); `add_location` rejects an unknown settlement
  (`unknown_settlement`) or one in another region
  (`settlement_region_mismatch`). `settlement_of(location)` returns the optional
  membership and `locations_of(settlement)` returns its locations by ascending
  location ID, never insertion order. Names
  are optional (`std::optional<std::string>`); an unnamed place is identified
  by ID alone. `World::connect` links two existing locations with a positive
  integer travel cost in **world-seconds** (`TravelCost`). Connections are
  undirected and owned by the world; self-connections, duplicates (either
  direction), unknown locations, and non-positive costs return a
  `ConnectResult` other than `connected` and change nothing. `neighbors(id)`
  iterates by ascending destination ID. `find_path`
  (`core/world/pathfinding.hpp`) runs integer-cost Dijkstra and returns a
  `PathResult` (`found`, `same_location`, `unreachable`, or
  `unknown_location`) with the path and total world-second cost. Among
  equal-cost routes it returns the one whose location IDs, read from start to
  goal, are lexicographically smallest.
- `build_aveloria_world` (`core/world/aveloria.hpp`) returns the compiled-in,
  deterministic Aveloria example world with fixed IDs and literal names
  (decision 0009). Region: Aveloria. Villages: Marale and Brelin. Locations
  (authored, not generated): Marale Homes, Marale Fields, Marale Market,
  Aveloria Road, Brelin Homes, Brelin Fields, Brelin Market. Each village's
  three locations are interconnected, and Marale Market and Brelin Market
  link to Aveloria Road, so the villages are connected. Costs are walking
  time in world-seconds. Marale's and Brelin's locations are recorded as
  members of their villages in the `World`; Aveloria Road belongs to no
  settlement.
- `SimulationClock` owns integer simulation time and a positive fixed step.
- `world_time_from` (simulation target) derives `WorldTime` (day index, hour,
  minute, time of day) from clock time and the world-day duration with integer
  math; units and flooring rules are documented in `world_calendar.hpp`. The
  calendar is a 24-hour day only.
- `RandomStream` owns its generator state; stream IDs derive independent,
  repeatable streams from a world seed. `derive_stream_id` / `derive_stream`
  (infrastructure target) map (world seed, system id, optional entity id) to a
  stream with documented mixing; this is the domain contract, so adding an
  entity or system never changes another stream. Pass a stable system id such
  as `static_cast<std::uint64_t>(SimulationSystem::needs)`.
- `SimulationRunner` owns a clock, a single mutable default random stream
  (foundation-only, not the domain contract; domain code should use derived
  streams), and a `TickScheduler`,
  and calls a supplied update function once per step.
- `TickScheduler` (simulation target) runs registered systems each tick in the
  fixed phase order defined by `tick_phase_order` in `tick_scheduler.hpp`:
  commands, needs, activities, resolution, lifecycle. The runner then advances
  time. Systems in a phase run in registration order, and a system registered
  with a cadence key runs only when `is_due` holds for the zero-based tick.
  With no systems registered, runner behavior is unchanged.
- `Logger` writes informational, warning, and error records to a supplied
  output stream, optionally including current simulation time.

Determinism details and floating-point limitations are recorded in
[`docs/decisions/0001-determinism.md`](docs/decisions/0001-determinism.md).
The guarantee is same-build reproducibility: the same seed, configuration, and
build give identical integer domain state. Cross-platform bit-for-bit results
are not yet promised. Fixed-seed golden-value tests in
`tests/core/golden_determinism_tests.cpp` cover the scheduler, cadence gating,
derived random streams, and calendar time; see
[`docs/decisions/0004-numerics-and-determinism-scope.md`](docs/decisions/0004-numerics-and-determinism-scope.md).
World-day pacing is recorded in
[`docs/decisions/0002-world-day-pacing.md`](docs/decisions/0002-world-day-pacing.md).

Project workflow and architecture constraints for contributors and agents are
in [`AGENTS.md`](AGENTS.md). The proposed architecture and first living-world
milestone are tracked in
[`docs/plans/0001-headless-world-initiation.md`](docs/plans/0001-headless-world-initiation.md).
The broader staged direction, including the proposed SDL3 introduction, is in
[`docs/plans/0002-proposed-development-roadmap.md`](docs/plans/0002-proposed-development-roadmap.md).
The feature backlog, descriptions, and GitHub issue links are collected in
[`docs/github-feature-issues.md`](docs/github-feature-issues.md).
