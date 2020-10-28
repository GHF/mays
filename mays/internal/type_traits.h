// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#ifndef MAYS_INTERNAL_TYPE_TRAITS_H
#define MAYS_INTERNAL_TYPE_TRAITS_H

#include <type_traits>

namespace mays::internal {

// True if T sign-extends when right-shifted (two's complement representation is assumed).
template <typename T, bool = std::is_signed_v<T>>
struct has_arithmetic_shift : std::integral_constant<bool, (T{-1} >> 1) == T{-1}> {};

template <typename T>
inline constexpr bool has_arithmetic_shift_v = has_arithmetic_shift<T>::value;

}  // namespace mays::internal

#endif  // MAYS_INTERNAL_TYPE_TRAITS_H
