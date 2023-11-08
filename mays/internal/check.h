// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#ifndef MAYS_INTERNAL_CHECK_H
#define MAYS_INTERNAL_CHECK_H

#ifndef MAYS_HANDLE_CHECK_FAILURE
// NOLINTNEXTLINE(misc-include-cleaner)
#include <cstdlib>
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MAYS_HANDLE_CHECK_FAILURE(condition_string) std::abort()
#endif  // MAYS_HANDLE_CHECK_FAILURE

#if __has_cpp_attribute(unlikely)
#if defined(__clang__)
#if __cplusplus < 202002L
#pragma clang diagnostic ignored "-Wc++20-extensions"
#endif  // __cplusplus < 202002L
#endif  // defined(__clang__)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MAYS_ATTRIBUTE_UNLIKELY [[unlikely]]
#else
#define MAYS_ATTRIBUTE_UNLIKELY
#endif  // __has_cpp_attribute(unlikely)

// NOLINTBEGIN(cppcoreguidelines-avoid-do-while)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MAYS_CHECK(condition)                                                                   \
  do {                                                                                          \
    static_assert(std::is_same_v<decltype(condition), bool>,                                    \
                  "Condition expression must be type bool, not just convertible to type bool. " \
                  "Example: MAYS_CHECK(ptr != nullptr) instead of MAYS_CHECK(ptr)");            \
    if (!(condition))                                                                           \
      MAYS_ATTRIBUTE_UNLIKELY { MAYS_HANDLE_CHECK_FAILURE(#condition); }                        \
  } while (false)
// NOLINTEND(cppcoreguidelines-avoid-do-while)

#endif  // MAYS_INTERNAL_CHECK_H
