// (C) Copyright 2021 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#include "add.h"

#include <cstdint>
#include <optional>

#include <catch2/catch_all.hpp>

namespace mays {
namespace {

TEST_CASE("Add checks for overflow", "[add]") {
  const auto [a, b, c] = GENERATE(table<int8_t, int8_t, std::optional<int8_t>>({
      {64, 64, std::nullopt},
      {-64, -65, std::nullopt},
      {1, 2, 3},
  }));

  CAPTURE(a, b, c);
  CHECK(c == Add(a, b));
}

TEST_CASE("AddInto checks for overflow against a specific type", "[add]") {
  CHECK(180 == AddInto<int>(int8_t{100}, int8_t{80}));

  // Mixed signs work but observes normal C rules for promotion and needs this coercion to produce
  // negative sums.
  CHECK(!Add(-2, 1U).has_value());
  CHECK(-1 == AddInto<int>(-2, 1U));
}

}  // namespace
}  // namespace mays
