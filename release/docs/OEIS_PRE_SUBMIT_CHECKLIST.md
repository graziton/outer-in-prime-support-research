# OEIS pre-submission checklist

## Completed

- [x] Exact definition fixed: outer-in decimal digit permutation.
- [x] Sequence restriction fixed explicitly: m not divisible by 10.
- [x] Fixed points m=T(m) excluded explicitly.
- [x] Exact gcd/radical criterion proved and documented.
- [x] Exactly 22 primitive terms certified for 0 < m < 10^10.
- [x] 19 lower-range terms and 3 ten-digit terms reconciled.
- [x] Three ten-digit pairs independently factor-checked.
- [x] Independent verifier: 22/22 VERIFIED, 0 failures.
- [x] 90 ten-digit raw chunks audited with contiguous coverage.
- [x] Superseeker lookup returned no matching sequence.
- [x] Related OEIS entries checked and distinguished.
- [x] No priority claim included.
- [x] b-file agrees with the 22 displayed terms.

## Human submission actions

1. Log in to OEIS and use the current New Sequence submission page.
2. Paste the contents of `oeis/entry_draft.txt` into the appropriate entry fields.
3. Use the sequence offset 1,1 and keywords base,more,nonn.
4. The 22 terms already fit comfortably in the main data field. The included
   b-file is retained for reproducibility and need not be uploaded unless useful.
5. The human submitter should make the final contribution under their own OEIS
   account and personally confirm that the formulas, code, and computation are
   understood.

## Claims not to make

Do not describe the sequence as “the first ever”, “never studied”, or the complete
infinite sequence. The certified bound is only m < 10^10 with m not divisible by 10.
