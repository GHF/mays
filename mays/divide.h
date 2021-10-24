// (C) Copyright 2021 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#ifndef MAYS_DIVIDE_H
#define MAYS_DIVIDE_H

#include "divide_round_nearest.h"
#include "divide_round_up.h"
#include "round_policy.h"

namespace mays {

// Computes the quotient of a pair of number using the RoundPolicy specified.
template <typename N, typename D>
[[nodiscard]] constexpr auto Divide(RoundPolicy round_policy, N dividend, D divisor)
    -> decltype(dividend / divisor) {
  if (round_policy == RoundPolicy::kRoundTowardZero) {
    return dividend / divisor;
  }
  if (round_policy == RoundPolicy::kRoundToNearest) {
    return DivideRoundNearest(dividend, divisor);
  }
  return DivideRoundUp(dividend, divisor);
}

}  // namespace mays

#endif  // MAYS_DIVIDE_H
