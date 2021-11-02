// (C) Copyright 2014 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#ifndef MAYS_AVERAGE_H
#define MAYS_AVERAGE_H

#include <type_traits>

#include "internal/polyfill.h"
#include "internal/type_traits.h"

namespace mays {

// Averages two signed integers without overflowing. The naive average |(a + b) / 2| is erroneous
// when the sum of the inputs overflows integer limits. This function works by summing the halves of
// the input values and then rounding the sum towards zero.
//
// Examples:
//   // (100 + 120) would overflow a int8_t, so |(a + b) / 2| results in undefined behavior
//   constexpr int8_t kAlpha = Average(int8_t{100}, int8_t{120});  // kAlpha = 110
//
//   // (120 - -100) would overflow similarly, so |a + (b - a) / 2| is wrong too
//   constexpr int8_t kBeta = Average(int8_t{-100}, int8_t{120});  // kBeta = 10
//
//   // |3 / 2| evaluates to 1 and |5 / 2| to 2, so |a / 2 + b / 2| is wrong too
//   constexpr int kGamma = Average(3, 5);  // kGamma = 4
template <typename T>
[[nodiscard]] constexpr T Average(T a, T b) {
  static_assert(std::is_integral_v<T>, "Valid only for integers");
  if constexpr (std::is_signed_v<T>) {
    static_assert(::mays::internal::has_arithmetic_shift_v<T>,
                  "Arithmetic right shift not available");
    // Shifts divide by two, rounded towards negative infinity.
    const T sum_halves = (a >> 1) + (b >> 1);
    // This has error of magnitude one if both are odd.
    const T both_odd = (a & b) & 1;
    // Round toward zero; add one if one input is odd and sum is negative.
    const T round_to_zero = T{sum_halves < 0} & (a ^ b);
    // Result is sum of halves corrected for rounding.
    return static_cast<T>(sum_halves + both_odd + round_to_zero);
  } else {
    // Use a (maybe) more compiler-friendly form for unsigned integers, where the absolute
    // difference of a and b will always be in range.
    auto avg_of = [](T x, T y) { return x + (y - x) / 2; };
    if (a > b) {
      return avg_of(b, a);
    }
    return avg_of(a, b);
  }
}

}  // namespace mays

#endif  // MAYS_AVERAGE_H
