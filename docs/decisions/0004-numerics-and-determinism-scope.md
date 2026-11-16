# Decision 0004: numeric representation and determinism scope

## Decisions

- The required guarantee is same-build reproducibility: identical seed,
  configuration, and build produce identical domain state and summaries.
  Bit-for-bit cross-platform results are not promised yet.
- Canonical domain state uses integers: nanosecond time, whole-unit resource
  quantities, and fixed-scale integers for needs, attributes, and skills.
  Each fixed scale is documented where it is defined.
- Floating point is not used in domain rules whose results feed canonical
  state. This keeps cross-platform reproducibility likely, but it is not a
  guarantee until golden-value tests run on more than one platform.
- Inventory and counter arithmetic is checked; overflow and negative
  quantities are explicit failures.
- Random streams are derived from the world seed plus a stable system and
  entity identifier, so adding an entity does not change other streams.
  Decision 0001 still governs the generator itself.

## Golden-value coverage

`tests/core/golden_determinism_tests.cpp` asserts fixed-seed golden values for
derived stream ids and outputs, the runner's random stream, a scheduler run
with cadence gating, and calendar time. Only integer state is hashed. These
values verify same-build reproducibility. They are not a cross-platform
promise: no other platform has been run yet, so a golden mismatch on another
platform is a finding to investigate, not a guarantee being broken. Floating
point values (for example `uniform_real`) are deliberately excluded.
