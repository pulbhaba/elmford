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
