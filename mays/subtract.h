// (C) Copyright 2021 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#ifndef MAYS_SUBTRACT_H
#define MAYS_SUBTRACT_H

#include <optional>
#include <type_traits>

namespace mays {

#if !__has_builtin(__builtin_sub_overflow)
#pragma error "No compiler support for checked arithmetic intrinsics"
#endif  // !__has_builtin(__builtin_sub_overflow)

// Performs subtraction with checking for overflow. Returns the difference if no overflow occurred,
// std::nullopt otherwise. The result type is _not_ the type given by promotion rules, but rather
// the type to which both operands can be implicitly converted.
//
// Example:
//   const uint8_t saturated_x_difference = Subtract(uint8_t{10}, uint8_t{x}).value_or(0);
template <typename M, typename S, typename Difference = std::common_type_t<M, S>>
[[nodiscard]] constexpr std::optional<Difference> Subtract(M minuend, S subtrahend) {
  Difference result;
  if (__builtin_sub_overflow(minuend, subtrahend, &result)) {
    return std::nullopt;
  }
  return result;
}

// Shorthand to perform subtraction on operands whose types are deduced then check for overflow
// against a specific explicitly-provided type.
//
// Example:
//   const std::optional<int> difference = SubtractInto<int>(int8_t{-10}, int8_t{127});
// |difference| contains int{-137} without overflow.
template <typename Difference, typename M, typename S>
[[nodiscard]] constexpr std::optional<Difference> SubtractInto(M minuend, S subtrahend) {
  return Subtract<M, S, Difference>(minuend, subtrahend);
}

}  // namespace mays

#endif  // MAYS_SUBTRACT_H
