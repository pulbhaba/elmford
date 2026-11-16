# Headless fantasy world simulation: project initiation

**Status:** Proposed architecture. World-day pacing and the design questions
resolved in [`0003-non-ui-task-plan.md`](0003-non-ui-task-plan.md) (decisions
0003-0008) are agreed; items below marked **Agreed** are settled, the rest
remain proposals. Implementation proceeds through the task issues in plan
0003.

## Goal

Grow Elmford from its current deterministic, headless C++20 foundation into a
small living-world simulation. The world should advance independently of a
player, support autonomous settlements and inhabitants, and remain suitable
for PC and mobile hardware, large regional worlds, persistence, and possible
distributed multiplayer over time.

The first playable-simulation milestone is deliberately narrow: one seeded
world, one settlement, a small population of individually simulated
inhabitants, basic needs and occupations, resource production and consumption,
and periodic headless summaries over several simulated days.

## Current repository baseline

Elmford currently has a C++20 CMake project, a headless command-line program,
Catch2 3.7.1 tests, a fixed-step nanosecond clock, seedable random streams, a
simulation runner, and structured logging. The runtime uses the standard
library. The determinism guarantees and limits are recorded in
[`../decisions/0001-determinism.md`](../decisions/0001-determinism.md).

This is useful infrastructure, but the current `core` target combines
simulation scheduling with infrastructure services. As domain systems are
introduced, separate domain rules from infrastructure while preserving the
small headless build and existing behavior.

## Established constraints

- C++20 and CMake; support Windows and Linux.
- SDL3 is the eventual application/platform integration. The simulation core
  must not depend on SDL3, and SDL3 is not needed for the prototype.
- Catch2 tests run headlessly.
- Prefer the standard library and small established dependencies. Do not add a
  game engine, ECS, database, or networking framework without a demonstrated
  need and an explained trade-off.
- Use fixed-step simulation time and explicit, reproducible random seeds.
- Keep simulation state separate from presentation state and avoid unnecessary
  allocation and per-frame work.
- Introduce individual simulation first. Settlement and regional fidelity are
  future levels, not part of the first implementation.

## Proposed architecture

Use three major layers with one-way dependencies:

1. **Simulation core** owns world state and domain rules: time progression,
   regions, settlements, inhabitants, resources, needs, activities, economy,
   and simulation systems. It has no SDL, UI, rendering, audio, file-format, or
   network dependency.
2. **Game application** owns platform setup, input, rendering, audio, and
   player interaction. The first application remains a CLI. An eventual SDL3
   client calls the core through an application-facing simulation interface,
   sends explicit commands, and reads immutable snapshots or events.
3. **Infrastructure** supplies configuration/content loading, deterministic
   random services, logging/diagnostics, persistence adapters, and test
   utilities. Domain code receives the services it needs through narrow
   interfaces; it does not own platform implementations.

Keep a world as the authoritative simulation boundary. Give regions,
settlements, characters, resources, buildings, and activities stable typed
identifiers. Systems should operate on explicit state and elapsed simulation
time rather than on renderer frames. This keeps a later server or headless
process able to run the same world without an application client.

For future fidelity changes, define explicit boundaries between individual
character state, settlement aggregates, and regional aggregates. The first
version only implements individuals; later transitions should preserve
population and resource totals through deliberate conversion rules.

## Domain concepts and relationships

- A **World** owns its seed, simulation clock/calendar, and regions.
- A **Region** contains settlements and environmental context. Region ownership
  can later become a process/server boundary without making it a networking
  concern in domain code.
- A **Settlement** owns its population references, buildings, resource
  inventory, production capacity, and summary statistics.
- A **Character** is an inhabitant with an identity, attributes, skills,
  needs, occupation, current activity, and relationships. Initially, only
  hunger, rest, a small set of occupations, and simple activities are modeled.
- **Definitions** describe reusable content such as resource kinds,
  occupations, buildings, and production recipes. **State** contains the
  mutable values for a particular world. Definitions should be data-driven,
  but the initial content can be compiled-in C++ data to avoid an unneeded
  parser dependency.
- **Resources and inventories** represent stored quantities. Production
  recipes consume inputs and labor and produce outputs; consumption and
  production resolve through settlement inventory rules.
- **Simulation systems** update needs, select/advance activities, resolve
  production and consumption, and apply lifecycle events. Systems should use
  stable identifiers and deterministic ordering where order can affect the
  result.
- **Snapshots/statistics/events** expose observations to the CLI and future
  clients without giving presentation code mutable access to domain state.

## Proposed initial data models

These are deliberately small starting shapes; confirm names and exact field
types during architectural review.

- `WorldState`: seed, integer tick, calendar/time state, and regions.
- `RegionState`: stable ID, environment summary, and settlements.
- `SettlementState`: stable ID, inhabitants, buildings, inventories, and
  cumulative production/consumption/lifecycle counters.
- `CharacterState`: stable ID, age/life state, occupation ID, skill values,
  fixed-scale needs, activity state, and relationship references.
- `ResourceDefinition`: stable content ID and display name; runtime quantities
  use integer units.
- `Inventory`: resource ID to non-negative integer quantity, with checked
  transfers and explicit production/consumption results.
- `OccupationDefinition` and `ProductionRecipe`: inputs, outputs, labor
  requirements, and eligibility/skill requirements where needed.
- `SimulationConfig`: fixed step, update cadences, world seed, and initial
  population/content configuration.
- `WorldSnapshot` / `SettlementSummary`: read-only counts, inventory totals,
  occupation distribution, unmet needs, and recent production, consumption,
  and population changes.

Prefer integer time and quantities for canonical state. If fractional needs or
rates are required, use a documented fixed scale initially. Avoid floating
point in rules that must reproduce bit-for-bit across supported platforms
unless a decision record explicitly narrows that guarantee.

## Proposed scheduling and time model

A world day is 24 world-hours. The agreed default pacing maps one world day to
four hours of real-time play (6x world-time progression). Keep the world-day
duration, real-time duration per world day, and fixed simulation step as
separate configuration values. The code's current 1 ms step is an infrastructure
default that task #62 changes to one world-second (decision 0003); the step
is not itself a calendar unit. The headless
CLI remains a batch runner; a future application may use the configured pace
to schedule fixed steps. Wall-clock time does not change simulation results.

Each tick follows a deterministic phase order:

1. Apply queued external commands at a defined tick boundary.
2. Update individual needs using elapsed world time.
3. Select or continue character activities from needs, occupation, and
   available work.
4. Resolve activity work and settlement production/consumption transactions.
5. Apply lifecycle and settlement changes whose scheduled time has arrived.
6. Advance world time and publish requested observations/summaries.

The fixed simulation step and the cadence of individual needs, activities,
settlement reporting, and lifecycle updates remain to be chosen as those domain
systems are designed. Keep update order stable and isolate random streams by
stable entity/system identifiers so adding unrelated entities does not perturb
every other random decision. The present runner's single mutable stream is
sufficient for the current foundation but should not become the domain's
global randomness contract.

The 24-hour world day and four-hour real-time duration are agreed. How the
calendar-facing API represents sub-day time and how often each domain system
updates remain to be decided as those systems are designed.

## Proposed project and CMake structure

Evolve incrementally from the existing tree; do not move all files as a
prerequisite. The intended destination is:

```text
src/
  simulation/                 # domain state, IDs, definitions, systems
  infrastructure/             # clock, RNG services, config, logging, storage
  application/
    cli/                       # current headless executable and summaries
    sdl/                       # eventual SDL3 client; separate optional target
tests/
  simulation/                 # domain/system tests
  infrastructure/             # deterministic service tests
docs/
  decisions/                  # durable architectural decisions
  plans/                      # tracked implementation plans
```

Expose a simulation library target that links only standard-library code, an
infrastructure target where useful, and a CLI executable. Add SDL3 later as an
optional application dependency and never link it to simulation. Keep Catch2
version pinned and tests independently runnable without a display. Preserve
existing public interfaces unless a small migration is justified and
documented.

## First milestone implementation plan

Begin these steps only after the proposal and unresolved choices are reviewed:

1. Agree on the simulation time quantum, tick phase order, canonical numeric
   representation, and random stream ownership contract. Record material
   decisions in `docs/decisions/`.
2. Add stable IDs, compiled-in resource/occupation/recipe definitions, and the
   minimum world, settlement, inventory, and character state needed for one
   settlement.
3. Add deterministic seeded world/population creation and assign basic
   occupations.
4. Add hunger and rest progression, deterministic activity selection, and
   work/idle/rest activity states.
5. Add food production, consumption, storage limits if required, and clear
   transaction results for unmet demand.
6. Add scheduled lifecycle/population and settlement changes at the minimum
   scope needed for observable growth or decline. Keep rules explicit and
   testable.
7. Extend the CLI to print periodic structured settlement summaries: world
   day, population changes, resources, production/consumption, occupation
   counts, unmet needs, and growth/decline indicators.
8. Add focused Catch2 coverage for fixed-seed repeatability, needs, activity
   selection, resource accounting, schedule boundaries, and summary values.
   Tests must not launch SDL or require a graphical environment.
9. Document new public interfaces and any remaining determinism limits.

## Milestone acceptance

The milestone is complete when a user can run the headless executable with a
fixed seed and observe several simulated days for one settlement with a small
generated population; inhabitants have basic occupations and hunger/rest
needs; activities respond to needs and work; food production and consumption
change inventory; and periodic summaries report population changes, resource
quantities, occupation distribution, unmet needs, and settlement trend. The
same seed and configuration must produce the same domain state and summary
sequence on supported builds, subject to any documented platform limits.

## Assumptions and open questions for review

1. **World-time pace (Agreed):** 24-hour world day per four hours of
   real-time play. The configured fixed step (default 1 world-second, task #62) stays the
   only step; domain cadences are multiples of it
   ([decision 0003](../decisions/0003-domain-time-and-cadences.md)).
2. **Numeric representation (Agreed, decision 0004):** integer time and resource quantities,
   with fixed-scale needs if fractional rates are necessary. Confirm scale and
   overflow policy when defining domain types.
3. **Content format (Agreed, decision 0006):** compiled-in definitions for the first
   milestone, with a data-driven boundary and no external parser dependency.
4. **Behavior model (Agreed, decision 0006):** explicit prioritized rules for the initial
   small population. Consider utility scoring or behavior trees only when the
   rules demonstrate a concrete limitation.
5. **Population change (Agreed, decision 0007):** food-driven births and
   starvation, the smallest explicit lifecycle rule.
6. **Persistence (Agreed, decision 0008):** a minimal versioned save/load is
   in scope for #6; database and offline catch-up semantics remain deferred.
7. **Multiplayer:** keep commands, observations, stable IDs, and region seams
   suitable for future authority boundaries; defer protocols, distributed
   ownership, and synchronization.
8. **Determinism scope (Agreed, decision 0004):** same-build reproducibility
   first; cross-platform bit-for-bit results are not yet promised.
9. **Mobile/performance targets:** no hardware budget is specified yet. Measure
   after the first domain slice before selecting data-oriented or
   multithreaded complexity.

## Not in the first milestone

No SDL client, graphics, audio, trade between
settlements, regional aggregates, fidelity transitions, networking,
multiplayer hosting, full calendar/seasons, external content parser,
database, or comprehensive persistence implementation.
