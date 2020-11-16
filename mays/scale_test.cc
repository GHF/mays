// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#include "scale.h"

#include <catch2/catch.hpp>

namespace mays {
namespace {

TEST_CASE("Scale value by a ratio", "[scale]") {
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
      {333, 1, b, a},
      {1, 1, a, a},
      {0, 1, 0, b},

      // Scale the value 2
      {0, 2, a, b},
      {666, 2, b, a},
      {2, 2, a, a},
      {0, 2, 0, b},

      // Scale the value -1
      {0, -1, a, b},
      {-333, -1, b, a},
      {-1, -1, a, a},
      {0, -1, 0, b},

      // Scale the value -2
      {0, -2, a, b},
      {-666, -2, b, a},
      {-2, -2, a, a},
      {0, -2, 0, b},
  }));
  const auto scaler = MakeScaler<decltype(x)>(numerator, denominator);
  CHECK(result == scaler.Scale(x));
  CHECK(-result == scaler.Scale(-x));
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

TEST_CASE("Scale doesn't overflow naively", "[scale]") {
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
