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
- Keep each task low in complexity and scoped to one outcome so its merge
  request (MR) stays very small. Split broader work into separate task issues
  and MRs.
- Do not assume that a plan authorizes implementation when it explicitly calls
  for review and agreement first.

## GitHub feature and implementation workflow

Track repository tasks through issues in the Elmford repository. The workflow
below is the complete process for issue intake, implementation branches,
validation, and pull requests.

### Create or start work

1. For every repository task, search Elmford issues for an issue specific to
   that task and for its owning feature issue. Read matching issues, check for
   duplicates, and record the task's acceptance criteria, dependencies, and
   blockers before editing. Reuse the matching task issue when one exists.
2. If no task issue exists, create one with a clear description and acceptance
   criteria, linked as a sub-issue of the relevant existing feature. If no
   relevant feature issue exists, create the feature issue first and link the
   task beneath it. For a new feature list, create one feature issue and one
   clearly scoped task issue per deliverable.
3. Assign the task issue to the task owner. If an Elmford GitHub Project
   exists, add the issue and move it to `In Progress`. Post one concise start
   comment with relevant blockers or related issues.
4. Start every task branch from the latest `origin/develop`: fetch from
   `origin`, update local `develop` with a fast-forward only, then create a
   focused branch. Use `feat/ISSUE_NUMBER-short-description`,
   `fix/ISSUE_NUMBER-short-description`, `chore/ISSUE_NUMBER-short-description`,
   or `doc/ISSUE_NUMBER-short-description`, matching the task type. Never
   commit task work on `develop` or push directly to `develop`.
5. Before multi-file or multi-area implementation, state a concise plan and
   check `docs/plans/` for an existing plan. Keep implementation within the
   issue's acceptance criteria and this repository's architecture constraints.

### Implement and finish

1. Follow this file, `README.md`, the relevant decision records and plans, and
   the repository's documented build and validation commands. Keep unrelated
   working-tree changes intact.
2. Before finishing, inspect `git status` and `git diff`. Validate the change
   using the applicable repository commands and report exact commands and
   results. Do not add or run tests unless the task asks for testing or
   verification.
3. Commit only intended files with a message prefixed by the task type and
   referencing its issue: `feat:`, `fix:`, `chore:`, or `doc:` (for example,
   `feat: add world calendar (#ISSUE_NUMBER)`). Use the same type prefix in the
   pull request title.
4. Push only the focused task branch and open a pull request targeting
   `develop`. All changes enter `develop` through an approved pull request.
   Use a concise title and a body with `Summary`, `Changes`, `Validation`, and
   `Related issue` sections. Include `Closes #ISSUE_NUMBER` when the PR delivers
   that issue. Include screenshots for relevant graphical changes.
5. Link the pull request from the issue, and move the Project item to
   `In Review` when one exists. Post progress comments for material changes
   such as resolved dependencies, completed validation, or newly found
   blockers. Do not request merge or mark the issue complete until required
   checks and approvals pass. After merge and issue closure, move the Project
   item to `Done` when available.

If GitHub access or a required Project operation is unavailable, keep the
reviewable work ready locally and report the exact blocked external action.

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
