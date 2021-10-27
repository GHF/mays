// (C) Copyright 2021 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#ifndef MAYS_MULTIPLY_H
#define MAYS_MULTIPLY_H

#include <optional>
#include <type_traits>

namespace mays {

#if !__has_builtin(__builtin_mul_overflow)
#pragma error "No compiler support for checked arithmetic intrinsics"
#endif  // !__has_builtin(__builtin_mul_overflow)

// Performs multiplication with checking for overflow. Returns the product if no overflow occurred,
// std::nullopt otherwise. The result type is _not_ the type given by promotion rules, but rather
// the type to which both operands can be implicitly converted.
//
// Example:
//   uint8_t saturated_x_product = Multiply(int8_t{2}, int8_t{x}).value_or(x > 0 ? 127 : -128);
template <typename M0, typename M1, typename Product = std::common_type_t<M0, M1>>
[[nodiscard]] constexpr std::optional<Product> Multiply(M0 multiplicand, M1 multiplier) {
  Product result;
  if (__builtin_mul_overflow(multiplicand, multiplier, &result)) {
    return std::nullopt;
  }
  return result;
}

// Shorthand to perform multiplication on operands whose types are deduced then check for overflow
// against a specific explicitly-provided type.
//
// Example:
//   const std::optional product = MultiplyInto<int>(int8_t{8}, int8_t{16});
// |sum| contains int{128} without overflow.
template <typename Product, typename M0, typename M1>
[[nodiscard]] constexpr std::optional<Product> MultiplyInto(M0 multiplicand, M1 multiplier) {
  return Multiply<M0, M1, Product>(multiplicand, multiplier);
}

}  // namespace mays

#endif  // MAYS_MULTIPLY_H
