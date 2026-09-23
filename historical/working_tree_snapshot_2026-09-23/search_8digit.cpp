#include <bits/stdc++.h>
using namespace std;

using int64 = long long;


/*
============================================================
RANGE

We already completely searched <= 10^7.

Now search all 8-digit numbers:

10,000,000 <= n <= 99,999,999
============================================================
*/

const int START = 10'000'000;
const int END   = 100'000'000;


/*
============================================================
PRIMES UP TO SQRT(10^8) = 10,000
============================================================
*/

vector<int> buildPrimes() {

    const int LIMIT = 10'000;

    vector<bool> isPrime(LIMIT + 1, true);

    isPrime[0] = false;
    isPrime[1] = false;

    for (int i = 2; i * i <= LIMIT; ++i) {

        if (!isPrime[i])
            continue;

        for (int j = i * i; j <= LIMIT; j += i)
            isPrime[j] = false;
    }

    vector<int> primes;

    for (int i = 2; i <= LIMIT; ++i)
        if (isPrime[i])
            primes.push_back(i);

    return primes;
}


/*
============================================================
FAST 8-DIGIT OUTER-IN TRANSFORMATION

For

    abcdefgh

we need

    ahbgcfd e

i.e.

    a h b g c f d e

Split into:

    abcd
    efgh

Precompute contributions from each half.
============================================================
*/

int LEFT_PART[10'000];
int RIGHT_PART[10'000];


void buildTransformTables() {

    for (int x = 0; x < 10'000; ++x) {

        int d1 = x / 1000;
        int d2 = (x / 100) % 10;
        int d3 = (x / 10) % 10;
        int d4 = x % 10;

        /*
            digits abcd contribute:

            a _ b _ c _ d _
        */

        LEFT_PART[x] =
            d1 * 10'000'000 +
            d2 *    100'000 +
            d3 *      1'000 +
            d4 *         10;


        /*
            digits efgh contribute:

            _ h _ g _ f _ e
        */

        RIGHT_PART[x] =
            d4 * 1'000'000 +
            d3 *    10'000 +
            d2 *       100 +
            d1;
    }
}


inline int transform8(int n) {

    int high = n / 10'000;
    int low  = n % 10'000;

    return LEFT_PART[high] + RIGHT_PART[low];
}


/*
============================================================
EXACT SAME PRIME-SUPPORT TEST

We want:

    rad(a) == rad(b)

where rad(x) = product of DISTINCT prime divisors.

Instead of fully factoring both independently, we factor
them simultaneously.

If p divides exactly one of them, we can immediately reject.

This is exact, not probabilistic.
============================================================
*/

bool samePrimeSupport(
    int n,
    int t,
    const vector<int>& primes,
    int64& radical
) {

    int a = n;
    int b = t;

    int64 r = 1;


    for (int p : primes) {

        /*
            Once p^2 exceeds both remaining numbers,
            both remainders are either 1 or primes.
        */

        if (1LL * p * p > a &&
            1LL * p * p > b)
            break;


        bool hasA = (a % p == 0);
        bool hasB = (b % p == 0);


        /*
            Prime support differs.
        */

        if (hasA != hasB)
            return false;


        /*
            p divides both.
            Include p once in radical and remove all
            copies from both numbers.
        */

        if (hasA) {

            r *= p;

            while (a % p == 0)
                a /= p;

            while (b % p == 0)
                b /= p;
        }
    }


    /*
        Remaining parts are now either:

            1
            or a single prime > 10,000.

        If only one remains, supports differ.

        If both remain, equality means they must be
        the SAME prime.
    */

    if (a == 1 && b == 1) {

        radical = r;
        return true;
    }

    if (a == 1 || b == 1)
        return false;

    if (a != b)
        return false;


    /*
        Same remaining prime.
    */

    r *= a;

    radical = r;

    return true;
}


/*
============================================================
FACTOR INFORMATION

Used only for solutions, so this is cheap.
============================================================
*/

vector<pair<int,int>> factorize(
    int n,
    const vector<int>& primes
) {

    vector<pair<int,int>> f;

    int x = n;

    for (int p : primes) {

        if (1LL * p * p > x)
            break;

        if (x % p != 0)
            continue;

        int e = 0;

        while (x % p == 0) {
            x /= p;
            ++e;
        }

        f.push_back({p,e});
    }

    if (x > 1)
        f.push_back({x,1});

    return f;
}


string factorString(
    int n,
    const vector<int>& primes
) {

    auto f = factorize(n, primes);

    string ans;

    for (int i = 0; i < (int)f.size(); ++i) {

        if (i)
            ans += " * ";

        ans += to_string(f[i].first);

        if (f[i].second > 1) {
            ans += "^";
            ans += to_string(f[i].second);
        }
    }

    return ans;
}


int omega(
    int n,
    const vector<int>& primes
) {

    return (int)factorize(n, primes).size();
}


int Omega(
    int n,
    const vector<int>& primes
) {

    auto f = factorize(n, primes);

    int result = 0;

    for (auto [p,e] : f)
        result += e;

    return result;
}


/*
============================================================
T-ORBIT

Build complete orbit of n.
============================================================
*/

vector<int> getOrbit(int n) {

    vector<int> orbit;

    int x = n;

    do {

        orbit.push_back(x);
        x = transform8(x);

    } while (x != n);

    return orbit;
}


/*
============================================================
MAIN
============================================================
*/

int main() {

    cout << "============================================\n";
    cout << "8-DIGIT OUTER-IN PRIME SUPPORT SEARCH\n";
    cout << "============================================\n";

    cout << "Range: "
         << START
         << " to "
         << END - 1
         << "\n\n";


    /*
        --------------------------------------------------------
        Build primes.
        --------------------------------------------------------
    */

    cerr << "Building primes up to 10000...\n";

    vector<int> primes = buildPrimes();

    cerr << "Primes: "
         << primes.size()
         << "\n";


    /*
        --------------------------------------------------------
        Build fast transformation lookup.
        --------------------------------------------------------
    */

    buildTransformTables();

    cerr << "Transformation tables ready.\n";


    /*
        --------------------------------------------------------
        Output files.
        --------------------------------------------------------
    */

    ofstream allFile("solutions_8digit.csv");

    allFile
        << "n,Tn,rad,omega_n,omega_Tn,"
           "Omega_n,Omega_Tn,gcd,"
           "primitive,orbit_length,"
           "matching_edges\n";


    ofstream primitiveFile("primitive_8digit.csv");

    primitiveFile
        << "n,Tn,rad,orbit_length,matching_edges\n";


    ofstream orbitFile("matching_8digit_orbits.csv");

    orbitFile
        << "representative,orbit_length,"
           "matching_edges,orbit\n";


    /*
        To avoid processing the same matching orbit repeatedly.
    */

    unordered_set<int> processedMatchingOrbits;

    processedMatchingOrbits.reserve(1024);


    /*
        --------------------------------------------------------
        Counters.
        --------------------------------------------------------
    */

    long long totalSolutions = 0;

    long long primitiveSolutions = 0;

    long long totalMatchingEdges = 0;

    long long maxMatchingEdges = 0;

    long long consecutiveMatches = 0;

    long long matchingOrbits = 0;

    int maxOrbitLength = 0;

    int firstCounterexample = -1;


    auto startTime = chrono::high_resolution_clock::now();


    /*
        --------------------------------------------------------
        MAIN SEARCH
        --------------------------------------------------------
    */

    cerr << "Searching 8-digit numbers...\n";

    for (int n = START; n < END; ++n) {

        int t = transform8(n);


        /*
            Fixed points aren't interesting.
        */

        if (n == t)
            continue;


        int64 rad = 0;


        /*
            Main condition.
        */

        if (!samePrimeSupport(n, t, primes, rad))
            continue;


        /*
            Found a new solution.
        */

        ++totalSolutions;
        ++totalMatchingEdges;


        bool primitive = (n % 10 != 0);

        if (primitive)
            ++primitiveSolutions;


        /*
            GCD and factor information.
        */

        long long g = gcd((long long)n, (long long)t);

        int omN = omega(n, primes);
        int omT = omega(t, primes);

        int OmN = Omega(n, primes);
        int OmT = Omega(t, primes);


        /*
            ----------------------------------------------------
            Analyze this orbit.

            Since we discovered an edge, build its complete
            orbit and count ALL support-preserving edges.
            ----------------------------------------------------
        */

        vector<int> orbit = getOrbit(n);

        int L = (int)orbit.size();

        maxOrbitLength = max(maxOrbitLength, L);


        int matchingEdges = 0;


        for (int i = 0; i < L; ++i) {

            int a = orbit[i];
            int b = orbit[(i + 1) % L];

            int64 dummy;

            if (samePrimeSupport(a, b, primes, dummy))
                ++matchingEdges;
        }


        maxMatchingEdges =
            max<long long>(maxMatchingEdges, matchingEdges);


        /*
            Count consecutive matching edges.
        */

        int consecutive = 0;

        for (int i = 0; i < L; ++i) {

            int a = orbit[i];
            int b = orbit[(i + 1) % L];
            int c = orbit[(i + 2) % L];

            int64 r1, r2;

            bool ab = samePrimeSupport(
                a,b,primes,r1
            );

            bool bc = samePrimeSupport(
                b,c,primes,r2
            );

            if (ab && bc)
                ++consecutive;
        }


        consecutiveMatches += consecutive;


        /*
            Save only one orbit entry per matching orbit.
        */

        int representative =
            *min_element(orbit.begin(), orbit.end());


        if (!processedMatchingOrbits.count(representative)) {

            processedMatchingOrbits.insert(representative);

            ++matchingOrbits;


            orbitFile
                << representative << ','
                << L << ','
                << matchingEdges
                << ",\"";

            for (int i = 0; i < L; ++i) {

                if (i)
                    orbitFile << " -> ";

                orbitFile << orbit[i];
            }

            orbitFile << "\"\n";
        }


        /*
            Record counterexample orbit if necessary.
        */

        if (matchingEdges >= 2 &&
            firstCounterexample == -1) {

            firstCounterexample =
                representative;
        }


        /*
            Main CSV.
        */

        allFile
            << n << ','
            << t << ','
            << rad << ','
            << omN << ','
            << omT << ','
            << OmN << ','
            << OmT << ','
            << g << ','
            << (primitive ? 1 : 0) << ','
            << L << ','
            << matchingEdges
            << '\n';


        if (primitive) {

            primitiveFile
                << n << ','
                << t << ','
                << rad << ','
                << L << ','
                << matchingEdges
                << '\n';
        }


        /*
            Progress every 10 million numbers.
        */

        if ((n - START) % 10'000'000 == 0 &&
            n != START) {

            double progress =
                100.0 *
                (double)(n - START) /
                (double)(END - START);

            cerr << "Progress: "
                 << fixed << setprecision(1)
                 << progress
                 << "%\n";
        }
    }


    auto endTime =
        chrono::high_resolution_clock::now();

    double seconds =
        chrono::duration<double>(
            endTime - startTime
        ).count();


    /*
        --------------------------------------------------------
        FINAL RESULT
        --------------------------------------------------------
    */

    cout << "\n============================================\n";
    cout << "SEARCH COMPLETE\n";
    cout << "============================================\n";

    cout << "Range searched: "
         << START
         << " <= n < "
         << END
         << '\n';

    cout << "New solutions: "
         << totalSolutions
         << '\n';

    cout << "Primitive solutions: "
         << primitiveSolutions
         << '\n';

    cout << "Matching orbits: "
         << matchingOrbits
         << '\n';

    cout << "Maximum matching edges in one orbit: "
         << maxMatchingEdges
         << '\n';

    cout << "Consecutive matching-edge occurrences: "
         << consecutiveMatches
         << '\n';

    cout << "Maximum orbit length: "
         << maxOrbitLength
         << '\n';

    cout << "Runtime: "
         << fixed << setprecision(3)
         << seconds
         << " seconds\n";


    /*
        --------------------------------------------------------
        CONJECTURE TEST
        --------------------------------------------------------
    */

    cout << "\n============================================\n";
    cout << "CONJECTURE TEST\n";
    cout << "============================================\n";

    if (firstCounterexample == -1) {

        cout
            << "No orbit with 2 or more matching edges found.\n";

    } else {

        cout
            << "COUNTEREXAMPLE FOUND!\n";

        cout
            << "Representative: "
            << firstCounterexample
            << '\n';
    }


    if (consecutiveMatches == 0) {

        cout
            << "No consecutive matching edges found.\n";

    } else {

        cout
            << "Consecutive matching edges FOUND!\n";
    }


    cout << "\nFiles:\n";
    cout << "  solutions_8digit.csv\n";
    cout << "  primitive_8digit.csv\n";
    cout << "  matching_8digit_orbits.csv\n";

    cout << "\nAnalysis complete.\n";

    return 0;
}