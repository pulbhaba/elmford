# Decision 0001: deterministic simulation primitives

## Context

The initial simulation foundation needs stable integer time and seedable random
streams without tying simulation progress to wall-clock time or a renderer.

## Decisions

- The simulation clock stores elapsed time as signed 64-bit nanoseconds and
  advances only by a positive, configured fixed step. It checks for time and
  tick-count overflow.
- Random streams use a small SplitMix64 generator with a specified 64-bit
  arithmetic sequence. Each stream is initialized from the world seed and an
  explicit stream identifier, so adding a separate stream does not consume
  values from another stream.
- Integer range sampling uses rejection to avoid modulo bias. Floating range
  sampling uses 53 random bits to create values in `[minimum, maximum)`.
- The runner measures execution duration with `std::chrono::steady_clock`, but
  this measurement never affects simulation time or update count.

## Determinism guarantees and limits

For the same seed, stream identifier, and call sequence, `next_u64()` and
integer-range results are bit-for-bit stable across supported platforms. The
generator and bounded integer mapping use only specified unsigned 64-bit
operations. Floating values use the same random bits and half-open range, but
their final floating-point representation can vary slightly with compiler,
floating-point environment, or standard-library implementation. Wall-clock
durations are inherently variable. This generator is for repeatable simulation
choices, not cryptographic use.
