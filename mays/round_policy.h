// (C) Copyright 2021 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#ifndef MAYS_ROUND_POLICY_H
#define MAYS_ROUND_POLICY_H

namespace mays {

enum class RoundPolicy {
  kRoundTowardZero,
  kRoundToNearest,
  kRoundAwayFromZero,
};

}  // namespace mays

#endif  // MAYS_ROUND_POLICY_H
