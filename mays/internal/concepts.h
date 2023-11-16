// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#ifndef MAYS_INTERNAL_TYPE_TRAITS_H
#define MAYS_INTERNAL_TYPE_TRAITS_H

#include <concepts>
#include <type_traits>

namespace mays::internal {

// True if |From| is implicitly convertible to |To| without going through a narrowing conversion.
// May be available in future C++ versions through WG21 P0870 (see
// https://github.com/cplusplus/papers/issues/724).
template <class From, class To>
concept is_convertible_without_narrowing = std::convertible_to<From, To> && requires(From&& x) {
  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  { std::type_identity_t<To[]>{std::forward<From>(x)} } -> std::same_as<To[1]>;
};

// True if T sign-extends when right-shifted (two's complement representation is assumed).
template <class T>
concept has_arithmetic_shift =
    std::is_integral_v<T> && std::bool_constant<(T{-1} >> 1) == T{-1}>::value;

}  // namespace mays::internal

#endif  // MAYS_INTERNAL_TYPE_TRAITS_H
