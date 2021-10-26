// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#ifndef MAYS_DIVIDE_ROUND_NEAREST_H
#define MAYS_DIVIDE_ROUND_NEAREST_H

#include <limits>
#include <optional>
#include <type_traits>
#include <utility>

#include "internal/check.h"
#include "nabs.h"
#include "negate_if.h"

namespace mays {

// Divides |dividend| by |divisor|, returning a quotient that is rounded to the nearest integer.
// Halves are rounded away from zero. Returns std::nullopt in case of divide-by-zero or signed
// overflow.
//
// Example:
//   int quotient = DivideRoundNearest(3, 4);  // |quotient| is 1
//   int quotient = DivideRoundNearest(-5, 2);  // |quotient| is -3
//
// Note that the return type follows the same integer promotion rules as normal division:
//   auto quotient = DivideRoundNearest(int8_t{1}, int{1});  // quotient has type int
//   static_assert(std::is_same_v<decltype(quotient), int>);
//
// Also note that this does not allow you to divide between numbers of different signedness and
// integer literals are typed, because signed numbers would be converted to unsigned in a lossy way:
//   DivideRoundNearest(int8_t{1}, 1);    // OK, |1| has type int
//   DivideRoundNearest(uint8_t{1}, 1U);  // OK, |1U| has type unsigned int
//   DivideRoundNearest(uint8_t{1}, 1);   // Won't compile
template <typename N,
          typename D,
          typename Quotient = decltype(std::declval<N>() / std::declval<D>())>
[[nodiscard]] constexpr std::optional<Quotient> DivideRoundNearest(N dividend, D divisor) {
  static_assert(std::is_integral_v<N> && std::is_integral_v<D>,
                "Function is valid only for integers");
  static_assert(std::is_signed_v<N> == std::is_signed_v<D>,
                "dividend and divisor signedness don't match");
  if (divisor == 0) {
    return std::nullopt;
  }

  using QuotientT = decltype(dividend / divisor);
  if constexpr (std::is_signed_v<N>) {
    if constexpr (std::is_signed_v<D>) {
      if (dividend == std::numeric_limits<Quotient>::min() && divisor == D{-1}) {
        return std::nullopt;
      }
    }

    // Same algorithm as for unsigned but remainder and half-divisor are mapped to negative values
    // with Nabs to easily compare them
    const bool quotient_positive = ((dividend > 0) == (divisor > 0));
    const QuotientT round_away = Nabs(dividend % divisor) < (Nabs(divisor) + 1) / 2;
    return dividend / divisor - NegateIf(round_away, quotient_positive);
  }

  // Equals one if the remainder is greater than or equal to half of divisor
  const QuotientT round_up = dividend % divisor > (divisor - 1) / 2;
  return dividend / divisor + round_up;
}

}  // namespace mays

#endif  // MAYS_DIVIDE_ROUND_UP_H
