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

// NOLINTBEGIN(cppcoreguidelines-avoid-do-while)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MAYS_CHECK(condition)                                                                   \
  do {                                                                                          \
    static_assert(std::same_as<decltype(condition), bool>,                                      \
                  "Condition expression must be type bool, not just convertible to type bool. " \
                  "Example: MAYS_CHECK(ptr != nullptr) instead of MAYS_CHECK(ptr)");            \
    if (!(condition)) [[unlikely]] {                                                            \
      MAYS_HANDLE_CHECK_FAILURE(#condition);                                                    \
    }                                                                                           \
  } while (false)
// NOLINTEND(cppcoreguidelines-avoid-do-while)

#endif  // MAYS_INTERNAL_CHECK_H
