// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#include "scale.h"

#include <catch2/catch.hpp>

namespace mays {
namespace {

TEST_CASE("Scale signed value by a ratio", "[scale]") {
  const int a = 3;
  const int b = 1000;
  const auto [result, x, numerator, denominator] = GENERATE_COPY(table<int, int, int, int>({
      // Scale the value 0
      {0, 0, a, b},
      {0, 0, b, a},
      {0, 0, a, a},
      {0, 0, 0, b},

      // Scale the value 1
      {0, 1, a, b},
      {333, 1, b, a},  // rounded towards zero
      {1, 1, a, a},
      {0, 1, 0, b},

      // Scale the value 2
      {0, 2, a, b},
      {666, 2, b, a},  // rounded towards zero
      {2, 2, a, a},
      {0, 2, 0, b},

      // Scale the value -1
      {0, -1, a, b},
      {-333, -1, b, a},  // rounded towards zero
      {-1, -1, a, a},
      {0, -1, 0, b},

      // Scale the value -2
      {0, -2, a, b},
      {-666, -2, b, a},  // rounded towards zero
      {-2, -2, a, a},
      {0, -2, 0, b},
  }));
  const auto scaler = MakeScaler<decltype(x)>(numerator, denominator);
  CHECK(result == scaler.Scale(x));
  CHECK(-result == scaler.Scale(-x));
}

TEST_CASE("Scale unsigned value by a ratio", "[scale]") {
  const unsigned a = 3;
  const unsigned b = 1000;
  const auto [result, x, numerator, denominator] =
      GENERATE_COPY(table<unsigned, unsigned, unsigned, unsigned>({
          // Scale the value 0
          {0, 0, a, b},
          {0, 0, b, a},
          {0, 0, a, a},
          {0, 0, 0, b},

          // Scale the value 1
          {0, 1, a, b},
          {333, 1, b, a},  // rounded towards zero
          {1, 1, a, a},
          {0, 1, 0, b},

          // Scale the value 2
          {0, 2, a, b},
          {666, 2, b, a},  // rounded towards zero
          {2, 2, a, a},
          {0, 2, 0, b},
      }));
  CHECK(result == Scale(x, numerator, denominator));
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("Scale value by a ratio with a RoundPolicy",
                   "[scale]",
                   int,
                   unsigned,
                   int8_t,
                   uint16_t) {
  const auto [round_policy, numerator, denominator, result] =
      GENERATE_COPY(table<RoundPolicy, TestType, TestType, TestType>({
          // One and a half
          {RoundPolicy::kRoundTowardZero, 3, 2, 1},
          {RoundPolicy::kRoundToNearest, 3, 2, 2},
          {RoundPolicy::kRoundAwayFromZero, 3, 2, 2},

          // 0 < |remainder| < .5
          {RoundPolicy::kRoundTowardZero, 4, 3, 1},
          {RoundPolicy::kRoundToNearest, 4, 3, 1},
          {RoundPolicy::kRoundAwayFromZero, 4, 3, 2},

          // .5 < |remainder| < 1
          {RoundPolicy::kRoundTowardZero, 5, 3, 1},
          {RoundPolicy::kRoundToNearest, 5, 3, 2},
          {RoundPolicy::kRoundAwayFromZero, 5, 3, 2},
      }));
  CAPTURE(round_policy, numerator, denominator);
  CHECK(result == Scale(TestType{1}, numerator, denominator, round_policy));
}

TEST_CASE("Scale value by an integer unit rate", "[scale]") {
  const int scale = GENERATE(2, 1, -1, -2);
  const int a = 10 * scale;
  const int b = scale;
  const auto [result, x] = GENERATE(table<int, int>({{0, 0}, {10, 1}, {20, 2}}));
  CHECK(result == Scale(x, a, b));
  constexpr Scaler<int16_t, int, int> scaler(1'000, 1'001);
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  static_assert(29970 == scaler.Scale(30'000));  // |scaled| is 29'970
}

TEST_CASE("Scale doesn't overflow int8_t naively", "[scale]") {
  // 107 * 11 would overflow past std::numeric_limits<int8_t>::max()
  const auto [result, x] = GENERATE(table<int, int8_t>({{118, 109}, {127, 117}}));
  const int8_t numerator = 12;
  const int8_t denominator = 11;
  constexpr auto scaler = MakeScaler<decltype(x)>(numerator, denominator);
  CHECK(result == scaler.Scale(x));
  CHECK(-result == scaler.Scale(-x));

  // This should fail because the result would overflow the output type.
  // static_assert(128 == Scale(int8_t{118}, numerator, denominator));
}

TEST_CASE("Scale doesn't overflow uint8_t naively", "[scale]") {
  // 107 * 11 would overflow past std::numeric_limits<int8_t>::max()
  const auto [result, x] = GENERATE(table<unsigned int, uint8_t>({{249, 229}, {255, 234}}));
  const uint8_t numerator = 12;
  const uint8_t denominator = 11;
  CHECK(result == Scale(x, numerator, denominator));
}

TEST_CASE("Scale has no restrictions on types smaller than int", "[scale]") {
  // This ratio generates remainders that would overflow computations done with only int8_t.
  const int8_t numerator = 11;
  const int8_t denominator = 100;
  const auto [result, x] = GENERATE(table<int, int8_t>({{13, 127}, {1, 10}}));
  constexpr auto scaler = MakeScaler<decltype(x)>(numerator, denominator);
  CHECK(result == scaler.Scale(x));
  CHECK(-result == scaler.Scale(-x));
}

}  // namespace
}  // namespace mays
