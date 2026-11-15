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

Both arguments are optional. The defaults are seed `0` and `1000` ticks. Each
tick advances simulation time by 1 ms. The output includes the seed, executed
step count, final simulation time, wall-clock execution duration, update count,
and a deterministic checksum from one random draw per update. The duration can
vary between runs; simulation results and checksum remain stable for identical
inputs.

Use `fantasy_sim --help` to show the supported arguments. Invalid arguments
produce an error on standard error and a non-zero exit code.

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

Project workflow and architecture constraints for contributors and agents are
in [`AGENTS.md`](AGENTS.md). The proposed architecture and first living-world
milestone are tracked in
[`docs/plans/0001-headless-world-initiation.md`](docs/plans/0001-headless-world-initiation.md).
