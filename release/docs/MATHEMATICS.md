# Mathematics

## Definition

For the decimal expansion

    n = d1 d2 ... dk,

let

    T(n) = d1 dk d2 d{k-1} d3 d{k-2} ...

where the digits are taken alternately from the two ends inward.

The sequence in this release consists of positive integers n satisfying

    n != T(n)
    rad(n) = rad(T(n))
    n is not divisible by 10.

The last condition is an explicit restriction on the input sequence. Terms
ending in 0 are outside the chosen sequence, even if they satisfy the arithmetic
property.

## Exact gcd criterion

Let

    g = gcd(n,T(n)),
    n = a*g,
    T(n) = b*g,
    gcd(a,b) = 1.

Then

    rad(n) = rad(T(n))  <=>  rad(a*b) | g.

Proof: if the prime supports are equal, every prime dividing a must divide
T(n)=b*g. Since gcd(a,b)=1 it must divide g; similarly every prime dividing b
must divide g. Hence rad(a*b) divides g. Conversely, if rad(a*b) divides g,
then every prime in a or b already occurs in g, so ag and bg have exactly the
same prime support.

## Exhaustive result

Exactly 22 terms occur for

    0 < n < 10^10, n not divisible by 10.

Nineteen occur below 10^9 and three occur in the ten-digit interval.

The three ten-digit pairs are:

    1938285196 -> 1699318528    gcd 26551852    ratio 73:64
    2240563518 -> 2821450356    gcd 82983834    ratio 27:34
    2952612351 -> 2195532261    gcd 75708009    ratio 39:29

## Repeated blocks

Suppose B and C are equal-length decimal blocks and T(BB)=CC. Then

    T(B^(2r)) = C^(2r)

for every r >= 1, where B^(2r) means decimal concatenation of 2r copies.

If A and C0 are the numerical values of B and C, and

    Q_r = 1 + 10^m + ... + 10^((2r-1)m),

then the repeated blocks are A*Q_r and C0*Q_r, and

    10^m + 1 | Q_r.

Consequently, if g=gcd(A,C0), A=a*g, C0=b*g, gcd(a,b)=1, the sufficient
condition

    rad(a*b) | g*(10^m+1)

proves equal prime support for every r.

This gives the documented infinite families:

    224 -> 242   (even repetitions)
    448 -> 484   (even repetitions)
    117 -> 171   (117 repeated 6r times)

For the last family use the 9-digit blocks 117117117 and 171171171. Their
reduced ratio is 13:19, and

    10^9 + 1 = 7*11*13*19*52579.
