#include <bits/stdc++.h>
using namespace std;

using int64 = long long;

struct Record {
    int64 n;
    int64 t;
};


/*
============================================================
OUTER-IN TRANSFORMATION
Works for numbers of arbitrary length that fit in uint64_t.
============================================================
*/

uint64_t T(uint64_t n) {

    string s = to_string(n);
    string r;

    int l = 0;
    int h = (int)s.size() - 1;

    while (l <= h) {

        r.push_back(s[l]);

        if (l != h)
            r.push_back(s[h]);

        ++l;
        --h;
    }

    uint64_t x = 0;

    for (char c : r)
        x = x * 10 + (c - '0');

    return x;
}


/*
============================================================
FACTORIZATION
Only called for the 19 known solutions, so speed is irrelevant.
============================================================
*/

vector<pair<uint64_t,int>> factorize(uint64_t n) {

    vector<pair<uint64_t,int>> f;

    for (uint64_t p = 2; p * p <= n; ++p) {

        if (n % p != 0)
            continue;

        int e = 0;

        while (n % p == 0) {

            n /= p;
            ++e;
        }

        f.push_back({p,e});
    }

    if (n > 1)
        f.push_back({n,1});

    return f;
}


string factorString(uint64_t n) {

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


string supportString(uint64_t n) {

    auto f = factorize(n);

    string s = "{";

    for (size_t i = 0; i < f.size(); ++i) {

        if (i)
            s += ",";

        s += to_string(f[i].first);
    }

    s += "}";

    return s;
}


/*
============================================================
RADICAL
============================================================
*/

uint64_t rad(uint64_t n) {

    uint64_t r = 1;

    for (auto [p,e] : factorize(n))
        r *= p;

    return r;
}


/*
============================================================
OMEGA / OMEGA WITH MULTIPLICITY
============================================================
*/

int omega(uint64_t n) {

    return (int)factorize(n).size();
}


int Omega(uint64_t n) {

    int s = 0;

    for (auto [p,e] : factorize(n))
        s += e;

    return s;
}


/*
============================================================
READ CSV

We only need first two columns:
n,Tn
============================================================
*/

vector<Record> readCSV(
    const string& filename
) {

    ifstream in(filename);

    vector<Record> result;

    if (!in) {

        cerr
            << "Could not open "
            << filename
            << "\n";

        return result;
    }

    string line;

    getline(in,line); // header

    while (getline(in,line)) {

        if (line.empty())
            continue;

        stringstream ss(line);

        string a,b;

        getline(ss,a,',');
        getline(ss,b,',');

        if (a.empty() || b.empty())
            continue;

        Record r;

        r.n = stoull(a);
        r.t = stoull(b);

        result.push_back(r);
    }

    return result;
}


/*
============================================================
EXPONENT DIFFERENCES
============================================================
*/

map<uint64_t,int> exponentMap(uint64_t n) {

    map<uint64_t,int> m;

    for (auto [p,e] : factorize(n))
        m[p] = e;

    return m;
}


string exponentDifference(uint64_t n, uint64_t t) {

    auto A = exponentMap(n);
    auto B = exponentMap(t);

    set<uint64_t> primes;

    for (auto [p,e] : A)
        primes.insert(p);

    for (auto [p,e] : B)
        primes.insert(p);

    string s;

    for (uint64_t p : primes) {

        int a = A.count(p) ? A[p] : 0;
        int b = B.count(p) ? B[p] : 0;

        int d = b - a;

        if (!s.empty())
            s += ", ";

        s += to_string(p);
        s += ":";
        s += (d >= 0 ? "+" : "");
        s += to_string(d);
    }

    return s;
}


/*
============================================================
MAIN
============================================================
*/

int main() {

    cout << "============================================\n";
    cout << "PRIMITIVE SOLUTION RESEARCH ANALYSIS\n";
    cout << "============================================\n";


    /*
        Load both datasets.
    */

    vector<Record> A =
        readCSV("primitive_solutions.csv");

    vector<Record> B =
        readCSV("primitive_9digit_fast.csv");


    vector<Record> all = A;

    all.insert(
        all.end(),
        B.begin(),
        B.end()
    );


    /*
        Sort and remove duplicates.
    */

    sort(
        all.begin(),
        all.end(),
        [](const Record& a, const Record& b) {
            return a.n < b.n;
        }
    );

    all.erase(
        unique(
            all.begin(),
            all.end(),
            [](const Record& a, const Record& b) {
                return a.n == b.n;
            }
        ),
        all.end()
    );


    cout << "\nTotal primitive solutions loaded: "
         << all.size()
         << "\n";


    /*
    ========================================================
    1. MAIN TABLE
    ========================================================
    */

    cout << "\n============================================\n";
    cout << "ALL PRIMITIVE SOLUTIONS\n";
    cout << "============================================\n";

    cout
        << left
        << setw(12) << "n"
        << setw(12) << "T(n)"
        << setw(12) << "gcd"
        << setw(14) << "a=n/g"
        << setw(14) << "b=T/g"
        << setw(12) << "rad"
        << "\n";

    cout << string(76,'-') << "\n";


    /*
        Maps used later.
    */

    map<string,vector<uint64_t>> supportGroups;

    map<pair<uint64_t,uint64_t>,vector<uint64_t>>
        reducedGroups;

    map<int,vector<uint64_t>> lengthGroups;


    for (auto r : all) {

        uint64_t g = gcd(
            (uint64_t)r.n,
            (uint64_t)r.t
        );

        uint64_t a = r.n / g;
        uint64_t b = r.t / g;

        uint64_t R = rad(r.n);

        cout
            << setw(12) << r.n
            << setw(12) << r.t
            << setw(12) << g
            << setw(14) << a
            << setw(14) << b
            << setw(12) << R
            << "\n";


        supportGroups[
            supportString(r.n)
        ].push_back(r.n);


        reducedGroups[{a,b}]
            .push_back(r.n);


        lengthGroups[
            (int)to_string(r.n).size()
        ].push_back(r.n);
    }


    /*
    ========================================================
    2. EXPONENT DIFFERENCES
    ========================================================
    */

    cout << "\n============================================\n";
    cout << "EXPONENT DIFFERENCE VECTORS\n";
    cout << "============================================\n";

    for (auto r : all) {

        cout
            << r.n
            << " -> "
            << r.t
            << " : "
            << exponentDifference(r.n,r.t)
            << "\n";
    }


    /*
    ========================================================
    3. REDUCED PAIR REPETITIONS
    ========================================================
    */

    cout << "\n============================================\n";
    cout << "REPEATED REDUCED PAIRS\n";
    cout << "============================================\n";

    bool anyRepeated = false;

    for (auto& [pair,vals] : reducedGroups) {

        if (vals.size() <= 1)
            continue;

        anyRepeated = true;

        cout
            << pair.first
            << " : "
            << pair.second
            << " -> ";

        for (auto n : vals)
            cout << n << " ";

        cout << "\n";
    }

    if (!anyRepeated)
        cout << "No repeated reduced pair.\n";


    /*
    ========================================================
    4. REPEATED PRIME SUPPORT
    ========================================================
    */

    cout << "\n============================================\n";
    cout << "PRIME-SUPPORT GROUPS\n";
    cout << "============================================\n";

    for (auto& [support,vals] : supportGroups) {

        if (vals.size() > 1) {

            cout
                << support
                << " : ";

            for (auto n : vals)
                cout << n << " ";

            cout << "\n";
        }
    }


    /*
    ========================================================
    5. DIGIT-LENGTH DISTRIBUTION
    ========================================================
    */

    cout << "\n============================================\n";
    cout << "DIGIT-LENGTH DISTRIBUTION\n";
    cout << "============================================\n";

    for (auto& [d,vals] : lengthGroups) {

        cout
            << d
            << " digits : "
            << vals.size()
            << " primitive solutions\n";
    }


    /*
    ========================================================
    6. OMEGA TRANSITIONS
    ========================================================
    */

    cout << "\n============================================\n";
    cout << "OMEGA TRANSITIONS\n";
    cout << "============================================\n";

    map<pair<int,int>,vector<uint64_t>> omegaTransitions;

    for (auto r : all) {

        omegaTransitions[
            {Omega(r.n),Omega(r.t)}
        ].push_back(r.n);
    }

    for (auto& [pair,vals] : omegaTransitions) {

        cout
            << pair.first
            << " -> "
            << pair.second
            << " : ";

        for (auto n : vals)
            cout << n << " ";

        cout << "\n";
    }


    /*
    ========================================================
    7. ORBIT CHECK

    Every primitive solution's orbit must have:
        - exactly one support-preserving edge
        - no consecutive matching edges
    ========================================================
    */

    cout << "\n============================================\n";
    cout << "ORBIT CHECK\n";
    cout << "============================================\n";

    int badOrbits = 0;

    for (auto r : all) {

        vector<uint64_t> orbit;

        uint64_t x = r.n;

        while (
            find(
                orbit.begin(),
                orbit.end(),
                x
            )
            == orbit.end()
        ) {

            orbit.push_back(x);
            x = T(x);
        }


        int matches = 0;

        int consecutive = 0;

        int L = orbit.size();


        for (int i = 0; i < L; ++i) {

            uint64_t a = orbit[i];

            uint64_t b =
                orbit[(i+1)%L];

            if (rad(a) == rad(b))
                ++matches;
        }


        for (int i = 0; i < L; ++i) {

            uint64_t a = orbit[i];

            uint64_t b =
                orbit[(i+1)%L];

            uint64_t c =
                orbit[(i+2)%L];

            if (
                rad(a) == rad(b) &&
                rad(b) == rad(c)
            )
                ++consecutive;
        }


        cout
            << r.n
            << " : orbit length "
            << L
            << ", matching edges "
            << matches
            << ", consecutive "
            << consecutive
            << "\n";


        if (matches != 1 ||
            consecutive != 0)
            ++badOrbits;
    }


    /*
    ========================================================
    FINAL
    ========================================================
    */

    cout << "\n============================================\n";
    cout << "FINAL SUMMARY\n";
    cout << "============================================\n";

    cout
        << "Primitive solutions: "
        << all.size()
        << "\n";

    cout
        << "Problematic orbits: "
        << badOrbits
        << "\n";

    if (badOrbits == 0)
        cout
            << "All known primitive solutions have exactly\n"
            << "one support-preserving edge and no consecutive\n"
            << "support-preserving edges.\n";

    return 0;
}