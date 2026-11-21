# Non-UI feature and task plan

**Status:** Agreed scope for the headless milestone (features #2-#6 and #10).
Implementation is in progress; see Progress. UI, SDL3, and
game-engine features (#7 graphical foundation, #8 visualization, #9 player
RPG) are **deferred** until the headless features are complete; they will get
their own task breakdown then.

This plan turns plans [0001](0001-headless-world-initiation.md) and
[0002](0002-proposed-development-roadmap.md) into task issues. Those plans
remain the architectural proposal; the decisions below resolve their open
questions.

## Agreed decisions

| Topic | Decision | Record |
| --- | --- | --- |
| Time | Default fixed step becomes 1 world-second; domain cadences are multiples of it; 24-hour world day | [0002](../decisions/0002-world-day-pacing.md), [0003](../decisions/0003-domain-time-and-cadences.md) |
| Numerics | Integer canonical state, fixed-scale needs, checked arithmetic | [0004](../decisions/0004-numerics-and-determinism-scope.md) |
| Determinism | Same-build reproducibility first; cross-platform not yet promised | [0004](../decisions/0004-numerics-and-determinism-scope.md) |
| Navigation | Location graph plus deterministic Dijkstra pathfinding in #3 | [0005](../decisions/0005-navigation.md) |
| Naming | Suffix-based place names with seeded prefixes (proposed) | [0009](../decisions/0009-place-naming.md), [plan 0004](0004-place-naming.md) |
| Content | Compiled-in C++ definitions behind registries | [0006](../decisions/0006-content-and-behavior.md) |
| Behavior | Prioritized explicit rules | [0006](../decisions/0006-content-and-behavior.md) |
| Population | Food-driven births and starvation | [0007](../decisions/0007-settlement-lifecycle.md) |
| Persistence | Minimal versioned save/load is in scope for #6 | [0008](../decisions/0008-persistence-and-performance-targets.md) |
| Targets | 100 inhabitants, 30 world days; measure, no CI gate | [0008](../decisions/0008-persistence-and-performance-targets.md) |

## Open risks and questions

- **Tick cost.** The default step moves from 1 ms to 1 world-second so 30
  world days is about 2.6M ticks (target: minutes to tens of minutes). Task
  #57 measures it; a coarser step or cheaper cadence checks are the fallbacks.
- **Initial content** (occupations, resources, recipes, rates, thresholds) is
  chosen inside tasks #39, #42, #44, #46, and #48 and documented there.
- **Save format** is chosen in #58.
- Mobile/hardware budgets remain unspecified; measure after the first
  settlement slice.
- Multiplayer, regional/settlement fidelity levels, and a command queue for
  external clients keep their seams but are not implemented.

## Task breakdown

Each task is one small, reviewable pull request. "Blocked by" lists direct
dependencies only. Tasks may be done in any order the dependencies allow;
the suggested order is top to bottom.

### Feature #2: Complete the simulation foundation

| # | Task | Blocked by |
| --- | --- | --- |
| #15, #18 | Fixed-step CLI option; world-day pacing config (done) | |
| #62 | Default fixed step becomes one world-second | #20 |
| #22 | Separate simulation and infrastructure CMake targets | #20 |
| #23 | World calendar and sub-day time types | #22 |
| #24 | Validate simulation configuration | #23 |
| #25 | Configure domain update cadences | #24, #62 |
| #26 | Deterministic tick phase scheduler | #25 |
| #27 | Per-system and per-entity random stream derivation | #20 |
| #28 | Golden determinism coverage and docs | #26, #27 |

### Feature #3: Model the world and its locations

| # | Task | Blocked by |
| --- | --- | --- |
| #29 | Typed stable identifiers | #22 |
| #30 | World, region, settlement, location state | #29 |
| #31 | Location connections | #30 |
| #32 | Deterministic pathfinding | #31 |
| #33 | Aveloria example world | #32 |
| #34 | CLI world overview | #33 |

### Feature #4: Simulate individual characters

| # | Task | Blocked by |
| --- | --- | --- |
| #35 | Character, resource, occupation identifiers | #29 |
| #36 | Inventory with checked transfers | #35 |
| #37 | Character specification, attributes, skills | #35 |
| #38 | Fixed-scale needs model | #37, #25 |
| #39 | Occupation definitions and registry | #35 |
| #40 | Character state, behavioral state, activity | #36, #38, #39, #37 |
| #41 | Seeded character generation | #40, #27 |

### Feature #5: Run an autonomous settlement

| # | Task | Blocked by |
| --- | --- | --- |
| #42 | Resource definitions and registry | #35 |
| #43 | Settlement state and inventory rules | #30, #36, #42, #40 |
| #44 | Production and consumption transactions | #43 |
| #45 | Prioritized activity selection | #40, #43 |
| #46 | Needs progression system | #45, #44, #26 |
| #47 | Work assignment and production resolution | #44, #45, #39, #26 |
| #48 | Food-driven births and starvation | #47, #41, #26 |
| #49 | Settlement summary statistics | #47, #48 |
| #50 | CLI seeded settlement with periodic summaries | #49, #33 |
| #51 | Multi-day repeatability verification | #50 |

### Feature #6: Integrate simulation systems and observability

| # | Task | Blocked by |
| --- | --- | --- |
| #52 | Integrate systems in documented phase order | #48, #46 |
| #53 | Structured simulation events | #52 |
| #54 | Statistics aggregation | #53, #49 |
| #55 | Read-only world snapshots | #52, #49 |
| #56 | Longer-duration runs and integration tests | #53, #54, #55 |
| #57 | Performance measurement and documentation | #56 |
| #58 | Save/load architecture decision | #55 |
| #59 | Minimal versioned save/load | #58 |
| #60 | Save/load round-trip determinism | #59, #56 |

### Feature #10: Documentation

| # | Task | Blocked by |
| --- | --- | --- |
| #20 | Record design decisions and this plan | |
| #21 | README feature index | #20 |

Every task that changes behavior updates the README or decision records it
affects and includes headless Catch2 coverage as its acceptance criteria
state.

## Milestone

Features #2-#6 together deliver plan 0001's milestone: a seeded Aveloria
settlement with individually simulated inhabitants running for several world
days headless, with summaries, events, statistics, a 100-inhabitant/30-day
measured run, and minimal save/load.

## Progress

- 2026-11-21: Implemented #30 on `feat/30-world-state`: `World` with regions,
  settlements, and locations, ID lookup, ascending-ID iteration, rejection
  results, optional names, and Catch2 coverage. Connections remain #31.
- 2026-11-18: Started #29 on `feat/29-typed-stable-identifiers`; blockers #22
  and #20 are closed. This task precedes world state (#30) and character IDs
  (#35).
- 2026-11-18: Completed #29's typed IDs and deterministic allocator. The
  default headless configure/build and all 43 Catch2 tests pass.
- 2026-11-16: Decisions agreed; task issues #20-#60 created under their
  features. Default step changed to one world-second (#62). No implementation yet beyond #15 and #18.
- 2026-11-16: #28 adds golden determinism tests and docs (same-build only), the last task of feature #2 (after #26 and #27).
