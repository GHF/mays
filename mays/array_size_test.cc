// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#include "array_size.h"

#include <catch2/catch.hpp>

namespace mays {
namespace {

TEST_CASE("Deduce array size", "[array_size]") {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
  int a[2];
  static_assert(2 == ArraySize(a));
  static_assert(2 == ArraySize<0>(a));
  static_assert(sizeof(a) / sizeof(a[0]) == ArraySize(a));
}

TEST_CASE("Deduce multidimensional array sizes", "[array_size]") {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
  int a[3][2][1];
  static_assert(3 == ArraySize(a));
  static_assert(3 == ArraySize<0>(a));
  static_assert(2 == ArraySize<1>(a));
  static_assert(1 == ArraySize<2>(a));
}

}  // namespace
}  // namespace mays
