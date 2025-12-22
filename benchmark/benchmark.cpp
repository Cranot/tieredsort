/*
 * tieredsort - Benchmark Suite
 *
 * Compares tieredsort against std::sort and std::stable_sort.
 * Run with: g++ -std=c++17 -O3 -o benchmark benchmark/benchmark.cpp && ./benchmark
 */

#include "tieredsort.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <string>
#include <functional>

// =============================================================================
// Timing Utilities
// =============================================================================

template<typename Func>
double measure_us(Func&& f, int runs = 5) {
    // Warmup
    f();

    double total = 0;
    for (int i = 0; i < runs; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        f();
        auto end = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration<double, std::micro>(end - start).count();
    }
    return total / runs;
}

// =============================================================================
// Data Generators
// =============================================================================

std::vector<int32_t> gen_random(size_t n, uint32_t seed = 12345) {
    std::mt19937 rng(seed);
    std::vector<int32_t> data(n);
    for (size_t i = 0; i < n; i++) data[i] = static_cast<int32_t>(rng());
    return data;
}

std::vector<int32_t> gen_sorted(size_t n) {
    std::vector<int32_t> data(n);
    for (size_t i = 0; i < n; i++) data[i] = static_cast<int32_t>(i);
    return data;
}

std::vector<int32_t> gen_reversed(size_t n) {
    std::vector<int32_t> data(n);
    for (size_t i = 0; i < n; i++) data[i] = static_cast<int32_t>(n - i);
    return data;
}

std::vector<int32_t> gen_few_unique(size_t n, uint32_t seed = 12345) {
    std::mt19937 rng(seed);
    std::vector<int32_t> data(n);
    for (size_t i = 0; i < n; i++) data[i] = static_cast<int32_t>(rng() % 10);
    return data;
}

std::vector<int32_t> gen_dense(size_t n, uint32_t seed = 12345) {
    std::mt19937 rng(seed);
    std::vector<int32_t> data(n);
    for (size_t i = 0; i < n; i++) data[i] = static_cast<int32_t>(rng() % 101);  // 0-100
    return data;
}

std::vector<int32_t> gen_nearly_sorted(size_t n, uint32_t seed = 12345) {
    std::mt19937 rng(seed);
    auto data = gen_sorted(n);
    size_t swaps = n / 20;  // 5% swaps
    for (size_t i = 0; i < swaps; i++) {
        size_t a = rng() % n;
        size_t b = rng() % n;
        std::swap(data[a], data[b]);
    }
    return data;
}

std::vector<int32_t> gen_organ_pipe(size_t n) {
    std::vector<int32_t> data(n);
    size_t half = n / 2;
    for (size_t i = 0; i < half; i++) {
        data[i] = static_cast<int32_t>(i);
        data[n - 1 - i] = static_cast<int32_t>(i);
    }
    if (n % 2) data[half] = static_cast<int32_t>(half);
    return data;
}

std::vector<int32_t> gen_zipf(size_t n, uint32_t seed = 12345) {
    std::mt19937 rng(seed);
    std::vector<int32_t> data(n);
    // Simple Zipf-like distribution
    for (size_t i = 0; i < n; i++) {
        double u = std::uniform_real_distribution<>(0.0, 1.0)(rng);
        data[i] = static_cast<int32_t>(std::pow(n, u));
    }
    return data;
}

// =============================================================================
// Benchmark Runner
// =============================================================================

struct BenchResult {
    std::string pattern;
    double std_sort;
    double stable_sort;
    double tieredsort;
    double speedup;
};

void print_header() {
    std::cout << "\n";
    std::cout << std::setw(20) << "Pattern"
              << std::setw(15) << "std::sort"
              << std::setw(15) << "stable_sort"
              << std::setw(15) << "tieredsort"
              << std::setw(15) << "Speedup"
              << "\n";
    std::cout << std::string(80, '-') << "\n";
}

void print_result(const BenchResult& r) {
    std::cout << std::setw(20) << r.pattern
              << std::setw(12) << std::fixed << std::setprecision(0) << r.std_sort << " us"
              << std::setw(12) << std::fixed << std::setprecision(0) << r.stable_sort << " us"
              << std::setw(12) << std::fixed << std::setprecision(0) << r.tieredsort << " us"
              << std::setw(12) << std::fixed << std::setprecision(2) << r.speedup << "x"
              << "\n";
}

template<typename Generator>
BenchResult benchmark_pattern(const std::string& name, Generator gen, size_t n) {
    BenchResult result;
    result.pattern = name;

    // Benchmark std::sort
    {
        auto data = gen(n);
        result.std_sort = measure_us([&]() {
            auto copy = data;
            std::sort(copy.begin(), copy.end());
        });
    }

    // Benchmark std::stable_sort
    {
        auto data = gen(n);
        result.stable_sort = measure_us([&]() {
            auto copy = data;
            std::stable_sort(copy.begin(), copy.end());
        });
    }

    // Benchmark tieredsort
    {
        auto data = gen(n);
        result.tieredsort = measure_us([&]() {
            auto copy = data;
            tiered::sort(copy.begin(), copy.end());
        });
    }

    result.speedup = result.std_sort / result.tieredsort;
    return result;
}

void run_benchmarks(size_t n) {
    std::cout << "\n========================================\n";
    std::cout << "     tieredsort Benchmark (n = " << n << ")\n";
    std::cout << "========================================\n";

    print_header();

    std::vector<BenchResult> results;

    results.push_back(benchmark_pattern("Random", [](size_t n) { return gen_random(n, 12345); }, n));
    results.push_back(benchmark_pattern("Sorted", gen_sorted, n));
    results.push_back(benchmark_pattern("Reversed", gen_reversed, n));
    results.push_back(benchmark_pattern("Nearly Sorted", [](size_t n) { return gen_nearly_sorted(n, 12345); }, n));
    results.push_back(benchmark_pattern("Few Unique", [](size_t n) { return gen_few_unique(n, 12345); }, n));
    results.push_back(benchmark_pattern("Dense (0-100)", [](size_t n) { return gen_dense(n, 12345); }, n));
    results.push_back(benchmark_pattern("Organ Pipe", gen_organ_pipe, n));
    results.push_back(benchmark_pattern("Zipf", [](size_t n) { return gen_zipf(n, 12345); }, n));

    for (const auto& r : results) {
        print_result(r);
    }

    // Summary
    double total_std = 0, total_tiered = 0;
    for (const auto& r : results) {
        total_std += r.std_sort;
        total_tiered += r.tieredsort;
    }

    std::cout << std::string(80, '-') << "\n";
    std::cout << std::setw(20) << "TOTAL"
              << std::setw(12) << std::fixed << std::setprecision(0) << total_std << " us"
              << std::setw(15) << ""
              << std::setw(12) << std::fixed << std::setprecision(0) << total_tiered << " us"
              << std::setw(12) << std::fixed << std::setprecision(2) << (total_std / total_tiered) << "x"
              << "\n";
}

void run_scaling_benchmark() {
    std::cout << "\n========================================\n";
    std::cout << "     Scaling Benchmark (Random Data)\n";
    std::cout << "========================================\n";

    std::cout << "\n";
    std::cout << std::setw(15) << "Size"
              << std::setw(15) << "std::sort"
              << std::setw(15) << "tieredsort"
              << std::setw(15) << "Speedup"
              << "\n";
    std::cout << std::string(60, '-') << "\n";

    for (size_t n : {1000, 10000, 100000, 500000, 1000000}) {
        auto data = gen_random(n);

        double std_time = measure_us([&]() {
            auto copy = data;
            std::sort(copy.begin(), copy.end());
        });

        double tiered_time = measure_us([&]() {
            auto copy = data;
            tiered::sort(copy.begin(), copy.end());
        });

        std::cout << std::setw(15) << n
                  << std::setw(12) << std::fixed << std::setprecision(0) << std_time << " us"
                  << std::setw(12) << std::fixed << std::setprecision(0) << tiered_time << " us"
                  << std::setw(12) << std::fixed << std::setprecision(2) << (std_time / tiered_time) << "x"
                  << "\n";
    }
}

void run_type_benchmarks() {
    std::cout << "\n========================================\n";
    std::cout << "     Type Comparison (n = 100,000)\n";
    std::cout << "========================================\n";

    size_t n = 100000;
    std::mt19937 rng(12345);

    auto bench_type = [&](const std::string& name, auto gen_func) {
        auto data = gen_func();
        using T = typename decltype(data)::value_type;

        double std_time = measure_us([&]() {
            auto copy = data;
            std::sort(copy.begin(), copy.end());
        });

        double tiered_time = measure_us([&]() {
            auto copy = data;
            tiered::sort(copy.begin(), copy.end());
        });

        std::cout << std::setw(15) << name
                  << std::setw(12) << std::fixed << std::setprecision(0) << std_time << " us"
                  << std::setw(12) << std::fixed << std::setprecision(0) << tiered_time << " us"
                  << std::setw(12) << std::fixed << std::setprecision(2) << (std_time / tiered_time) << "x"
                  << "\n";
    };

    std::cout << "\n";
    std::cout << std::setw(15) << "Type"
              << std::setw(15) << "std::sort"
              << std::setw(15) << "tieredsort"
              << std::setw(15) << "Speedup"
              << "\n";
    std::cout << std::string(60, '-') << "\n";

    bench_type("int32_t", [&]() {
        std::vector<int32_t> v(n);
        for (size_t i = 0; i < n; i++) v[i] = static_cast<int32_t>(rng());
        return v;
    });

    bench_type("uint32_t", [&]() {
        std::vector<uint32_t> v(n);
        for (size_t i = 0; i < n; i++) v[i] = static_cast<uint32_t>(rng());
        return v;
    });

    bench_type("int64_t", [&]() {
        std::vector<int64_t> v(n);
        for (size_t i = 0; i < n; i++) v[i] = static_cast<int64_t>(rng()) << 32 | rng();
        return v;
    });

    bench_type("uint64_t", [&]() {
        std::vector<uint64_t> v(n);
        for (size_t i = 0; i < n; i++) v[i] = static_cast<uint64_t>(rng()) << 32 | rng();
        return v;
    });

    bench_type("float", [&]() {
        std::vector<float> v(n);
        std::uniform_real_distribution<float> dist(-1e6f, 1e6f);
        for (size_t i = 0; i < n; i++) v[i] = dist(rng);
        return v;
    });

    bench_type("double", [&]() {
        std::vector<double> v(n);
        std::uniform_real_distribution<double> dist(-1e10, 1e10);
        for (size_t i = 0; i < n; i++) v[i] = dist(rng);
        return v;
    });
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char** argv) {
    size_t n = 100000;
    if (argc > 1) {
        n = std::stoull(argv[1]);
    }

    std::cout << "========================================\n";
    std::cout << "       tieredsort Benchmark Suite\n";
    std::cout << "========================================\n";

    run_benchmarks(n);
    run_scaling_benchmark();
    run_type_benchmarks();

    std::cout << "\n========================================\n";
    std::cout << "             Benchmark Complete\n";
    std::cout << "========================================\n";

    return 0;
}
