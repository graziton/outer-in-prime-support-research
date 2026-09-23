# Data status

The canonical dataset contains 22 distinct terms, in increasing order.

The first 19 are the complete primitive solutions below 10^9 from the original
exhaustive search. The final three are the complete hits in the full interval
10^9 <= n < 10^10.

All 22 candidates were independently verified by recomputing T(n), gcd, reduced
ratios, and the exact support criterion. All have nonzero final digit and are not
fixed points of T.

The three ten-digit values are additionally supported by explicit factorizations
of both members of each pair; those factorizations are recorded in the canonical
data CSV.

The dataset does not assert completeness above 10^10 or without the nonzero-last-
digit restriction.
