/*
 * tieredsort usage examples
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include "include/tieredsort.hpp"

using namespace std;
using namespace std::chrono;

int main() {
    cout << "=== tieredsort Examples ===" << endl << endl;

    // Example 1: Basic usage with vector
    {
        cout << "1. Basic usage:" << endl;
        vector<int> data = {5, 2, 8, 1, 9, 3, 7, 4, 6};

        cout << "   Before: ";
        for (int x : data) cout << x << " ";
        cout << endl;

        tiered::sort(data.begin(), data.end());

        cout << "   After:  ";
        for (int x : data) cout << x << " ";
        cout << endl << endl;
    }

    // Example 2: Different types
    {
        cout << "2. Different types:" << endl;

        // 64-bit integers
        vector<int64_t> big_ints = {1000000000000LL, -500000000000LL, 999999999999LL};
        tiered::sort(big_ints.begin(), big_ints.end());
        cout << "   int64_t: " << big_ints[0] << ", " << big_ints[1] << ", " << big_ints[2] << endl;

        // Floats
        vector<float> floats = {3.14f, -2.71f, 1.41f, 0.0f, -0.5f};
        tiered::sort(floats.begin(), floats.end());
        cout << "   float:   ";
        for (float x : floats) cout << x << " ";
        cout << endl;

        // Doubles
        vector<double> doubles = {3.14159, -2.71828, 1.41421};
        tiered::sort(doubles.begin(), doubles.end());
        cout << "   double:  ";
        for (double x : doubles) cout << x << " ";
        cout << endl << endl;
    }

    // Example 3: Pre-allocated buffer (zero allocation during sort)
    {
        cout << "3. Zero-allocation with buffer:" << endl;
        vector<int> data = {5, 2, 8, 1, 9};
        vector<int> buffer(data.size());  // Reusable buffer

        tiered::sort(data.begin(), data.end(), buffer.data());

        cout << "   Sorted: ";
        for (int x : data) cout << x << " ";
        cout << endl << endl;
    }

    // Example 4: Performance comparison
    {
        cout << "4. Performance comparison (n=100,000):" << endl;

        mt19937 rng(42);
        uniform_int_distribution<int> dist(0, 1000000);

        vector<int> original(100000);
        for (int& x : original) x = dist(rng);

        vector<int> data = original;

        // std::sort
        auto start = high_resolution_clock::now();
        sort(data.begin(), data.end());
        auto end = high_resolution_clock::now();
        double t_std = duration<double, micro>(end - start).count();

        // tieredsort
        data = original;
        start = high_resolution_clock::now();
        tiered::sort(data.begin(), data.end());
        end = high_resolution_clock::now();
        double t_tiered = duration<double, micro>(end - start).count();

        printf("   std::sort:   %.0f μs\n", t_std);
        printf("   tieredsort:  %.0f μs (%.1fx faster)\n", t_tiered, t_std / t_tiered);
        cout << endl;
    }

    // Example 5: Dense data (counting sort kicks in)
    {
        cout << "5. Dense data (ages 0-100):" << endl;

        mt19937 rng(42);
        uniform_int_distribution<int> dist(0, 100);

        vector<int> ages(100000);
        for (int& x : ages) x = dist(rng);

        vector<int> data = ages;

        auto start = high_resolution_clock::now();
        sort(data.begin(), data.end());
        auto end = high_resolution_clock::now();
        double t_std = duration<double, micro>(end - start).count();

        data = ages;
        start = high_resolution_clock::now();
        tiered::sort(data.begin(), data.end());
        end = high_resolution_clock::now();
        double t_tiered = duration<double, micro>(end - start).count();

        printf("   std::sort:   %.0f μs\n", t_std);
        printf("   tieredsort:  %.0f μs (%.1fx faster) ← counting sort!\n", t_tiered, t_std / t_tiered);
        cout << endl;
    }

    // Example 6: Already sorted (pattern detection kicks in)
    {
        cout << "6. Already sorted data:" << endl;

        vector<int> sorted(100000);
        for (int i = 0; i < 100000; i++) sorted[i] = i;

        vector<int> data = sorted;

        auto start = high_resolution_clock::now();
        sort(data.begin(), data.end());
        auto end = high_resolution_clock::now();
        double t_std = duration<double, micro>(end - start).count();

        data = sorted;
        start = high_resolution_clock::now();
        tiered::sort(data.begin(), data.end());
        end = high_resolution_clock::now();
        double t_tiered = duration<double, micro>(end - start).count();

        printf("   std::sort:   %.0f μs\n", t_std);
        printf("   tieredsort:  %.0f μs (%.1fx faster) ← pattern detected!\n", t_tiered, t_std / t_tiered);
    }

    return 0;
}
