// (C) Copyright 2021 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#ifndef MAYS_REDUCE_H
#define MAYS_REDUCE_H

#include <numeric>
#include <tuple>
#include <type_traits>

namespace mays {
namespace detail {

// Compute greatest common denominator of arbitrary arguments, of which there must be at least two.
template <typename T, typename... Ts>
constexpr std::common_type_t<T, Ts...> Gcd(const T& first_value, const Ts&... values) {
  if constexpr (sizeof...(Ts) == 1) {
    return std::gcd(first_value, values...);
  } else {
    return std::gcd(first_value, Gcd(values...));
  }
}

template <typename... Ts>
[[nodiscard]] constexpr std::tuple<Ts...> Reduce(const Ts&... values) {
  const std::common_type_t<Ts...> gcd = ::mays::detail::Gcd(values...);
  return std::tuple(static_cast<Ts>(values / gcd)...);
}

}  // namespace detail

// Given at least two integers, return a tuple containing integers of the same types, each divided
// by the GCD of all of the inputs. The elements of the returned tuple is then "simplified" so that
// they are setwise coprime (but not necessarily pairwise coprime).
//
// This performs |sizeof...(Ts) - 1| calls to std::gcd.
using ::mays::detail::Reduce;  // NOLINT(misc-unused-using-decls)

// Overload that helps deduce and unpack a tuple of integers to simplify.
template <typename... Ts>
[[nodiscard]] constexpr std::tuple<Ts...> Reduce(const std::tuple<Ts...>& values) {
  return std::apply(::mays::detail::Reduce<Ts...>, values);
}

}  // namespace mays

#endif  // MAYS_REDUCE_H
