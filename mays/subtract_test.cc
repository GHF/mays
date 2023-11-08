// (C) Copyright 2021 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#include "subtract.h"

#include <cstdint>
#include <optional>

#include <catch2/catch.hpp>

namespace mays {
namespace {

TEST_CASE("Subtract checks for overflow", "[subtract]") {
  const auto [a, b, c] = GENERATE(table<int8_t, int8_t, std::optional<int8_t>>({
      {0, -128, std::nullopt},
      {-127, 127, std::nullopt},
      {3, 2, 1},
  }));

  CAPTURE(a, b, c);
  CHECK(c == Subtract(a, b));
}

TEST_CASE("SubtractInto checks for overflow against a specific type", "[subtract]") {
  CHECK(-137 == SubtractInto<int>(int8_t{-10}, int8_t{127}));

  // Mixed signs work but observes normal C rules for promotion and needs this coercion to produce
  // negative differences.
  CHECK(!Subtract(1, 2U).has_value());
  CHECK(-1 == SubtractInto<int>(1, 2U));
}

}  // namespace
}  // namespace mays
