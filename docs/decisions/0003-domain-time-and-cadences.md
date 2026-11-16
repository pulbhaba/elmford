# Decision 0003: domain time, cadences, and tick phases

## Context

Decision 0002 fixed the world-day pacing but left the domain update quantum
and per-system cadences open.

## Decisions

- Do not introduce a separate domain quantum. The `fixed_step` in
  `SimulationConfig` remains the only simulation step, and its default
  changes from 1 ms to **1 world-second** (task #62). The CLI can still
  change it with `--step-ns`.
- Domain systems (needs, activities, lifecycle, reporting) run on cadences
  stored in `SimulationConfig`, each a positive integer multiple of the fixed
  step. Configuration validation rejects other values.
- World time (day index, hour, minute) is derived with integer arithmetic from
  the clock and the configured world-day duration. The calendar is limited to
  a 24-hour day; weeks, months, and seasons are out of scope.
- Each tick runs systems in a fixed phase order: commands, needs, activities,
  resolution, lifecycle, then advance time. Systems in one phase run in
  registration order.

## Consequences

At 1 ms a world day would be 86.4 million ticks and a 30-day batch run about
2.6 billion, too slow for tests and long headless runs, which compute ticks as
fast as possible. At 1 second a world day is 86,400 ticks and 30 days is about
2.6 million, so a 30-day run with 100 inhabitants should take minutes to tens
of minutes at most. The step is the finest granularity of domain time; travel
costs use world-seconds. In-game real-time pacing (decision 0002) is
unaffected: 6x pacing at a 1 s step is about 6 ticks per real second. Task #57
measures the actual run time.
