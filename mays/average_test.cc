// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#include "average.h"

#include <catch2/catch.hpp>

namespace mays {
namespace {

TEST_CASE("Average(a, b) = Average(b, a)", "[average]") {
  constexpr int a = 2;
  constexpr int b = 7;
  constexpr int c = 4;
  static_assert(c == Average(a, b));
  static_assert(c == Average(b, a));
  static_assert(-c == Average(-a, -b));
  static_assert(-c == Average(-b, -a));
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
