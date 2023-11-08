// (C) Copyright 2019 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#include "nabs.h"

#include <limits>

#include <catch2/catch.hpp>

namespace mays {
namespace {

TEST_CASE("Take negative absolute value", "[nabs]") {
  static_assert(0 == Nabs(0));
  static_assert(-1 == Nabs(1));
  static_assert(-2 == Nabs(2));
  static_assert(-3 == Nabs(3));
  static_assert(-1 == Nabs(-1));
  static_assert(-2 == Nabs(-2));
  static_assert(-3 == Nabs(-3));
  static_assert(std::numeric_limits<int>::min() == Nabs(std::numeric_limits<int>::min()));
  static_assert(std::numeric_limits<int>::min() + 1 == Nabs(std::numeric_limits<int>::max()));
}

}  // namespace
}  // namespace mays
