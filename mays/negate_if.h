// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#ifndef MAYS_NEGATE_IF_H
#define MAYS_NEGATE_IF_H

#include <type_traits>

namespace mays {

// Returns the negation of |i| if |negate| is true, |i| untouched otherwise.
//
// Examples:
//   int value = NegateIf(10, false);  // |value| is 10
//   value = NegateIf(10, true);  // |value| is -10
//
//   // Return type is deduced from the input parameter
//   auto value = NegateIf(int8_t{-4}, true);  // |value| has type int8_t
template <typename T>
[[nodiscard]] constexpr T NegateIf(T i, bool negate) {
  static_assert(std::is_integral_v<T>, "Function is valid only for integers");

  // Branch-free version
  if (false) {
    using TU = std::make_unsigned_t<T>;
    // Splat negate bit into all bit positions. |sign| takes a value of either 0 or -1.
    const auto sign = static_cast<TU>(-T{negate});
    // If |i| is positive (|sign| = 0), |i| is unchanged by xor and subtracts 0.
    // Otherwise if |i| is negative (|sign| = -1), this is equivalent to |~i + 1| or |-i|.
    return static_cast<T>((static_cast<TU>(i) ^ sign) - sign);
  }

  return negate ? -i : i;
}

}  // namespace mays

#endif  // MAYS_NEGATE_IF_H
