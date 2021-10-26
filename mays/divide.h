// (C) Copyright 2021 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#ifndef MAYS_DIVIDE_H
#define MAYS_DIVIDE_H

#include <limits>
#include <optional>
#include <type_traits>
#include <utility>

#include "divide_round_nearest.h"
#include "divide_round_up.h"
#include "round_policy.h"

namespace mays {

// Computes the quotient of a pair of number using the RoundPolicy specified.
// Returns std::nullopt in case of divide-by-zero or signed overflow.
template <typename N,
          typename D,
          typename Quotient = decltype(std::declval<N>() / std::declval<D>())>
[[nodiscard]] constexpr std::optional<Quotient> Divide(RoundPolicy round_policy,
                                                       N dividend,
                                                       D divisor) {
  if (round_policy == RoundPolicy::kRoundTowardZero) {
    if (divisor == 0) {
      return std::nullopt;
    }
    if constexpr (std::is_signed_v<N> && std::is_signed_v<D>) {
      if (dividend == std::numeric_limits<Quotient>::min() && divisor == D{-1}) {
        return std::nullopt;
      }
    }
    return dividend / divisor;
  }
  if (round_policy == RoundPolicy::kRoundToNearest) {
    return DivideRoundNearest(dividend, divisor);
  }
  return DivideRoundUp(dividend, divisor);
}

}  // namespace mays

#endif  // MAYS_DIVIDE_H
