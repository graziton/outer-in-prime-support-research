
#include <bits/stdc++.h>
using namespace std;


/*
============================================================
FIXED-RATIO OUTER-IN DIGIT SOLVER
============================================================

We define T(n) by taking:

    first digit,
    last digit,
    second digit,
    second-last digit,
    ...

Examples:

    12345678 -> 18273645
    12345    -> 15324

For a fixed reduced ratio a:b we solve:

    T(n) = (b/a) * n

equivalently:

    a*T(n) = b*n

We then test whether n and T(n) have exactly the same
set of distinct prime factors.

The solver works with digit strings, so it can handle
numbers much larger than 64-bit integers.
============================================================
*/


/*
============================================================
CONFIGURATION
============================================================
*/

const int MAX_DIGITS = 20;

/*
    Maximum number of ratio-solutions stored per digit length.

    This is only a safety limit. It should never be reached
    for the small ratio classes we are currently studying.
*/
const size_t MAX_SOLUTIONS_PER_LENGTH = 5'000'000;


/*
============================================================
OUTER-IN TRANSFORMATION
============================================================
*/

string transformNumber(const string& s)
{
    string t;
    t.reserve(s.size());

    int l = 0;
    int r = (int)s.size() - 1;

    while (l <= r)
    {
        t.push_back(s[l]);

        if (l != r)
            t.push_back(s[r]);

        ++l;
        --r;
    }

    return t;
}


/*
============================================================
DECIMAL STRING MODULO SMALL INTEGER
============================================================
*/

long long stringMod(const string& s, long long m)
{
    long long rem = 0;

    for (char c : s)
    {
        rem = (rem * 10 + (c - '0')) % m;
    }

    return rem;
}


/*
============================================================
MULTIPLY DECIMAL STRING BY SMALL INTEGER

Used only for final verification of a candidate.
============================================================
*/

string multiplyStringBySmall(
    const string& s,
    int multiplier
)
{
    string result;
    result.reserve(s.size() + 10);

    int carry = 0;

    for (int i = (int)s.size() - 1; i >= 0; --i)
    {
        int value =
            (s[i] - '0') * multiplier + carry;

        result.push_back(
            char('0' + (value % 10))
        );

        carry = value / 10;
    }

    while (carry > 0)
    {
        result.push_back(
            char('0' + (carry % 10))
        );

        carry /= 10;
    }

    reverse(result.begin(), result.end());

    /*
        Remove leading zeroes.
    */

    size_t first =
        result.find_first_not_of('0');

    if (first == string::npos)
        return "0";

    return result.substr(first);
}


/*
============================================================
DIVIDE DECIMAL STRING BY SMALL INTEGER

Assumes exact divisibility.

Example:

    3024 / 8 = 378
============================================================
*/

string divideStringBySmall(
    const string& s,
    int divisor
)
{
    string result;
    result.reserve(s.size());

    long long rem = 0;

    for (char c : s)
    {
        long long cur =
            rem * 10 + (c - '0');

        int q =
            (int)(cur / divisor);

        rem =
            cur % divisor;

        if (!result.empty() || q != 0)
        {
            result.push_back(
                char('0' + q)
            );
        }
    }

    if (result.empty())
        return "0";

    return result;
}


/*
============================================================
SMALL-INTEGER RADICAL

rad(n) = product of distinct prime factors.

Examples:

    rad(72) = rad(2^3 * 3^2)
            = 2 * 3
            = 6
============================================================
*/

long long radicalSmall(int n)
{
    long long r = 1;

    for (int p = 2;
         1LL * p * p <= n;
         ++p)
    {
        if (n % p != 0)
            continue;

        r *= p;

        while (n % p == 0)
            n /= p;
    }

    if (n > 1)
        r *= n;

    return r;
}


/*
============================================================
CHECK PRIME-SUPPORT CONDITION FOR A RATIO SOLUTION

Suppose:

    gcd(a,b) = 1
    a*T(n) = b*n

Then necessarily:

    n   = a*g
    T(n)= b*g

for some g.

The prime supports of n and T(n) are equal iff every
prime appearing in a or b also appears in g.

Thus:

    rad(a*b) | g

Since g=n/a:

    a*rad(a*b) | n

This lets us test the prime-support condition using only
division/modulo, without factoring the potentially huge n.
============================================================
*/

bool hasSamePrimeSupportForRatio(
    const string& n,
    int a,
    int b
)
{
    long long radAB =
        radicalSmall(a * b);

    long long required =
        1LL * a * radAB;

    return stringMod(n, required) == 0;
}


/*
============================================================
VERIFY PRIME-SUPPORT CONDITION DIRECTLY

This is an extra safety check used on actual solutions.

For huge strings, we use the ratio criterion above as the
main test, but this function can independently verify the
logic using gcd only when values fit in uint64_t.

Currently our ratio solver is allowed to go up to 20 digits,
so this direct check is not generally possible.

Therefore the ratio-based theorem above is the exact check.
============================================================
*/


/*
============================================================
TRANSITION

At one decimal column, we solve:

    a*Tdigit + carry_in
        =
    b*Ndigit + 10*carry_out

equivalently:

    b*x - a*y + carry_in
        =
    10*carry_out

where:

    x = digit of n
    y = corresponding digit of T(n)
============================================================
*/

struct Transition
{
    int x;
    int y;
    int nextCarry;
};


/*
============================================================
RATIO SOLVER
============================================================
*/

class RatioSolver
{
private:

    int L;
    int a;
    int b;

    /*
        source[r] = which digit of n appears at output
        position r of T(n).

        For 8 digits:

        n:
            d0 d1 d2 d3 d4 d5 d6 d7

        T(n):
            d0 d7 d1 d6 d2 d5 d3 d4

        therefore:

            source = 0,7,1,6,2,5,3,4
    */

    vector<int> source;

    /*
        Current digit assignment.

        -1 = unknown.
    */

    vector<int> digit;

    /*
        Ratio solutions for the current digit length.
    */

    vector<string> solutions;

    /*
        Safe signed carry range.

        The recurrence is:

            nextCarry =
                (b*x - a*y + carry)/10

        For our small a,b this bound is more than enough.
    */

    int carryLimit;

    /*
        transitions[carry + carryLimit]
    */

    vector<vector<Transition>> transitions;


public:

    RatioSolver(
        int length,
        int numeratorA,
        int numeratorB
    )
        :
        L(length),
        a(numeratorA),
        b(numeratorB),
        source(length),
        digit(length, -1)
    {

        /*
            Build the digit permutation T.
        */

        for (int r = 0; r < L; ++r)
        {
            if (r % 2 == 0)
            {
                source[r] = r / 2;
            }
            else
            {
                source[r] =
                    L - 1 - r / 2;
            }
        }


        /*
            Safe carry bound.
        */

        carryLimit =
            max(a,b) + 2;


        transitions.resize(
            2 * carryLimit + 1
        );


        /*
            Precompute all valid digit transitions.
        */

        for (int carry = -carryLimit;
             carry <= carryLimit;
             ++carry)
        {
            auto& v =
                transitions[
                    carry + carryLimit
                ];


            for (int x = 0; x <= 9; ++x)
            {
                for (int y = 0; y <= 9; ++y)
                {
                    /*
                        b*x - a*y + carry
                            =
                        10*nextCarry
                    */

                    int value =
                        b * x -
                        a * y +
                        carry;


                    if (value % 10 != 0)
                        continue;


                    int nextCarry =
                        value / 10;


                    if (
                        nextCarry < -carryLimit ||
                        nextCarry > carryLimit
                    )
                        continue;


                    v.push_back({
                        x,
                        y,
                        nextCarry
                    });
                }
            }
        }
    }


    /*
    ========================================================
    DFS
    ========================================================
    */

    void dfs(
        int column,
        int carry
    )
    {
        /*
            Safety limit.
        */

        if (
            solutions.size()
            >=
            MAX_SOLUTIONS_PER_LENGTH
        )
            return;


        /*
            All digit columns processed.
        */

        if (column == L)
        {
            /*
                Final carry must vanish.
            */

            if (carry != 0)
                return;


            /*
                First digit cannot be zero.
            */

            if (digit[0] <= 0)
                return;


            /*
                Construct n.
            */

            string n;
            n.reserve(L);

            for (int d : digit)
            {
                n.push_back(
                    char('0' + d)
                );
            }


            /*
            ------------------------------------------------
            INDEPENDENT FINAL ARITHMETIC VERIFICATION

            Verify exactly:

                a*T(n) = b*n

            using decimal-string multiplication.

            This protects against bugs in the DFS itself.
            ------------------------------------------------
            */

            string t =
                transformNumber(n);

            string lhs =
                multiplyStringBySmall(
                    t,
                    a
                );

            string rhs =
                multiplyStringBySmall(
                    n,
                    b
                );

            if (lhs != rhs)
                return;


            /*
                Only now accept the solution.
            */

            solutions.push_back(n);

            return;
        }


        /*
        ----------------------------------------------------
        We process decimal columns from RIGHT to LEFT.

        n position:
            L - 1 - column

        T output position:
            L - 1 - column

        source[] tells us which digit of n appears there.
        ----------------------------------------------------
        */

        int nPos =
            L - 1 - column;

        int tOutputPos =
            L - 1 - column;

        int tPos =
            source[tOutputPos];


        int knownX =
            digit[nPos];

        int knownY =
            digit[tPos];


        const auto& transitionsHere =
            transitions[
                carry + carryLimit
            ];


        for (const auto& tr :
             transitionsHere)
        {
            int x = tr.x;
            int y = tr.y;


            /*
            ------------------------------------------------
            CRITICAL CORRECTION

            If T maps a digit position to itself, then
            that position contains the SAME digit in n
            and T(n).

            Without this condition the previous program
            could generate false "solutions".
            ------------------------------------------------
            */

            if (nPos == tPos && x != y)
                continue;


            /*
                Existing assignments must agree.
            */

            if (
                knownX != -1 &&
                knownX != x
            )
                continue;


            if (
                knownY != -1 &&
                knownY != y
            )
                continue;


            /*
                Leading digit cannot be zero in n.

                Since T preserves the first digit, this also
                guarantees T(n) has the same length.
            */

            if (
                nPos == 0 &&
                x == 0
            )
                continue;


            if (
                tPos == 0 &&
                y == 0
            )
                continue;


            /*
                Save previous assignments.
            */

            int oldX =
                digit[nPos];

            int oldY =
                digit[tPos];


            /*
                Assign.

                If nPos == tPos, we already established x==y.
            */

            digit[nPos] = x;
            digit[tPos] = y;


            /*
                Continue to next column.
            */

            dfs(
                column + 1,
                tr.nextCarry
            );


            /*
                Restore.
            */

            digit[nPos] = oldX;
            digit[tPos] = oldY;


            /*
                Stop if our safety limit was reached.
            */

            if (
                solutions.size()
                >=
                MAX_SOLUTIONS_PER_LENGTH
            )
                return;
        }
    }


    /*
    ========================================================
    SOLVE
    ========================================================
    */

    vector<string> solve()
    {
        solutions.clear();

        fill(
            digit.begin(),
            digit.end(),
            -1
        );

        dfs(0,0);

        return solutions;
    }
};


/*
============================================================
ANALYZE A RATIO SOLUTION
============================================================
*/

struct Result
{
    int a;
    int b;
    int digits;

    string n;
    string t;
    string g;

    bool primitive;
    bool supportMatch;
};


/*
============================================================
ANALYZE ONE SOLUTION
============================================================
*/

Result analyzeSolution(
    int a,
    int b,
    const string& n
)
{
    string t =
        transformNumber(n);


    /*
        Because:

            a*T(n)=b*n

        and gcd(a,b)=1:

            n=a*g
            T(n)=b*g

        therefore g=n/a.
    */

    string g =
        divideStringBySmall(
            n,
            a
        );


    bool supportMatch =
        hasSamePrimeSupportForRatio(
            n,
            a,
            b
        );


    bool primitive =
        n.back() != '0';


    Result r;

    r.a = a;
    r.b = b;
    r.digits = (int)n.size();

    r.n = n;
    r.t = t;
    r.g = g;

    r.primitive = primitive;
    r.supportMatch = supportMatch;

    return r;
}


/*
============================================================
BUILT-IN SANITY TEST

These tests are deliberately simple.

Known valid:

    3024 -> 3402
    8*T(n) = 9*n

Known invalid:

    25488 -> 28584

It was accidentally accepted by the old buggy solver.
We explicitly verify that it is NOT a ratio solution.
============================================================
*/

bool runSanityTests()
{
    cout
        << "============================================\n"
        << "SANITY TESTS\n"
        << "============================================\n";


    /*
        Test transformation.
    */

    if (
        transformNumber("12345678")
        !=
        "18273645"
    )
    {
        cerr
            << "ERROR: T(12345678) test failed.\n";

        return false;
    }


    if (
        transformNumber("12345")
        !=
        "15243"
    )
    {
        cerr
            << "ERROR: T(12345) test failed.\n";

        return false;
    }


    /*
        Valid ratio example.
    */

    string n1 = "3024";
    string t1 =
        transformNumber(n1);


    string lhs1 =
        multiplyStringBySmall(
            t1,
            8
        );

    string rhs1 =
        multiplyStringBySmall(
            n1,
            9
        );


    if (lhs1 != rhs1)
    {
        cerr
            << "ERROR: known 3024 ratio test failed.\n";

        return false;
    }


    /*
        Invalid example from previous buggy solver.
    */

    string n2 = "25488";
    string t2 =
        transformNumber(n2);


    string lhs2 =
        multiplyStringBySmall(
            t2,
            8
        );

    string rhs2 =
        multiplyStringBySmall(
            n2,
            9
        );


    if (lhs2 == rhs2)
    {
        cerr
            << "ERROR: false candidate 25488 was accepted.\n";

        return false;
    }


    /*
        Prime-support criterion for 3024.

            rad(8*9)=6

            required divisor =
                8*6
                =48

            3024 is divisible by 48.
    */

    if (
        !hasSamePrimeSupportForRatio(
            "3024",
            8,
            9
        )
    )
    {
        cerr
            << "ERROR: prime-support test for 3024 failed.\n";

        return false;
    }


    /*
        25488 should not be accepted anyway because it isn't
        actually a ratio solution.
    */

    cout
        << "All sanity tests passed.\n\n";

    return true;
}


/*
============================================================
MAIN
============================================================
*/

int main()
{
    /*
        ----------------------------------------------------
        SANITY TESTS
        ----------------------------------------------------
    */

    if (!runSanityTests())
    {
        return 1;
    }


    /*
        ----------------------------------------------------
        RATIO CLASSES
        ----------------------------------------------------

        First:
            8:9

        Second:
            9:8
        ----------------------------------------------------
    */

    vector<pair<int,int>> ratios =
    {
        {8,9},
        {9,8}
    };


    /*
        Output.
    */

    ofstream csv(
        "ratio_solver_results_corrected.csv"
    );

    csv
        << "a,b,digits,n,Tn,g,"
           "primitive,support_match\n";


    cout
        << "============================================\n"
        << "FIXED-RATIO OUTER-IN SOLVER\n"
        << "============================================\n";

    cout
        << "Maximum digits: "
        << MAX_DIGITS
        << "\n";


    /*
        ----------------------------------------------------
        PROCESS EACH RATIO
        ----------------------------------------------------
    */

    for (auto [a,b] : ratios)
    {
        cout
            << "\n============================================\n";

        cout
            << "RATIO "
            << a
            << ":"
            << b
            << "\n";

        cout
            << "Equation: "
            << a
            << "*T(n) = "
            << b
            << "*n\n";

        cout
            << "============================================\n";


        int totalRatioSolutions = 0;
        int totalSupportMatches = 0;
        int totalPrimitiveMatches = 0;


        /*
        ----------------------------------------------------
        Process every digit length.
        ----------------------------------------------------
        */

        for (
            int L = 2;
            L <= MAX_DIGITS;
            ++L
        )
        {
            RatioSolver solver(
                L,
                a,
                b
            );


            vector<string> ratioSolutions =
                solver.solve();


            if (
                ratioSolutions.empty()
            )
            {
                continue;
            }


            int supportMatches = 0;
            int primitiveMatches = 0;


            cout
                << "\n"
                << L
                << " digits: "
                << ratioSolutions.size()
                << " ratio-solutions\n";


            /*
            ------------------------------------------------
            Analyze every ratio solution.
            ------------------------------------------------
            */

            for (
                const string& n :
                ratioSolutions
            )
            {
                Result r =
                    analyzeSolution(
                        a,
                        b,
                        n
                    );


                ++totalRatioSolutions;


                if (r.supportMatch)
                {
                    ++supportMatches;
                    ++totalSupportMatches;

                    if (r.primitive)
                    {
                        ++primitiveMatches;
                        ++totalPrimitiveMatches;
                    }
                }


                /*
                    Save everything.
                */

                csv
                    << r.a
                    << ','
                    << r.b
                    << ','
                    << r.digits
                    << ','
                    << r.n
                    << ','
                    << r.t
                    << ','
                    << r.g
                    << ','
                    << (r.primitive ? 1 : 0)
                    << ','
                    << (r.supportMatch ? 1 : 0)
                    << '\n';


                /*
                    Only display genuine prime-support matches.
                */

                if (r.supportMatch)
                {
                    cout
                        << "  MATCH "
                        << r.n
                        << " -> "
                        << r.t;


                    if (r.primitive)
                    {
                        cout
                            << "  [primitive]";
                    }
                    else
                    {
                        cout
                            << "  [trailing-zero]";
                    }


                    cout << '\n';
                }
            }


            cout
                << "  support matches: "
                << supportMatches
                << '\n';


            cout
                << "  primitive support matches: "
                << primitiveMatches
                << '\n';
        }


        /*
        ----------------------------------------------------
        RATIO SUMMARY
        ----------------------------------------------------
        */

        cout
            << "\n--------------------------------------------\n";


        cout
            << "TOTAL ratio-solutions: "
            << totalRatioSolutions
            << '\n';


        cout
            << "TOTAL support matches: "
            << totalSupportMatches
            << '\n';


        cout
            << "TOTAL primitive support matches: "
            << totalPrimitiveMatches
            << '\n';
    }


    /*
    ========================================================
    DONE
    ========================================================
    */

    cout
        << "\n============================================\n"
        << "DONE\n"
        << "============================================\n";


    cout
        << "Results saved to:\n"
        << "  ratio_solver_results_corrected.csv\n";


    return 0;
}
