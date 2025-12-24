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
Algorithm      Random       Dense (0-100)    vs std::sort
─────────────────────────────────────────────────────────
std::sort      5,364 us     3,347 us         1.0x
tieredsort     1,494 us       159 us         3.6x - 21x
```

**Highlights:**
- **3.6x faster** on random data
- **16-21x faster** on dense/few-unique data
- **6.2x faster** on Zipf distributions

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

| Pattern | std::sort | tieredsort | vs std |
|---------|----------:|-----------:|-------:|
| Random | 5,364 us | **1,494 us** | **3.6x** |
| Sorted | 1,442 us | 1,440 us | 1.0x |
| Reversed | 1,123 us | 1,148 us | 0.98x |
| Nearly Sorted | 2,009 us | 2,081 us | 0.97x |
| Few Unique (10) | 2,595 us | **159 us** | **16.3x** |
| Dense (0-100) | 3,347 us | **159 us** | **21.1x** |
| Organ Pipe | 6,820 us | 6,715 us | 1.0x |
| Zipf | 4,278 us | **685 us** | **6.2x** |

### Scaling Benchmark (Random Data)

| Size | std::sort | tieredsort | Speedup |
|-----:|----------:|-----------:|--------:|
| 1,000 | 29 us | 11 us | 2.5x |
| 10,000 | 437 us | 218 us | 2.0x |
| 100,000 | 5,358 us | 1,489 us | 3.6x |
| 500,000 | 30,974 us | 9,750 us | 3.2x |
| 1,000,000 | 65,216 us | 20,602 us | 3.2x |

### Type Performance (n = 100,000)

| Type | std::sort | tieredsort | Speedup |
|------|----------:|-----------:|--------:|
| int32_t | 5,519 us | 1,481 us | **3.7x** |
| uint32_t | 5,218 us | 1,475 us | **3.5x** |
| int64_t | 5,416 us | 3,211 us | 1.7x |
| uint64_t | 5,403 us | 3,962 us | 1.4x |
| float | 7,370 us | 1,433 us | **5.1x** |
| double | 7,506 us | 4,132 us | 1.8x |

### Correctness Validation ✓

**All tests passed with 100% correctness:**
- **159/159 unit tests** (size scaling, edge cases, negative numbers, stability verification)
- **15/15 real-world patterns** across 3 data sizes (10K, 100K, 1M)
- **10 random seeds** tested for consistency
- **Statistical significance** verified (100 runs, p < 0.001)

### When to Use tieredsort

| Data Type | Speedup | Recommendation |
|-----------|---------|----------------|
| Random integers | 3.6x | Use tieredsort |
| Dense values (ages, scores) | 16-21x | **Strongly recommend** |
| Zipf distributions | 6.2x | Use tieredsort |
| 32-bit types (int32, float) | 3.5-5x | Use tieredsort |
| 64-bit types | 1.4-1.8x | Modest improvement |
| Already sorted/reversed | ~1x | Either works |

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
| pdqsort | Pattern-defeating QS | ~2x | ~2x | No | Any |
| **tieredsort** | **3-tier adaptive** | **3.6x** | **21x** | **Yes** | Numeric |
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
- **Perf**: Lazy allocation - no longer allocates temp buffer for sorted, reversed, or dense data paths (eliminates ~400KB allocation overhead for these cases)

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
