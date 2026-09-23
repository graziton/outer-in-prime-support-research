# Outer-In Prime-Support Research

Research archive for the decimal outer-in digit transformation

\[
T(d_1d_2\ldots d_k)
=
d_1d_kd_2d_{k-1}d_3d_{k-2}\ldots
\]

with particular interest in integers satisfying

- \(n \ne T(n)\)
- \(\operatorname{rad}(n)=\operatorname{rad}(T(n))\)

The repository preserves the computational record, mathematical analysis, verification data, OEIS material, paper sources, and historical research artifacts.

## Repository status

This repository was organized from the research workspace on 2026-09-23.

### Authoritative release

`release/` is the authoritative frozen research package dated 2026-09-22.

It contains the final computational dataset, source code, verification material, mathematical documentation, related-work notes, OEIS draft material, and the manuscript.

The final release records 22 verified nonzero-ending solutions below \(10^{10}\):

- 19 solutions below \(10^9\)
- 3 solutions with \(10^9 \le n < 10^{10}\)

The release documentation defines the precise search scope and should be consulted before interpreting completeness claims.

### Historical record

`historical/` preserves earlier material without silently rewriting it.

Important preserved artifacts include:

- `research_v1_original.zip` — exact supplied historical research archive
- `legacy_release_snapshot.zip` — exact older release snapshot
- `working_tree_snapshot_2026-09-23/` — exact snapshot of the loose research files present in the original workspace
- `final_package_metadata_2026-09-22/` — preserved release metadata and audit material
- `workstation_config_2026-09-23/` — preserved development-machine configuration

Historical material is retained for provenance and reproducibility and is not assumed to represent the final state of the research.

### OEIS

`release/oeis/` contains the frozen OEIS draft material that accompanied the 2026-09-22 release.

`oeis/` contains current submission-status documentation maintained separately from that frozen draft.

The current OEIS submission state may differ from the frozen draft files.

### Paper

`release/paper/` contains the frozen manuscript source and PDF associated with the authoritative release.

`paper/` contains repository-level notes about the manuscript.

### Archive

`archive/` is reserved for future distributable repository bundles and archival packaging.

## Important provenance rule

The `release/` directory is the authoritative final package.

Historical files are preserved as historical evidence and should not be silently replaced with newer versions. Where a historical artifact differs from the final release, both are retained and the distinction should be documented.

## Reproducibility

Start with:

- `release/docs/ARCHIVE_GUIDE.md`
- `release/docs/COMPUTATION_PROVENANCE.md`
- `release/docs/DATA_STATUS.md`
- `release/docs/MATHEMATICS.md`
- `release/docs/RELATED_WORK.md`

Verification material is in:

`release/verification/`

The final package includes an independent verification workflow and the raw ten-digit search chunks used for the documented search.

## Research scope

The project concerns the equality of prime supports rather than equality of the integers themselves. The central arithmetic reduction is documented in `release/docs/MATHEMATICS.md`.

The repository also preserves research into repeated-block families, ratio-based constructions, orbit analysis, and related OEIS sequences.

## Notes on claims

This repository does not by itself establish completeness above the documented search range, mathematical priority, or uniqueness beyond the stated computational and mathematical evidence.

Use the dated release documentation when citing computational scope.
