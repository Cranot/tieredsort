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
std::sort      5,352 us     3,082 us         1.0x
ska_sort       2,297 us       976 us         2.3x
tieredsort     1,402 us       267 us         3.8x - 11.5x
```

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
- Compiled with `-O3 -std=c++17 -march=native`

### Results (n = 100,000)

| Pattern | std::sort | ska_sort | tieredsort | vs std | vs ska |
|---------|----------:|---------:|-----------:|-------:|-------:|
| Random | 5,352 us | 2,297 us | **1,402 us** | **3.8x** | **1.6x** |
| Sorted | 1,894 us | 1,668 us | 1,598 us | 1.2x | 1.0x |
| Reversed | 1,354 us | 1,697 us | 1,443 us | 0.9x | 1.2x |
| Nearly Sorted | 2,306 us | 2,082 us | 2,073 us | 1.1x | 1.0x |
| Few Unique (10) | 2,344 us | 860 us | **276 us** | **8.5x** | **3.1x** |
| Dense (0-100) | 3,082 us | 976 us | **267 us** | **11.5x** | **3.7x** |

### Results (n = 1,000,000)

| Pattern | std::sort | ska_sort | tieredsort | vs std | vs ska |
|---------|----------:|---------:|-----------:|-------:|-------:|
| Random | 64,269 us | 25,124 us | **20,204 us** | **3.2x** | **1.2x** |
| Dense (0-100) | 34,216 us | 10,079 us | **3,595 us** | **9.5x** | **2.8x** |

### When to Use tieredsort

| Data Type | Speedup | Recommendation |
|-----------|---------|----------------|
| Random integers | 3-4x vs std::sort | Use tieredsort |
| Dense values (ages, scores) | 8-12x vs std::sort | Use tieredsort |
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

### Stable Sort

```cpp
// Equal elements maintain their relative order
std::vector<int> scores = {85, 90, 85, 92, 90};
tiered::stable_sort(scores.begin(), scores.end());
// First 85 stays before second 85, first 90 stays before second 90
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

| Library | Type | Random | Dense | Stable | Types |
|---------|------|--------|-------|--------|-------|
| std::sort | Introsort | 1.0x | 1.0x | No | Any |
| pdqsort | Pattern-defeating QS | 2.0x | 2.0x | No | Any |
| ska_sort | Hybrid radix | 2.3x | 3.2x | No | Numeric |
| **tieredsort** | **3-tier adaptive** | **3.8x** | **11.5x** | Yes | Numeric |
| vqsort | SIMD (AVX2+) | 10-20x | - | No | Numeric |

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

### `tiered::stable_sort(first, last)`

Stable sort - equal elements maintain their relative order.

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
