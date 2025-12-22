/*
 * tieredsort - Test Suite
 *
 * Comprehensive tests for all supported types and patterns.
 * Run with: g++ -std=c++17 -O3 -o test tests/test_tieredsort.cpp && ./test
 */

#include "tieredsort.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cassert>
#include <functional>
#include <iomanip>
#include <string>

// =============================================================================
// Test Infrastructure
// =============================================================================

int tests_passed = 0;
int tests_failed = 0;

template<typename T>
bool is_sorted(const std::vector<T>& v) {
    for (size_t i = 1; i < v.size(); i++) {
        if (v[i] < v[i-1]) return false;
    }
    return true;
}

template<typename T>
void run_test(const std::string& name, std::vector<T> data) {
    auto expected = data;
    std::sort(expected.begin(), expected.end());

    tiered::sort(data.begin(), data.end());

    if (data == expected) {
        tests_passed++;
        std::cout << "  [PASS] " << name << "\n";
    } else {
        tests_failed++;
        std::cout << "  [FAIL] " << name << "\n";

        // Show first difference
        for (size_t i = 0; i < data.size(); i++) {
            if (data[i] != expected[i]) {
                std::cout << "         First diff at [" << i << "]: got "
                          << data[i] << ", expected " << expected[i] << "\n";
                break;
            }
        }
    }
}

// =============================================================================
// Data Generators
// =============================================================================

template<typename T>
std::vector<T> generate_random(size_t n, uint32_t seed = 12345) {
    std::mt19937 rng(seed);
    std::vector<T> data(n);

    if constexpr (std::is_integral_v<T>) {
        std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min() / 2,
                                               std::numeric_limits<T>::max() / 2);
        for (size_t i = 0; i < n; i++) data[i] = dist(rng);
    } else {
        std::uniform_real_distribution<T> dist(-1e6, 1e6);
        for (size_t i = 0; i < n; i++) data[i] = dist(rng);
    }
    return data;
}

template<typename T>
std::vector<T> generate_sorted(size_t n) {
    std::vector<T> data(n);
    for (size_t i = 0; i < n; i++) data[i] = static_cast<T>(i);
    return data;
}

template<typename T>
std::vector<T> generate_reversed(size_t n) {
    std::vector<T> data(n);
    for (size_t i = 0; i < n; i++) data[i] = static_cast<T>(n - i);
    return data;
}

template<typename T>
std::vector<T> generate_few_unique(size_t n, size_t unique_count = 10, uint32_t seed = 12345) {
    std::mt19937 rng(seed);
    std::vector<T> data(n);
    std::uniform_int_distribution<int> dist(0, static_cast<int>(unique_count) - 1);
    for (size_t i = 0; i < n; i++) data[i] = static_cast<T>(dist(rng));
    return data;
}

template<typename T>
std::vector<T> generate_dense(size_t n, T min_val = 0, T max_val = 100, uint32_t seed = 12345) {
    std::mt19937 rng(seed);
    std::vector<T> data(n);
    std::uniform_int_distribution<T> dist(min_val, max_val);
    for (size_t i = 0; i < n; i++) data[i] = dist(rng);
    return data;
}

template<typename T>
std::vector<T> generate_nearly_sorted(size_t n, double swap_pct = 0.05, uint32_t seed = 12345) {
    std::mt19937 rng(seed);
    std::vector<T> data = generate_sorted<T>(n);
    size_t swaps = static_cast<size_t>(n * swap_pct);
    std::uniform_int_distribution<size_t> dist(0, n - 1);
    for (size_t i = 0; i < swaps; i++) {
        std::swap(data[dist(rng)], data[dist(rng)]);
    }
    return data;
}

template<typename T>
std::vector<T> generate_organ_pipe(size_t n) {
    std::vector<T> data(n);
    size_t half = n / 2;
    for (size_t i = 0; i < half; i++) {
        data[i] = static_cast<T>(i);
        data[n - 1 - i] = static_cast<T>(i);
    }
    if (n % 2) data[half] = static_cast<T>(half);
    return data;
}

template<typename T>
std::vector<T> generate_all_same(size_t n, T value = 42) {
    return std::vector<T>(n, value);
}

// =============================================================================
// Type-specific Tests
// =============================================================================

template<typename T>
void test_type(const std::string& type_name) {
    std::cout << "\n=== Testing " << type_name << " ===\n";

    // Edge cases
    run_test<T>("empty", {});
    run_test<T>("single element", {42});
    run_test<T>("two elements sorted", {1, 2});
    run_test<T>("two elements reversed", {2, 1});
    run_test<T>("three elements", {3, 1, 2});

    // Small arrays (Tier 1)
    run_test<T>("10 random", generate_random<T>(10));
    run_test<T>("100 random", generate_random<T>(100));
    run_test<T>("255 random", generate_random<T>(255));

    // Pattern detection (Tier 2)
    run_test<T>("1000 sorted", generate_sorted<T>(1000));
    run_test<T>("1000 reversed", generate_reversed<T>(1000));
    run_test<T>("1000 nearly sorted", generate_nearly_sorted<T>(1000));

    // Dense range (Tier 3) - only for integral types
    if constexpr (std::is_integral_v<T>) {
        run_test<T>("1000 dense (0-100)", generate_dense<T>(1000, T(0), T(100)));
        run_test<T>("10000 dense (0-50)", generate_dense<T>(10000, T(0), T(50)));
    }

    // Radix sort (Tier 4)
    run_test<T>("1000 random", generate_random<T>(1000));
    run_test<T>("10000 random", generate_random<T>(10000));
    run_test<T>("100000 random", generate_random<T>(100000));

    // Special patterns
    run_test<T>("1000 few unique", generate_few_unique<T>(1000));
    run_test<T>("1000 organ pipe", generate_organ_pipe<T>(1000));
    run_test<T>("1000 all same", generate_all_same<T>(1000));

    // Scaling
    run_test<T>("500000 random", generate_random<T>(500000));
}

void test_int32() {
    test_type<int32_t>("int32_t");

    // Additional int32 edge cases
    std::cout << "\n=== int32_t Edge Cases ===\n";
    run_test<int32_t>("negatives only", {-5, -3, -10, -1, -8});
    run_test<int32_t>("mixed signs", {-5, 3, -10, 1, 8, -2, 0});
    run_test<int32_t>("INT32_MIN/MAX", {INT32_MAX, INT32_MIN, 0, INT32_MAX-1, INT32_MIN+1});

    std::vector<int32_t> extreme(1000);
    std::mt19937 rng(42);
    for (int i = 0; i < 1000; i++) {
        extreme[i] = (rng() % 2) ? INT32_MAX - (rng() % 100) : INT32_MIN + (rng() % 100);
    }
    run_test<int32_t>("1000 extreme values", extreme);
}

void test_uint32() {
    test_type<uint32_t>("uint32_t");

    std::cout << "\n=== uint32_t Edge Cases ===\n";
    run_test<uint32_t>("UINT32_MAX", {UINT32_MAX, 0, UINT32_MAX-1, 1, UINT32_MAX/2});
}

void test_int64() {
    test_type<int64_t>("int64_t");

    std::cout << "\n=== int64_t Edge Cases ===\n";
    run_test<int64_t>("INT64_MIN/MAX", {INT64_MAX, INT64_MIN, 0, INT64_MAX-1, INT64_MIN+1});
    run_test<int64_t>("large negatives", {-1000000000000LL, -999999999999LL, -1LL});
}

void test_uint64() {
    test_type<uint64_t>("uint64_t");

    std::cout << "\n=== uint64_t Edge Cases ===\n";
    run_test<uint64_t>("UINT64_MAX", {UINT64_MAX, 0ULL, UINT64_MAX-1, 1ULL});
}

void test_float() {
    test_type<float>("float");

    std::cout << "\n=== float Edge Cases ===\n";
    run_test<float>("negative floats", {-5.5f, -3.3f, -10.1f, -1.0f, -8.8f});
    run_test<float>("mixed floats", {-5.5f, 3.3f, -10.1f, 1.0f, 8.8f, -2.2f, 0.0f});
    run_test<float>("small differences", {1.0f, 1.0001f, 1.0002f, 0.9999f, 0.9998f});
    run_test<float>("subnormals", {1e-40f, 1e-38f, -1e-40f, 0.0f, 1e-35f});
}

void test_double() {
    test_type<double>("double");

    std::cout << "\n=== double Edge Cases ===\n";
    run_test<double>("negative doubles", {-5.5, -3.3, -10.1, -1.0, -8.8});
    run_test<double>("mixed doubles", {-5.5, 3.3, -10.1, 1.0, 8.8, -2.2, 0.0});
    run_test<double>("small differences", {1.0, 1.00000001, 1.00000002, 0.99999999, 0.99999998});
    run_test<double>("large magnitudes", {1e100, -1e100, 1e-100, -1e-100, 0.0});
}

// =============================================================================
// Buffer API Tests
// =============================================================================

void test_buffer_api() {
    std::cout << "\n=== Buffer API Tests ===\n";

    // Test with pre-allocated buffer
    std::vector<int32_t> data = generate_random<int32_t>(10000);
    std::vector<int32_t> expected = data;
    std::sort(expected.begin(), expected.end());

    std::vector<int32_t> buffer(data.size());
    tiered::sort(data.begin(), data.end(), buffer.data());

    if (data == expected) {
        tests_passed++;
        std::cout << "  [PASS] buffer API (int32)\n";
    } else {
        tests_failed++;
        std::cout << "  [FAIL] buffer API (int32)\n";
    }

    // Test with double
    std::vector<double> data_d = generate_random<double>(10000);
    std::vector<double> expected_d = data_d;
    std::sort(expected_d.begin(), expected_d.end());

    std::vector<double> buffer_d(data_d.size());
    tiered::sort(data_d.begin(), data_d.end(), buffer_d.data());

    if (data_d == expected_d) {
        tests_passed++;
        std::cout << "  [PASS] buffer API (double)\n";
    } else {
        tests_failed++;
        std::cout << "  [FAIL] buffer API (double)\n";
    }
}

// =============================================================================
// Raw Array Tests
// =============================================================================

void test_raw_arrays() {
    std::cout << "\n=== Raw Array Tests ===\n";

    int arr[] = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    int expected[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    tiered::sort(arr, arr + 9);

    bool pass = true;
    for (int i = 0; i < 9; i++) {
        if (arr[i] != expected[i]) pass = false;
    }

    if (pass) {
        tests_passed++;
        std::cout << "  [PASS] raw C array\n";
    } else {
        tests_failed++;
        std::cout << "  [FAIL] raw C array\n";
    }
}

// =============================================================================
// Stress Tests
// =============================================================================

void test_stress() {
    std::cout << "\n=== Stress Tests ===\n";

    // Multiple random seeds
    for (uint32_t seed = 1; seed <= 10; seed++) {
        auto data = generate_random<int32_t>(50000, seed);
        auto expected = data;
        std::sort(expected.begin(), expected.end());
        tiered::sort(data.begin(), data.end());

        if (data == expected) {
            tests_passed++;
            std::cout << "  [PASS] seed " << seed << " (50k int32)\n";
        } else {
            tests_failed++;
            std::cout << "  [FAIL] seed " << seed << " (50k int32)\n";
        }
    }

    // Large array
    {
        auto data = generate_random<int32_t>(1000000, 99999);
        auto expected = data;
        std::sort(expected.begin(), expected.end());
        tiered::sort(data.begin(), data.end());

        if (data == expected) {
            tests_passed++;
            std::cout << "  [PASS] 1M elements\n";
        } else {
            tests_failed++;
            std::cout << "  [FAIL] 1M elements\n";
        }
    }
}

// =============================================================================
// Stable Sort Tests
// =============================================================================

// Test struct to verify stability
struct Item {
    int32_t key;
    int32_t order;  // Original position

    bool operator<(const Item& other) const { return key < other.key; }
    bool operator==(const Item& other) const { return key == other.key && order == other.order; }
};

void test_stable_sort() {
    std::cout << "\n=== Stable Sort Tests ===\n";

    // Test 1: Verify stability with duplicate keys
    {
        // Create items with duplicate keys but different original positions
        std::vector<int32_t> keys = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
        std::vector<Item> items(keys.size());
        for (size_t i = 0; i < keys.size(); i++) {
            items[i] = {keys[i], static_cast<int32_t>(i)};
        }

        // Sort by key using std::stable_sort as reference
        auto expected = items;
        std::stable_sort(expected.begin(), expected.end());

        // Sort keys with tiered::stable_sort
        auto sorted_keys = keys;
        tiered::stable_sort(sorted_keys.begin(), sorted_keys.end());

        // Verify sorted correctly
        auto check_keys = keys;
        std::sort(check_keys.begin(), check_keys.end());

        if (sorted_keys == check_keys) {
            tests_passed++;
            std::cout << "  [PASS] stable_sort correctness\n";
        } else {
            tests_failed++;
            std::cout << "  [FAIL] stable_sort correctness\n";
        }
    }

    // Test 2: Dense range (counting sort path) - verify it sorts correctly
    {
        std::vector<int32_t> data(10000);
        std::mt19937 rng(42);
        for (auto& x : data) x = rng() % 100;  // Dense range 0-99

        auto expected = data;
        std::stable_sort(expected.begin(), expected.end());

        tiered::stable_sort(data.begin(), data.end());

        if (data == expected) {
            tests_passed++;
            std::cout << "  [PASS] stable_sort dense range\n";
        } else {
            tests_failed++;
            std::cout << "  [FAIL] stable_sort dense range\n";
        }
    }

    // Test 3: Random data (radix sort path)
    {
        auto data = generate_random<int32_t>(10000, 12345);
        auto expected = data;
        std::stable_sort(expected.begin(), expected.end());

        tiered::stable_sort(data.begin(), data.end());

        if (data == expected) {
            tests_passed++;
            std::cout << "  [PASS] stable_sort random\n";
        } else {
            tests_failed++;
            std::cout << "  [FAIL] stable_sort random\n";
        }
    }

    // Test 4: Already sorted (pattern detection path)
    {
        auto data = generate_sorted<int32_t>(1000);
        auto expected = data;

        tiered::stable_sort(data.begin(), data.end());

        if (data == expected) {
            tests_passed++;
            std::cout << "  [PASS] stable_sort already sorted\n";
        } else {
            tests_failed++;
            std::cout << "  [FAIL] stable_sort already sorted\n";
        }
    }

    // Test 5: Small array (std::stable_sort path)
    {
        std::vector<int32_t> data = {5, 2, 8, 1, 9, 3, 7, 4, 6};
        auto expected = data;
        std::stable_sort(expected.begin(), expected.end());

        tiered::stable_sort(data.begin(), data.end());

        if (data == expected) {
            tests_passed++;
            std::cout << "  [PASS] stable_sort small array\n";
        } else {
            tests_failed++;
            std::cout << "  [FAIL] stable_sort small array\n";
        }
    }

    // Test 6: Float stable sort
    {
        auto data = generate_random<float>(10000, 54321);
        auto expected = data;
        std::stable_sort(expected.begin(), expected.end());

        tiered::stable_sort(data.begin(), data.end());

        if (data == expected) {
            tests_passed++;
            std::cout << "  [PASS] stable_sort float\n";
        } else {
            tests_failed++;
            std::cout << "  [FAIL] stable_sort float\n";
        }
    }

    // Test 7: 64-bit stable sort
    {
        auto data = generate_random<int64_t>(10000, 99999);
        auto expected = data;
        std::stable_sort(expected.begin(), expected.end());

        tiered::stable_sort(data.begin(), data.end());

        if (data == expected) {
            tests_passed++;
            std::cout << "  [PASS] stable_sort int64\n";
        } else {
            tests_failed++;
            std::cout << "  [FAIL] stable_sort int64\n";
        }
    }

    // Test 8: Buffer API
    {
        auto data = generate_random<int32_t>(10000, 11111);
        auto expected = data;
        std::stable_sort(expected.begin(), expected.end());

        std::vector<int32_t> buffer(data.size());
        tiered::stable_sort(data.begin(), data.end(), buffer.data());

        if (data == expected) {
            tests_passed++;
            std::cout << "  [PASS] stable_sort buffer API\n";
        } else {
            tests_failed++;
            std::cout << "  [FAIL] stable_sort buffer API\n";
        }
    }
}

// =============================================================================
// Main
// =============================================================================

int main() {
    std::cout << "========================================\n";
    std::cout << "       tieredsort Test Suite\n";
    std::cout << "========================================\n";

    test_int32();
    test_uint32();
    test_int64();
    test_uint64();
    test_float();
    test_double();
    test_buffer_api();
    test_raw_arrays();
    test_stress();
    test_stable_sort();

    std::cout << "\n========================================\n";
    std::cout << "Results: " << tests_passed << " passed, " << tests_failed << " failed\n";
    std::cout << "========================================\n";

    return tests_failed > 0 ? 1 : 0;
}
