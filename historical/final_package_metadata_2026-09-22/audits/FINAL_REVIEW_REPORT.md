# Final review report - 2026-09-22

## 1. Supplied material

The uploaded `research_v1.zip` was unpacked and inventoried. It contains 181 files
(plus directory entries) representing the full research snapshot. All source,
data, documentation, logs, and the final paper were retained in the nested
`research_v1_original.zip` without modification.

## 2. Source compilation

All 17 C++ source files in the historical research archive compiled successfully
with g++ -std=c++17 in the audit environment. Compilation problems therefore do
not block reproduction of the archived programs.

The authoritative clean search source and independent verifier also compiled
successfully.

## 3. Base-search code audit

The authoritative search constructs the outer-in permutation for one fixed decimal
length, excludes inputs divisible by 10, performs only necessary small-prime
filters, computes gcd(n,T(n)), and applies the exact criterion

    rad((n/g)*(T(n)/g)) | g.

The small-prime filters are only rejection filters, so their incompleteness cannot
create false negatives. Divisibility by 3 and 9 is preserved by digit permutation
and therefore does not need a separate support comparison.

64-bit unsigned integers are sufficient for all numbers below 10^10.

A small-range execution of the authoritative algorithm reproduced exactly the
13 known terms below 10^6.

## 4. Ten-digit raw-output audit

The historical archive contains exactly 90 ten-digit chunk files:

    1,000,000,000 ... 9,900,000,000

in increments of 100,000,000. The resulting intervals are contiguous and terminate
at 10,000,000,000.

Exactly three chunks contain a data row:

- 1,900,000,000 chunk: 1938285196 -> 1699318528
- 2,200,000,000 chunk: 2240563518 -> 2821450356
- 2,900,000,000 chunk: 2952612351 -> 2195532261

All other chunks are header-only. The consolidated ten-digit output contains
exactly those three rows.

## 5. Canonical-data audit

The 22 canonical terms are strictly increasing. Nineteen are below 10^9 and three
are in [10^9,10^10).

For every canonical term, T(n) was reconstructed independently, n != T(n), n is
not divisible by 10, the gcd/reduced pair was recomputed, and the prime-support
condition was verified.

The independent C++ verifier produced:

    Candidates read:       22
    Verified matches:      22
    Non-verified outcomes: 0

## 6. Factorization audit

The current canonical dataset contains independent factorizations for all 22
pairs. The three newly discovered ten-digit pairs have matching prime supports
as follows:

    1938285196 = 2^2 * 73^2 * 90931
    1699318528 = 2^8 * 73 * 90931

    2240563518 = 2 * 3^5 * 17 * 139 * 1951
    2821450356 = 2^2 * 3^2 * 17^2 * 139 * 1951

    2952612351 = 3^3 * 13^2 * 29 * 53 * 421
    2195532261 = 3^2 * 13 * 29^2 * 53 * 421

## 7. Mathematics audit

The gcd/radical criterion is exact.

For repeated blocks, the identity

    10^m + 1 | 1 + 10^m + ... + 10^((2r-1)m)

holds because 10^m is -1 modulo 10^m+1 and there are an even number of terms.

The three documented infinite families are independently consistent with the
criterion:

    224 -> 242, support difference {7,11}, and 7*11 | 10^3+1.
    448 -> 484, support difference {7,11}, and 7*11 | 10^3+1.
    117 -> 171, using 9-digit blocks, support difference {13,19}, and
    13*19 | 10^9+1.

## 8. Paper audit

The final manuscript compiles with no LaTeX errors. It is 7 pages and was rendered
and visually inspected. No clipped text, overlapping content, or missing glyphs
were observed.

The manuscript intentionally leaves `[Author]` as the one field requiring the
human author's personal identity.

## 9. OEIS audit

The final entry uses offset 1,1, the 22 terms in increasing order, the explicit
non-divisible-by-10 restriction, the exact gcd criterion, the three proven families,
and conservative cross-references to related OEIS sequences.

Superseeker was run on the entire 22-term sequence on September 22, 2026 and
returned that the terms did not match anything in the table.

## 10. Final claim boundary

The final package claims only:

    exactly 22 terms for 0 < n < 10^10 with n not divisible by 10.

It does not claim completeness above 10^10, completeness without the restriction,
or formal priority over all prior literature.

## 11. Known historical issues retained but quarantined

The historical snapshot contains superseded material. These are intentionally
preserved rather than silently rewritten. The final package explicitly tells a
reviewer which directory is authoritative.

This is the final release boundary: the `release/` directory is the reproducibility
and submission package; the nested original research archive is provenance.
