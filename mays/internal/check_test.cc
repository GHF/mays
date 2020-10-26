// (C) Copyright 2020 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0

#include "check.h"

#include <optional>
#include <string>

#include <catch2/catch.hpp>

namespace mays {
namespace {

class CheckFixture {
 protected:
  void HandleCheckFailure(std::string condition_string) {
    condition_ = std::move(condition_string);
  }

  bool handler_called() { return condition_.has_value(); }
  const auto& condition() { return condition_; }

 private:
  std::optional<std::string> condition_;
};

#undef MAYS_HANDLE_CHECK_FAILURE
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MAYS_HANDLE_CHECK_FAILURE(condition_string) (HandleCheckFailure(condition_string))

using namespace Catch::Matchers;

TEST_CASE_METHOD(CheckFixture, "Check true condition does nothing", "[internal/assert]") {
  int i = 0;
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

TEST_CASE_METHOD(CheckFixture, "Check false condition calls custom handler", "[base/assert]") {
  MAYS_CHECK(1 == 2);

  CHECK(1 == handler_called());
  REQUIRE(condition().has_value());
  CHECK_THAT(*condition(), Equals("1 == 2"));
}

}  // namespace
}  // namespace mays
