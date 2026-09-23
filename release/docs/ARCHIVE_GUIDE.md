# Research archive guide

The top-level final package has two distinct parts.

## `release/`

This is the authoritative, cleaned reproducibility package. It contains only the
materials needed to understand and reproduce the base result, together with the
final OEIS draft and paper.

## `research_archive/research_v1_original.zip`

This is the exact research_v1 archive supplied for the final audit. It preserves
the full development history, including earlier search programs, intermediate
data, family-search experiments, logs, and superseded documents.

Some historical artifacts are intentionally not promoted into the authoritative
release. For example, the archive records an older block-search output-format bug,
and some family searches were exploratory or used stronger sufficient tests. None
of those artifacts is needed to establish the 22-term exhaustive base result.

The final result is supported by the ratio-independent search, the raw ten-digit
chunk coverage, the canonical dataset, and the independent verifier.

The original research_v1 `MANIFEST_SHA256.txt` is itself historical and predates
some files added later in that research directory. The final package manifest at
the release level is therefore the authoritative integrity record for this final
package.
