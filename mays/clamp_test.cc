// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#include "clamp.h"

#include <catch2/catch.hpp>

namespace mays {
namespace {

TEST_CASE("Clamp integer value to range", "[clamp]") {
  constexpr auto kLower = 1;
  constexpr auto kUpper = 10;
  constexpr auto kInRange = 5;
  static_assert(kInRange == Clamp(kInRange, kLower, kUpper));
  static_assert(kLower == Clamp(kLower, kLower, kUpper));
  static_assert(kLower == Clamp(kLower - 1, kLower, kUpper));
  static_assert(kUpper == Clamp(kUpper, kLower, kUpper));
  static_assert(kUpper == Clamp(kUpper + 1, kLower, kUpper));

  // |int| bounds for a |uint8_t| value
  static_assert(4 == Clamp(uint8_t{4}, kLower, kUpper));
  static_assert(4 == Clamp(uint8_t{4}, -kLower, kUpper));
}

TEST_CASE("Clamp floating point value to range", "[clamp]") {
  constexpr auto kLower = 1.;
  constexpr auto kUpper = 10.;
  constexpr auto kInRange = 5.;
  static_assert(kInRange == Clamp(kInRange, kLower, kUpper));
  static_assert(kLower == Clamp(kLower, kLower, kUpper));
  static_assert(kLower == Clamp(kLower - 1, kLower, kUpper));
  static_assert(kUpper == Clamp(kUpper, kLower, kUpper));
  static_assert(kUpper == Clamp(kUpper + 1, kLower, kUpper));

  // Sign of zero is preserved from input
  static_assert(+0. == Clamp(+0., -0., +0.));
  static_assert(-0. == Clamp(-0., -0., +0.));

  // +0 as a lower bound still passes through -0 and vice versa
  static_assert(-0. == Clamp(-0., +0., +1.));
  static_assert(+0. == Clamp(+0., -1., -0.));

  // |float| bounds for a |double| value
  static_assert(1. == Clamp(100., -1.f, 1.f));

  // Clamp infinity and clamp to infinities
  if constexpr (std::numeric_limits<float>::has_infinity) {
    constexpr auto kNegativeInf = -std::numeric_limits<float>::infinity();
    constexpr auto kPositiveInf = std::numeric_limits<float>::infinity();
    static_assert(kLower == Clamp(kNegativeInf, kLower, kUpper));
    static_assert(kUpper == Clamp(kPositiveInf, kLower, kUpper));
    static_assert(0.f == Clamp(0.f, kNegativeInf, kPositiveInf));
    static_assert(kNegativeInf == Clamp(kNegativeInf, kNegativeInf, kPositiveInf));
    static_assert(kPositiveInf == Clamp(kPositiveInf, kNegativeInf, kPositiveInf));
  }

  // Clamping NaN returns the NaN
  if constexpr (std::numeric_limits<float>::has_quiet_NaN) {
    auto clamped_nan = Clamp(std::numeric_limits<float>::quiet_NaN(), kLower, kUpper);
    CAPTURE(clamped_nan);
    CHECK(FP_NAN == std::fpclassify(clamped_nan));
  }
}

}  // namespace
}  // namespace mays
