# Proposed development roadmap

**Status:** Proposed. This roadmap describes a staged direction for Elmford;
the stage boundaries, scope, and deliverables remain open for review.

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

**Deliverable:** Eldoria containing Greenvale and Oakridge, with interconnected
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
- Save/load architecture evaluation.

**Deliverable:** An observable headless world simulation that can run for
longer periods, with the save/load architecture evaluated. Implementing
persistence remains a separate decision unless it is added to this stage's
scope.

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
- Stage 1 establishes the clock and scheduling foundation. Stage 5 coordinates
  domain updates and observability on top of that foundation.
- The roadmap is an ordering proposal, not a delivery schedule. Stage details
  should be refined before substantial implementation of each stage.
- The deliverable for Stage 5 describes observability and a save/load
  architecture evaluation. It does not promise working save/load support.

## Outstanding questions

1. Should Stage 2 include basic pathfinding, or should navigation be deferred
   until characters need to move between locations?
2. What population or resource behavior should count as “basic settlement
   growth” in Stage 4?
3. Should Stage 5 implement persistence, or only evaluate and document a
   persistence design?
4. What simulation duration and performance targets should Stage 5 use for
   longer-duration runs and profiling?
5. What is the minimum interaction expected in Stage 6 before the visual
   representation work in Stage 7?
