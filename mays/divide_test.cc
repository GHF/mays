// (C) Copyright 2021 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#include "divide.h"

#include <catch2/catch.hpp>

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
  CHECK(!Divide(round_policy, std::numeric_limits<int>::min(), -1).has_value());
  CHECK(!Divide(round_policy, 1, 0).has_value());
}

}  // namespace
}  // namespace mays
