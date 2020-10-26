// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#ifndef MAYS_INTERNAL_CHECK_H
#define MAYS_INTERNAL_CHECK_H

#ifndef MAYS_HANDLE_CHECK_FAILURE
#include <cstdlib>
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MAYS_HANDLE_CHECK_FAILURE(condition_string) std::abort()
#endif  // MAYS_HANDLE_CHECK_FAILURE

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MAYS_CHECK(condition)                \
  [&] {                                      \
    if (!(condition)) {                      \
      MAYS_HANDLE_CHECK_FAILURE(#condition); \
    }                                        \
  }()

#endif  // MAYS_INTERNAL_CHECK_H
