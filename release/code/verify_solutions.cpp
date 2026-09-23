/*
    Independent verifier for the outer-in prime-support problem.

    For each candidate it independently:
      1. computes T(n) from the decimal digits;
      2. computes gcd(n,T(n));
      3. reduces n:T(n) to a:b;
      4. checks the exact criterion rad(a*b) | gcd(n,T(n));
      5. optionally checks an expected T(n).

    It does not use the search program's internal state.

    It reports NEEDS_BIG_FACTOR rather than silently certifying a case where
    a*b cannot be factored into uint64_t.
*/

#include <boost/multiprecision/cpp_int.hpp>

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

using boost::multiprecision::cpp_int;
using std::string;
using std::uint64_t;
using std::vector;

static cpp_int gcd_cpp(cpp_int a, cpp_int b) {
    while (b != 0) {
        cpp_int r = a % b;
        a = b;
        b = r;
    }
    return a;
}

static string trim(const string& s) {
    size_t l = 0;
    while (l < s.size() && std::isspace(static_cast<unsigned char>(s[l]))) ++l;
    size_t r = s.size();
    while (r > l && std::isspace(static_cast<unsigned char>(s[r - 1]))) --r;
    return s.substr(l, r - l);
}

static bool is_decimal(const string& s) {
    if (s.empty()) return false;
    for (char c : s)
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    return true;
}

static string normalize_decimal(string s) {
    s = trim(s);
    size_t p = 0;
    while (p < s.size() && s[p] == '0') ++p;
    if (p == s.size()) return "0";
    return s.substr(p);
}

static string transform_outer_in(const string& n) {
    string t;
    t.reserve(n.size());

    size_t left = 0;
    size_t right = n.size() - 1;

    while (left <= right) {
        t.push_back(n[left]);
        if (left == right) break;
        t.push_back(n[right]);
        ++left;
        if (right == 0) break;
        --right;
    }
    return t;
}

static bool cpp_int_to_u64(const cpp_int& x, uint64_t& out) {
    if (x < 0) return false;
    static const cpp_int MAX_U64 = (cpp_int(1) << 64) - 1;
    if (x > MAX_U64) return false;
    out = x.convert_to<uint64_t>();
    return true;
}

static vector<uint64_t> factor_distinct_u64(uint64_t x) {
    vector<uint64_t> factors;
    if (x <= 1) return factors;

    if (x % 2 == 0) {
        factors.push_back(2);
        while (x % 2 == 0) x /= 2;
    }

    for (uint64_t p = 3; p <= x / p; p += 2) {
        if (x % p == 0) {
            factors.push_back(p);
            while (x % p == 0) x /= p;
        }
    }

    if (x > 1) factors.push_back(x);
    return factors;
}

struct Candidate {
    string n;
    string expected_t;
};

static bool parse_line(const string& raw, Candidate& out) {
    string line = trim(raw);
    if (line.empty() || line[0] == '#') return false;

    const string arrow = "->";
    size_t pos = line.find(arrow);

    string left;
    string right;

    if (pos == string::npos) {
        left = trim(line);
    } else {
        left = trim(line.substr(0, pos));
        right = trim(line.substr(pos + arrow.size()));
    }

    if (!is_decimal(left)) {
        size_t i = 0;
        while (i < left.size() &&
               !std::isdigit(static_cast<unsigned char>(left[i]))) ++i;

        if (i == left.size()) return false;

        size_t j = i;
        while (j < left.size() &&
               std::isdigit(static_cast<unsigned char>(left[j]))) ++j;

        left = left.substr(i, j - i);
    }

    if (!is_decimal(left)) return false;

    out.n = normalize_decimal(left);

    if (!right.empty() && is_decimal(right))
        out.expected_t = normalize_decimal(right);
    else
        out.expected_t.clear();

    return true;
}

int main(int argc, char** argv) {
    const string input_file =
        (argc >= 2) ? argv[1] : "known_primitive_solutions.txt";

    const string output_file =
        (argc >= 3) ? argv[2] : "verified_solutions.csv";

    std::ifstream fin(input_file);
    if (!fin) {
        std::cerr << "ERROR: cannot open input file: " << input_file << "\n";
        return 1;
    }

    std::ofstream fout(output_file);
    if (!fout) {
        std::cerr << "ERROR: cannot create output file: " << output_file << "\n";
        return 1;
    }

    fout << "n,T(n),primitive,fixed_point,gcd,reduced_a,reduced_b,"
            "rad_ab,prime_support_match,expected_T_match,status\n";

    size_t candidate_count = 0;
    size_t verified_count = 0;
    size_t support_match_count = 0;
    size_t failure_count = 0;

    string raw;
    while (std::getline(fin, raw)) {
        Candidate c;
        if (!parse_line(raw, c)) continue;

        ++candidate_count;

        const string t_str = transform_outer_in(c.n);

        cpp_int n(c.n);
        cpp_int t(t_str);

        const bool primitive =
            (c.n.size() == 1 || c.n.back() != '0');

        const bool fixed_point = (n == t);

        cpp_int g = gcd_cpp(n, t);
        cpp_int a = n / g;
        cpp_int b = t / g;
        cpp_int product = a * b;

        uint64_t product_u64 = 0;
        bool factorable_small = cpp_int_to_u64(product, product_u64);

        cpp_int rad_ab = 1;
        bool support_match = false;
        string status;

        if (!primitive) {
            status = "NON_PRIMITIVE";
        } else if (fixed_point) {
            status = "FIXED_POINT";
        } else if (!factorable_small) {
            status = "NEEDS_BIG_FACTOR";
        } else {
            vector<uint64_t> factors = factor_distinct_u64(product_u64);

            for (uint64_t p : factors) rad_ab *= p;

            support_match = true;
            for (uint64_t p : factors) {
                if (g % p != 0) {
                    support_match = false;
                    break;
                }
            }

            status = support_match ? "VERIFIED" : "SUPPORT_FAIL";
        }

        bool expected_t_match = true;
        if (!c.expected_t.empty())
            expected_t_match = (c.expected_t == t_str);

        if (status == "VERIFIED" && expected_t_match) {
            ++verified_count;
            ++support_match_count;
        } else if (status != "NEEDS_BIG_FACTOR") {
            ++failure_count;
        }

        fout << c.n << ','
             << t_str << ','
             << (primitive ? 1 : 0) << ','
             << (fixed_point ? 1 : 0) << ','
             << g << ','
             << a << ','
             << b << ','
             << rad_ab << ','
             << (support_match ? 1 : 0) << ','
             << (expected_t_match ? 1 : 0) << ','
             << ((status == "VERIFIED" && expected_t_match)
                     ? "VERIFIED"
                     : status)
             << '\n';
    }

    std::cout << "========================================\n";
    std::cout << "Candidates read:       " << candidate_count << '\n';
    std::cout << "Verified matches:      " << verified_count << '\n';
    std::cout << "Non-verified outcomes: " << failure_count << '\n';
    std::cout << "Output:                " << output_file << '\n';
    std::cout << "========================================\n";

    return 0;
}
