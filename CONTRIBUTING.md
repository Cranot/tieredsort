# Contributing to tieredsort

Thank you for your interest in contributing to tieredsort! This document provides guidelines and information for contributors.

## How to Contribute

### Reporting Bugs

1. Check if the bug has already been reported in [Issues](https://github.com/Cranot/tieredsort/issues)
2. If not, create a new issue with:
   - A clear, descriptive title
   - Steps to reproduce the bug
   - Expected vs actual behavior
   - Your environment (OS, compiler, compiler version)
   - Minimal code example if possible

### Suggesting Features

1. Open an issue with the `enhancement` label
2. Describe the feature and its use case
3. If possible, provide a rough implementation idea

### Submitting Pull Requests

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Make your changes
4. Run tests: `./test`
5. Run benchmarks to ensure no regression: `./benchmark`
6. Commit with a clear message: `git commit -m "Add feature X"`
7. Push and create a Pull Request

## Development Setup

```bash
# Clone your fork
git clone https://github.com/Cranot/tieredsort.git
cd tieredsort

# Build with CMake
mkdir build && cd build
cmake .. -DTIEREDSORT_BUILD_TESTS=ON -DTIEREDSORT_BUILD_BENCHMARKS=ON
cmake --build .

# Run tests
./test_tieredsort

# Run benchmarks
./benchmark
```

## Code Style

- Use C++17 features where appropriate
- Follow the existing code style
- Use meaningful variable names
- Add comments for complex logic
- Keep functions focused and small

### Naming Conventions

```cpp
// Functions: snake_case
void radix_sort_32(int* arr, size_t n);

// Types/Classes: PascalCase (but we mostly use functions)
struct SortConfig { ... };

// Constants: UPPER_SNAKE_CASE
constexpr int SMALL_ARRAY_THRESHOLD = 256;

// Namespaces: lowercase
namespace tiered { ... }
```

## Testing

All contributions must include tests. Add tests to `tests/test_tieredsort.cpp`:

```cpp
// Test pattern: test_<type>_<pattern>
void test_int32_random() {
    std::vector<int32_t> data = generate_random<int32_t>(10000);
    auto expected = data;
    std::sort(expected.begin(), expected.end());

    tiered::sort(data.begin(), data.end());

    assert(data == expected);
}
```

## Benchmarking

When adding new features, ensure they don't regress performance:

```bash
# Before your changes
./benchmark > before.txt

# After your changes
./benchmark > after.txt

# Compare
diff before.txt after.txt
```

## Areas of Interest

We're particularly interested in contributions for:

### 1. SIMD Implementation (High Priority)
- AVX2 implementation for 8x int32 parallelism
- AVX-512 for even more parallelism
- ARM NEON for mobile/ARM servers

### 2. Additional Types
- int16_t, uint16_t support
- int8_t, uint8_t support
- Custom key extraction (like ska_sort)

### 3. Parallel Sorting
- Thread pool for very large arrays
- Work-stealing for better load balancing

### 4. Benchmarks
- More real-world data patterns
- Comparison with more libraries
- Memory bandwidth analysis

## Questions?

Feel free to open an issue with the `question` label or reach out directly.

## License

By contributing, you agree that your contributions will be licensed under the MIT License.
