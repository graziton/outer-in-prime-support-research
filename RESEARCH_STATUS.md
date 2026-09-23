# Research Status

## Core object

For a decimal integer with digits \(d_1,\ldots,d_k\), the outer-in transformation is

\[
T(d_1d_2\ldots d_k)
=
d_1d_kd_2d_{k-1}d_3d_{k-2}\ldots
\]

The main condition studied is

\[
n\ne T(n),\qquad \operatorname{rad}(n)=\operatorname{rad}(T(n)).
\]

## Final documented dataset

The authoritative 2026-09-22 release contains 22 verified solutions below \(10^{10}\) under the documented nonzero-ending restriction.

The documented distribution is:

- 19 solutions below \(10^9\)
- 3 solutions in the ten-digit range \(10^9\le n<10^{10}\)

The authoritative search and verification scope is documented in:

`release/docs/COMPUTATION_PROVENANCE.md`

`release/docs/DATA_STATUS.md`

## Arithmetic reduction

For

\[
g=\gcd(n,T(n)),\qquad n=ag,\qquad T(n)=bg,\qquad \gcd(a,b)=1,
\]

the research establishes the exact criterion

\[
\operatorname{rad}(n)=\operatorname{rad}(T(n))
\iff
\operatorname{rad}(ab)\mid g.
\]

The proof and related constructions are preserved in:

`release/docs/MATHEMATICS.md`

## Documented infinite-family constructions

The release documents repeated-block constructions based on:

- 224 / 242
- 448 / 484
- 117 / 171

These are mathematical constructions within the research record and are distinguished from the finite exhaustive-search results.

## Zero-ending values

The restricted sequence deliberately excludes integers ending in zero.

Simple zero-ending families are documented separately in the research record. Their exclusion from the restricted sequence should not be interpreted as evidence that all zero-ending cases have been classified by the same search.

## Current OEIS submission state

As of 2026-09-23, A400317 and A400321 have been re-proposed for review following editor-requested changes.

The submission was updated to use `k` where requested and the two sequence entries were cross-related during the editing process.

The current editor instruction was that separate REFERENCES entries were unnecessary where the relevant links were already present, so those references were removed. The Holt papers remain represented through the LINKS material.

The frozen files under `release/oeis/` predate these final editor-requested changes and therefore should be treated as historical release material, not as the exact current submission text.

## Interpretation limits

The documented release does not claim:

- completeness above \(10^{10}\)
- a classification of every zero-ending solution
- formal mathematical priority

Those limits are part of the preserved research documentation.
