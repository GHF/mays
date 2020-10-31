// (C) Copyright 2019 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#include "sign_of.h"

#include <catch2/catch.hpp>

namespace mays {
namespace {

TEST_CASE("Determine sign of integer", "[sign_of]") {
  static_assert(0 == SignOf(0));
  static_assert(1 == SignOf(1));
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  static_assert(1 == SignOf(10));
  static_assert(1 == SignOf(std::numeric_limits<int>::max()));
  static_assert(-1 == SignOf(-1));
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  static_assert(-1 == SignOf(-10));
  static_assert(-1 == SignOf(std::numeric_limits<int>::min()));
}

TEST_CASE("Determine sign of unsigned", "[sign_of]") {
  static_assert(0 == SignOf(0U));
  static_assert(1 == SignOf(1U));
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  static_assert(1 == SignOf(10U));
  static_assert(1 == SignOf(std::numeric_limits<unsigned>::max()));
}

}  // namespace
}  // namespace mays
