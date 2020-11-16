// (C) Copyright 2014 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#ifndef MAYS_SCALE_H
#define MAYS_SCALE_H

#include <cstdint>
#include <type_traits>

#include "internal/check.h"
#include "nabs.h"

namespace mays {

// Multiplies a value |x| against a ratio of |numerator| over |denominator| while maintaining
// precision and avoiding unnecessary overflow. Note that the final result may still overflow the
// type that it represents if the ratio is over one.
//
// Example:
//   int scaled = Scale(30'000'000, 1000, 1001);  // |scaled| is 29'970'029
template <typename T, typename N, typename D>
[[nodiscard]] constexpr std::common_type_t<T, N, D> Scale(T x, N numerator, D denominator) {
  static_assert(std::is_integral_v<T> && std::is_integral_v<N> && std::is_integral_v<D>,
                "Function is valid only for integers");
  static_assert(
      std::is_signed_v<T> == std::is_signed_v<D> && std::is_signed_v<T> == std::is_signed_v<D>,
      "Arguments' signedness don't match");
  MAYS_CHECK(denominator != 0);

  const auto is_unit_rate = [numerator, denominator] {
    if constexpr (std::is_signed_v<T>) {
      return Nabs(denominator) == -1 && Nabs(numerator) <= Nabs(denominator);
    }
    return denominator == 1 && numerator >= denominator;
  };
  if (is_unit_rate()) {
    return x * (numerator * denominator);
  }

  // For types smaller than int, let promotion do the work.
  using int_t = std::common_type_t<T, N, D>;
  if constexpr (2 * sizeof(int_t) <= sizeof(decltype(x * numerator / denominator))) {
    return static_cast<int_t>(x * numerator / denominator);
  }

  // Magnitude of remainder is in the range [0, |denominator|), so check that the intermediate value
  // remainder * numerator can't overflow. The division is made safe by the unit rate optimization
  // branch above.
  if constexpr (std::is_signed_v<int_t>) {
    // NOLINTNEXTLINE(bugprone-assert-side-effect)
    MAYS_CHECK(Nabs(numerator) >= std::numeric_limits<int_t>::max() / (Nabs(denominator) + 1));
  } else {
    // NOLINTNEXTLINE(bugprone-assert-side-effect)
    MAYS_CHECK(numerator <= std::numeric_limits<int_t>::max() / (denominator - 1));
  }
  const int_t quotient = x / denominator;
  const int_t remainder = x % denominator;
  return quotient * numerator + remainder * numerator / denominator;
}

}  // namespace mays

#endif  // MAYS_SCALE_H
