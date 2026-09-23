# Computation provenance

## Authoritative base computation

The final completeness claim is:

    exactly 22 solutions for 0 < n < 10^10 with n not divisible by 10.

The decisive search is the ratio-independent implementation in
`code/exhaustive_search.cpp`. For a fixed decimal length it constructs T(n),
performs necessary small-prime support filters, computes gcd(n,T(n)), and applies
the exact criterion

    rad(a*b) | gcd(n,T(n)).

No reduced ratio is assumed by this search.

The clean search source is a history-free packaging of the audited core
implementation used for the ten-digit scan. The older historical source is
preserved in `research_archive/research_v1_original.zip`.

## Search coverage

The historical base computation had two stages:

1. globally exhaustive primitive computation below 10^9, yielding 19 terms;
2. a complete ratio-independent search of 10^9 <= n < 10^10, yielding 3 terms.

For the ten-digit stage, the archive contains 90 consecutive raw chunk files.
Their intervals are 100,000,000 integers wide, starting at 1,000,000,000 and
ending at 10,000,000,000. Exactly three raw chunks contain one hit each.
`verification/ten_digit_chunk_audit.csv` records the complete 90-chunk coverage.

## Independent verification

`code/verify_solutions.cpp` independently reconstructs T(n), computes gcd and
the reduced pair, and checks the exact criterion. It was run on the 22 canonical
candidates and returned 22 VERIFIED results with 0 non-verified outcomes.

For the current candidates, the verifier also performs explicit trial-division
factorization of a*b when it fits in uint64_t. This is a verification aid for the
current dataset, not a general-purpose large-integer factoring algorithm.

## Why the historical archive is separate

The supplied `research_v1` snapshot contains the complete exploratory history:
multiple search versions, ratio-restricted investigations, family searches,
logs, raw files, and earlier drafts. Some historical programs and documents are
superseded or exploratory. They are retained unchanged for provenance and are not
used to establish the final 22-term completeness result.

Use the `release/` directory as the authoritative reproducibility package.
