// (C) Copyright 2021 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#include "divide.h"

#include <cstdint>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

#include "round_policy.h"

namespace mays {
namespace {

TEST_CASE("Divide computes quotient based on RoundPolicy", "[divide]") {
  const auto [round_policy, a, b, c] = GENERATE(table<RoundPolicy, int, int, int>({
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

  CAPTURE(round_policy, a, b, c);
  CHECK(c == Divide(round_policy, a, b));
}

TEST_CASE("Divide returns nullopt for divide-by-zero and overflow", "[divide]") {
  const RoundPolicy round_policy = GENERATE(
      RoundPolicy::kRoundTowardZero, RoundPolicy::kRoundToNearest, RoundPolicy::kRoundAwayFromZero);
  CAPTURE(round_policy);
  CHECK(!Divide(round_policy, std::numeric_limits<int>::min(), -1).has_value());
  CHECK(!Divide(round_policy, 1, 0).has_value());

  // Check for overflow against a specific type
  CHECK(128 == DivideInto<int>(round_policy, int8_t{-128}, int8_t{-1}));
  CHECK(std::numeric_limits<int>::max() + int64_t{1} ==
        DivideInto<int64_t>(round_policy, std::numeric_limits<int>::min(), -1).value_or(0));
}

}  // namespace
}  // namespace mays
