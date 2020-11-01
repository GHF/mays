// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#ifndef MAYS_DIVIDE_ROUND_UP_H
#define MAYS_DIVIDE_ROUND_UP_H

#include <type_traits>

#include "internal/check.h"
#include "sign_of.h"

namespace mays {

// Divides |dividend| by |divisor|, returning a quotient that is rounded away from zero. This is the
// opposite rounding behavior to normal division. Often used in capacity computations.
//
// Example:
//   size_t num_chunks_needed = DivideRoundUp(bytes_needed, kChunkSize);
template <typename N, typename D>
[[nodiscard]] constexpr auto DivideRoundUp(N dividend, D divisor) -> decltype(dividend / divisor) {
  static_assert(std::is_integral_v<N> && std::is_integral_v<D>,
                "Function is valid only for integers");
  static_assert(std::is_signed_v<N> == std::is_signed_v<D>,
                "dividend and divisor signedness don't match");
  MAYS_CHECK(divisor != 0);

  // Avoid underflowing |dividend|
  if (dividend == 0) {
    return 0;
  }

  if constexpr (std::is_signed_v<N>) {
    // ALTERNATE ALGORITHM: signed version of sliding range algorithm
    if (false) {  // NOLINT(readability-simplify-boolean-expr)
      const bool quotient_positive = !((dividend > 0) ^ (divisor > 0));
      return (dividend - SignOf(dividend)) / divisor + (quotient_positive ? 1 : -1);
    }

    // For signed numbers, quotient is negative iff only one of operands is negative, but remainder
    // takes sign of dividend. Ensure that divisor is always non-negative to make sure division and
    // modulo don't yield opposing signs.
    if (divisor < 0) {
      dividend = -dividend;
      divisor = -divisor;
    }

    // This consumes the remainder of the division, which might be slower or bloatier than the
    // sliding range algorithm used for the unsigned version
    return dividend / divisor + SignOf(dividend % divisor);
  }

  // By diminishing |dividend|, exact quotients are decreased by one and non-exact quotients remain
  // the same. But all quotients are then increased by one, so the overall effect is to round up.
  return (dividend - N{1}) / divisor + 1;
}

}  // namespace mays

#endif  // MAYS_DIVIDE_ROUND_UP_H
