# Outer-In Prime-Support Research - Final Package

Date: 2026-09-22

This package contains the cleaned reproducibility release for the outer-in
decimal digit / equal-prime-support problem, plus the complete historical
research archive supplied for the final audit.

## Start here

Use `release/` as the authoritative package.

The central result is:

    Exactly 22 positive integers n satisfy

        n != T(n),
        rad(n) = rad(T(n)),
        n not divisible by 10,

    for 0 < n < 10^10.

The 22 terms are in `release/data/primitive_solutions_through_1e10.csv` and
`release/oeis/entry_draft.txt`.

The 10-digit completeness evidence is in
`release/verification/ten_digit_raw/`, with 90 consecutive 100-million-number
chunks covering [10^9,10^10). Exactly three chunks contain hits. The consolidated
three-hit file and an auditable chunk table are also included.

The independent verifier is `release/code/verify_solutions.cpp` and the candidate
input is `release/verification/final_22_verification_input.txt`.

The mathematical exposition is in `release/docs/MATHEMATICS.md` and the final
paper is in `release/paper/`.

The current OEIS draft is `release/oeis/entry_draft.txt`.

## Historical archive

`research_archive/research_v1_original.zip` is the exact research_v1 archive
provided for the final audit. Nothing from that supplied archive has been deleted
or silently rewritten. It contains the full exploratory computation history.

Use the historical archive for provenance; use `release/` for the final claims.

## Current external checks

The 22-term sequence was submitted to OEIS Superseeker on September 22, 2026.
The response was that the terms did not match anything in the table.

The related-work review also checked current OEIS entries A110751, A110819,
A244514, and A371307. These are related digit-reversal or cyclic-permutation
problems, not the same outer-in transformation.

## Submission scope

The package does not claim completeness above 10^10, does not classify terms with
the last digit zero, and does not make a formal priority claim about the problem.

The only remaining personal step for the human OEIS submitter is to log in under
their own account and submit the final entry after personally reviewing the
mathematics and code.
