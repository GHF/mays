// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#include "divide_round_nearest.h"

#include <climits>
#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

namespace mays {
namespace {

TEST_CASE("Compute quotient approximation", "[divide_round_nearest]") {
  SECTION("Zeroes") {
    static_assert(0 == DivideRoundNearest(0, 1));
    static_assert(0 == DivideRoundNearest(0, 2));
    static_assert(0U == DivideRoundNearest(0U, 1U));
    static_assert(0U == DivideRoundNearest(0U, 2U));
  }

  SECTION("Ones") {
    static_assert(1 == DivideRoundNearest(1, 1));
    static_assert(1U == DivideRoundNearest(1U, 1U));
  }

  SECTION("Halves") {
    static_assert(1 == DivideRoundNearest(1, 2));
    static_assert(1U == DivideRoundNearest(1U, 2U));
  }

  SECTION("Thirds") {
    static_assert(0 == DivideRoundNearest(1, 3));
    static_assert(0U == DivideRoundNearest(1U, 3U));
  }

  SECTION("Fourths") {
    static_assert(0 == DivideRoundNearest(1, 4));
    static_assert(0U == DivideRoundNearest(1U, 4U));
  }

  SECTION("Ones and a half") {
    static_assert(2 == DivideRoundNearest(3, 2));
    static_assert(2U == DivideRoundNearest(3U, 2U));
  }

  SECTION("0 < remainder < .5") {
    static_assert(1 == DivideRoundNearest(4, 3));
    static_assert(1U == DivideRoundNearest(4U, 3U));
  }

  SECTION(".5 < remainder < 1") {
    // NOLINTNEXTLINE(readability-magic-numbers)
    static_assert(2 == DivideRoundNearest(5, 3));
    // NOLINTNEXTLINE(readability-magic-numbers)
    static_assert(2U == DivideRoundNearest(5U, 3U));
  }

  SECTION("Exactly doubles") {
    static_assert(2 == DivideRoundNearest(2, 1));
    static_assert(2U == DivideRoundNearest(2U, 1U));
  }
}

TEST_CASE("Round negative quotients to nearest integer", "[divide_round_nearest]") {
  const auto [a, b, c] = GENERATE(table<int, int, int>({// Ones
                                                        {1, 1, 1},

                                                        // Halves
                                                        {1, 2, 1},

                                                        // Ones and a half
                                                        {3, 2, 2},

                                                        // Near zero
                                                        {1, 3, 0},  // odd divisor
                                                        {1, 4, 0},  // even divisor

                                                        // 0 < |remainder| < .5
                                                        {4, 3, 1},  // odd divisor
                                                        {5, 4, 1},  // even divisor

                                                        // .5 < |remainder| < 1
                                                        {5, 3, 2},  // odd divisor
                                                        {7, 4, 2},  // even divisor

                                                        // Exactly doubles
                                                        {2, 1, 2}}));
  CAPTURE(a, b, c);
  CHECK(-c == DivideRoundNearest(-a, b));
  CHECK(-c == DivideRoundNearest(a, -b));
}

TEST_CASE("Compute approximate quotient of large dividends", "[divide_round_nearest]") {
  // Check that the computation of "it takes 32 bytes to hold 255 bits" does not overflow, even when
  // done using unsigned 8-bit arithmetic
  static_assert((std::numeric_limits<uint8_t>::max() + 1U) / CHAR_BIT ==
                DivideRoundNearest(std::numeric_limits<uint8_t>::max(), uint8_t{CHAR_BIT}));

  // For signed positive numbers: 16 == DivideRoundNearest(127 / 8)
  static_assert((std::numeric_limits<int8_t>::max() + 1) / int{CHAR_BIT} ==
                DivideRoundNearest(std::numeric_limits<int8_t>::max(), int8_t{CHAR_BIT}));

  // For signed negative numbers: 16 == DivideRoundNearest(-127 / 8)
  static_assert((std::numeric_limits<int8_t>::min()) / int{CHAR_BIT} ==
                DivideRoundNearest(std::numeric_limits<int8_t>::min() + 1, int8_t{CHAR_BIT}));
}

}  // namespace
}  // namespace mays
