#include <bits/stdc++.h>
#include <boost/multiprecision/cpp_int.hpp>
using namespace std;
using boost::multiprecision::cpp_int;

// ================================================================
// OUTER-IN PRIME-SUPPORT SEARCH, v2
// ================================================================
// T(d1 d2 ... dk) = d1 dk d2 d{k-1} d3 d{k-2} ...
//
// We search the 12 REDUCED RATIO CLASSES already discovered:
//     n : T(n) = a : b
// so that
//     a*T(n) = b*n.
//
// For gcd(a,b)=1, writing n=a*g, T(n)=b*g gives the exact condition
//     rad(n)=rad(T(n))  <=>  rad(a*b) | g
//                              <=> a*rad(a*b) | n.
//
// This program searches the PRIMITIVE problem only:
//     n does not end in 0.
//
// Correctness safeguards:
//  * fixed points n=T(n) are discarded
//  * self-mapped positions enforce x=y
//  * every candidate is independently verified with cpp_int
//  * support equality is checked via the exact divisibility criterion
//  * per (ratio,length) node cap reports INCOMPLETE instead of silently
//    claiming an exhaustive search when a search becomes too large.
//
// Usage:
//     all_ratio_search_v2.exe
//     all_ratio_search_v2.exe 30
//     all_ratio_search_v2.exe 50 100000000
//     all_ratio_search_v2.exe 50 100000000 0   // all ratios
//     all_ratio_search_v2.exe 50 100000000 2   // only ratio index 2
//
// Ratio index is 1-based in printed output.
// ================================================================

struct Ratio {
    long long a, b;
    string name() const { return to_string(a) + ":" + to_string(b); }
};

struct Solver {
    Ratio r;
    int L = 0;
    unsigned long long nodeLimit = 100000000ULL;

    vector<int> tmap;          // output LSD position i gets input LSD digit tmap[i]

    long long M = 1;            // a*rad(a*b)
    long long lowFactor = 1;    // 2^v2(M) * 5^v5(M)
    int lowDigits = 0;          // number of low decimal digits needed for lowFactor
    vector<long long> p10modM;
    vector<long long> p10low;

    array<int, 100> d{};
    array<bool, 100> used{};

    long long residueM = 0;
    long long residueLow = 0;

    unsigned long long nodes = 0;
    unsigned long long ratioSolutions = 0;
    unsigned long long supportMatches = 0;
    bool capped = false;

    vector<pair<string,string>> matches;

    static long long gcdll(long long x, long long y) { return std::gcd(x, y); }

    static long long radical(long long x) {
        long long ans = 1;
        for (long long p = 2; p * p <= x; ++p) {
            if (x % p == 0) {
                ans *= p;
                while (x % p == 0) x /= p;
            }
        }
        if (x > 1) ans *= x;
        return ans;
    }

    static int vfactor(long long x, long long p) {
        int e = 0;
        while (x % p == 0) {
            x /= p;
            ++e;
        }
        return e;
    }

    static vector<int> buildTMap(int L) {
        vector<int> m(L);
        // Indices are from the LSD.
        // In MSD coordinates, output sources are:
        // 0, L-1, 1, L-2, 2, L-3, ...
        for (int i = 0; i < L; ++i) {
            int outMSD = L - 1 - i;
            int srcMSD;
            if ((outMSD & 1) == 0) srcMSD = outMSD / 2;
            else srcMSD = L - 1 - outMSD / 2;
            m[i] = L - 1 - srcMSD;
        }
        return m;
    }

    string numberString() const {
        string s;
        s.reserve(L);
        for (int i = L - 1; i >= 0; --i) s.push_back(char('0' + d[i]));
        return s;
    }

    string transformString(const string& s) const {
        string t;
        t.reserve(s.size());
        int n = (int)s.size();
        for (int j = 0; j < n; ++j) {
            int src = (j & 1) ? (n - 1 - j / 2) : (j / 2);
            t.push_back(s[src]);
        }
        return t;
    }

    bool finalVerify(const string& ns, const string& ts) const {
        cpp_int n(ns), t(ts);
        return cpp_int(r.a) * t == cpp_int(r.b) * n;
    }

    bool leadingAllowed(int pos, int val) const {
        return !(pos == L - 1 && val == 0);
    }

    bool allLowPositionsAssigned() const {
        if (lowDigits == 0) return true;
        if (lowDigits > L) return false;
        for (int i = 0; i < lowDigits; ++i) if (!used[i]) return false;
        return true;
    }

    bool lowFactorPasses() const {
        if (lowFactor == 1) return true;
        if (!allLowPositionsAssigned()) return true;
        return residueLow == 0;
    }

    void addDigit(int pos, int val) {
        d[pos] = val;
        used[pos] = true;
        residueM += (long long)val * p10modM[pos];
        residueM %= M;
        if (lowFactor != 1) {
            residueLow += (long long)val * p10low[pos];
            residueLow %= lowFactor;
        }
    }

    void recordCandidate(long long carryFinal) {
        if (carryFinal != 0 || capped) return;

        string ns = numberString();
        if (ns.back() == '0') return; // primitive search

        string ts = transformString(ns);
        if (ns == ts) return;
        if (ts[0] == '0') return;

        ++ratioSolutions;

        if (!finalVerify(ns, ts)) {
            cerr << "FATAL: independent ratio verification failed for "
                 << ns << " -> " << ts << " in ratio " << r.name() << "\n";
            exit(1);
        }

        // Exact support criterion: M | n.
        if (residueM != 0) return;

        ++supportMatches;
        matches.emplace_back(ns, ts);
    }

    void dfs(int i, long long carry) {
        if (capped) return;
        if (++nodes > nodeLimit) {
            capped = true;
            return;
        }

        // Cheap localized divisibility pruning from the powers of 2 and 5.
        if (!lowFactorPasses()) return;

        if (i == L) {
            recordCandidate(carry);
            return;
        }

        int p = i;
        int q = tmap[i];
        bool up = used[p];
        bool uq = used[q];

        // Equation at LSD position i:
        //     b*x[p] + carry = a*x[q] + 10*nextCarry
        if (up && uq) {
            long long v = (long long)r.b * d[p] + carry - (long long)r.a * d[q];
            if (v % 10 != 0) return;
            dfs(i + 1, v / 10);
            return;
        }

        if (p == q) {
            if (up) {
                long long v = (long long)(r.b - r.a) * d[p] + carry;
                if (v % 10 != 0) return;
                dfs(i + 1, v / 10);
                return;
            }

            for (int x = 0; x <= 9; ++x) {
                if (!leadingAllowed(p, x)) continue;
                long long v = (long long)(r.b - r.a) * x + carry;
                if (v % 10 != 0) continue;

                long long oldM = residueM, oldLow = residueLow;
                addDigit(p, x);
                dfs(i + 1, v / 10);
                residueM = oldM;
                residueLow = oldLow;
                used[p] = false;

                if (capped) return;
            }
            return;
        }

        if (up && !uq) {
            int x = d[p];
            for (int y = 0; y <= 9; ++y) {
                if (!leadingAllowed(q, y)) continue;
                long long v = (long long)r.b * x + carry - (long long)r.a * y;
                if (v % 10 != 0) continue;

                long long oldM = residueM, oldLow = residueLow;
                addDigit(q, y);
                dfs(i + 1, v / 10);
                residueM = oldM;
                residueLow = oldLow;
                used[q] = false;

                if (capped) return;
            }
            return;
        }

        if (!up && uq) {
            int y = d[q];
            for (int x = 0; x <= 9; ++x) {
                if (!leadingAllowed(p, x)) continue;
                long long v = (long long)r.b * x + carry - (long long)r.a * y;
                if (v % 10 != 0) continue;

                long long oldM = residueM, oldLow = residueLow;
                addDigit(p, x);
                dfs(i + 1, v / 10);
                residueM = oldM;
                residueLow = oldLow;
                used[p] = false;

                if (capped) return;
            }
            return;
        }

        // Neither digit assigned.
        for (int x = 0; x <= 9; ++x) {
            if (!leadingAllowed(p, x)) continue;
            for (int y = 0; y <= 9; ++y) {
                if (!leadingAllowed(q, y)) continue;
                long long v = (long long)r.b * x + carry - (long long)r.a * y;
                if (v % 10 != 0) continue;

                long long oldM = residueM, oldLow = residueLow;
                d[p] = x;
                d[q] = y;
                used[p] = used[q] = true;
                residueM = (residueM + (long long)x * p10modM[p]
                                      + (long long)y * p10modM[q]) % M;
                if (lowFactor != 1) {
                    residueLow = (residueLow + (long long)x * p10low[p]
                                            + (long long)y * p10low[q]) % lowFactor;
                }

                dfs(i + 1, v / 10);

                residueM = oldM;
                residueLow = oldLow;
                used[p] = used[q] = false;

                if (capped) return;
            }
        }
    }

    void prepare() {
        tmap = buildTMap(L);

        long long ab = r.a * r.b;
        long long rad = radical(ab);
        M = r.a * rad;

        int e2 = vfactor(M, 2);
        int e5 = vfactor(M, 5);

        // Primitive n cannot be divisible by both 2 and 5.
        if (e2 > 0 && e5 > 0) {
            lowFactor = 10;
            lowDigits = 1;
            return;
        }

        lowFactor = 1;
        for (int i = 0; i < e2; ++i) lowFactor *= 2;
        for (int i = 0; i < e5; ++i) lowFactor *= 5;

        lowDigits = max(e2, e5);
        // 10^lowDigits is divisible by 2^e2 * 5^e5.
        if (lowDigits > L) lowDigits = L;
    }

    void run() {
        prepare();
        if (e2e5Impossible()) {
            // leave counts at zero; caller labels as structurally impossible
            return;
        }

        p10modM.assign(L, 0);
        p10low.assign(L, 0);
        p10modM[0] = 1 % M;
        for (int i = 1; i < L; ++i) p10modM[i] = (p10modM[i - 1] * 10) % M;
        if (lowFactor != 1) {
            p10low[0] = 1 % lowFactor;
            for (int i = 1; i < L; ++i) p10low[i] = (p10low[i - 1] * 10) % lowFactor;
        }

        d.fill(0);
        used.fill(false);
        residueM = residueLow = 0;
        nodes = ratioSolutions = supportMatches = 0;
        capped = false;
        matches.clear();

        dfs(0, 0);
    }

    bool e2e5Impossible() const {
        long long e2 = 1, e5 = 1;
        while (M % (e2 * 2) == 0) e2 *= 2;
        while (M % (e5 * 5) == 0) e5 *= 5;
        return (e2 > 1 && e5 > 1);
    }
};

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Default run is deliberately targeted:
    // ratio 1 = 8:9, lengths 25..30.
    // This avoids rescanning the already-known <=24 digit range.
    int minDigits = 25;
    int maxDigits = 30;
    unsigned long long nodeLimit = 2000000000ULL;
    int onlyRatio = 1; // 1..12; 0 = all

    if (argc >= 2) onlyRatio = stoi(argv[1]);
    if (argc >= 3) minDigits = stoi(argv[2]);
    if (argc >= 4) maxDigits = stoi(argv[3]);
    if (argc >= 5) nodeLimit = stoull(argv[4]);

    if (minDigits < 2 || maxDigits < minDigits || maxDigits > 100) {
        cerr << "Usage: outer_in_ratio_search.exe [ratioIndex] [minDigits] [maxDigits] [nodeLimit]\n";
        cerr << "Example: outer_in_ratio_search.exe 1 25 30 2000000000\n";
        return 1;
    }
    if (onlyRatio < 0 || onlyRatio > 12) {
        cerr << "ratioIndex must be 0..12\n";
        return 1;
    }

    const vector<Ratio> ratios = {
        {8,9},
        {7,9},
        {9,8},
        {8,11},
        {112,121},
        {33,34},
        {13,12},
        {27,25},
        {32,27},
        {2257,2187},
        {625,643},
        {1331,1296}
    };

    cout << "Outer-in prime-support search v2\n";
    cout << "Primitive only (last digit != 0)\n";
    cout << "Lengths " << minDigits << ".." << maxDigits
         << ", node cap=" << nodeLimit << " per ratio/length\n";
    cout << "Ratio index: " << (onlyRatio == 0 ? string("ALL") : to_string(onlyRatio)) << "\n\n";

    ofstream csv("outer_in_ratio_results.csv");
    csv << "ratio_index,ratio,length,status,ratio_solutions,support_matches,nodes,M,solutions\n";

    for (int ri = 0; ri < (int)ratios.size(); ++ri) {
        if (onlyRatio != 0 && onlyRatio != ri + 1) continue;

        const Ratio R = ratios[ri];
        cout << "============================================================\n";
        cout << "RATIO " << (ri + 1) << " = " << R.name() << "\n";

        unsigned long long totalSupport = 0;

        for (int L = minDigits; L <= maxDigits; ++L) {
            Solver s;
            s.r = R;
            s.L = L;
            s.nodeLimit = nodeLimit;

            auto t0 = chrono::steady_clock::now();
            s.run();
            auto t1 = chrono::steady_clock::now();
            double sec = chrono::duration<double>(t1 - t0).count();

            string status = s.capped ? "INCOMPLETE" : "COMPLETE";
            totalSupport += s.supportMatches;

            cout << "L=" << setw(2) << L
                 << "  " << setw(10) << status
                 << "  ratio=" << setw(9) << s.ratioSolutions
                 << "  support=" << setw(4) << s.supportMatches
                 << "  nodes=" << setw(12) << s.nodes
                 << "  time=" << fixed << setprecision(3) << sec << "s";
            if (s.capped) cout << "  <-- raise node cap";
            cout << '\n';

            for (auto& [ns, ts] : s.matches) {
                cpp_int n(ns);
                cpp_int g = n / R.a;
                cout << "      MATCH  " << ns << " -> " << ts
                     << "   gcd=" << g << "\n";
            }
            cout.flush();

            csv << (ri + 1) << ',' << R.name() << ',' << L << ',' << status << ','
                << s.ratioSolutions << ',' << s.supportMatches << ',' << s.nodes << ','
                << s.M << ',';
            for (size_t k = 0; k < s.matches.size(); ++k) {
                if (k) csv << ';';
                csv << s.matches[k].first << "->" << s.matches[k].second;
            }
            csv << '\n';
        }

        cout << "TOTAL SUPPORT MATCHES for " << R.name() << " = " << totalSupport << "\n\n";
    }

    cout << "Done. CSV: outer_in_ratio_results.csv\n";
    return 0;
}
