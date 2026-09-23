#include <boost/multiprecision/cpp_int.hpp>
#include <iostream>
#include <string>
#include <vector>

using boost::multiprecision::cpp_int;
using namespace std;

struct Ratio { long long a, b; };

static string transform_T(const string& s) {
    string t;
    t.reserve(s.size());
    int l = 0, r = static_cast<int>(s.size()) - 1;
    while (l <= r) {
        t.push_back(s[l++]);
        if (l <= r) t.push_back(s[r--]);
    }
    return t;
}

static cpp_int to_cpp_int(const string& s) {
    cpp_int x = 0;
    for (char c : s) x = x * 10 + (c - '0');
    return x;
}

static long long radical(long long x) {
    long long r = 1;
    for (long long p = 2; p * p <= x; ++p) {
        if (x % p == 0) {
            r *= p;
            while (x % p == 0) x /= p;
        }
    }
    if (x > 1) r *= x;
    return r;
}

class Search {
public:
    Ratio ratio;
    long long modulus;          // a * rad(a*b)
    long long node_cap;
    bool primitive_only;

    long long nodes = 0;
    bool hit_node_cap = false;

    Search(Ratio r, long long cap, bool primitive)
        : ratio(r), node_cap(cap), primitive_only(primitive) {
        modulus = ratio.a * radical(ratio.a * ratio.b);
    }

    void search_length(int k) {
        // s[q] = source position (from the left) of the T(n) digit at
        // decimal position q from the right.
        vector<int> source(k);
        for (int q = 0; q < k; ++q) {
            int output_pos_from_left = k - 1 - q;
            source[q] = (output_pos_from_left % 2 == 0)
                      ? output_pos_from_left / 2
                      : k - 1 - output_pos_from_left / 2;
        }

        vector<int> digit(k, -1);
        vector<long long> pow10(k);
        pow10[0] = 1 % modulus;
        for (int i = 1; i < k; ++i)
            pow10[i] = (pow10[i - 1] * 10) % modulus;

        dfs(k, 0, 0, 0, 0, source, digit, pow10);
    }

private:
    void emit_if_valid(int k, const vector<int>& digit,
                       int carry_left, int carry_right, long long remainder) {
        if (carry_left != carry_right) return;
        if (remainder != 0) return;
        if (digit[k - 1] == 0) return;  // n must have k digits
        if (primitive_only && digit[0] == 0) return;

        string n;
        n.reserve(k);
        for (int d : digit) n.push_back(char('0' + d));

        const string t = transform_T(n);
        if (n == t) return;

        // Independent exact check: a*T(n) = b*n.
        const cpp_int N = to_cpp_int(n);
        const cpp_int T = to_cpp_int(t);
        if (cpp_int(ratio.a) * T != cpp_int(ratio.b) * N) return;

        const cpp_int g = N / ratio.a;
        cout << ratio.a << ',' << ratio.b << ',' << k << ','
             << n << ',' << t << ',' << g << '\n';
    }

    void dfs(int k, int q, int carry_left, int carry_right,
             long long remainder, const vector<int>& source,
             vector<int>& digit, const vector<long long>& pow10) {
        if (hit_node_cap) return;
        if (++nodes > node_cap) {
            hit_node_cap = true;
            return;
        }

        if (q == k) {
            emit_if_valid(k, digit, carry_left, carry_right, remainder);
            return;
        }

        const int n_pos = k - 1 - q;  // current n digit, from the left
        const int t_pos = source[q];  // current T digit, from the left

        const int fixed_n = digit[n_pos];
        const int fixed_t = digit[t_pos];

        const int min_n = (n_pos == k - 1) ? 1 : 0;
        const int min_t = (t_pos == k - 1) ? 1 : 0;

        for (int x = min_n; x <= 9; ++x) {
            if (fixed_n != -1 && x != fixed_n) continue;

            for (int y = min_t; y <= 9; ++y) {
                if (fixed_t != -1 && y != fixed_t) continue;
                if (n_pos == t_pos && x != y) continue;

                // Digit-wise multiplication carries:
                // a*y + cL = product_digit + 10*cL'
                // b*x + cR = product_digit + 10*cR'
                const int left = static_cast<int>(ratio.a * y + carry_left);
                const int right = static_cast<int>(ratio.b * x + carry_right);
                if (left % 10 != right % 10) continue;

                const int next_left = left / 10;
                const int next_right = right / 10;

                const bool set_n = (digit[n_pos] == -1);
                const bool set_t = (digit[t_pos] == -1 && t_pos != n_pos);
                digit[n_pos] = x;
                digit[t_pos] = y;

                const long long next_rem =
                    (remainder + static_cast<long long>(x) * pow10[q]) % modulus;

                dfs(k, q + 1, next_left, next_right, next_rem,
                    source, digit, pow10);

                if (set_t) digit[t_pos] = -1;
                if (set_n) digit[n_pos] = -1;
            }
        }
    }
};

int main(int argc, char** argv) {
    if (argc < 6) {
        cerr << "Usage: support_ratio_search a b min_digits max_digits node_cap [primitive=1]\n";
        return 1;
    }

    const Ratio r{stoll(argv[1]), stoll(argv[2])};
    const int min_digits = stoi(argv[3]);
    const int max_digits = stoi(argv[4]);
    const long long node_cap = stoll(argv[5]);
    const bool primitive_only = (argc < 7 || stoi(argv[6]) != 0);

    Search search(r, node_cap, primitive_only);

    cout << "# ratio=" << r.a << ':' << r.b
         << " modulus=" << search.modulus
         << " primitive_only=" << (primitive_only ? 1 : 0) << '\n';

    for (int k = min_digits; k <= max_digits && !search.hit_node_cap; ++k) {
        const long long before = search.nodes;
        search.search_length(k);
        cerr << "k=" << k
             << " nodes_added=" << (search.nodes - before)
             << " total_nodes=" << search.nodes << '\n';
    }

    cerr << "SUMMARY nodes=" << search.nodes
         << (search.hit_node_cap ? " NODE_CAP_REACHED" : " COMPLETE") << '\n';
}
