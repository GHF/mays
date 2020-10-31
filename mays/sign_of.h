// (C) Copyright 2014 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#ifndef MAYS_SIGN_OF_H
#define MAYS_SIGN_OF_H

#include <type_traits>

namespace mays {

// Gets the sign of a value as +1, -1, or 0 (if input is zero).
//
// Examples:
//   int sign = SignOf(10);  // |sign| is 1
//   sign = SignOf(-10);  // |sign| is -1
//   sign = SignOf(0);  // |sign| is 0
//
//   // Return type is deduced from the input parameter
//   auto sign = SignOf(int8_t{-10});  // |sign| has type int8_t
template <typename T>
[[nodiscard]] constexpr T SignOf(T i) {
  static_assert(std::is_integral_v<T>, "Function is valid only for integers");
  return (i > 0) - (i < 0);
}

}  // namespace mays

#endif  // MAYS_SIGN_OF_H
