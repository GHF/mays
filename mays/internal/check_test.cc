// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#include "check.h"

#include <optional>
#include <string>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

namespace mays {
namespace {

class CheckFixture {
 protected:
  void HandleCheckFailure(std::string condition_string) {
    condition_ = std::move(condition_string);
  }

  [[nodiscard]] bool handler_called() const { return condition_.has_value(); }
  [[nodiscard]] const auto& condition() const { return condition_; }

 private:
  std::optional<std::string> condition_;
};

#undef MAYS_HANDLE_CHECK_FAILURE
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MAYS_HANDLE_CHECK_FAILURE(condition_string) (HandleCheckFailure(condition_string))

using namespace Catch::Matchers;

TEST_CASE_METHOD(CheckFixture, "Check true condition does nothing", "[internal/assert]") {
  const int i = 0;
  MAYS_CHECK(i == 0);
  MAYS_CHECK(1 == 1);
  CHECK(false == handler_called());
}

TEST_CASE_METHOD(CheckFixture,
                 "Check true condition can be used in constexpr",
                 "[internal/assert]") {
  // Shadow HandleCheckFailure.
  auto HandleCheckFailure = [](auto) { FAIL(); };
  [[maybe_unused]] constexpr auto foo = [&] {
    MAYS_CHECK(1 == 1);
    return 1;
  }();
}

TEST_CASE_METHOD(CheckFixture, "Check false condition calls custom handler", "[internal/assert]") {
  MAYS_CHECK(1 == 2);

  CHECK(handler_called());
  REQUIRE(condition().has_value());
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  CHECK_THAT(*condition(), Equals("1 == 2"));
}

TEST_CASE_METHOD(CheckFixture, "Check condition can use names from binding", "[internal/assert]") {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
  const int kArray[] = {0, 0};
  const auto [a, b] = kArray;  // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
  MAYS_CHECK(a == b);
}

}  // namespace
}  // namespace mays
