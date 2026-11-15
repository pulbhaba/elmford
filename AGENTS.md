# Elmford agent instructions

## Start here

- Treat the repository root as the directory containing this file.
- Before changing code or documentation, read this file, `README.md`, and the
  relevant decision records in `docs/decisions/`.
- Keep documentation aligned with the implemented code. Label proposals and
  unresolved choices clearly; do not present them as settled implementation.

## Plans and workflow

- Before planning Elmford work, inspect `docs/plans/` for an existing plan that
  covers it. Update that plan when the work is the same; create a new plan for
  a distinct objective.
- Number new plans with the next unused four-digit number and name them
  `NNNN-short-topic.md`. Never reuse a number.
- Write or update the plan before substantial implementation. Keep its status,
  decisions, progress, and outstanding questions current, and mark it complete
  only when its outcome is achieved.
- Work in small, reviewable increments. Inspect the diff before committing.
- Do not assume that a plan authorizes implementation when it explicitly calls
  for review and agreement first.
- Do not start or update GitHub issues unless the user asks for GitHub-backed
  work.

## Project constraints

- Use C++20, CMake, and Catch2. Prefer the standard library and small,
  established dependencies. Explain a concrete need before proposing another
  dependency.
- Keep the simulation domain independent of SDL, rendering, audio, and UI.
  SDL3 belongs in the eventual application layer and is not needed by the
  headless prototype.
- Keep domain state separate from presentation state. Keep infrastructure
  services such as logging, random-number generation, configuration, and
  persistence adapters outside domain rules.
- Use fixed simulation time steps, explicit seeds, reproducible random streams,
  stable iteration/order where it affects results, and clear ownership.
- Design seams for persistence, regional simulation, and multiplayer without
  implementing those systems ahead of the current milestone.
- Implement individual inhabitants first. Keep settlement and regional
  simulation as later fidelity levels; do not build all levels at once.
- Do not silently reopen established technology choices or make major domain
  decisions. Record consequential choices in `docs/decisions/` and explain
  trade-offs in the plan or decision record.

## Build and validation

- Follow the existing CMake presets and README. Keep generated build output
  out of commits.
- Add or run tests only when the task asks for testing or verification.
- When validation is requested, favor headless CMake/CTest workflows; tests
  must not require SDL or a graphical environment.
