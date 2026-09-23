#include <bits/stdc++.h>
using namespace std;

/*
============================================================
 OUTER-IN DIGIT TRANSFORMATION
============================================================

T(12345678) = 18273645
T(12345)    = 15324
*/

long long T(long long n) {
    string s = to_string(n);
    string t;

    int l = 0;
    int r = (int)s.size() - 1;

    while (l <= r) {
        t.push_back(s[l]);

        if (l != r)
            t.push_back(s[r]);

        ++l;
        --r;
    }

    return stoll(t);
}


/*
============================================================
 PRIME FACTORIZATION
============================================================
*/

vector<pair<long long,int>> factorize(long long n) {

    vector<pair<long long,int>> f;

    for (long long p = 2; p * p <= n; ++p) {

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


long long radical(long long n) {

    auto f = factorize(n);

    long long r = 1;

    for (auto [p,e] : f)
        r *= p;

    return r;
}


int omega(long long n) {

    return (int)factorize(n).size();
}


int Omega(long long n) {

    int result = 0;

    for (auto [p,e] : factorize(n))
        result += e;

    return result;
}


string factorString(long long n) {

    auto f = factorize(n);

    string s;

    for (int i = 0; i < (int)f.size(); ++i) {

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
 PRIME SUPPORT AS A STRING
============================================================
*/

string supportString(long long n) {

    auto f = factorize(n);

    string s = "{";

    for (int i = 0; i < (int)f.size(); ++i) {

        if (i)
            s += ",";

        s += to_string(f[i].first);
    }

    s += "}";

    return s;
}


/*
============================================================
 DIGIT INFORMATION
============================================================
*/

string digitString(long long n) {
    return to_string(n);
}


int digitSum(long long n) {

    int s = 0;

    while (n > 0) {
        s += n % 10;
        n /= 10;
    }

    return s;
}


int numDigits(long long n) {
    return (int)to_string(n).size();
}


/*
============================================================
 EXPONENT DIFFERENCES

 Returns:
      v_p(T(n)) - v_p(n)

 for each prime occurring in either number.
============================================================
*/

map<long long,int> exponentMap(long long n) {

    map<long long,int> mp;

    for (auto [p,e] : factorize(n))
        mp[p] = e;

    return mp;
}


void printExponentDifference(long long n, long long tn) {

    auto a = exponentMap(n);
    auto b = exponentMap(tn);

    set<long long> primes;

    for (auto [p,e] : a)
        primes.insert(p);

    for (auto [p,e] : b)
        primes.insert(p);

    cout << "Exponent differences: ";

    bool first = true;

    for (long long p : primes) {

        int ea = a.count(p) ? a[p] : 0;
        int eb = b.count(p) ? b[p] : 0;

        int d = eb - ea;

        if (!first)
            cout << ", ";

        first = false;

        cout << p << ":" << d;
    }

    cout << '\n';
}


/*
============================================================
 MAIN
============================================================
*/

int main() {

    ifstream input("bfile.txt");

    if (!input) {
        cerr << "Could not open bfile.txt\n";
        return 1;
    }


    vector<long long> numbers;

    string line;

    while (getline(input,line)) {

        if (line.empty())
            continue;

        if (line[0] == '#')
            continue;

        stringstream ss(line);

        long long index;
        long long n;

        ss >> index >> n;

        if (ss)
            numbers.push_back(n);
    }


    cout << "============================================\n";
    cout << "OUTER-IN PRIME SUPPORT RESEARCH ANALYSIS\n";
    cout << "============================================\n";

    cout << "Loaded solutions: "
         << numbers.size() << "\n\n";


    /*
    ========================================================
     SECTION 1
     FULL SOLUTION TABLE
    ========================================================
    */

    cout << "FULL SOLUTION DATA\n";
    cout << "------------------------------------------------------------\n";

    for (long long n : numbers) {

        long long tn = T(n);
        long long t2 = T(tn);

        long long r1 = radical(n);
        long long r2 = radical(tn);
        long long r3 = radical(t2);

        long long g = gcd(n,tn);

        cout << "\nn = " << n << '\n';
        cout << "T(n) = " << tn << '\n';
        cout << "T^2(n) = " << t2 << '\n';

        cout << "factor(n) = "
             << factorString(n) << '\n';

        cout << "factor(T(n)) = "
             << factorString(tn) << '\n';

        cout << "factor(T^2(n)) = "
             << factorString(t2) << '\n';

        cout << "support = "
             << supportString(n) << '\n';

        cout << "rad(n) = " << r1 << '\n';
        cout << "rad(T(n)) = " << r2 << '\n';
        cout << "rad(T^2(n)) = " << r3 << '\n';

        cout << "omega(n) = "
             << omega(n)
             << ", omega(T(n)) = "
             << omega(tn)
             << '\n';

        cout << "Omega(n) = "
             << Omega(n)
             << ", Omega(T(n)) = "
             << Omega(tn)
             << '\n';

        cout << "gcd(n,T(n)) = "
             << g
             << '\n';

        cout << "n/gcd = "
             << n/g
             << '\n';

        cout << "T(n)/gcd = "
             << tn/g
             << '\n';

        printExponentDifference(n,tn);

        cout << "two-step support equality? ";

        if (r1 == r2 && r2 == r3)
            cout << "YES\n";
        else
            cout << "NO\n";
    }


    /*
    ========================================================
     SECTION 2
     SOLUTIONS BY DIGIT LENGTH
    ========================================================
    */

    map<int,int> digitCount;
    map<int,int> primitiveCount;

    for (long long n : numbers) {

        int d = numDigits(n);

        digitCount[d]++;

        if (n % 10 != 0)
            primitiveCount[d]++;
    }


    cout << "\n\nSOLUTIONS BY DIGIT LENGTH\n";
    cout << "------------------------------------------------------------\n";

    for (auto [d,c] : digitCount) {

        cout << d
             << " digits: "
             << c
             << " total, "
             << primitiveCount[d]
             << " primitive\n";
    }


    /*
    ========================================================
     SECTION 3
     PRIME SUPPORT GROUPS
    ========================================================
    */

    map<string,vector<long long>> supportGroups;

    for (long long n : numbers)
        supportGroups[supportString(n)].push_back(n);


    cout << "\n\nGROUPED BY PRIME SUPPORT\n";
    cout << "------------------------------------------------------------\n";

    for (auto &[support,vals] : supportGroups) {

        cout << support << " : ";

        for (long long n : vals)
            cout << n << " ";

        cout << '\n';
    }


    /*
    ========================================================
     SECTION 4
     OMEGA / OMEGA TOTAL PATTERNS
    ========================================================
    */

    map<pair<int,int>,vector<long long>> omegaGroups;

    map<int,vector<long long>> deltaOmega;

    for (long long n : numbers) {

        long long tn = T(n);

        int a = Omega(n);
        int b = Omega(tn);

        omegaGroups[{a,b}].push_back(n);

        deltaOmega[b-a].push_back(n);
    }


    cout << "\n\nOMEGA TOTAL TRANSITIONS\n";
    cout << "------------------------------------------------------------\n";

    for (auto &[k,vals] : omegaGroups) {

        cout << "Omega: "
             << k.first
             << " -> "
             << k.second
             << " : ";

        for (auto n : vals)
            cout << n << " ";

        cout << '\n';
    }


    cout << "\n\nDELTA OMEGA DISTRIBUTION\n";
    cout << "------------------------------------------------------------\n";

    for (auto &[d,vals] : deltaOmega) {

        cout << "Delta = "
             << d
             << " : "
             << vals.size()
             << " cases\n";
    }


    /*
    ========================================================
     SECTION 5
     GCD STRUCTURE
    ========================================================
    */

    cout << "\n\nGCD / COPRIME REMAINDER STRUCTURE\n";
    cout << "------------------------------------------------------------\n";

    for (long long n : numbers) {

        long long tn = T(n);
        long long g = gcd(n,tn);

        cout << n
             << " : "
             << n/g
             << " / "
             << tn/g
             << '\n';
    }


    /*
    ========================================================
     SECTION 6
     T-ORBIT
    ========================================================
    */

    cout << "\n\nT ORBITS\n";
    cout << "------------------------------------------------------------\n";

    for (long long n : numbers) {

        cout << n << " : ";

        long long x = n;

        do {

            cout << x << " -> ";

            x = T(x);

        } while (x != n);

        cout << n << '\n';
    }


    /*
    ========================================================
     SECTION 7
     CHECK STRONGER CONDITIONS
    ========================================================
    */

    int twoStep = 0;
    int threeStep = 0;

    for (long long n : numbers) {

        long long a = n;
        long long b = T(a);
        long long c = T(b);
        long long d = T(c);

        long long r = radical(a);

        if (radical(b) == r &&
            radical(c) == r)
            twoStep++;

        if (radical(b) == r &&
            radical(c) == r &&
            radical(d) == r)
            threeStep++;
    }


    cout << "\n\nSTRONGER ORBIT CONDITIONS\n";
    cout << "------------------------------------------------------------\n";

    cout << "rad(n) = rad(T(n)) = rad(T^2(n)): "
         << twoStep << '\n';

    cout << "rad(n) = rad(T(n)) = rad(T^2(n)) = rad(T^3(n)): "
         << threeStep << '\n';


    /*
    ========================================================
     SECTION 8
     10-PRIMITIVE SOLUTIONS
    ========================================================
    */

    cout << "\n\n10-PRIMITIVE SOLUTIONS\n";
    cout << "------------------------------------------------------------\n";

    for (long long n : numbers) {

        if (n % 10 != 0)
            cout << n << " -> " << T(n) << '\n';
    }


    cout << "\n\nAnalysis complete.\n";

    return 0;
}