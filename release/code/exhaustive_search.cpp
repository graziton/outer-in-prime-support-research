#include <algorithm>
#include <atomic>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <mutex>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

using u64 = std::uint64_t;

struct Permutation {
    int len = 0;
    std::vector<int> src_from_right;
    std::vector<u64> pow10;
};

Permutation make_perm(int len) {
    Permutation p;
    p.len = len;
    p.src_from_right.resize(len);
    p.pow10.resize(len);

    std::vector<int> order;
    order.reserve(len);
    int L = 0, R = len - 1;
    while (L <= R) {
        order.push_back(L++);
        if (L <= R) order.push_back(R--);
    }

    for (int j = 0; j < len; ++j) {
        const int src_left = order[j];
        const int out_from_right = len - 1 - j;
        p.src_from_right[out_from_right] = len - 1 - src_left;
    }

    p.pow10[0] = 1;
    for (int i = 1; i < len; ++i) p.pow10[i] = p.pow10[i - 1] * 10ULL;
    return p;
}

inline u64 transform_fixed(u64 n, const Permutation& p) {
    int d[20] = {};
    u64 x = n;
    for (int i = 0; i < p.len; ++i) {
        d[i] = int(x % 10ULL);
        x /= 10ULL;
    }

    u64 t = 0;
    for (int out = 0; out < p.len; ++out)
        t += u64(d[p.src_from_right[out]]) * p.pow10[out];
    return t;
}

/*
Exact criterion:
    g = gcd(n,t), n = a*g, t = b*g, gcd(a,b)=1.
Then
    rad(n) = rad(t)  <=>  rad(a*b) | g.
*/
inline bool support_part_of_g(u64 q, u64 g) {
    while (q > 1) {
        const u64 d = std::gcd(q, g);
        if (d == 1) return false;
        q /= d;
    }
    return true;
}

inline bool exact_match(u64 n, u64 t) {
    if (n == t) return false;
    const u64 g = std::gcd(n, t);
    if (g == 1) return false;
    return support_part_of_g(n / g, g) && support_part_of_g(t / g, g);
}

/*
Cheap necessary support tests.
Divisibility by 3 and 9 is automatically preserved because T is a digit
permutation, so they need no explicit test here.
*/
inline bool cheap_reject(u64 n, u64 t) {
    if ((n & 1ULL) != (t & 1ULL)) return true;
    if ((n % 5ULL == 0) != (t % 5ULL == 0)) return true;
    if ((n % 7ULL == 0) != (t % 7ULL == 0)) return true;
    if ((n % 11ULL == 0) != (t % 11ULL == 0)) return true;
    if ((n % 13ULL == 0) != (t % 13ULL == 0)) return true;
    if ((n % 17ULL == 0) != (t % 17ULL == 0)) return true;
    if ((n % 19ULL == 0) != (t % 19ULL == 0)) return true;
    if ((n % 23ULL == 0) != (t % 23ULL == 0)) return true;
    return false;
}

struct Worker {
    std::atomic<u64>& next;
    u64 end;
    u64 chunk;
    const Permutation& perm;
    std::ofstream& out;
    std::mutex& out_mutex;
    std::atomic<u64>& found;

    void operator()() const {
        while (true) {
            const u64 lo = next.fetch_add(chunk, std::memory_order_relaxed);
            if (lo >= end) return;
            const u64 hi = std::min(end, lo + chunk);

            for (u64 n = lo; n < hi; ++n) {
                if (n % 10ULL == 0) continue;

                const u64 t = transform_fixed(n, perm);
                if (cheap_reject(n, t)) continue;
                if (!exact_match(n, t)) continue;

                {
                    std::lock_guard<std::mutex> lock(out_mutex);
                    out << n << ',' << t << ',' << std::gcd(n, t) << '\n';
                }
                found.fetch_add(1, std::memory_order_relaxed);
            }
        }
    }
};

void usage(const char* exe) {
    std::cerr << "Usage: " << exe << " START END THREADS CHUNK OUTPUT.csv\n";
    std::cerr << "Scans START <= n < END, excluding n%10==0, "
                 "for n!=T(n) and rad(n)=rad(T(n)).\n";
}

int main(int argc, char** argv) {
    if (argc != 6) {
        usage(argv[0]);
        return 1;
    }

    u64 start = 0, end = 0, chunk = 0;
    int threads = 0;
    try {
        start = std::stoull(argv[1]);
        end = std::stoull(argv[2]);
        threads = std::stoi(argv[3]);
        chunk = std::stoull(argv[4]);
    } catch (...) {
        usage(argv[0]);
        return 1;
    }

    if (start >= end || end == 0 || threads <= 0 || chunk == 0) {
        std::cerr << "Invalid arguments.\n";
        return 1;
    }

    int len = 0;
    for (u64 x = start; x > 0; x /= 10ULL) ++len;

    u64 ten_pow_len = 1;
    for (int i = 0; i < len; ++i) ten_pow_len *= 10ULL;

    if (end > ten_pow_len) {
        std::cerr << "Use one decimal digit-length per run; split at powers of 10.\n";
        return 1;
    }

    const Permutation perm = make_perm(len);

    std::ofstream out(argv[5]);
    if (!out) {
        std::cerr << "Cannot open output: " << argv[5] << '\n';
        return 1;
    }
    out << "n,T(n),gcd\n";

    std::atomic<u64> next(start);
    std::atomic<u64> found(0);
    std::mutex out_mutex;
    std::vector<std::thread> pool;
    pool.reserve(threads);

    std::cout << "Scanning [" << start << "," << end << ")\n"
              << "digits=" << len << " threads=" << threads
              << " chunk=" << chunk << "\n";

    for (int i = 0; i < threads; ++i)
        pool.emplace_back(Worker{next, end, chunk, perm, out, out_mutex, found});

    for (auto& th : pool) th.join();

    out.flush();
    std::cout << "FOUND " << found.load() << "\n";
    std::cout << "OUTPUT " << argv[5] << "\n";
    return 0;
}
