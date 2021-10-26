// (C) Copyright 2021 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#include "reduce.h"

#include <catch2/catch.hpp>

namespace mays {
namespace {

TEST_CASE("Reduce simplifies numbers by their GCD", "[reduce]") {
  constexpr std::tuple<int8_t, int, int> reduced = Reduce(int8_t{2 * 3 * 3}, 2 * 3 * 5, 3 * 5 * 7);
  const auto [a, b, c] = reduced;
  CHECK(2 * 3 == a);
  CHECK(2 * 5 == b);
  CHECK(5 * 7 == c);
}

}  // namespace
}  // namespace mays
