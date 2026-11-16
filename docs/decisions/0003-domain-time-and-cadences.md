# Decision 0003: domain time, cadences, and tick phases

## Context

Decision 0002 fixed the world-day pacing but left the domain update quantum
and per-system cadences open.

## Decisions

- Do not introduce a new time quantum. The fixed step already in
  `SimulationConfig` (`fixed_step`, default 1 ms) remains the only simulation
  step. The CLI can still change it with `--step-ns`.
- Domain systems (needs, activities, lifecycle, reporting) run on cadences
  stored in `SimulationConfig`, each a positive integer multiple of the fixed
  step. Configuration validation rejects other values.
- World time (day index, hour, minute) is derived with integer arithmetic from
  the clock and the configured world-day duration. The calendar is limited to
  a 24-hour day; weeks, months, and seasons are out of scope.
- Each tick runs systems in a fixed phase order: commands, needs, activities,
  resolution, lifecycle, then advance time. Systems in one phase run in
  registration order.

## Consequences and risk

At the 1 ms default a world day is 86,400,000 ticks, so a 30-day run is about
2.6 billion ticks. Cadence checks must be cheap, and long headless runs may
use a coarser `--step-ns`. Task #57 measures this and records any mitigation;
changing the default step would be a new decision.
