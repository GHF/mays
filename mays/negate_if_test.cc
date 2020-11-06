// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#include "negate_if.h"

#include <limits>

#include <catch2/catch.hpp>

namespace mays {
namespace {

TEST_CASE("Conditionally negate integer", "[sign_of]") {
  SECTION("Extreme values") {
    static_assert(0 == NegateIf(0, true));
    static_assert(std::numeric_limits<int>::min() ==
                  NegateIf(std::numeric_limits<int>::min(), false));
  }

  SECTION("Common values") {
    int i = GENERATE(1, 2, 3, 4);
    CHECK(i == NegateIf(i, false));
    CHECK(-i == NegateIf(i, true));
    CHECK(-i == NegateIf(-i, false));
    CHECK(i == NegateIf(-i, true));
  }
}

}  // namespace
}  // namespace mays