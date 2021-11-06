// (C) Copyright 2021 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#include "crc.h"

#include <string_view>

#include <catch2/catch.hpp>

namespace mays {
namespace {

TEST_CASE("Compute CRCs against their catalog \"check\" values", "[crc]") {
  constexpr auto compute_crc = []([[maybe_unused]] auto crc) {
    constexpr std::string_view kTestString = "123456789";
    return decltype(crc)::Compute(kTestString.data(), kTestString.size());
  };
  CHECK(0x26 == compute_crc(Crc<Crc6Darc>()));
  CHECK(0x75 == compute_crc(Crc<Crc7Mmc>()));
  CHECK(0x26 == compute_crc(Crc<Crc8Bluetooth>()));
  CHECK(0x059e == compute_crc(Crc<Crc15Can>()));
  CHECK(0xbb3d == compute_crc(Crc<Crc16Arc>()));
  CHECK(0x31c3 == compute_crc(Crc<Crc16Xmodem>()));
  CHECK(0x04f03 == compute_crc(Crc<Crc17CanFd>()));
  CHECK(0x0ed841 == compute_crc(Crc<Crc21CanFd>()));
  CHECK(0xc25a56 == compute_crc(Crc<Crc24Ble>()));
  CHECK(0x21cf02 == compute_crc(Crc<Crc24Openpgp>()));
  CHECK(0xfc891918 == compute_crc(Crc<Crc32Bzip2>()));
  CHECK(0xcbf43926 == compute_crc(Crc<Crc32IsoHdlc>()));
}

TEST_CASE("Compute CRCs with an explicit initial value", "[crc]") {
  // Crc24Openpgp normally has a non-zero initial value.
  Crc<Crc24Openpgp> crc(/*initial_value=*/0);

  // Run the feedback shift register a single time, so that the polynomial is subtracted from it.
  crc.AppendBits<1>(0b1);

  // By inspection, the only contents of the register should be the polynomial.
  CHECK(Crc24Openpgp::kPolynomial == crc.GetCheckValue());
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("Compute CRCs in parts is same as in one step",
                   "[crc]",
                   Crc6Darc,
                   Crc7Mmc,
                   Crc8Bluetooth,
                   Crc15Can,
                   Crc16Arc,
                   Crc16Xmodem,
                   Crc17CanFd,
                   Crc21CanFd,
                   Crc24Ble,
                   Crc24Openpgp,
                   Crc32Bzip2,
                   Crc32IsoHdlc) {
  constexpr std::string_view kTestString = "123456789";
  constexpr auto compute_crc = [kTestString] {
    return Crc<TestType>::Compute(kTestString.data(), kTestString.size());
  };
  SECTION("Octet-oriented data") {
    constexpr auto compute_crc_in_parts = [str0 = kTestString.substr(0, 3),
                                           str1 = kTestString.substr(3)] {
      Crc<TestType> crc;
      crc.AppendOctets(str0.data(), str0.size());
      crc.AppendOctets(str1.data(), str1.size());
      return crc.GetCheckValue();
    };
    CHECK(compute_crc() == compute_crc_in_parts());
  }

  SECTION("Bit-oriented data") {
    constexpr auto compute_crc_by_bits = [str0 = kTestString.substr(0, 3),
                                          str1 = kTestString.substr(3)] {
      Crc<TestType> crc;
      for (const auto c : str0) {
        crc.template AppendBits<8>(c);
      }
      crc.AppendOctets(str1.data(), str1.size());
      return crc.GetCheckValue();
    };
    CHECK(compute_crc() == compute_crc_by_bits());
  }
}

TEST_CASE("Compose bit-oriented computation", "[crc]") {
  SECTION("Reflected") {
    using TestType = Crc16Arc;
    static_assert(TestType::kReflect);
    Crc<TestType> crc;
    crc.AppendBits<12>(0xbed);  // NOLINT(readability-magic-numbers)
    crc.AppendBits<4>(0x9U);    // NOLINT(readability-magic-numbers)
    CHECK(Crc<TestType>::Compute("\xed\x9b", 2) == crc.GetCheckValue());
  }
  SECTION("Unreflected") {
    using TestType = Crc15Can;
    static_assert(!TestType::kReflect);
    Crc<TestType> crc;
    crc.AppendBits<12>(0xbed);  // NOLINT(readability-magic-numbers)
    crc.AppendBits<4>(0x9);     // NOLINT(readability-magic-numbers)
    CHECK(Crc<TestType>::Compute("\xbe\xd9", 2) == crc.GetCheckValue());
  }
  SECTION("More bits in data than in polynomial") {
    using TestType = Crc7Mmc;
    static_assert(!TestType::kReflect);
    Crc<TestType> crc;
    crc.AppendBits<16>(0xcafe);  // NOLINT(readability-magic-numbers)
    crc.AppendBits<8>(0x91);     // NOLINT(readability-magic-numbers)
    CHECK(Crc<TestType>::Compute("\xca\xfe\x91", 3) == crc.GetCheckValue());
  }
  SECTION("More bits in data than in DataBitWidth") {
    using TestType = Crc24Ble;
    static_assert(TestType::kReflect);
    Crc<TestType> crc;
    crc.AppendBits<4>(0xcafe);  // NOLINT(readability-magic-numbers)
    crc.AppendBits<0>(0b100);
    crc.AppendBits<4>(0x91);  // NOLINT(readability-magic-numbers)
    CHECK(Crc<TestType>::Compute("\x1e", 1) == crc.GetCheckValue());
  }
  SECTION("Odd number of bits") {
    using TestType = Crc6Darc;
    static_assert(TestType::kReflect);
    Crc<TestType> crc;
    crc.AppendBits<3>(0b100);
    crc.AppendBits<2>(0b01);
    crc.AppendBits<1>(0b0);
    crc.AppendBits<2>(0b10);

    // 0x8c == 0b10'0'01'100
    CHECK(Crc<TestType>::Compute("\x8c", 1) == crc.GetCheckValue());
  }
}

}  // namespace
}  // namespace mays
