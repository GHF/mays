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
  CHECK(0x26 == compute_crc(Crc<Crc8Bluetooth>()));
  CHECK(0x059e == compute_crc(Crc<Crc15Can>()));
  CHECK(0xbb3d == compute_crc(Crc<Crc16Arc>()));
  CHECK(0x31c3 == compute_crc(Crc<Crc16Xmodem>()));
  CHECK(0x04f03 == compute_crc(Crc<Crc17CanFd>()));
  CHECK(0x0ed841 == compute_crc(Crc<Crc21CanFd>()));
  CHECK(0xc25a56 == compute_crc(Crc<Crc24Ble>()));
  CHECK(0x21cf02 == compute_crc(Crc<Crc24Openpgp>()));
  CHECK(0xcbf43926 == compute_crc(Crc<Crc32IsoHdlc>()));
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("Compute CRCs in parts is same as in one step",
                   "[crc]",
                   Crc8Bluetooth,
                   Crc15Can,
                   Crc16Arc,
                   Crc16Xmodem,
                   Crc17CanFd,
                   Crc21CanFd,
                   Crc24Ble,
                   Crc24Openpgp,
                   Crc32IsoHdlc) {
  constexpr std::string_view kTestString = "123456789";
  constexpr auto compute_crc = [kTestString] {
    return Crc<TestType>::Compute(kTestString.data(), kTestString.size());
  };
  constexpr auto compute_crc_in_parts = [str0 = kTestString.substr(0, 3),
                                         str1 = kTestString.substr(3)] {
    Crc<TestType> crc;
    crc.AppendOctets(str0.data(), str0.size());
    crc.AppendOctets(str1.data(), str1.size());
    return crc.GetCheckValue();
  };
  CHECK(compute_crc() == compute_crc_in_parts());
}

}  // namespace
}  // namespace mays
