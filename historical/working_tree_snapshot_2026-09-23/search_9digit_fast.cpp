#include <bits/stdc++.h>
using namespace std;

using u64 = uint64_t;
using i64 = int64_t;

/*
============================================================
9-DIGIT OUTER-IN TRANSFORMATION

d1 d2 d3 d4 d5 d6 d7 d8 d9

        -->

d1 d9 d2 d8 d3 d7 d4 d6 d5
============================================================
*/


/*
    Precomputed contribution tables.

    high = d1 d2 d3 d4
    low  = d5 d6 d7 d8 d9
*/

static uint32_t HIGH_T[10000];
static uint32_t LOW_T[100000];


void buildTransformTables() {

    for (int x = 0; x < 10000; ++x) {

        int d1 = x / 1000;
        int d2 = (x / 100) % 10;
        int d3 = (x / 10) % 10;
        int d4 = x % 10;

        /*
            d1 d2 d3 d4 contribute:

            d1 _ d2 _ d3 _ d4 _

        */

        HIGH_T[x] =
            d1 * 100000000U +
            d2 *   1000000U +
            d3 *     10000U +
            d4 *       100U;
    }


    for (int x = 0; x < 100000; ++x) {

        int d5 = x / 10000;
        int d6 = (x / 1000) % 10;
        int d7 = (x / 100) % 10;
        int d8 = (x / 10) % 10;
        int d9 = x % 10;

        /*
            _ d9 _ d8 _ d7 _ d6 d5

        */

        LOW_T[x] =
            d9 * 10000000U +
            d8 *   100000U +
            d7 *     1000U +
            d6 *       10U +
            d5;
    }
}


inline uint32_t transform9(uint32_t n) {

    uint32_t high = n / 100000U;
    uint32_t low  = n % 100000U;

    return HIGH_T[high] + LOW_T[low];
}


/*
============================================================
CORRECT 2/5 CONDITION

For primitive n:

    d9 != 0

Same divisibility by 2:
    n % 2 == T(n) % 2

Same divisibility by 5:
    n % 5 == 0  iff  T(n) % 5 == 0

Since T(n) ends in d5 and n ends in d9:

    d9 parity == d5 parity

and

    (d9 == 5) == (d5 == 0 || d5 == 5)

We avoid testing these repeatedly by generating only the
valid (d5,d9) combinations.
============================================================
*/


struct DigitPair {
    uint8_t d5;
    uint8_t d9;
};


/*
    There are exactly 34 valid (d5,d9) pairs.

    These are:

        d9 = 5:
            d5 = 0,5

        d9 = odd non-5:
            d5 = 1,3,7,9

        d9 = even:
            d5 = 2,4,6,8
*/


vector<DigitPair> buildValidDigitPairs() {

    vector<DigitPair> v;

    for (int d9 = 1; d9 <= 9; ++d9) {

        for (int d5 = 0; d5 <= 9; ++d5) {

            bool div2n = (d9 % 2 == 0);
            bool div2t = (d5 % 2 == 0);

            bool div5n = (d9 == 5);
            bool div5t = (d5 == 0 || d5 == 5);

            if (div2n != div2t)
                continue;

            if (div5n != div5t)
                continue;

            v.push_back({
                (uint8_t)d5,
                (uint8_t)d9
            });
        }
    }

    return v;
}


/*
============================================================
SMALL PRIME SIGNATURE FILTER

We precompute divisibility signatures modulo

    M = 3 * 7 * 11 * 13 * 17 * 19
      = 969969

For every remainder r, signature[r] tells us which of
those primes divide r.

If:

    signature[n % M] != signature[t % M]

then prime support definitely differs.

This is only a FILTER.

The final test below is exact.
============================================================
*/

constexpr int M = 969969;

static uint8_t signature[M];


void buildSignatureTable() {

    memset(signature, 0, sizeof(signature));

    const int primes[] = {
        3, 7, 11, 13, 17, 19
    };

    for (int bit = 0; bit < 6; ++bit) {

        int p = primes[bit];

        for (int r = 0; r < M; r += p)
            signature[r] |= (uint8_t)(1U << bit);
    }
}


/*
============================================================
EXACT SAME-PRIME-SUPPORT TEST

This is the key optimization.

Let:

    g = gcd(n,t)
    a = n/g
    b = t/g

Every prime common to n and t is in g.

Therefore n and t have exactly the same prime support iff
all prime factors of a and b are already contained in g.

We remove those factors using repeated gcd.

NO FACTORIZATION REQUIRED.
============================================================
*/

inline bool allFactorsContained(
    u64 x,
    u64 g
) {

    while (x > 1) {

        u64 h = std::gcd(x, g);

        if (h == 1)
            return false;

        x /= h;
    }

    return true;
}


inline bool samePrimeSupport(
    u64 n,
    u64 t
) {

    u64 g = std::gcd(n, t);

    /*
        If g=1 and n,t > 1, they cannot have the same
        prime support.
    */

    if (g == 1)
        return false;

    u64 a = n / g;
    u64 b = t / g;

    if (!allFactorsContained(a, g))
        return false;

    if (!allFactorsContained(b, g))
        return false;

    return true;
}


/*
============================================================
FULL FACTORIZATION

Used ONLY after a solution is found.
============================================================
*/

vector<pair<u64,int>> factorize(u64 n) {

    vector<pair<u64,int>> result;

    for (u64 p = 2; p * p <= n; ++p) {

        if (n % p != 0)
            continue;

        int e = 0;

        while (n % p == 0) {

            n /= p;
            ++e;
        }

        result.push_back({p,e});
    }

    if (n > 1)
        result.push_back({n,1});

    return result;
}


string factorString(u64 n) {

    auto f = factorize(n);

    string s;

    for (size_t i = 0; i < f.size(); ++i) {

        if (i)
            s += " * ";

        s += to_string(f[i].first);

        if (f[i].second > 1) {

            s += "^";
            s += to_string(f[i].second);
        }
    }

    return s;
}


/*
============================================================
ORBIT

For 9 digits this transformation has order 4.
============================================================
*/

array<uint32_t,4> getOrbit(uint32_t n) {

    array<uint32_t,4> o;

    o[0] = n;
    o[1] = transform9(o[0]);
    o[2] = transform9(o[1]);
    o[3] = transform9(o[2]);

    return o;
}


/*
============================================================
ONE THREAD'S RESULTS
============================================================
*/

struct Solution {

    uint32_t n;
    uint32_t t;

    u64 rad;

    uint32_t gcdValue;

    array<uint32_t,4> orbit;

    int matchingEdges;

    string factorN;
    string factorT;
};


/*
============================================================
ANALYZE A FOUND SOLUTION

This is NOT called for every candidate.
Only very rare actual solutions reach here.
============================================================
*/

Solution analyzeSolution(uint32_t n) {

    uint32_t t = transform9(n);

    auto orbit = getOrbit(n);

    int matchingEdges = 0;

    u64 commonRad = 1;


    for (int i = 0; i < 4; ++i) {

        uint32_t a = orbit[i];
        uint32_t b = orbit[(i + 1) % 4];

        if (samePrimeSupport(a,b)) {

            ++matchingEdges;

            /*
                Compute rad from factorization only here.
            */
            auto fa = factorize(a);

            commonRad = 1;

            for (auto [p,e] : fa)
                commonRad *= p;
        }
    }


    u64 g = std::gcd(
        (u64)n,
        (u64)t
    );


    Solution s;

    s.n = n;
    s.t = t;

    s.rad = commonRad;

    s.gcdValue = (uint32_t)g;

    s.orbit = orbit;

    s.matchingEdges = matchingEdges;

    s.factorN = factorString(n);
    s.factorT = factorString(t);

    return s;
}


/*
============================================================
WORKER

Each thread receives a range of the first four digits.

That gives excellent load balancing.
============================================================
*/

struct WorkerResult {

    uint64_t candidates = 0;
    uint64_t passedSmallPrimeFilter = 0;

    vector<uint32_t> solutions;

    uint64_t multiEdgeOrbits = 0;
    uint64_t consecutiveMatches = 0;
};


void worker(
    int highBegin,
    int highEnd,
    const vector<DigitPair>& pairs,
    WorkerResult& result
) {

    for (int high = highBegin;
         high < highEnd;
         ++high) {

        /*
            high = d1 d2 d3 d4

            d1 is automatically nonzero because high>=1000.
        */

        for (int tail = 0;
             tail < 1000;
             ++tail) {

            /*
                tail = d6 d7 d8

                We iterate the 34 valid combinations of
                (d5,d9).
            */

            uint32_t base =
                (uint32_t)high * 100000U +
                (uint32_t)tail * 10U;


            for (const auto& pair : pairs) {

                uint32_t n =
                    base +
                    (uint32_t)pair.d5 * 10000U +
                    pair.d9;


                ++result.candidates;


                uint32_t t = transform9(n);


                /*
                    Ignore fixed points.

                    These are trivial because n = T(n).
                */
                if (n == t)
                    continue;

                /*
                    Small-prime signature filter.
                */

                uint8_t a =
                    signature[n % M];

                uint8_t b =
                    signature[t % M];


                if (a != b)
                    continue;


                ++result.passedSmallPrimeFilter;


                /*
                    Exact test.
                */

                if (!samePrimeSupport(n,t))
                    continue;


                /*
                    Record solution.

                    We don't factor here.
                */

                result.solutions.push_back(n);
            }
        }
    }
}


/*
============================================================
MAIN
============================================================
*/

int main() {

    cout << "============================================\n";
    cout << "FAST 9-DIGIT OUTER-IN SEARCH\n";
    cout << "============================================\n";

    cout << "Range: 100000000 <= n < 1000000000\n\n";


    /*
        --------------------------------------------------------
        Build transformation tables.
        --------------------------------------------------------
    */

    buildTransformTables();

    cout << "Transformation tables ready.\n";


    /*
        --------------------------------------------------------
        Build valid digit pairs.
        --------------------------------------------------------
    */

    auto pairs = buildValidDigitPairs();

    cout << "Valid (d5,d9) pairs: "
         << pairs.size()
         << "\n";


    /*
        Sanity check.

        There are exactly 33 valid (d5,d9) pairs.
    */

    if (pairs.size() != 33) {

        cerr
            << "ERROR: digit-pair filter construction failed.\n";

        return 1;
    }


    /*
        --------------------------------------------------------
        Build small-prime signature table.
        --------------------------------------------------------
    */

    buildSignatureTable();

    cout << "Small-prime filter ready.\n";



    /*
        --------------------------------------------------------
        Number of threads.
        --------------------------------------------------------
    */

    unsigned threadCount =
        thread::hardware_concurrency();

    if (threadCount == 0)
        threadCount = 4;


    /*
        Don't create ridiculous numbers of threads.
    */

    threadCount =
        min<unsigned>(
            threadCount,
            32
        );


    cout << "Threads: "
         << threadCount
         << "\n\n";


    /*
        --------------------------------------------------------
        Split the 9000 possible high prefixes among threads.
        --------------------------------------------------------
    */

    vector<thread> threads;

    vector<WorkerResult> results(threadCount);


    int totalHigh = 9000;

    int block =
        (totalHigh + threadCount - 1)
        / threadCount;


    auto startTime =
        chrono::high_resolution_clock::now();


    for (unsigned id = 0;
         id < threadCount;
         ++id) {

        int begin =
            1000 + (int)id * block;

        int end =
            min(
                10000,
                begin + block
            );


        if (begin >= end)
            continue;


        threads.emplace_back(
            worker,
            begin,
            end,
            cref(pairs),
            ref(results[id])
        );
    }


    /*
        Wait for everybody.
    */

    for (auto& th : threads)
        th.join();


    /*
        --------------------------------------------------------
        Merge results.
        --------------------------------------------------------
    */

    uint64_t totalCandidates = 0;

    uint64_t totalSmallPassed = 0;

    vector<uint32_t> solutions;


    for (auto& r : results) {

        totalCandidates += r.candidates;

        totalSmallPassed +=
            r.passedSmallPrimeFilter;

        solutions.insert(
            solutions.end(),
            r.solutions.begin(),
            r.solutions.end()
        );
    }


    sort(
        solutions.begin(),
        solutions.end()
    );


    solutions.erase(
        unique(
            solutions.begin(),
            solutions.end()
        ),
        solutions.end()
    );


    /*
        --------------------------------------------------------
        Analyze actual solutions.

        Very few are expected, so full factorization is fine.
        --------------------------------------------------------
    */

    vector<Solution> analyzed;

    analyzed.reserve(
        solutions.size()
    );


    for (uint32_t n : solutions) {

        analyzed.push_back(
            analyzeSolution(n)
        );
    }


    /*
        --------------------------------------------------------
        Count orbit properties.
        --------------------------------------------------------
    */

    int maxMatchingEdges = 0;

    int consecutiveMatches = 0;


    for (const auto& s : analyzed) {

        maxMatchingEdges =
            max(
                maxMatchingEdges,
                s.matchingEdges
            );


        /*
            For 4-cycle:

                n -> T(n) -> T^2(n)

            Check every starting point.
        */

        for (int i = 0; i < 4; ++i) {

            uint32_t a =
                s.orbit[i];

            uint32_t b =
                s.orbit[(i+1)%4];

            uint32_t c =
                s.orbit[(i+2)%4];


            if (samePrimeSupport(a,b) &&
                samePrimeSupport(b,c)) {

                ++consecutiveMatches;
            }
        }
    }


    auto endTime =
        chrono::high_resolution_clock::now();


    double seconds =
        chrono::duration<double>(
            endTime-startTime
        ).count();


    /*
        --------------------------------------------------------
        SAVE RESULTS
        --------------------------------------------------------
    */

    ofstream out(
        "primitive_9digit_fast.csv"
    );

    out
        << "n,Tn,rad,gcd,orbit_length,"
           "matching_edges,factor_n,factor_Tn\n";


    for (const auto& s : analyzed) {

        out
            << s.n << ','
            << s.t << ','
            << s.rad << ','
            << s.gcdValue << ','
            << 4 << ','
            << s.matchingEdges << ','
            << '"'
            << s.factorN
            << "\",\""
            << s.factorT
            << "\"\n";


        cout << "\nSOLUTION\n";
        cout << "n       = " << s.n << '\n';
        cout << "T(n)    = " << s.t << '\n';
        cout << "rad     = " << s.rad << '\n';
        cout << "gcd     = " << s.gcdValue << '\n';
        cout << "factors = " << s.factorN << '\n';
        cout << "Tfact   = " << s.factorT << '\n';

        cout << "orbit   = ";

        for (int i = 0; i < 4; ++i) {

            if (i)
                cout << " -> ";

            cout << s.orbit[i];
        }

        cout << '\n';

        cout
            << "matching edges in orbit = "
            << s.matchingEdges
            << '\n';
    }


    /*
        --------------------------------------------------------
        FINAL SUMMARY
        --------------------------------------------------------
    */

    cout << "\n============================================\n";
    cout << "SEARCH COMPLETE\n";
    cout << "============================================\n";

    cout
        << "Candidates scanned: "
        << totalCandidates
        << '\n';

    cout
        << "Passed small-prime filter: "
        << totalSmallPassed
        << '\n';

    cout
        << "Primitive solutions found: "
        << solutions.size()
        << '\n';

    cout
        << "Maximum matching edges in an orbit: "
        << maxMatchingEdges
        << '\n';

    cout
        << "Consecutive matching edges: "
        << consecutiveMatches
        << '\n';

    cout
        << "Runtime: "
        << fixed
        << setprecision(3)
        << seconds
        << " seconds\n";


    cout << "\n============================================\n";
    cout << "CHECKS\n";
    cout << "============================================\n";

    if (maxMatchingEdges <= 1)
        cout << "No multi-edge orbit found.\n";
    else
        cout << "COUNTEREXAMPLE FOUND: multi-edge orbit.\n";

    if (consecutiveMatches == 0)
        cout << "No consecutive matching edges found.\n";
    else
        cout << "Consecutive matching edges FOUND.\n";


    cout << "\nOutput:\n";
    cout << "  primitive_9digit_fast.csv\n";

    return 0;
}