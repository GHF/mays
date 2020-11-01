// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#include "divide_round_up.h"

#include <cstddef>
#include <limits>

#include <catch2/catch.hpp>

namespace mays {
namespace {

TEST_CASE("Compute quotient ceiling", "[divide_round_up]") {
  SECTION("Zeroes") {
    static_assert(0 == DivideRoundUp(0, 1));
    static_assert(0 == DivideRoundUp(0, 2));
    static_assert(0U == DivideRoundUp(0U, 1U));
    static_assert(0U == DivideRoundUp(0U, 2U));
  }

  SECTION("Ones") {
    static_assert(1 == DivideRoundUp(1, 1));
    static_assert(1U == DivideRoundUp(1U, 1U));
  }

  SECTION("Halves") {
    static_assert(1 == DivideRoundUp(1, 2));
    static_assert(1U == DivideRoundUp(1U, 2U));
  }

  SECTION("Ones and a half") {
    static_assert(2 == DivideRoundUp(3, 2));
    static_assert(2U == DivideRoundUp(3U, 2U));
  }

  SECTION("Not integral or half") {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    static_assert(2 == DivideRoundUp(5, 3));
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    static_assert(2U == DivideRoundUp(5U, 3U));
  }

  SECTION("Exactly doubles") {
    static_assert(2 == DivideRoundUp(2, 1));
    static_assert(2U == DivideRoundUp(2U, 1U));
  }
}

TEST_CASE("Round negative quotients away from zero", "[divide_round_up]") {
  SECTION("Ones") {
    static_assert(-1 == DivideRoundUp(-1, 1));
    static_assert(-1 == DivideRoundUp(1, -1));
  }

  SECTION("Halves") {
    static_assert(-1 == DivideRoundUp(-1, 2));
    static_assert(-1 == DivideRoundUp(1, -2));
  }

  SECTION("Ones and a half") {
    static_assert(-2 == DivideRoundUp(-3, 2));
    static_assert(-2 == DivideRoundUp(3, -2));
  }

  SECTION("Not integral or half") {
    constexpr auto a = 5;
    constexpr auto b = 3;
    constexpr auto c = 2;
    static_assert(-c == DivideRoundUp(-a, b));
    static_assert(-c == DivideRoundUp(a, -b));
  }

  SECTION("Exactly doubles") {
    static_assert(-2 == DivideRoundUp(-2, 1));
    static_assert(-2 == DivideRoundUp(2, -1));
  }
}

TEST_CASE("Compute quotient ceilings of large dividends", "[divide_round_up]") {
  // Check that the computation of "it takes 32 bytes to hold 255 bits" does not overflow, even when
  // done using unsigned 8-bit arithmetic
  static_assert((std::numeric_limits<uint8_t>::max() + 1U) / CHAR_BIT ==
                DivideRoundUp(std::numeric_limits<uint8_t>::max(), uint8_t{CHAR_BIT}));

  // For signed positive numbers: 16 == DivideRoundUp(127 / 8)
  static_assert((std::numeric_limits<int8_t>::max() + 1) / int{CHAR_BIT} ==
                DivideRoundUp(std::numeric_limits<int8_t>::max(), int8_t{CHAR_BIT}));

  // For signed negative numbers: 16 == DivideRoundUp(-127 / 8)
  static_assert((std::numeric_limits<int8_t>::min()) / int{CHAR_BIT} ==
                DivideRoundUp(std::numeric_limits<int8_t>::min() + 1, int8_t{CHAR_BIT}));
}

}  // namespace
}  // namespace mays
