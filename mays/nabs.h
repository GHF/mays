// (C) Copyright 2014 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#ifndef MAYS_NABS_H
#define MAYS_NABS_H

#include <type_traits>

#include "internal/concepts.h"

namespace mays {

// Takes the negation of the absolute value of an integer. Unlike abs, this is defined for all input
// values, including INT_MIN. See ISO/IEC 14882:2017 § 29.9.2 [c.math.abs] ¶ 2 which defers to ISO C
// § 7.20.6.1 ¶ 2 and footnote for the description of abs/labs/llabs behavior with INT_MIN in two's
// complement.
template <typename T>
[[nodiscard]] constexpr T Nabs(T i) {
  static_assert(std::is_integral_v<T> && std::is_signed_v<T>, "Valid only for signed integers");
  // Check if signed right shift sign extends (i.e. arithmetic right shift).
  if constexpr (::mays::internal::has_arithmetic_shift<T>) {
    constexpr int num_bits = sizeof(T) * 8;
    // Splat sign bit into all bit positions. |sign| takes a value of either 0 or -1.
    const T sign = i >> (num_bits - 1);
    // If |i| is positive (|sign| = 0), |i| is unchanged by xor and subtracted from 0.
    // Otherwise if |i| is negative (|sign| = -1), this is equivalent to |-(1 + ~i)| or just |i|.
    return sign - (i ^ sign);
  } else {
    return i <= 0 ? i : -i;
  }
}

}  // namespace mays

#endif  // MAYS_NABS_H
