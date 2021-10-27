// (C) Copyright 2014 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#ifndef MAYS_CLAMP_H
#define MAYS_CLAMP_H

#include <limits>
#include <type_traits>

#include "internal/check.h"
#include "internal/type_traits.h"

namespace mays {

// Clamps a value to a range. The lower bound must be less than or equal to the uppper bound.
//
// Example:
//   auto clamped_value = Clamp(int{value}, 16, 235);  // 16 ≤ |clamped_value| ≤ 235
//
// For values of integer types, the bounds must be either implicitly convertible to the value's type
// without a narrowing conversion (e.g. signed -> unsigned) or otherwise be a non-impossible bound
// (e.g. a lower bound greater than the maximum value representable).
//
// Example:
//   auto clamped_value = Clamp(int8_t{value}, -1024, 1024);  // |clamped_value| = |value|
//
// Negative clamp lower bounds are permitted even for unsigned values to be clamped.
//
// Example:
//   auto clamped_value = Clamp(uint8_t{value}, -1024, 200);  // |clamped_value| ≤ 200
//
// For values of other types, the types of the bounds are deduced from that of the value. This means
// that, for example, |float| values will have |float| bounds. C++ permits implicit conversions from
// integers to |float| and more precise floating-point types to |float|. This behavior is allowed
// here to let |float| bounds parameters accept integer literals and |double| literals.
//
// Example:
//   auto clamped_value = Clamp(float{value}, 0, +.01);  // std requires clamp(value, 0.f, +.01f)
//
// Modern compilers will warn about the above implicit conversions if the literals' values can't be
// exactly represented with |float|.
template <typename T, typename L, typename H, typename = std::enable_if_t<std::is_integral_v<T>>>
[[nodiscard]] constexpr T Clamp(T v, L lo, H hi) {
  if constexpr (!internal::is_convertible_without_narrowing_v<L, T>) {
    static_assert(std::is_integral_v<L>, "Integer Clamp's lower bound must be an integer.");
    MAYS_CHECK(internal::cmp_less_equal(lo, std::numeric_limits<T>::max()));
  }
  if constexpr (!internal::is_convertible_without_narrowing_v<H, T>) {
    static_assert(std::is_integral_v<H>, "Integer Clamp's upper bound must be an integer.");
    MAYS_CHECK(internal::cmp_greater_equal(hi, std::numeric_limits<T>::min()));
  }
  MAYS_CHECK(internal::cmp_less_equal(lo, hi));
  const T lower_bounded = internal::cmp_greater_equal(v, lo) ? v : T(lo);
  const T both_bounded = internal::cmp_less_equal(lower_bounded, hi) ? lower_bounded : T(hi);
  return both_bounded;
}

template <typename T, typename = std::enable_if_t<!std::is_integral_v<T>>>
[[nodiscard]] constexpr T Clamp(T v,
                                internal::type_identity_t<T> lo,
                                internal::type_identity_t<T> hi) {
  if constexpr (std::is_floating_point_v<T>) {
    // Check for NaNs without using |std::isnan|, which is not constexpr
    MAYS_CHECK(lo == lo);
    MAYS_CHECK(hi == hi);
    if (v != v) {  // NOLINT(misc-redundant-expression)
      return v;
    }
  }

  MAYS_CHECK(lo <= hi);
  const T lower_bounded = v >= lo ? v : lo;
  const T both_bounded = lower_bounded <= hi ? lower_bounded : hi;
  return both_bounded;
}

}  // namespace mays

#endif  // MAYS_CLAMP_H
