// (C) Copyright 2021 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#ifndef MAYS_ADD_H
#define MAYS_ADD_H

#include <optional>
#include <type_traits>

namespace mays {

#if !__has_builtin(__builtin_add_overflow)
#pragma error "No compiler support for checked arithmetic intrinsics"
#endif  // !__has_builtin(__builtin_add_overflow)

// Performs addition with checking for overflow. Returns the sum if no overflow occurred,
// std::nullopt otherwise. The result type is _not_ the type given by promotion rules, but rather
// the type to which both operands can be implicitly converted.
//
// Example:
//   const uint8_t saturated_x_sum = Add(int8_t{100}, int8_t{x}).value_or(127);
template <typename A0, typename A1, typename Sum = std::common_type_t<A0, A1>>
[[nodiscard]] constexpr std::optional<Sum> Add(A0 addend0, A1 addend1) {
  Sum result{};
  if (__builtin_add_overflow(addend0, addend1, &result)) {
    return std::nullopt;
  }
  return result;
}

// Shorthand to perform addition on operands whose types are deduced then check for overflow against
// a specific explicitly-provided type.
//
// Example:
//   const std::optional sum = AddInto<int>(int8_t{100}, int8_t{80});
// |sum| contains int{180} without overflow.
template <typename Sum, typename A0, typename A1>
[[nodiscard]] constexpr std::optional<Sum> AddInto(A0 addend0, A1 addend1) {
  return Add<A0, A1, Sum>(addend0, addend1);
}

}  // namespace mays

#endif  // MAYS_ADD_H
