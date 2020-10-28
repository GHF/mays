// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#ifndef MAYS_INTERNAL_POLYFILL_H
#define MAYS_INTERNAL_POLYFILL_H

#if __cpp_lib_constexpr_utility
#include <utility>
#else
#include <type_traits>
#endif  // __cpp_lib_constexpr_utility

namespace mays::internal {

#if __cpp_lib_constexpr_utility
using std::swap;
#else
// std::swap is not constexpr until C++20. This does not accept array types.
template <typename T>
constexpr void swap(T& x, T& y) {
  static_assert(std::is_move_constructible_v<T>);
  static_assert(std::is_move_assignable_v<T>);
  T t(std::move(x));
  x = std::move(y);
  y = std::move(t);
}
#endif  // __cpp_lib_constexpr_utility

}  // namespace mays::internal

#endif  // MAYS_INTERNAL_POLYFILL_H
