# Historical research archive status

`research_v1_original.zip` is the exact archive supplied for the final audit.
It is retained as an immutable historical record.

It contains multiple generations of code and documents. Some items describe
intermediate research stages rather than the final result. In particular:

- older snapshots correctly record that the project had not yet established the
  10-digit exhaustive result;
- the archive records a known presentation bug in an older block-search CSV and
  explicitly notes that it was not used as public evidence;
- several family-search programs are exploratory and are not used to prove the
  absence of families;
- the historical manifest inside research_v1 covers only a subset of files and
  predates later additions to that directory.

These points are why the final package separates the historical archive from the
clean `release/` directory. The authoritative base result is supported by the
ratio-independent search, the 90 raw ten-digit chunks, the canonical dataset, and
the independent verifier contained in `release/`.
