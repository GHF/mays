// (C) Copyright 2019 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#ifndef MAYS_ARRAY_SIZE_H
#define MAYS_ARRAY_SIZE_H

#include <cstddef>

namespace mays {
namespace detail {

// Recursively reduces rank of the array type until either Rank = 0U, which returns the current
// dimension's bound, or the array type runs out of dimensions, which fails at the incomplete
// primary template.
template <typename T, unsigned Rank>
struct array_size;  // base case (primary template) is undefined

template <typename T, size_t N>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
struct array_size<T[N], 0U> {
  static constexpr size_t value = N;
};

template <typename T, size_t N, unsigned Rank>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
struct array_size<T[N], Rank> : array_size<T, Rank - 1> {};

template <typename T, unsigned Rank>
constexpr size_t array_size_v = array_size<T, Rank>::value;

}  // namespace detail

// Gets the number of elements in any array that has a defined size. Unlike sizeof(array) /
// sizeof(array[0]), this will fail to compile if used with a pointer rather than an array. Unlike
// std::extent<decltype(array)>(), this will fail to compile rather than return 0 for arrays of
// unknown bound. For multi-dimensional arrays, this returns the bound of the (Rank - 1)th dimension
// (starting from the left).
//
// Examples:
//   int a[2];
//   constexpr size_t kArrayASize = mays::ArraySize(a);  // kArrayASize = 2
//   int b[3][2][1];
//   constexpr size_t kArrayBSize = mays::ArraySize<2>(b);  // kArrayBSize = 1
//
// Note that "array parameters" are simply annotated pointers and not truly array types:
//   void Foo(int elements[3]) {
//     // Does not compile because |elements| is (int*) not (int[3]).
//     // |sizeof(elements) / sizeof(elements[0])| compiles but evaluates to probably 1 or 2
//     // |std::extent_v<decltype(elements)>| compiles but evaluates to 0
//     const size_t num_elements = mays::ArraySize(elements);
//   }
template <size_t Rank = 0, typename T = void, size_t N = 0>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
constexpr size_t ArraySize(const T (&)[N]) {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
  return ::mays::detail::array_size_v<T[N], Rank>;
}

}  // namespace mays

#endif  // MAYS_ARRAY_SIZE_H
