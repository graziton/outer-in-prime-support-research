#include <bits/stdc++.h>
using namespace std;

/*
    ------------------------------------------------------------
    OUTER-IN DIGIT TRANSFORMATION

    Example:
        12345678 -> 18273645
        1234567  -> 1726354
        1800     -> 1080

    Definition:
        first, last, second, second-last, ...
    ------------------------------------------------------------
*/

int transformNumber(int n) {
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

    return stoi(t);
}


/*
    ------------------------------------------------------------
    Prime factorization from SPF table.

    Returns:
        vector<pair<prime, exponent>>
    ------------------------------------------------------------
*/

vector<pair<int, int>> factorize(int n, const vector<int>& spf) {
    vector<pair<int, int>> factors;

    while (n > 1) {
        int p = spf[n];
        int e = 0;

        while (n % p == 0) {
            n /= p;
            ++e;
        }

        factors.push_back({p, e});
    }

    return factors;
}


/*
    ------------------------------------------------------------
    rad(n) = product of DISTINCT prime factors of n
    ------------------------------------------------------------
*/

long long radicalFromSPF(int n, const vector<int>& spf) {
    long long r = 1;

    while (n > 1) {
        int p = spf[n];
        r *= p;

        while (n % p == 0)
            n /= p;
    }

    return r;
}


/*
    ------------------------------------------------------------
    Omega(n):
    total number of prime factors WITH multiplicity.

    Example:
        1800 = 2^3 * 3^2 * 5^2

        Omega(1800) = 3 + 2 + 2 = 7
    ------------------------------------------------------------
*/

int omega(int n, const vector<int>& spf) {
    int result = 0;

    while (n > 1) {
        int p = spf[n];

        while (n % p == 0) {
            n /= p;
            ++result;
        }
    }

    return result;
}


/*
    ------------------------------------------------------------
    omega_distinct(n):
    number of DISTINCT prime factors.

    Example:
        1800 = 2^3 * 3^2 * 5^2

        omega_distinct(1800) = 3
    ------------------------------------------------------------
*/

int omega_distinct(int n, const vector<int>& spf) {
    int result = 0;

    while (n > 1) {
        int p = spf[n];
        ++result;

        while (n % p == 0)
            n /= p;
    }

    return result;
}


/*
    ------------------------------------------------------------
    Prime factorization as a readable string
    ------------------------------------------------------------
*/

string factorizationString(int n, const vector<int>& spf) {
    auto f = factorize(n, spf);

    if (f.empty())
        return "1";

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


/*
    ------------------------------------------------------------
    Digit sum
    ------------------------------------------------------------
*/

int digitSum(int n) {
    int s = 0;

    while (n > 0) {
        s += n % 10;
        n /= 10;
    }

    return s;
}


/*
    ------------------------------------------------------------
    GCD helper
    ------------------------------------------------------------
*/

long long gcdll(long long a, long long b) {
    return std::gcd(a, b);
}


/*
    ------------------------------------------------------------
    MAIN
    ------------------------------------------------------------
*/

int main() {

    /*
        Start here.

        10^7 = 10,000,000

        Increase only after verifying everything.
    */

    const int LIMIT = 10'000'000;

    cerr << "Building SPF up to " << LIMIT << "...\n";


    /*
        --------------------------------------------------------
        SMALLEST PRIME FACTOR SIEVE
        --------------------------------------------------------
    */

    vector<int> spf(LIMIT + 1);

    for (int i = 0; i <= LIMIT; ++i)
        spf[i] = i;

    spf[0] = 0;

    if (LIMIT >= 1)
        spf[1] = 1;

    for (int i = 2; 1LL * i * i <= LIMIT; ++i) {

        if (spf[i] != i)
            continue;

        for (long long j = 1LL * i * i;
             j <= LIMIT;
             j += i) {

            if (spf[j] == j)
                spf[j] = i;
        }
    }

    cerr << "SPF completed.\n";


    /*
        --------------------------------------------------------
        PRECOMPUTE RAD(n)

        Since rad(n) <= n, int64 is plenty.
        Having this array makes the main search much faster.
        --------------------------------------------------------
    */

    vector<long long> rad(LIMIT + 1, 1);

    for (int p = 2; p <= LIMIT; ++p) {

        if (spf[p] != p)
            continue;  // not prime

        for (int x = p; x <= LIMIT; x += p)
            rad[x] *= p;
    }

    cerr << "rad(n) completed.\n";


    /*
        --------------------------------------------------------
        OUTPUT FILES
        --------------------------------------------------------
    */

    ofstream allFile("all_solutions.csv");
    ofstream primitiveFile("primitive_solutions.csv");
    ofstream orbitFile("two_step_solutions.csv");
    ofstream bFile("bfile.txt");

    allFile
        << "n,Tn,rad_n,rad_Tn,"
           "omega_n,omega_Tn,"
           "Omega_n,Omega_Tn,"
           "gcd_n_Tn,"
           "digit_sum_n,"
           "factorization_n,"
           "factorization_Tn\n";

    primitiveFile
        << "n,Tn,rad,"
           "omega_n,omega_Tn,"
           "Omega_n,Omega_Tn,"
           "gcd,"
           "factorization_n,"
           "factorization_Tn\n";

    orbitFile
        << "n,Tn,T2n,rad\n";

    bFile << "#  n such that rad(n) = rad(T(n)), n != T(n)\n";
    bFile << "#  Generated up to " << LIMIT << "\n";
    bFile << "#\n";


    /*
        --------------------------------------------------------
        COUNTERS
        --------------------------------------------------------
    */

    long long totalSolutions = 0;
    long long primitiveSolutions = 0;
    long long twoStepSolutions = 0;

    vector<long long> byDigits(20, 0);
    vector<long long> primitiveByDigits(20, 0);


    /*
        --------------------------------------------------------
        MAIN SEARCH
        --------------------------------------------------------
    */

    cerr << "Searching...\n";

    for (int n = 1; n <= LIMIT; ++n) {

        int Tn = transformNumber(n);

        /*
            Ignore fixed points:
                n == T(n)
        */
        if (n == Tn)
            continue;

        /*
            Main condition:
                rad(n) == rad(T(n))
        */
        if (rad[n] != rad[Tn])
            continue;


        /*
            We found a solution.
        */

        ++totalSolutions;

        int digits = (int)to_string(n).size();

        if (digits < (int)byDigits.size())
            ++byDigits[digits];


        /*
            Extra arithmetic information.
        */

        int omN = omega_distinct(n, spf);
        int omT = omega_distinct(Tn, spf);

        int OmN = omega(n, spf);
        int OmT = omega(Tn, spf);

        long long g = gcdll(n, Tn);


        /*
            Full output.
        */

        allFile
            << n << ','
            << Tn << ','
            << rad[n] << ','
            << rad[Tn] << ','
            << omN << ','
            << omT << ','
            << OmN << ','
            << OmT << ','
            << g << ','
            << digitSum(n) << ','
            << '"'
            << factorizationString(n, spf)
            << "\",\""
            << factorizationString(Tn, spf)
            << "\"\n";


        /*
            OEIS-style b-file:
            Just the sequence terms.
        */

        bFile << totalSolutions << " " << n << "\n";


        /*
            ----------------------------------------------------
            10-PRIMITIVE SOLUTIONS

            Exclude numbers ending in 0.

            These are particularly important because appending
            zeros can create obvious infinite families.
            ----------------------------------------------------
        */

        if (n % 10 != 0) {

            ++primitiveSolutions;

            if (digits < (int)primitiveByDigits.size())
                ++primitiveByDigits[digits];

            primitiveFile
                << n << ','
                << Tn << ','
                << rad[n] << ','
                << omN << ','
                << omT << ','
                << OmN << ','
                << OmT << ','
                << g << ','
                << '"'
                << factorizationString(n, spf)
                << "\",\""
                << factorizationString(Tn, spf)
                << "\"\n";
        }


        /*
            ----------------------------------------------------
            TWO-STEP CONDITION

            n -> T(n) -> T(T(n))

            Require:

                rad(n)
                =
                rad(T(n))
                =
                rad(T^2(n))

            and make sure we aren't looking at a fixed point.
            ----------------------------------------------------
        */

        int T2n = transformNumber(Tn);

        if (rad[n] == rad[T2n]) {

            ++twoStepSolutions;

            orbitFile
                << n << ','
                << Tn << ','
                << T2n << ','
                << rad[n] << '\n';
        }
    }


    /*
        --------------------------------------------------------
        SUMMARY
        --------------------------------------------------------
    */

    cout << "\n============================================\n";
    cout << "SEARCH COMPLETE\n";
    cout << "============================================\n";

    cout << "Limit: " << LIMIT << '\n';

    cout << "Nontrivial solutions: "
         << totalSolutions << '\n';

    cout << "10-primitive solutions: "
         << primitiveSolutions << '\n';

    cout << "Two-step solutions: "
         << twoStepSolutions << '\n';


    /*
        Distribution by number of digits.
    */

    cout << "\nSolutions by digit length:\n";

    for (int d = 1; d < (int)byDigits.size(); ++d) {

        if (byDigits[d] == 0)
            continue;

        cout << d << " digits: "
             << byDigits[d] << '\n';
    }


    cout << "\nPrimitive solutions by digit length:\n";

    for (int d = 1; d < (int)primitiveByDigits.size(); ++d) {

        if (primitiveByDigits[d] == 0)
            continue;

        cout << d << " digits: "
             << primitiveByDigits[d] << '\n';
    }


    cout << "\nFiles created:\n";
    cout << "  all_solutions.csv\n";
    cout << "  primitive_solutions.csv\n";
    cout << "  two_step_solutions.csv\n";
    cout << "  bfile.txt\n";

    return 0;
}