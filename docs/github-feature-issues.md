# Elmford feature list

**Status:** The nine feature issues are open in GitHub. Their scopes come from
[`plans/0001-headless-world-initiation.md`](plans/0001-headless-world-initiation.md)
and [`plans/0002-proposed-development-roadmap.md`](plans/0002-proposed-development-roadmap.md).
Features #2-#6 and #10 are broken into task issues in
[`plans/0003-non-ui-task-plan.md`](plans/0003-non-ui-task-plan.md), with design
questions resolved in decisions 0003-0008. Features #7-#9 (SDL3, visualization,
player RPG) are deferred until the headless features are complete and have no
task issues yet.

The linked issues contain these descriptions, acceptance criteria, and design
notes.

## 1. Feature: Complete the simulation foundation ([#2](https://github.com/pulbhaba/elmford/issues/2))

### Description

Complete the headless foundation needed by later world features. The current
baseline already has a fixed-step clock, deterministic random streams, a
simulation runner, logging, and a CLI; this feature covers the remaining
domain-facing time, configuration, and module boundaries in the proposed
roadmap.

### Acceptance criteria

- The simulation core has a clear library boundary separate from application
  and infrastructure responsibilities.
- World-facing time and calendar concepts have documented units and advance
  only through fixed simulation steps.
- Simulation configuration has an explicit interface suitable for reproducible
  runs.
- The headless executable continues to run without SDL or a graphical
  environment.
- Determinism behavior and any limits remain documented and have focused
  automated coverage.

### Design notes

Resolve relevant open design questions and update decision records before
substantial implementation.

### Tasks

Task issues: #15, #18 (done), #22-#28, #62. Decisions: 0003, 0004.

## 2. Feature: Model the world and its locations ([#3](https://github.com/pulbhaba/elmford/issues/3))

### Description

Add the geographical model that contains regions, settlements, and locations,
with stable identities and explicit connections. The roadmap's example world
is Aveloria with Marale and Brelin.

### Acceptance criteria

- A world can own regions, and regions can contain settlements and locations.
- Regions, settlements, and locations use stable typed identifiers.
- The example world can represent Marale and Brelin with interconnected
  locations.
- Navigation behavior and whether basic pathfinding belongs in this feature are
  agreed and documented before implementation. If pathfinding is included, its
  results are deterministic for the same world state and query.
- The model is independent of SDL, rendering, and presentation code.

### Design notes

Resolve the navigation question in the roadmap and applicable architecture
questions in the initiation plan.

### Agreed decisions

Basic pathfinding is included ([decision 0005](decisions/0005-navigation.md)).

### Tasks

Task issues: #29-#34.

## 3. Feature: Simulate individual characters ([#4](https://github.com/pulbhaba/elmford/issues/4))

### Description

Introduce inhabitants as individually identifiable simulation entities with
the attributes and state needed for later autonomous behavior.

### Acceptance criteria

- A character has a stable identity and explicit attributes, skills, needs,
  occupation, inventory, behavioral state, and current activity.
- Character specifications and mutable character state have a clear boundary.
- Character creation can produce repeatable results from an explicit world
  seed.
- Character state and updates are independent of UI and renderer state.
- Focused automated coverage verifies identity, initialization, and
  repeatability.

### Design notes

Resolve the initiation plan's open design questions before substantial
implementation.

### Tasks

Task issues: #35-#41. Decisions: 0004, 0006.

## 4. Feature: Run an autonomous settlement ([#5](https://github.com/pulbhaba/elmford/issues/5))

### Description

Make a settlement operate over simulated time through character activities,
occupations, needs, and resource production and consumption.

### Acceptance criteria

- Resource definitions and settlement inventories represent quantities with
  documented rules.
- Production and consumption resolve through explicit transactions, including
  clear results when demand cannot be met.
- Character activities respond to work assignments and basic needs such as
  hunger and rest.
- A seeded settlement can run for several simulated days with repeatable
  results.
- Headless summaries show population, occupations, resource quantities,
  production and consumption, unmet needs, and settlement trend.

### Design notes

Confirm the initiation plan's time, numeric, behavior, and lifecycle decisions
before substantial implementation.

### Agreed decisions

Food-driven births and starvation; prioritized rules; compiled-in content (decisions 0006, 0007).

### Tasks

Task issues: #42-#51.

## 5. Feature: Integrate simulation systems and observability ([#6](https://github.com/pulbhaba/elmford/issues/6))

### Description

Coordinate world, character, and settlement updates and make longer-running
simulation behavior observable through events, statistics, and profiling.

### Acceptance criteria

- Simulation systems run in a documented, deterministic phase order.
- Structured events and statistics expose meaningful simulation changes without
  giving presentation code mutable domain state.
- Headless runs report results over longer durations and have documented
  performance measurements.
- The save/load architecture is evaluated and documented, and a minimal
  versioned save/load round-trips deterministically.
- Integration coverage verifies coordination and repeatable observations.

### Design notes

Refine duration and performance targets before implementation.

### Agreed decisions

A minimal versioned save/load is in scope, in addition to the architecture evaluation; target is 100 inhabitants over 30 world days (decision 0008).

### Tasks

Task issues: #52-#60.

## 6. Feature: Establish the graphical application foundation ([#7](https://github.com/pulbhaba/elmford/issues/7))

### Description

Add an SDL3-based application shell that connects to the existing simulation
core while keeping platform and presentation concerns outside the core.

### Acceptance criteria

- SDL3 setup and shutdown, window creation, input handling, and the application
  loop are implemented in an application target.
- The application can advance the existing simulation core and present a basic
  world view.
- The simulation core and headless executable still build and run without SDL3.
- The graphical target has clear build and run instructions for supported
  platforms.

### Design notes

Introduce SDL3 only in the application layer. Confirm platform and dependency
requirements before implementation.

## 7. Feature: Visualize the world and settlement ([#8](https://github.com/pulbhaba/elmford/issues/8))

### Description

Represent the simulated world in the graphical application so a player can
inspect and navigate a settlement.

### Acceptance criteria

- The application presents terrain, settlement structures, and character
  representations.
- The user can move the camera and select a character.
- The view reads simulation snapshots or events and does not mutate core state
  directly.
- The graphical view uses the headless simulation interface without adding SDL
  dependencies to the simulation core.

### Design notes

Refine the minimum visual and interaction scope before implementation.

## 8. Feature: Add player RPG interaction ([#9](https://github.com/pulbhaba/elmford/issues/9))

### Description

Introduce the player as an individual participant in the autonomous world,
with basic movement and interactions with inhabitants and activities.

### Acceptance criteria

- A player-controlled character can move through the world.
- The player can interact with NPCs and access character inventory.
- Dialogue and activity or quest interactions have defined application-facing
  flows.
- Player commands enter the simulation through an explicit interface, and
  clients observe results through snapshots or events.
- The autonomous simulation remains usable without a player client.

### Design notes

Define the minimum interaction scope and behavior before implementation.

## 9. Feature: Maintain project documentation and contributor guidance ([#10](https://github.com/pulbhaba/elmford/issues/10))

### Description

Keep Elmford's README, contributor instructions, architecture plans, decisions,
and feature index accurate and useful as the project grows. Documentation must
distinguish current implementation from proposed roadmap work and avoid
presenting unresolved design choices as settled.

### Tasks

- [ ] Commit and maintain the Elmford GitHub implementation workflow in
  `AGENTS.md`, including issue intake, branch conventions, validation, and pull
  request tracking.
- [ ] Update `README.md` with a concise feature index linking to roadmap issues
  and distinguishing implemented capabilities from proposed work.
- [ ] Review plans `0001` and `0002` against feature issues #2–#9; align scope,
  dependencies, and open questions without treating proposals as approved
  design.
- [ ] After design review, record agreed architectural choices in
  `docs/decisions/` and update the related plan status.
- [ ] Keep the feature index and roadmap issue links current as implementation
  work is completed.

### Acceptance criteria

- The README, AGENTS file, plans, and decisions clearly distinguish implemented
  behavior, proposed work, and approved decisions.
- The README and feature index link to the relevant GitHub issues.
- Each documentation task is reviewable and the updated documentation agrees
  with the current code and project decisions.

### Design notes

The roadmap and initiation plan remain proposals. Resolve their outstanding
questions through review before substantial simulation implementation.

### Task issues

Task issues: #20, #21 (plus completed #12, #14).
