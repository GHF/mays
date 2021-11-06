🧮 mays 🌽
==========
[![Tests](https://github.com/GHF/mays/actions/workflows/unit_tests.yml/badge.svg?branch=dev&event=push)](https://github.com/GHF/mays/actions/workflows/unit_tests.yml)
[![Coverage](https://codecov.io/gh/GHF/mays/branch/dev/graph/badge.svg?token=Q34FIKR6FN)](https://codecov.io/gh/GHF/mays)
[![Formatting](https://github.com/GHF/mays/actions/workflows/clang-format-check.yml/badge.svg?branch=dev&event=push)](https://github.com/GHF/mays/actions/workflows/clang-format-check.yml)
[![Static analysis](https://github.com/GHF/mays/actions/workflows/clang-tidy-push.yml/badge.svg?branch=dev&event=push)](https://github.com/GHF/mays/actions/workflows/clang-tidy-push.yml)

`mays` is a well-tested header-only collection of “safe-ish” C++ integer math routines intended for
GNU-compatible compilers (Clang and GCC). They are intended to help coders avoid subtle machine and
C/C++ arithmetic pitfalls, as well as perform compile-time safety checks and precomputation with
`constexpr`.

Example
-------
This uses the [`Scale`](/mays/scale.h) and [`Reduce`](/mays/reduce.h) routines to build a `Scaler`
that scales `int16_t` values at compile time:

```c++
#include "mays/reduce.h"
#include "mays/scale.h"

// Simplify the ratio 51 / 114 to 17 / 38 at compile time.
constexpr std::tuple ratio = mays::Reduce(int16_t{51}, int16_t{114});

// Construct a Scaler that operates on int16_t values using |ratio|.
// Because this is marked constexpr, if the ratio can overflow the scaling
// computation then this will fail to compile.
constexpr mays::Scaler scaler = mays::MakeScaler<int16_t>(ratio);

// This performs a scaling operation on the value 30000. The return type is
// std::optional<int16_t> to represent the possibility of overflow when
// scaling up numbers. The ratio here is ≤ 1 so overflow is not possible.
constexpr std::optional value = scaler.Scale(int16_t{30'000});
static_assert(value == 13'421);

// It's also possible to use a different rounding policy.
static_assert(scaler.Scale(int16_t{30'000},
                           mays::RoundPolicy::kRoundAwayFromZero) == 13'422);

// |scaler| can be used on values not known at compile time, e.g. if you're
// building a viewport resizing function:
std::function<int16_t(int16_t)> get_resized_dimension = [scaler](int16_t dim) {
  return scaler.Scale(dim).value();
};
```

Usage
-----
The included `CMakeLists.txt` exports a CMake library target `mays`. “Linking” your project against
it will pull in the necessary include directory, which is intended to be the root of this project:

```cmake
# Assuming the mays project has already been included
target_link_libraries(${YOUR_PROJECT_NAME} PRIVATE mays)
```

To download the code from GitHub and include the `mays` CMake project in your own project,
[FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) is a good choice:

```cmake
Include(FetchContent)

FetchContent_Declare(
  mays
  GIT_REPOSITORY https://github.com/GHF/mays.git
  GIT_TAG        dev
)

FetchContent_MakeAvailable(mays)

add_executable(my_program main.cpp)
target_link_libraries(my_program PRIVATE mays)
```

To build and run the unit tests, download the code and build it with CMake:

```
git clone https://github.com/GHF/mays.git
cd mays
cmake -S . -B build
cd build
cmake --build .
ctest
```

For other build systems, it's only necessary to place the header files into your build:

```
top level
├── LICENSE
├── README.md
└── mays
    ├── internal
    │   └── ….h
    └── ….h
```

Guide
-----
The code can be logically organized by their intended purpose:

### Overflow-safe basic arithmetic
- [Add](/mays/add.h)
- [Divide](/mays/divide.h) Flexible rounding mode using its [`RoundPolicy`](/mays/round_policy.h) parameter
- [DivideRoundNearest](/mays/divide_round_nearest.h)
- [DivideRoundUp](/mays/divide_round_up.h)
- [Multiply](/mays/multiply.h)
- [Subtract](/mays/subtract.h)

### Safety-conscious computation
- [Average](/mays/average.h)
- [Clamp](/mays/clamp.h)
- [Nabs](/mays/nabs.h)
- [Scale](/mays/scale.h)

### Numeric utilities
- [ArraySize](/mays/array_size.h)
- [NegateIf](/mays/negate_if.h)
- [Reduce](/mays/reduce.h)
- [SignOf](/mays/sign_of.h)

### Opinionated tasks
- [RangeMap](/mays/range_map.h) Joystick-to-process value mapping code
- [Crc](/mays/crc.h) Single-header (no C++ or mays includes) CRC with compile-time generated look-up tables

License
-------
`mays` is licensed under the [Apache 2.0 license](/LICENSE).
