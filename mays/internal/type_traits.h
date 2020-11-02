// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#ifndef MAYS_INTERNAL_TYPE_TRAITS_H
#define MAYS_INTERNAL_TYPE_TRAITS_H

#include <type_traits>

#include "polyfill.h"

namespace mays::internal {

// True if |From| is implicitly convertible to |To| without going through a narrowing conversion.
// Will likely be included in C++2b through WG21 P0870 (see
// https://github.com/cplusplus/papers/issues/724).
template <typename From, typename To, typename Enable = void>
struct is_convertible_without_narrowing : std::false_type {};

// Implement "construct array of From" technique from P0870R4 with SFINAE instead of requires.
template <typename From, typename To>
struct is_convertible_without_narrowing<
    From,
    To,
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    std::void_t<decltype(type_identity_t<To[]>{std::declval<From>()})>> : std::true_type {};

template <typename From, typename To>
constexpr bool is_convertible_without_narrowing_v =
    is_convertible_without_narrowing<From, To>::value;

// True if T sign-extends when right-shifted (two's complement representation is assumed).
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
struct has_arithmetic_shift : std::integral_constant<bool, (T{-1} >> 1) == T{-1}> {};

template <typename T>
constexpr bool has_arithmetic_shift_v = has_arithmetic_shift<T>::value;

}  // namespace mays::internal

#endif  // MAYS_INTERNAL_TYPE_TRAITS_H
