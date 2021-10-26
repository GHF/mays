// (C) Copyright 2019 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#include "range_map.h"

#include <catch2/catch.hpp>

namespace mays {
namespace {

constexpr int16_t kServoMin = 1000;
constexpr int16_t kServoCenter = 1500;
constexpr int16_t kServoMax = 2000;

TEST_CASE("Map value in a range to another range", "[range_map]") {
  constexpr RangeMap map({-50, 100}, {kServoMin, kServoMax});

  CHECK(kServoMin == map.Map(-50));
  CHECK(1500 == map.Map(25));
  CHECK(kServoMax == map.Map(100));

  // Test that the input is clamped against the range limits.
  CHECK(kServoMin == map.Map(-1000000));
  CHECK(kServoMax == map.Map(1000000));

  CHECK(1333 == map.Map(0));
  CHECK(1667 == map.Map(50));
}

TEST_CASE("Inputs deadband maps to output midpoint", "[range_map]") {
  constexpr int kDeadband = 10;
  constexpr RangeMap map({-100, 100}, {kServoMin, kServoMax}, kDeadband);

  // Test center
  CHECK(kServoCenter == map.Map(0));

  // Test end stops
  CHECK(kServoMin == map.Map(-100));
  CHECK(kServoMax == map.Map(100));

  SECTION("Deadband inputs") {
    const int i = GENERATE(range(-kDeadband, kDeadband + 1));
    CAPTURE(i);
    CHECK(kServoCenter == map.Map(i));
  }

  // Value just outside of deadband
  CAPTURE(map.Map(11));

  // Leaving the deadband should move the output value.
  CHECK(kServoCenter < map.Map(11));

  // But the output shouldn't jump to what it would without deadband.
  constexpr int kNoDeadbandOut = 1555;  // kServoCenter + 11% of output range
  CHECK(kNoDeadbandOut > map.Map(11));
}

TEST_CASE("Map to an inverted range", "[range_map]") {
  // The output range goes high to low.
  constexpr RangeMap map({-100, 100}, {kServoMax, kServoMin});

  CHECK(kServoMax == map.Map(-100));
  CHECK(kServoCenter == map.Map(0));
  CHECK(kServoMin == map.Map(100));

  CHECK(1250 == map.Map(50));
  CHECK(1750 == map.Map(-50));
}

TEST_CASE("Map between ranges of various width parities", "[range_map]") {
  const auto in_lo = GENERATE(as<int8_t>(), -100, -99);
  const auto in_hi = GENERATE(as<int8_t>(), 99, 100);
  const auto out_a = GENERATE(0, 1);
  const auto out_b = GENERATE(9, 10);
  const auto deadband = GENERATE(as<int8_t>(), 0, 1);
  const RangeMap map({in_lo, in_hi}, {out_a, out_b}, deadband);

  CAPTURE(in_lo, in_hi, out_a, out_b, deadband);
  CHECK(out_a == map.Map(in_lo));
  CHECK(out_b == map.Map(in_hi));
  CHECK(Average(out_a, out_b) == map.Map(Average(in_lo, in_hi)));
}

}  // namespace
}  // namespace mays
