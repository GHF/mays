// (C) Copyright 2021 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#include "multiply.h"

#include <cstdint>
#include <optional>

#include <catch2/catch_all.hpp>

namespace mays {
namespace {

TEST_CASE("Multiply checks for overflow", "[multiply]") {
  const auto [a, b, c] = GENERATE(table<int8_t, int8_t, std::optional<int8_t>>({
      {8, 16, std::nullopt},
      {-8, -16, std::nullopt},
      {8, -16, -128},
      {2, 3, 6},
  }));

  CAPTURE(a, b, c);
  CHECK(c == Multiply(a, b));
}

TEST_CASE("MultiplyInto checks for overflow against a specific type", "[multiply]") {
  CHECK(128 == MultiplyInto<int>(int8_t{8}, int8_t{16}));

  // Mixed signs work but observes normal C rules for promotion and needs this coercion to produce
  // negative products.
  CHECK(!Multiply(-1, 1U).has_value());
  CHECK(-1 == MultiplyInto<int>(-1, 1U));
}

}  // namespace
}  // namespace mays
