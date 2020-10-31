// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#ifndef MAYS_INTERNAL_CHECK_H
#define MAYS_INTERNAL_CHECK_H

#include <type_traits>

#ifndef MAYS_HANDLE_CHECK_FAILURE
#include <cstdlib>
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MAYS_HANDLE_CHECK_FAILURE(condition_string) std::abort()
#endif  // MAYS_HANDLE_CHECK_FAILURE

#if __has_cpp_attribute(unlikely)
#define MAYS_ATTRIBUTE_UNLIKELY [[unlikely]]
#else
#define MAYS_ATTRIBUTE_UNLIKELY
#endif  // __has_cpp_attribute(unlikely)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MAYS_CHECK(condition)                                                                   \
  [&] {                                                                                         \
    static_assert(std::is_same_v<decltype(condition), bool>,                                    \
                  "Condition expression must be type bool, not just convertible to type bool. " \
                  "Example: MAYS_CHECK(ptr != nullptr) instead of MAYS_CHECK(ptr)");            \
    if (!(condition))                                                                           \
      MAYS_ATTRIBUTE_UNLIKELY { MAYS_HANDLE_CHECK_FAILURE(#condition); }                        \
  }()

#endif  // MAYS_INTERNAL_CHECK_H
