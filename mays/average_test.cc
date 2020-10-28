// (C) Copyright 2019 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#include "average.h"

#include <catch2/catch.hpp>

namespace mays {
namespace {

TEST_CASE("Average(a, b) = Average(b, a)", "[average]") {
  static_assert(4 == Average(2, 7));
  static_assert(4 == Average(7, 2));
  static_assert(-4 == Average(-2, -7));
  static_assert(-4 == Average(-7, -2));
}

TEST_CASE("Average(a + 1, b + 1) = Average(a, b) + 1", "[average]") {
  static_assert(3 == Average(2, 4));
  static_assert(4 == Average(3, 5));
  static_assert(-3 == Average(-2, -4));
  static_assert(-4 == Average(-3, -5));
}

TEST_CASE("Average(a, -b) = -Average(-a, b)", "[average]") {
  static_assert(1 == Average(4, -1));
  static_assert(-1 == Average(-4, 1));
  static_assert(0 == Average(1, -2));
  static_assert(0 == Average(-1, 2));
  static_assert(1 == Average(0, 3));
  static_assert(-1 == Average(0, -3));
}

TEST_CASE("Average doesn't overflow for signed int", "[average]") {
  // ...if a + b overflows.
  static_assert(0x5000'0001 == Average(0x5000'0000, 0x5000'0002));
  static_assert(-2'000'000'001 == Average(-2'000'000'000, -2'000'000'002));

  // ...if a - b overflows.
  static_assert(-1 == Average(2'000'000'000, -2'000'000'002));

  // ...if b - a overflows.
  static_assert(1 == Average(-2'000'000'000, 2'000'000'002));
}

TEST_CASE("Average works with unsigned int", "[average]") {
  // ...if a + b overflows.
  static_assert(0x9000'0001U == Average(0x9000'0000U, 0x9000'0002U));

  // ...if a > b.
  static_assert(0x9000'0001U == Average(0x9000'0002U, 0x9000'0000U));
}

}  // namespace
}  // namespace mays
