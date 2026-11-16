# Decision 0006: content format and behavior model

- Resources, occupations, and production recipes are compiled-in C++ data for
  the first milestone, behind registry interfaces so a data loader can
  replace them later. No parser dependency is added.
- Character activity selection uses explicit prioritized rules: critical
  hunger -> eat, critical rest -> rest, assigned work -> work, otherwise idle.
  Ties resolve by a documented stable order.
- Utility scoring or behavior trees are reconsidered only when the rules show
  a concrete limitation, recorded in a new decision.
