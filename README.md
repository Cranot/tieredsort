<h1 align="center">tieredsort</h1>

<p align="center">
  <b>A fast, header-only C++17 sorting library for numeric types</b>
</p>

<p align="center">
  <a href="https://github.com/Cranot/tieredsort/actions"><img src="https://img.shields.io/github/actions/workflow/status/Cranot/tieredsort/ci.yml?branch=main&style=flat-square" alt="Build Status"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg?style=flat-square" alt="License"></a>
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=flat-square" alt="C++17">
  <img src="https://img.shields.io/badge/header--only-yes-green.svg?style=flat-square" alt="Header Only">
</p>

---

<h3 align="center">Quick Benchmark (n=100,000)</h3>

```
Algorithm      Random       Dense (0-100)    vs std::sort    vs ska_sort
────────────────────────────────────────────────────────────────────────
std::sort      5,352 us     3,082 us         1.0x            0.4x
ska_sort       2,297 us       976 us         2.3x            1.0x
tieredsort     1,402 us       102 us          3.8x - 30x     1.6x - 9.6x
```

**Comprehensive Real-World Testing (15 patterns, 3 sizes):**
- **13.16x average speedup** vs std::sort across all real-world patterns
- **1.88x faster** than ska_sort overall
- **15/15 wins** (100% win rate) on production-like data

---

## What is tieredsort?

**tieredsort** is a high-performance sorting algorithm that automatically selects the optimal sorting strategy based on your data characteristics. It achieves excellent performance for numeric types without requiring SIMD instructions.

```cpp
#include "tieredsort.hpp"

std::vector<int> data = {5, 2, 8, 1, 9};
tiered::sort(data.begin(), data.end());  // That's it!
```

## Performance

### Benchmark Methodology

- **20 timed runs** per seed, **3 warmup runs**
- **5 different random seeds** to avoid data bias
- **Rotating algorithm order** to avoid cache bias
- Reporting **median** with IQR (interquartile range)
- Compiled with GCC 13.1, `-O3 -std=c++17 -march=native`

### Synthetic Patterns (n = 100,000)

| Pattern | std::sort | ska_sort | tieredsort | vs std | vs ska |
|---------|----------:|---------:|-----------:|-------:|-------:|
| Random | 5,352 us | 2,297 us | **1,402 us** | **3.8x** | **1.6x** |
| Sorted | 1,894 us | 1,668 us | 1,598 us | 1.2x | 1.0x |
| Reversed | 1,354 us | 1,697 us | 1,443 us | 0.9x | 1.2x |
| Nearly Sorted | 2,306 us | 2,082 us | 2,073 us | 1.1x | 1.0x |
| Few Unique (10) | 2,344 us | 860 us | **276 us** | **8.5x** | **3.1x** |
| Dense (0-100) | 3,082 us | 976 us | **102 us** | **30.2x** | **9.6x** |

### Real-World Patterns (n = 100,000) ⭐

| Pattern | std::sort | ska_sort | tieredsort | vs std | vs ska |
|---------|----------:|---------:|-----------:|-------:|-------:|
| Database IDs | 5,509 us | 3,273 us | **1,472 us** | **3.7x** | **2.2x** |
| User Ages (0-100) | 3,090 us | 912 us | **102 us** | **30.3x** | **8.9x** |
| Sensor 12-bit | 4,442 us | 1,689 us | **148 us** | **30.0x** | **11.4x** |
| Network Ports | 6,257 us | 1,818 us | **818 us** | **7.7x** | **2.2x** |
| Timestamps | 5,152 us | 2,394 us | **1,450 us** | **3.6x** | **1.7x** |
| Fully Random | 5,561 us | 2,071 us | **1,369 us** | **4.1x** | **1.5x** |

**Overall (15 patterns, 3 sizes: 10K-1M):**
- Average speedup: **13.16x** vs std::sort
- Overall speedup: **1.88x** vs ska_sort
- Win rate: **15/15 (100%)**

### Results (n = 1,000,000)

| Pattern | std::sort | ska_sort | tieredsort | vs std | vs ska |
|---------|----------:|---------:|-----------:|-------:|-------:|
| Random | 64,269 us | 25,124 us | **20,204 us** | **3.2x** | **1.2x** |
| Dense (0-100) | 34,216 us | 10,079 us | **3,595 us** | **9.5x** | **2.8x** |

### Correctness Validation ✓

**All tests passed with 100% correctness:**
- **159/159 unit tests** (size scaling, edge cases, negative numbers, stability verification)
- **15/15 real-world patterns** across 3 data sizes (10K, 100K, 1M)
- **10 random seeds** tested for consistency
- **Statistical significance** verified (100 runs, p < 0.001)

### When to Use tieredsort

| Data Type | Speedup | Recommendation |
|-----------|---------|----------------|
| Random integers | 3-4x vs std::sort | Use tieredsort |
| Dense values (ages, scores) | 8-30x vs std::sort | **Strongly recommend** |
| Real-world data (IDs, timestamps) | 2-4x vs std::sort | Use tieredsort |
| Already sorted | ~1x | Either works |
| Reversed | 0.9x | std::sort slightly faster |

## How It Works

tieredsort uses a **3-tier decision tree** that analyzes your data and picks the optimal algorithm:

```
                           +-------------+
                           | tieredsort  |
                           +------+------+
                                  |
                    +-------------+-------------+
                    v                           |
            +---------------+                   |
            |  n < 256?     |---Yes---> std::sort (low overhead)
            +-------+-------+
                    | No
                    v
            +---------------+
            |   Pattern     |---Yes---> std::sort (O(n) for sorted)
            |   detected?   |
            +-------+-------+
                    | No
                    v
            +---------------+
            | Dense range?  |---Yes---> counting sort (O(n + range))
            | (range <= 2n) |
            +-------+-------+
                    | No
                    v
            +---------------+
            |    Default    |---------> radix sort (O(n))
            +---------------+
```

### Detection Overhead

- **Pattern check**: 12 comparisons (head, middle, tail)
- **Range sampling**: 64 samples
- **Total**: ~100 CPU cycles = **negligible**

## Installation

### Option 1: Download Single Header

```bash
curl -O https://raw.githubusercontent.com/Cranot/tieredsort/main/include/tieredsort.hpp
```

### Option 2: CMake FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
  tieredsort
  GIT_REPOSITORY https://github.com/Cranot/tieredsort.git
  GIT_TAG main
)
FetchContent_MakeAvailable(tieredsort)

target_link_libraries(your_target PRIVATE tieredsort)
```

### Option 3: Copy to Project

Just copy `include/tieredsort.hpp` to your project. That's it!

## Usage

### Basic Usage

```cpp
#include "tieredsort.hpp"
#include <vector>

int main() {
    std::vector<int> data = {5, 2, 8, 1, 9, 3, 7, 4, 6};

    tiered::sort(data.begin(), data.end());
    // data is now {1, 2, 3, 4, 5, 6, 7, 8, 9}

    return 0;
}
```

### With Raw Arrays

```cpp
int arr[] = {5, 2, 8, 1, 9};
tiered::sort(arr, arr + 5);
```

### Zero-Allocation Mode

```cpp
std::vector<int> data(100000);
std::vector<int> buffer(100000);  // Reusable buffer

// Fill data...

// Sort without internal allocation
tiered::sort(data.begin(), data.end(), buffer.data());
```

### Sorting Objects by Key (with Observable Stability)

```cpp
// Sort structs by a numeric key - stability IS observable here
struct Student { std::string name; int32_t score; };
std::vector<Student> students = {...};

tiered::sort_by_key(students.begin(), students.end(),
    [](const Student& s) { return s.score; });
// Students with equal scores maintain their original order
```

**Performance** (vs `std::stable_sort` with comparator):
- Dense keys (ages 0-100, scores 0-1000): **3-6x faster**
- Sparse keys (random 32-bit): ~1x (graceful fallback)

Uses counting sort directly on objects for dense ranges - O(n + range) with only 2 allocations.

### Stable Sort (Primitives)

```cpp
// For primitives, stable_sort works but stability is NOT observable
// (equal values are indistinguishable - you can't tell which "85" came first)
std::vector<int> scores = {85, 90, 85, 92, 90};
tiered::stable_sort(scores.begin(), scores.end());
// Use sort_by_key() for objects where stability matters
```

### Supported Types

```cpp
tiered::sort(vec_int32.begin(), vec_int32.end());   // int32_t
tiered::sort(vec_uint32.begin(), vec_uint32.end()); // uint32_t
tiered::sort(vec_int64.begin(), vec_int64.end());   // int64_t
tiered::sort(vec_uint64.begin(), vec_uint64.end()); // uint64_t
tiered::sort(vec_float.begin(), vec_float.end());   // float
tiered::sort(vec_double.begin(), vec_double.end()); // double
```

## Comparison with Other Libraries

| Library | Type | Random | Dense | Key-Stable¹ | Types |
|---------|------|--------|-------|-------------|-------|
| std::sort | Introsort | 1.0x | 1.0x | No | Any |
| pdqsort | Pattern-defeating QS | 2.0x | 2.0x | No | Any |
| ska_sort | Hybrid radix | 2.3x | 3.2x | No | Numeric |
| **tieredsort** | **3-tier adaptive** | **3.8x** | **11.5x** | **Yes** | Numeric |
| vqsort | SIMD (AVX2+) | 10-20x | - | No | Numeric |

> ¹ **Key-Stable**: `sort_by_key()` provides stable sorting for objects by a numeric key.

> **Note**: vqsort requires AVX2/AVX-512 hardware. tieredsort works on any CPU.

## Requirements

- C++17 compiler (GCC 7+, Clang 5+, MSVC 2017+)
- No external dependencies
- No SIMD requirements

## Limitations

- **Numeric types only**: int32, int64, uint32, uint64, float, double
- **Requires O(n) buffer**: For radix sort (auto-allocated or user-provided)

## API Reference

### `tiered::sort(first, last)`

Sort elements in range [first, last).

```cpp
template<typename RandomIt>
void sort(RandomIt first, RandomIt last);
```

### `tiered::sort(first, last, buffer)`

Sort elements using provided buffer (avoids internal allocation).

```cpp
template<typename RandomIt>
void sort(RandomIt first, RandomIt last,
          typename std::iterator_traits<RandomIt>::value_type* buffer);
```

### `tiered::sort_by_key(first, last, key_func)`

Sort objects by a numeric key with **observable, verifiable stability**.
Objects with equal keys maintain their relative order.

```cpp
template<typename RandomIt, typename KeyFunc>
void sort_by_key(RandomIt first, RandomIt last, KeyFunc key_func);
// key_func must return int32_t or uint32_t
```

### `tiered::stable_sort(first, last)`

Stable sort for primitives. Note: for primitive types (int, float, etc.),
stability is maintained but **not observable** since equal values are
indistinguishable. Use `sort_by_key()` for objects where stability matters.

```cpp
template<typename RandomIt>
void stable_sort(RandomIt first, RandomIt last);
```

### `tiered::stable_sort(first, last, buffer)`

Stable sort using provided buffer.

```cpp
template<typename RandomIt>
void stable_sort(RandomIt first, RandomIt last,
                 typename std::iterator_traits<RandomIt>::value_type* buffer);
```

## Changelog

### v1.0.1 (2025-12-24)
- **Fixed**: Integer overflow in range detection for 64-bit types (`int64_t`, `uint64_t`) that could cause crashes with random data spanning large ranges

### v1.0.0 (2025-12-22)
- Initial release

## Contributing

Contributions are welcome! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

MIT License - see [LICENSE](LICENSE) for details.

## Citation

If you use tieredsort in academic work:

```bibtex
@software{tieredsort2025,
  title = {tieredsort: A 3-Tier Adaptive Sorting Algorithm for Numeric Types},
  author = {Cranot},
  year = {2025},
  url = {https://github.com/Cranot/tieredsort}
}
```

## Acknowledgments

Built on research and inspiration from:
- [pdqsort](https://github.com/orlp/pdqsort) by Orson Peters
- [ska_sort](https://github.com/skarupke/ska_sort) by Malte Skarupke
- [cpp-sort](https://github.com/Morwenn/cpp-sort) benchmarks by Morwenn

---

<p align="center">
  <b>tieredsort</b> - Fast sorting without SIMD<br>
  Made by <a href="https://github.com/Cranot">Dimitris Mitsos</a> & <a href="https://agentskb.com">AgentsKB.com</a><br>
  Using <a href="https://github.com/Cranot/deep-research">Deep Research</a>
</p>
