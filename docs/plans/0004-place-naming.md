# Place naming plan

**Status:** In progress; the design is proposed in
[decision 0009](../decisions/0009-place-naming.md) and awaiting review.
Feature [#75](https://github.com/pulbhaba/elmford/issues/75).

## Objective

A deterministic naming module that builds names from a phonetic prefix and a
place-kind suffix, used for the example world and for places created as the
world grows.

## Tasks

| # | Task | Blocked by |
| --- | --- | --- |
| #76 | Record decision and plan (this change) | |
| #77 | Place-name tables | #76 |
| #78 | Deterministic generator with uniqueness | #77 |
| #79 | Name places on demand: settlements at population 20, others when defined or requested | #78, #30, #48 |

## Open questions

- Prefix syllable tables (the suffix split per kind is agreed).
- When regions and cities receive generated names (settlements: at population 20; others: when a name is defined or requested).
- Whether settlements carry a stored village/city kind (settlement model, #30).
- Naming of locations inside settlements (currently authored, not generated).

## Progress

- 2026-11-16: Agreed: names are optional; a settlement needs a name only at population 20 (configurable default); suffix split per kind agreed.
- 2026-11-16: Feature and tasks created; example world renamed to Aveloria
  with Marale and Brelin in #3, #33, and the docs.
