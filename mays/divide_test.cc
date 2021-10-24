// (C) Copyright 2021 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#include <catch2/catch.hpp>

#include "divide.h"

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

}  // namespace
}  // namespace mays
