# Decision 0008: persistence scope and performance targets

## Persistence

Feature #6 includes a minimal, dependency-free, versioned save/load adapter,
beyond the original evaluation-only proposal. The format and boundary are
chosen in task #58 before implementation. Persistence stays outside domain
rules, files carry a format version, and a resumed run must equal an
uninterrupted run. Databases, offline catch-up, and multiplayer
synchronization remain out of scope.

## Targets

The longer-run target is 100 inhabitants over 30 world days. Measure and
record timings (task #57); there is no hard CI performance gate yet. Revisit
the targets after the first full settlement slice.
