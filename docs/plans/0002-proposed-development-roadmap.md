# Proposed development roadmap

**Status:** Proposed. This roadmap describes a staged direction for Elmford.
Stages 1-5 are broken into task issues in
[`0003-non-ui-task-plan.md`](0003-non-ui-task-plan.md); Stages 6-8 (SDL3,
visualization, RPG) are deferred until the headless stages are complete.

## Purpose

Elmford's long-term goal spans simulation infrastructure, autonomous world
simulation, and a graphical RPG. Implementing that goal as one change would
make the code harder to review, understand, and maintain. This roadmap divides
the work into stages that can be designed and delivered incrementally.

The current architecture proposal and first living-world milestone are in
[`0001-headless-world-initiation.md`](0001-headless-world-initiation.md). This
roadmap provides a broader sequence beyond that initial milestone. It does not
replace the architecture review or settle the detailed design of later stages.

## Stage 1 – Simulation Foundation

**SDL3:** Not included.

Establish the infrastructure on which the rest of the simulation depends.

- C++20 project structure and CMake.
- Simulation Core library.
- World clock and calendar.
- Fixed simulation timestep.
- Deterministic random number generator.
- Logging and diagnostics.
- Configuration management.
- Simulation lifecycle and scheduling.
- Catch2 testing infrastructure.

**Deliverable:** A headless executable that advances simulated time and
produces structured logs.

## Stage 2 – World and Location Model

**SDL3:** Not included.

Establish the geographical structure of the world.

- World.
- Regions.
- Settlements.
- Locations.
- Navigation graph.
- Basic pathfinding.

**Deliverable:** Aveloria containing Marale and Brelin, with interconnected
locations.

## Stage 3 – Character Model

**SDL3:** Not included.

Introduce individually modeled inhabitants.

- Character specifications.
- Attributes and skills.
- Needs and motivations.
- Occupations.
- Inventory.
- Behavioural states.
- Basic activities.

**Deliverable:** A settlement containing identifiable inhabitants with
individual characteristics.

## Stage 4 – Autonomous Settlement Simulation

**SDL3:** Not included.

Make the world operate autonomously.

- Resource definitions.
- Production and consumption.
- Occupation responsibilities.
- Character activity selection.
- Food and rest requirements.
- Settlement population statistics.
- Basic settlement growth.

**Deliverable:** A settlement that can operate autonomously over several
simulated days.

## Stage 5 – Simulation Integration and Observability

**SDL3:** Not included.

Bring the simulation subsystems together and make their behavior observable.

- Simulation scheduling.
- Character and settlement update coordination.
- Structured simulation events.
- Statistical reporting.
- Longer-duration simulation tests.
- Performance profiling.
- Save/load architecture evaluation and a minimal versioned save/load
  (agreed in decision 0008).

**Deliverable:** An observable headless world simulation that can run for
longer periods, with a documented save/load architecture and a minimal
versioned save/load that round-trips deterministically.

## Stage 6 – Game Application Foundation

**SDL3:** Introduced in this stage.

Create the graphical application shell and connect it to the existing
simulation core.

- SDL3 initialisation and shutdown.
- Window management.
- Input handling.
- Rendering integration.
- Application loop.
- Simulation Core integration.
- Basic camera and world presentation.

**Deliverable:** A graphical application capable of displaying the existing
simulated world.

## Stage 7 – Visual World Representation

**SDL3:** Required.

Turn the graphical foundation into a visual settlement experience.

- Terrain representation.
- Settlement visualisation.
- Buildings and structures.
- Character representations.
- Character selection.
- Camera movement.
- Basic interaction.

**Deliverable:** An Age of Empires-style settlement view.

## Stage 8 – RPG Interaction

**SDL3:** Required.

Introduce the player as an individual inhabitant of the autonomous world.

- Player-controlled character.
- Character movement.
- Interaction with NPCs.
- Inventory interface.
- Dialogue.
- Activities and quests.

**Deliverable:** The player can participate in the existing autonomous world
rather than controlling the entire settlement.

## Scope and sequencing notes

- Stages 1–5 remain headless and keep SDL3 out of the simulation core.
- SDL3 enters at Stage 6, in the application layer. The simulation core remains
  usable without a graphical client.
- Keep the simulation-facing interface usable by future clients, including
  VR applications. Clients submit simulation commands and consume immutable
  snapshots or events; input and presentation details stay in the application
  layer.
- Stage 1 establishes the clock and scheduling foundation. Stage 5 coordinates
  domain updates and observability on top of that foundation.
- The roadmap is an ordering proposal, not a delivery schedule. Stage details
  should be refined before substantial implementation of each stage.
- Stage 5 includes a minimal save/load (decision 0008); databases, offline
  catch-up, and multiplayer synchronization are not included.

## Outstanding questions

Resolved (see decisions 0003-0008 and plan 0003):

1. Stage 2 includes basic pathfinding ([decision 0005](../decisions/0005-navigation.md)).
2. "Basic settlement growth" means food-driven births and starvation
   ([decision 0007](../decisions/0007-settlement-lifecycle.md)).
3. Stage 5 implements a minimal versioned save/load
   ([decision 0008](../decisions/0008-persistence-and-performance-targets.md)).
4. Stage 5 targets 100 inhabitants over 30 world days, measured with no CI gate
   (decision 0008).

Still open (deferred with Stages 6-8):

5. What is the minimum interaction expected in Stage 6 before the visual
   representation work in Stage 7?
