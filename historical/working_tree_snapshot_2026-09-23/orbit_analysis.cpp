#include <bits/stdc++.h>
using namespace std;

/*
============================================================
OUTER-IN TRANSFORMATION

12345678 -> 18273645
1234567  -> 1726354
1800     -> 1080
============================================================
*/

int T(int n) {
    string s = to_string(n);
    string t;
    t.reserve(s.size());

    int l = 0;
    int r = (int)s.size() - 1;

    while (l <= r) {
        t.push_back(s[l]);

        if (l != r)
            t.push_back(s[r]);

        ++l;
        --r;
    }

    int result = 0;

    for (char c : t)
        result = result * 10 + (c - '0');

    return result;
}


/*
============================================================
BUILD rad(n)

rad(n) = product of distinct prime divisors of n

Example:

1800 = 2^3 * 3^2 * 5^2
rad(1800) = 2*3*5 = 30
============================================================
*/

void buildRad(int limit, vector<int>& rad) {

    vector<bool> isPrime(limit + 1, true);

    isPrime[0] = false;
    isPrime[1] = false;

    for (int i = 2; 1LL * i * i <= limit; ++i) {

        if (!isPrime[i])
            continue;

        for (long long j = 1LL * i * i;
             j <= limit;
             j += i) {

            isPrime[(int)j] = false;
        }
    }

    rad.assign(limit + 1, 1);

    for (int p = 2; p <= limit; ++p) {

        if (!isPrime[p])
            continue;

        for (int x = p; x <= limit; x += p)
            rad[x] *= p;
    }
}


/*
============================================================
NUMBER OF DIGITS
============================================================
*/

int digitCount(int n) {
    return (int)to_string(n).size();
}


/*
============================================================
MAIN
============================================================
*/

int main() {

    /*
        Start with exactly the range we already searched.

        After verification, we can increase this.
    */

    const int LIMIT = 10'000'000;


    cout << "============================================\n";
    cout << "OUTER-IN ORBIT ANALYSIS\n";
    cout << "============================================\n";
    cout << "Limit: " << LIMIT << "\n\n";


    /*
    --------------------------------------------------------
    BUILD RADICAL TABLE
    --------------------------------------------------------
    */

    cerr << "Building rad(n)...\n";

    vector<int> rad;

    buildRad(LIMIT, rad);

    cerr << "rad(n) completed.\n";


    /*
    --------------------------------------------------------
    VISITED ARRAY

    Every number belongs to exactly one T-orbit.

    We process every orbit exactly once.
    --------------------------------------------------------
    */

    vector<unsigned char> visited(LIMIT + 1, 0);


    /*
    --------------------------------------------------------
    GLOBAL STATISTICS
    --------------------------------------------------------
    */

    long long orbitCount = 0;

    long long nontrivialOrbitCount = 0;

    long long matchingEdgeCount = 0;

    long long orbitsWithMatch = 0;

    long long maxMatchingEdges = 0;

    long long consecutiveMatchingCases = 0;

    long long primitiveMatchingEdges = 0;

    int maxOrbitLength = 0;


    /*
        Store the first counterexample, if one exists.
    */

    vector<int> counterexampleOrbit;


    /*
    --------------------------------------------------------
    OUTPUT FILES
    --------------------------------------------------------
    */

    ofstream matchingFile("matching_orbits.csv");

    matchingFile
        << "representative,orbit_length,matching_edges,"
           "primitive_matching_edges,orbit\n";

    ofstream counterFile("counterexamples.txt");

    ofstream orbitStats("orbit_statistics.csv");

    orbitStats
        << "representative,digits,orbit_length,"
           "matching_edges,consecutive_matching_edges\n";


    /*
    --------------------------------------------------------
    PROCESS EVERY ORBIT
    --------------------------------------------------------
    */

    cerr << "Processing orbits...\n";

    for (int start = 1; start <= LIMIT; ++start) {

        if (visited[start])
            continue;


        /*
            Construct this entire orbit.
        */

        vector<int> orbit;

        int x = start;

        while (!visited[x]) {

            visited[x] = 1;

            orbit.push_back(x);

            x = T(x);
        }


        ++orbitCount;

        int L = (int)orbit.size();

        maxOrbitLength = max(maxOrbitLength, L);


        /*
            Ignore trivial fixed-point orbits when testing
            our conjecture.
        */

        if (L == 1)
            continue;

        ++nontrivialOrbitCount;


        /*
        ----------------------------------------------------
        COUNT MATCHING EDGES

        Edge i is:

            orbit[i] -> orbit[(i+1)%L]

        We include the closing edge back to the start.
        ----------------------------------------------------
        */

        int matchingEdges = 0;

        int primitiveMatching = 0;

        int consecutiveEdges = 0;

        for (int i = 0; i < L; ++i) {

            int a = orbit[i];
            int b = orbit[(i + 1) % L];

            bool match = (rad[a] == rad[b]);

            if (match) {

                ++matchingEdges;
                ++matchingEdgeCount;

                if (a % 10 != 0)
                    ++primitiveMatching;
            }
        }


        /*
        ----------------------------------------------------
        COUNT CONSECUTIVE MATCHING EDGES

        A pair of consecutive edges means:

            rad(a) = rad(b)
            rad(b) = rad(c)

        which is precisely the stronger two-step condition.
        ----------------------------------------------------
        */

        for (int i = 0; i < L; ++i) {

            int a = orbit[i];
            int b = orbit[(i + 1) % L];
            int c = orbit[(i + 2) % L];

            if (rad[a] == rad[b] &&
                rad[b] == rad[c]) {

                ++consecutiveEdges;
                ++consecutiveMatchingCases;
            }
        }


        primitiveMatchingEdges += primitiveMatching;


        if (matchingEdges > 0)
            ++orbitsWithMatch;


        maxMatchingEdges =
            max<long long>(maxMatchingEdges, matchingEdges);


        /*
        ----------------------------------------------------
        SAVE ORBIT STATISTICS
        ----------------------------------------------------
        */

        orbitStats
            << orbit[0] << ','
            << digitCount(orbit[0]) << ','
            << L << ','
            << matchingEdges << ','
            << consecutiveEdges << '\n';


        /*
        ----------------------------------------------------
        SAVE ORBITS CONTAINING MATCHES
        ----------------------------------------------------
        */

        if (matchingEdges > 0) {

            matchingFile
                << orbit[0] << ','
                << L << ','
                << matchingEdges << ','
                << primitiveMatching << ",\"";

            for (int i = 0; i < L; ++i) {

                if (i)
                    matchingFile << " -> ";

                matchingFile << orbit[i];
            }

            matchingFile << "\"\n";
        }


        /*
        ----------------------------------------------------
        COUNTEREXAMPLE

        Our conjecture predicts:

            matchingEdges <= 1

        So save any orbit violating this.
        ----------------------------------------------------
        */

        if (matchingEdges >= 2 &&
            counterexampleOrbit.empty()) {

            counterexampleOrbit = orbit;

            counterFile
                << "FIRST COUNTEREXAMPLE\n\n";

            counterFile
                << "Orbit length: "
                << L << "\n\n";

            for (int i = 0; i < L; ++i) {

                int a = orbit[i];
                int b = orbit[(i + 1) % L];

                counterFile
                    << a
                    << " -> "
                    << b
                    << "   ";

                if (rad[a] == rad[b])
                    counterFile << "MATCH";

                counterFile << '\n';
            }
        }
    }


    /*
    ========================================================
    FINAL RESULTS
    ========================================================
    */

    cout << "\n============================================\n";
    cout << "RESULTS\n";
    cout << "============================================\n";

    cout << "Total T-orbits: "
         << orbitCount << '\n';

    cout << "Nontrivial orbits: "
         << nontrivialOrbitCount << '\n';

    cout << "Orbits containing a matching edge: "
         << orbitsWithMatch << '\n';

    cout << "Total matching edges: "
         << matchingEdgeCount << '\n';

    cout << "Maximum matching edges in one orbit: "
         << maxMatchingEdges << '\n';

    cout << "Maximum orbit length: "
         << maxOrbitLength << '\n';

    cout << "Consecutive matching-edge occurrences: "
         << consecutiveMatchingCases << '\n';

    cout << "Primitive matching edges: "
         << primitiveMatchingEdges << '\n';


    /*
    ========================================================
    CONJECTURE TEST
    ========================================================
    */

    cout << "\n============================================\n";
    cout << "CONJECTURE TEST\n";
    cout << "============================================\n";

    cout << "Conjecture:\n";
    cout << "Every nontrivial orbit contains at most one\n";
    cout << "edge with equal prime-support sets.\n\n";


    if (maxMatchingEdges <= 1) {

        cout << "RESULT: NO COUNTEREXAMPLE FOUND.\n";
        cout << "Maximum observed matching edges = "
             << maxMatchingEdges << "\n";

    } else {

        cout << "RESULT: COUNTEREXAMPLE FOUND.\n";
        cout << "Maximum matching edges = "
             << maxMatchingEdges << "\n";

        cout << "\nCounterexample orbit:\n";

        for (int i = 0;
             i < (int)counterexampleOrbit.size();
             ++i) {

            if (i)
                cout << " -> ";

            cout << counterexampleOrbit[i];
        }

        cout << " -> "
             << counterexampleOrbit[0]
             << '\n';
    }


    /*
    ========================================================
    STRONGER CONJECTURE

    No two consecutive matching edges.
    ========================================================
    */

    cout << "\n============================================\n";
    cout << "TWO-STEP CONDITION\n";
    cout << "============================================\n";

    cout << "rad(n) = rad(T(n)) = rad(T^2(n))\n";
    cout << "Occurrences found: "
         << consecutiveMatchingCases << '\n';

    if (consecutiveMatchingCases == 0) {

        cout << "NO EXAMPLE FOUND in the search range.\n";

    } else {

        cout << "EXAMPLES FOUND.\n";
    }


    /*
    ========================================================
    FILES
    ========================================================
    */

    cout << "\nFiles created:\n";
    cout << "  matching_orbits.csv\n";
    cout << "  counterexamples.txt\n";
    cout << "  orbit_statistics.csv\n";

    cout << "\nAnalysis complete.\n";

    return 0;
}