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

## Core interfaces

- `SimulationClock` owns integer simulation time and a positive fixed step.
- `RandomStream` owns its generator state; stream IDs derive independent,
  repeatable streams from a world seed.
- `SimulationRunner` owns a clock and default random stream and calls a supplied
  update function once per step.
- `Logger` writes informational, warning, and error records to a supplied
  output stream, optionally including current simulation time.

Determinism details and floating-point limitations are recorded in
[`docs/decisions/0001-determinism.md`](docs/decisions/0001-determinism.md).
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
