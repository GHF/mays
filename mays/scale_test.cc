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
  CHECK(result == Scale(x, numerator, denominator));
  CHECK(-result == Scale(-x, numerator, denominator));
}

TEST_CASE("Scale value by an integer unit rate", "[scale]") {
  const int scale = GENERATE(2, 1, -1, -2);
  const int a = 10 * scale;
  const int b = scale;
  const auto [result, x] = GENERATE(table<int, int>({{0, 0}, {10, 1}, {20, 2}}));
  CHECK(result == Scale(x, a, b));
}

TEST_CASE("Scale doesn't overflow naively", "[scale]") {
  // 107 * 11 would overflow past std::numeric_limits<int8_t>::max()
  const auto [result, x] = GENERATE(table<int, int8_t>({{118, 109}, {127, 117}}));
  const int8_t numerator = 12;
  const int8_t denominator = 11;
  CHECK(result == Scale(x, numerator, denominator));
  CHECK(-result == Scale(-x, numerator, denominator));

  // This should fail because the result would overflow the output type.
  // static_assert(128 == Scale(int8_t{118}, numerator, denominator));

  // This should fail to compile because the choice of ratio may result in overflow.
  // [[maybe_unused]] constexpr int8_t scaled = Scale<int8_t, int8_t, int8_t>(1, 11, 100);
}

}  // namespace
}  // namespace mays
