// (C) Copyright 2014 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#ifndef MAYS_AVERAGE_H
#define MAYS_AVERAGE_H

#include <type_traits>

#include "internal/polyfill.h"
#include "internal/type_traits.h"

namespace mays {

// Averages two signed integers without overflowing. The naive average |(a + b) / 2| is erroneous
// when the sum of the inputs overflows integer limits. This function works by summing the halves of
// the input values and then rounding the sum towards zero.
template <typename T>
constexpr T Average(T a, T b) {
  static_assert(std::is_integral_v<T>, "Valid only for integers");
  if constexpr (std::is_signed_v<T>) {
    static_assert(::mays::internal::has_arithmetic_shift_v<T>,
                  "Arithmetic right shift not available");
    // Shifts divide by two, rounded towards negative infinity.
    const T sum_halves = (a >> 1) + (b >> 1);
    // This has error of magnitude one if both are odd.
    const T both_odd = (a & b) & 1;
    // Round toward zero; add one if one input is odd and sum is negative.
    const T round_to_zero = (sum_halves < 0) & (a ^ b);
    // Result is sum of halves corrected for rounding.
    return sum_halves + both_odd + round_to_zero;
  }

  // Use a more compiler-friendly form for unsigned integers, where the absolute
  // difference of a and b will always be in range.
  if (a > b) {
    ::mays::internal::swap(a, b);
  }
  return a + (b - a) / 2;
}

}  // namespace mays

#endif  // MAYS_AVERAGE_H
