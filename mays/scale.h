// (C) Copyright 2014 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#ifndef MAYS_SCALE_H
#define MAYS_SCALE_H

#include <limits>
#include <optional>
#include <type_traits>

#include "add.h"
#include "divide.h"
#include "internal/check.h"
#include "multiply.h"
#include "nabs.h"
#include "round_policy.h"

namespace mays {

// Construct an object that multiplies a integer of type |In| against a ratio of |numerator| over
// |denominator| while maintaining precision and avoiding unnecessary overflow. Results that are not
// integers will be rounded per |round_policy|.
//
// Note that the final result may still overflow its domain (if the numerator/denominator ratio is
// over 1) and it's difficult to determine when this will happen because scaling integers is usually
// a precision-losing process.
//
// This object may be constructed as constexpr or constinit to get compile-type checking. The Reduce
// function or std::ratio may be helpful in precomputing coprime ratios to reduce Scaler
// intermediate value magnitudes, thereby increasing the breadth of safe inputs.
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
    if constexpr (std::is_signed_v<Intermediate>) {
      // Degenerate ratio that—while representing a real, large scale—can only scale the value 0
      // without overflow in practice anyways. This would be handled by the fast unit rate branch,
      // which would have to deal with this case and input value 1 specially to avoid returning an
      // incorrect result.
      MAYS_CHECK(numerator_ != std::numeric_limits<Intermediate>::min() || denominator_ != -1);
    }
    MAYS_CHECK(is_unit_rate() || can_promote() || can_pre_divide());
  }

  [[nodiscard]] constexpr std::optional<Out> Scale(
      In in,
      RoundPolicy round_policy = RoundPolicy::kRoundTowardZero) const {
    // Optimize out the division if possible.
    if (is_unit_rate()) {
      return MultiplyInto<Out>(in, (Intermediate{numerator_} * denominator_));
    }

    // For types smaller than int, let promotion do the work.
    if constexpr (can_promote()) {
      if (const std::optional result = Divide<Intermediate, Intermediate>(
              round_policy, Intermediate{in} * numerator_, denominator_);
          result.has_value()) {
        // |Intermediate| and |Out| have the same signedness so a roundtrip conversion is sufficient
        // to determine if |result| is in range of |Out|.
        if (result.value() == static_cast<Out>(result.value())) {
          return result.value();
        }
      }
      return std::nullopt;
    } else {
      MAYS_CHECK(can_pre_divide());
      const Intermediate quotient = in / denominator_;
      const Intermediate remainder = in % denominator_;
      if (const std::optional scaled_remainder =
              Divide(round_policy, remainder * numerator_, denominator_);
          scaled_remainder.has_value()) {
        if (const std::optional scaled_quotient = Multiply(quotient, numerator_);
            scaled_quotient.has_value()) {
          return AddInto<Out>(scaled_quotient.value(), scaled_remainder.value());
        }
      }
      return std::nullopt;
    }
  }

 private:
  // Note that this may be |int| even if all the types are unsigned, if each of the types can be
  // converted to |int| without narrowing.
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
    if (numerator_ == 0) {
      return true;
    }
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
    if constexpr (std::is_signed_v<In>) {
      // NOLINTBEGIN(clang-analyzer-core.DivideZero)
      return Nabs(numerator_) >=
             std::numeric_limits<Intermediate>::max() / (Nabs(denominator_) + 1);
      // NOLINTEND(clang-analyzer-core.DivideZero)
    }
    return numerator_ <= std::numeric_limits<Intermediate>::max() / (denominator_ - 1);
  }

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const Numerator numerator_;
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const Denominator denominator_;
};

// Create a Scaler whose |Numerator| and |Denominator| types are deduced from the arguments passed
// to it.
template <typename T, typename N, typename D>
[[nodiscard]] constexpr Scaler<T, N, D> MakeScaler(N numerator, D denominator) {
  return {numerator, denominator};
}

// Create a Scaler whose |Numerator| and |Denominator| types are deduced from the two-member
// decomposable type (e.g. std::tuple, std::pair, struct { int num; int den; }, etc) passed to it.
template <typename T, typename Tuple>
[[nodiscard]] constexpr auto MakeScaler(const Tuple& ratio) {
  const auto [numerator, denominator] = ratio;
  return MakeScaler<T>(numerator, denominator);
}

// Multiplies a value |x| against a ratio of |numerator| over |denominator| while maintaining
// precision and avoiding unnecessary overflow. Results that are not integers will be rounded
// per |round_policy|.
//
// Note that the final result may still overflow its domain and it's difficult to determine when
// |denominator| while maintaining precision and avoiding unnecessary overflow. Results that are not
// integers will be rounded towards zero.
//
// Example:
//   int scaled = Scale(30'000'000, 1000, 1001);  // |scaled| is 29'970'029
template <typename T, typename N, typename D>
[[nodiscard]] constexpr std::optional<typename Scaler<T, N, D>::Out>
Scale(T x, N numerator, D denominator, RoundPolicy round_policy = RoundPolicy::kRoundTowardZero) {
  const auto scaler = MakeScaler<T>(numerator, denominator);
  return scaler.Scale(x, round_policy);
}

}  // namespace mays

#endif  // MAYS_SCALE_H
