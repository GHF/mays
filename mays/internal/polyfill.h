// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#ifndef MAYS_INTERNAL_POLYFILL_H
#define MAYS_INTERNAL_POLYFILL_H

#include <version>

#if __cpp_lib_integer_comparison_functions
#include <utility>
#else
#include <limits>
#include <type_traits>
#endif  // __cpp_lib_integer_comparison_functions

#if __cpp_lib_type_identity
#include <type_traits>
#endif  // __cpp_lib_type_identity

namespace mays::internal {

#if __cpp_lib_integer_comparison_functions
using std::cmp_greater;
using std::cmp_greater_equal;
using std::cmp_less;
using std::cmp_less_equal;
#else
template <class T, class U>
[[nodiscard]] constexpr bool cmp_less(T t, U u) {
  using UT = std::make_unsigned_t<T>;
  using UU = std::make_unsigned_t<U>;
  if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
    return t < u;
  } else if constexpr (std::is_signed_v<T>) {
    return t < 0 ? true : UT(t) < u;
  } else {
    return u < 0 ? false : t < UU(u);
  }
}

template <class T, class U>
[[nodiscard]] constexpr bool cmp_greater(T t, U u) {
  return cmp_less(u, t);
}

template <class T, class U>
[[nodiscard]] constexpr bool cmp_less_equal(T t, U u) {
  return !cmp_greater(t, u);
}

template <class T, class U>
[[nodiscard]] constexpr bool cmp_greater_equal(T t, U u) {
  return !cmp_less(t, u);
}
#endif  // __cpp_lib_integer_comparison_functions

#if __cpp_lib_type_identity
using std::type_identity;
using std::type_identity_t;
#else
template <typename T>
struct type_identity {
  using type = T;
};

template <class T>
using type_identity_t = typename type_identity<T>::type;
#endif  // __cpp_lib_type_identity

}  // namespace mays::internal

#endif  // MAYS_INTERNAL_POLYFILL_H
