# Decision 0009: place naming

**Status:** proposed for review; the tables below are a starting set, not final.

Regions, villages, and cities get generated names built as
`prefix + suffix`, so places created as the world grows (for example a new
village in a region) are named consistently and reproducibly.

- **Suffix** comes from a per-kind pool. Starting suffixes: `oria`, `ale`,
  `alee`, `hill`, `lin`, `ee`, `ige`, `eak`. Proposed pools (overlap allowed):
  region `oria`, `alee`, `eak`; village `ale`, `lin`, `ee`, `hill`; city
  `ige`, `alee`, `eak`.
- **Prefix** is one or two phonetic syllables: an optional consonant onset
  (`b`, `br`, `d`, `k`, `l`, `m`, `th`, `v`, ...) plus a vowel nucleus (`a`,
  `e`, `i`, `o`, `u`, ...) and an optional coda (`r`, `l`, `n`, ...). Join
  rules avoid triple consonants and doubled vowels at the prefix/suffix seam.
- **Determinism:** the generator draws from a `RandomStream` derived from the
  world seed, a stable naming system id, and the new place's ID (see
  `derive_stream`), indexing ordered constant tables. It uses integers only,
  so names do not depend on unrelated systems.
- **Uniqueness:** names are unique within a world, compared case-insensitively.
  On collision the generator retries a bounded number of times, then lengthens
  the prefix by one syllable.
- **Scope:** regions, villages, and cities only. Location names (homes, fields,
  market) are authored content, not generated. Whether a settlement's
  village/city size is a stored kind is left to the settlement model (#30) and
  is not decided here.
- The compiled-in Aveloria example world (#33) uses literal names that follow
  these rules: region Aveloria (`avel` + `oria`), villages Marale (`mar` +
  `ale`) and Brelin (`bre` + `lin`). These replace the earlier placeholder
  names; they are not generated at runtime.
