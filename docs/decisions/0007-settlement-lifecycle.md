# Decision 0007: settlement population change

The smallest population rule for the first milestone is food-driven:

- At a world-day boundary, if the food surplus exceeds a configured
  threshold, one inhabitant is born (created by the seeded generator).
- If hunger has gone unmet for a configured number of consecutive days, one
  inhabitant dies.
- Thresholds live in configuration with documented defaults. Rules run only
  in the lifecycle phase and are deterministic.

Ageing, relationships, inheritance, and broader demographics are out of
scope.
