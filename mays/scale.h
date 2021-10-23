// (C) Copyright 2014 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#ifndef MAYS_SCALE_H
#define MAYS_SCALE_H

#include <cstdint>
#include <limits>
#include <type_traits>

#include "internal/check.h"
#include "nabs.h"

namespace mays {

// Construct an object that multiplies a integer of type |In| against a ratio of |numerator| over
// |denominator| while maintaining precision and avoiding unnecessary overflow. Note that the final
// result may still overflow its domain.
//
// This object may be constructed as constexpr or constinit to get compile-type checking.
//
// See also MakeScaler, which can deduce the |Numerator| and |Denominator| types from arguments.
//
// Example:
//   constexpr Scaler<int16_t, int, int> scaler(1'000, 1'001);
//   auto scaled = scaler.Scale(30'000);  // |scaled| is 29'970 and has type int
template <typename In, typename Numerator, typename Denominator>
class Scaler final {
 public:
  using Out = std::common_type_t<In, Numerator, Denominator>;

  constexpr Scaler(Numerator numerator, Denominator denominator)
      : numerator_(numerator), denominator_(denominator) {
    MAYS_CHECK(denominator_ != 0);
    // NOLINTNEXTLINE(bugprone-assert-side-effect)
    MAYS_CHECK(is_unit_rate() || can_promote() || can_pre_divide());
  }

  [[nodiscard]] constexpr Out Scale(In in) const {
    // Optimize out the division if possible.
    if (is_unit_rate()) {
      return in * (numerator_ * denominator_);
    }

    // For types smaller than int, let promotion do the work.
    if constexpr (can_promote()) {
      return static_cast<Out>(in * numerator_ / denominator_);
    } else {
      MAYS_CHECK(can_pre_divide());
      const Intermediate quotient = in / denominator_;
      const Intermediate remainder = in % denominator_;
      return quotient * numerator_ + remainder * numerator_ / denominator_;
    }
  }

 private:
  using Intermediate =
      decltype(std::declval<In>() * std::declval<Numerator>() / std::declval<Denominator>());

  static_assert(std::is_integral_v<In> && std::is_integral_v<Out> &&
                    std::is_integral_v<Numerator> && std::is_integral_v<Denominator>,
                "Function is valid only for integers");
  static_assert(std::is_signed_v<In> == std::is_signed_v<Out> &&
                    std::is_signed_v<In> == std::is_signed_v<Numerator> &&
                    std::is_signed_v<In> == std::is_signed_v<Denominator>,
                "Arguments' signedness don't match");

  [[nodiscard]] constexpr bool is_unit_rate() const {
    if constexpr (std::is_signed_v<In>) {
      return Nabs(denominator_) == -1 && Nabs(numerator_) <= Nabs(denominator_);
    }
    return denominator_ == 1 && numerator_ >= denominator_;
  }

  [[nodiscard]] static constexpr bool can_promote() {
    return 2 * sizeof(std::common_type_t<In, Numerator, Denominator>) <= sizeof(Intermediate);
  }

  [[nodiscard]] constexpr bool can_pre_divide() const {
    // Magnitude of remainder is in the range [0, |denominator|), so check that the intermediate
    // value remainder * numerator can't overflow. Avoid this check for this unit rate case
    // because it divides by zero.
    if constexpr (std::is_signed_v<Intermediate>) {
      return Nabs(numerator_) >=
             std::numeric_limits<Intermediate>::max() / (Nabs(denominator_) + 1);
    }
    return numerator_ <= std::numeric_limits<Intermediate>::max() / (denominator_ - 1);
  }

  const Numerator numerator_;
  const Denominator denominator_;
};

// Create a Scaler whose |Numerator| and |Denominator| types are deduced from the arguments passed
// to it.
template <typename T, typename N, typename D>
[[nodiscard]] constexpr Scaler<T, N, D> MakeScaler(N numerator, D denominator) {
  return {numerator, denominator};
}

// Multiplies a value |x| against a ratio of |numerator| over |denominator| while maintaining
// precision and avoiding unnecessary overflow. Note that the final result may still overflow the
// type that it represents if the ratio is over one.
//
// Example:
//   int scaled = Scale(30'000'000, 1000, 1001);  // |scaled| is 29'970'029
template <typename T, typename N, typename D>
[[nodiscard]] constexpr typename Scaler<T, N, D>::Out Scale(T x, N numerator, D denominator) {
  const auto scaler = MakeScaler<T>(numerator, denominator);
  return scaler.Scale(x);
}

}  // namespace mays

#endif  // MAYS_SCALE_H