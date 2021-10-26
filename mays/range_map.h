// (C) Copyright 2014 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#ifndef MAYS_RANGE_MAP_H
#define MAYS_RANGE_MAP_H

#include <algorithm>
#include <numeric>
#include <tuple>
#include <type_traits>

#include "average.h"
#include "clamp.h"
#include "internal/check.h"
#include "internal/type_traits.h"
#include "nabs.h"
#include "negate_if.h"
#include "scale.h"

namespace mays {

// Maps one range of integer values linearly to another, with deadband. Care should be taken to
// choose ranges and integer types that scale without overflow.
//
// Example:
//   constexpr RangeMap map(/*in_range=*/{int8_t{-127}, int8_t{127}},
//                          /*out_ends*/{1000, 2000},
//                          /*deadband=*/int8_t{10});
//   const int kMapped = map.Map(11);  // kMapped = 1505
template <typename In, typename Out>
class RangeMap final {
 public:
  // Construct a mapping for an input range spanning from |in_range| into an output range spanning
  // from |out_a| to |out_b| (both ranges are inclusive).
  //
  // The input range must span strictly more than twice |deadband| and the limits must be in sorted
  // order. |deadband| must be non-negative. The output range limits may be decreasing in order.
  // Regardless, the low end of the input range will map to |out_a| and the high end will map to
  // |out_b|.
  //
  // Input range is a tuple in order to differentiate the input range from deadband and output
  // range when reading an instantiation.
  // Example:
  //   RangeMap map({-100, 100}, {-333, 333}, 5);
  // Compared to:
  //   RangeMap map(-100, 100, -333, 333, 5);  // Less readable
  constexpr RangeMap(std::tuple<In, In> in_range, std::tuple<Out, Out> out_ends, In deadband = 0)
      : in_lo_(std::get<0>(in_range)),
        in_hi_(std::get<1>(in_range)),
        deadband_(deadband),
        in_midpoint_(Average(in_lo_, in_hi_)),
        out_range_(std::minmax(std::get<0>(out_ends), std::get<1>(out_ends))),
        out_midpoint_(Average(std::get<0>(out_ends), std::get<1>(out_ends))),
        in_to_out_scaler_(ComputeScaler(std::get<0>(out_ends) > std::get<1>(out_ends))),
        requires_out_clamp_(requires_out_clamp()) {}

  // Linearly map |value| from the input range to output range. Values that are within |deadband|
  // distance to the midpoint of the input range will be mapped as if they are midpoint. If |value|
  // is outside of the input range, it will be clamped to the nearest input range limit.
  [[nodiscard]] constexpr Out Map(In value) const {
    // Center the input range on zero.
    In centered_input = Clamp(value, in_lo_, in_hi_) - in_midpoint_;

    // Cut away the deadband from the centered input.
    if (Nabs(centered_input) > -deadband_) {
      centered_input = 0;
    } else {
      centered_input -= SignOf(centered_input) * deadband_;
    }

    // Scale by output to input ratio. Round away from 0 so that values immediately outside of the
    // deadband map to 0.
    const auto centered_output =
        in_to_out_scaler_.Scale(centered_input, RoundPolicy::kRoundAwayFromZero);
    MAYS_CHECK(centered_output.has_value());  // NOLINT(bugprone-assert-side-effect)

    // Shift range from zero into range.
    const auto out_value = static_cast<Out>(centered_output.value() + out_midpoint_);

    // Clamp output within range.
    if (requires_out_clamp_) {
      return Clamp(out_value, std::get<0>(out_range_), std::get<1>(out_range_));
    }
    return out_value;
  }

 private:
  // Type used for computation (deduced from promotion).
  using Intermediate = decltype(std::declval<In>() + std::declval<Out>());
  static_assert(std::is_integral_v<Intermediate> && std::is_signed_v<Intermediate>,
                "Only valid for signed integers.");

  constexpr Intermediate in_width() const {
    MAYS_CHECK(in_lo_ < in_hi_);
    MAYS_CHECK(deadband_ >= 0);
    // Input width might overflow Intermediate, so conservatively limit their magnitudes.
    // NOLINTNEXTLINE(bugprone-assert-side-effect)
    MAYS_CHECK(Nabs(in_hi_) > std::numeric_limits<Intermediate>::min() / 2);
    // NOLINTNEXTLINE(bugprone-assert-side-effect)
    MAYS_CHECK(Nabs(in_lo_) > std::numeric_limits<Intermediate>::min() / 2);
    MAYS_CHECK(Intermediate{in_hi_} - in_lo_ > 2 * deadband_);
    return Intermediate{in_hi_} - in_lo_ - 2 * deadband_;
  }

  constexpr Intermediate out_width() const {
    // NOLINTNEXTLINE(bugprone-assert-side-effect)
    MAYS_CHECK(Nabs(std::get<1>(out_range_)) > std::numeric_limits<Intermediate>::min() / 2);
    // NOLINTNEXTLINE(bugprone-assert-side-effect)
    MAYS_CHECK(Nabs(std::get<0>(out_range_)) > std::numeric_limits<Intermediate>::min() / 2);
    return Intermediate{std::get<1>(out_range_)} - std::get<0>(out_range_);
  }

  [[nodiscard]] constexpr bool requires_out_clamp() const {
    return (in_width() % 2) || (out_width() % 2);
  }

  constexpr Scaler<In, Intermediate, Intermediate> ComputeScaler(bool invert) {
    const auto in_width = this->in_width();
    const auto out_width = NegateIf(this->out_width(), invert);
    const auto gcd = std::gcd(in_width, out_width);
    return {out_width / gcd, in_width / gcd};
  }

  const In in_lo_;
  const In in_hi_;
  const In deadband_;

  const In in_midpoint_;
  const std::tuple<Out, Out> out_range_;  // Sorted output limits.
  const Out out_midpoint_;
  const Scaler<In, Intermediate, Intermediate> in_to_out_scaler_;
  const bool requires_out_clamp_;
};

// Template deduction guides
template <typename In, typename Out>
RangeMap(std::initializer_list<In>, std::initializer_list<Out>) -> RangeMap<In, Out>;

template <typename In, typename Out>
RangeMap(std::initializer_list<In>, std::initializer_list<Out>, In) -> RangeMap<In, Out>;

}  // namespace mays

#endif  // MAYS_RANGE_MAP_H
