// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#include "average.h"

#include <catch2/catch.hpp>

namespace mays {
namespace {

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("Average(a, b) = Average(b, a)",
                   "[average]",
                   unsigned int,
                   int,
                   int8_t,
                   uint8_t,
                   int64_t,
                   uint64_t) {
  const auto [a, b, c] = GENERATE(table<TestType, TestType, TestType>({
      {1, 7, 4},
      {2, 7, 4},
  }));
  CAPTURE(a, b);
  CHECK(c == Average(a, b));
  CHECK(c == Average(b, a));
  if constexpr (std::is_signed_v<TestType>) {
    CHECK(-c == Average(-a, -b));
    CHECK(-c == Average(-b, -a));
  }

#ifdef __cpp_lib_interpolate
  CHECK(Average(a, b) == std::midpoint(a, b));

  // For odd sums, Average rounds towards 0 while std::midpoint rounds towards the first argument.
  if ((a + b) % 2 == 1) {
    CHECK(Average(b, a) != std::midpoint(b, a));
  } else {
    CHECK(Average(b, a) == std::midpoint(b, a));
  }
#endif  // __cpp_lib_interpolate
}

TEST_CASE("Average(a + 1, b + 1) = Average(a, b) + 1", "[average]") {
  constexpr int a = 2;
  constexpr int b = 4;
  constexpr int c = 3;
  static_assert(c == Average(a, b));
  static_assert(c + 1 == Average(a + 1, b + 1));
  static_assert(-c == Average(-a, -b));
  static_assert(-c - 1 == Average(-a - 1, -b - 1));
}

TEST_CASE("Average(a, -b) = -Average(-a, b)", "[average]") {
  static_assert(1 == Average(4, -1));
  static_assert(-1 == Average(-4, 1));
  static_assert(0 == Average(1, -2));
  static_assert(0 == Average(-1, 2));
  static_assert(1 == Average(0, 3));
  static_assert(-1 == Average(0, -3));
}

TEST_CASE("Average doesn't overflow for int", "[average]") {
  SECTION("...if a + b overflows") {
    constexpr int a = 0x5000'0000;
    constexpr int b = 0x5000'0002;
    constexpr int c = 0x5000'0001;
    static_assert(c == Average(a, b));
  }

  SECTION("...if a + b underflows") {
    constexpr int a = -2'000'000'002;
    constexpr int b = -2'000'000'000;
    constexpr int c = -2'000'000'001;
    static_assert(c == Average(a, b));
  }

  SECTION("...if a - b overflows") {
    constexpr int a = 2'000'000'000;
    constexpr int b = -2'000'000'002;
    constexpr int c = -1;
    static_assert(c == Average(a, b));
  }

  SECTION("...if b - a overflows") {
    constexpr int a = -2'000'000'000;
    constexpr int b = 2'000'000'002;
    constexpr int c = 1;
    static_assert(c == Average(a, b));
  }
}

TEST_CASE("Average works with unsigned int", "[average]") {
  // ...if a + b overflows.
  constexpr unsigned int a = 0x9000'0000;
  constexpr unsigned int b = 0x9000'0002;
  constexpr unsigned int c = 0x9000'0001;
  static_assert(c == Average(a, b));

  // ...if a > b.
  static_assert(c == Average(b, a));
}

}  // namespace
}  // namespace mays
