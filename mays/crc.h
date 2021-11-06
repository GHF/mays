// (C) Copyright 2021 Xo Wang <xo@geekshavefeelings.com>
// SPDX-License-Identifier: Apache-2.0
// vim: et:sw=2:ts=2:tw=100

#ifndef MAYS_CRC_H
#define MAYS_CRC_H

#include <cstddef>
#include <cstdint>

namespace mays {

// Computes a cyclic redundancy check (CRC) over a message using the CRC model parameters specified
// by |Traits|. Some CRC models are provided as aliases of the |CrcTraits| helper class.
//
// Example:
//   constexpr std::string_view kStr = "123456789";
//   constexpr uint16_t check_value = Crc<Crc16Arc>::Compute(kStr.data(), kStr.size());
//   // |check_value| is 0xbb3d
//
// This class can also be instantiated to store the state of a CRC over a series of messages.
//
// Example:
//   Crc<Crc16Arc> crc;
//   crc.AppendOctets("123", 3);
//   crc.AppendOctets("45", 2);
//   crc.AppendOctets("6789", 4);
//   constexpr uint16_t check_value = crc.GetCheckValue();  // |check_value| is 0xbb3d
//
// The model used is originally specified in "A Painless Guide to CRC Error Detection Algorithms"
// (Ross N. Williams, 1993), accessed at https://zlib.net/crc_v3.txt and the implementation is also
// written from first principles using the ideas therein, including documenting the implementation
// simultaneously as a linear-feedback shift register (LFSR) and as mod-2 polynomial long division.
template <typename Traits>
class Crc {
 public:
  using RegisterType = typename Traits::RegisterType;

  // Construct a CRC computation whose state is initialized with |initial_value|, which is specified
  // higher-power-left and will be reflected as necessary for the CRC model.
  constexpr explicit Crc(RegisterType initial_value = Traits::kInitialValue)
      : remainder_(Traits::kReflect
                       ? ReflectBits<RegisterType, Traits::kPolynomialBitWidth>(initial_value)
                       : initial_value) {}

  // Computes a CRC check value over a sequence of octets.
  // The |Octet| template parameter must be an 8-bit type, e.g. uint8_t, std::byte, char, etc.
  template <typename Octet>
  [[nodiscard]] static constexpr RegisterType Compute(const Octet* data, size_t length) {
    Crc crc;
    crc.AppendOctets(data, length);
    return crc.GetCheckValue();
  }

  // Processes a sequence of octets through the CRC. May be called multiple times to process parts
  // of a full sequence. Calls to this function do not commutate. The |Octet| template parameter
  // must be an 8-bit type, e.g. uint8_t, std::byte, char, etc.
  template <typename Octet>
  constexpr void AppendOctets(const Octet* data, size_t length) {
    // Templated on |Octet| instead of taking |const void*| to allow constexpr computation.
    static_assert(sizeof(Octet) == sizeof(uint8_t));

    // Each iteration shifts an octet into the long division feedback system (i.e. for 8 cycles).
    for (size_t i = 0; i < length; i++) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      const auto data_byte = static_cast<uint8_t>(data[i]);
      const auto [remainder_msbyte, remainder_lsbytes] = SplitRemainder();

      // Add the remainder to the next eight bits of message.
      const uint8_t dividend = data_byte ^ remainder_msbyte;

      // |remainder_lsbytes| contains all the remainder bits that do not participate in the feedback
      // with the 8 message bits, aside from being shifted 8 positions. The shift is associative
      // w.r.t. addition, and was already performed by |SplitRemainder|, so add those bits back in
      // after operating the feedback shift register.
      remainder_ = GetRemainderForBits(dividend) ^ remainder_lsbytes;
    }
  }

  // Processes the rightmost |DataBitWidth| bits in |value| through the CRC. Other bits in |value|
  // are ignored. May be called multiple times to process parts of a full sequence. Calls to this
  // function do not commutate.
  template <size_t DataBitWidth, typename DataType>
  constexpr void AppendBits(DataType value) {
    static_assert(DataBitWidth <= sizeof(DataType) * 8,
                  "Can not process more bits than in type DataType");

    // Put |value| in a type that's unsigned and large enough to hold both |remainder_| and |value|.
    // This is similar to using std::make_unsigned_t<std::common_type_t<DataType, RegisterType>> but
    // without including <type_traits>.
    auto padded_value = [value] {
      if constexpr (sizeof(DataType) > sizeof(RegisterType)) {
        if constexpr (sizeof(DataType) <= sizeof(unsigned int)) {
          return static_cast<unsigned int>(value);
        } else if constexpr (sizeof(DataType) <= sizeof(unsigned long)) {
          return static_cast<unsigned long>(value);
        } else {
          static_assert(sizeof(DataType) == sizeof(unsigned long long));
          return static_cast<unsigned long>(value);
        }
      } else {
        return static_cast<RegisterType>(value);
      }
    }();
    using DividendType = decltype(padded_value);

    // Mask off all but the rightmost |DataBitWidth| bits.
    constexpr auto kMask =
        static_cast<DividendType>(~(static_cast<DividendType>(-1) << DataBitWidth));
    const DividendType masked_value = padded_value & kMask;

    // Add |remainder_| to |masked_value|, taking into account |Traits::kReflect| to line up bits.
    const DividendType dividend = [this, masked_value] {
      if constexpr (Traits::kReflect) {
        return static_cast<DividendType>(masked_value ^ remainder_);
      }
      // In the unreflected model, |remainder_| has opposite orientation to |kReversePolynomial| so
      // it must be reflected. This also places its highest-power cofficient on the right, where
      // it's more convenient, but then message bits must also be reflected to match. Finally,
      // |GetRemainderForBits(…)| expects the data bits MSb-left, so reflect once more.
      return ReflectBits<DividendType, DataBitWidth>(
          ReflectBits<DividendType, DataBitWidth>(masked_value) ^
          ReflectBits<DividendType, Traits::kPolynomialBitWidth>(remainder_));
    }();

    // Run the feedback system |DataBitWidth| cycles and obtain the remainder.
    remainder_ = GetRemainderForBits<DividendType, DataBitWidth>(dividend);
  }

  // Returns the current CRC check value.
  [[nodiscard]] constexpr RegisterType GetCheckValue() const {
    return remainder_ ^ Traits::kOutputXorMask;
  }

 private:
  // Returns a struct containing:
  // - Highest-power bits (up to 8) of |remainder_|. In the case that |remainder_| has fewer than 8
  //   bits: right-aligned if |Traits::kReflected|, else left-aligned.
  // - Any other bits of |remainder|. Occupies the rightmost bits in |RegisterType| but these bits
  //   are left-aligned if |Traits::kReflected| is false,
  //   e.g. for CRC-15 unreflected: 0b0xxx'xxxx'0000'0000
  [[nodiscard]] constexpr auto SplitRemainder() const {
    // Return type (needs to be decomposed with bindings)
    struct RemainderParts {
      uint8_t ms_byte;
      RegisterType ls_bytes;
    };

    // When the polynomial width is smaller than an octet, the shift register cyclic feedback occurs
    // before 8 cycles (i.e. within |GetRemainderForBits(…)|) and all remainder bits participate in
    // the feedback. So |ls_bytes| is zero in this case.
    if constexpr (Traits::kPolynomialBitWidth <= 8) {
      if constexpr (Traits::kReflect) {
        return RemainderParts{static_cast<uint8_t>(remainder_), 0};
      } else {
        // First bit of message data to shift in is its MSb, so line up the remainder to the left.
        return RemainderParts{static_cast<uint8_t>(remainder_ << (8 - Traits::kPolynomialBitWidth)),
                              0};
      }
    } else {
      if constexpr (Traits::kReflect) {
        return RemainderParts{static_cast<uint8_t>(remainder_),
                              static_cast<RegisterType>(remainder_ >> 8)};
      } else {
        // First bit of message data needs to be added to the next bit of remainder, which is at its
        // leftmost position. Shift the remainder right to line it up with the message octet.
        return RemainderParts{static_cast<uint8_t>(remainder_ >> (Traits::kPolynomialBitWidth - 8)),
                              static_cast<RegisterType>((remainder_ << 8) & kPolynomialMask)};
      }
    }
  }

  // Compute the remainder of |value| divided by |Traits::kPolynomial| in which each bit is a
  // coefficient of a |DataBitWidth|-th power polynomial, right-aligned within |DataType| with the
  // LSb in the ones (rightmost) position (i.e. unreflected). The order by which the coefficients of
  // |value| are shifted out depends on |Traits::kReflect|.
  //
  // Any additional bits to the left of |value|'s |DataBitWidth| bits (e.g. the current
  // |remainder_|) can also participate in the long division cyclic feedback. However, if there are
  // more than |Traits::kPolynomialBitWidth| bits loaded into |value|, those bits may be lost and
  // that case isn't tested.
  template <typename DataType, size_t DataBitWidth = sizeof(DataType) * 8>
  [[nodiscard]] static constexpr RegisterType GetRemainderForBits(DataType value) {
    static_assert(DataBitWidth <= sizeof(DataType) * 8,
                  "Can not process more bits than DataType holds");

    // |kReversePolynomial| is higher-power-right, so this unintuitively negative reflect condition
    // is to orient the MSb (left) of unreflected data towards the right in order to account for
    // "reflecting the world" (Williams).
    if constexpr (!Traits::kReflect) {
      value = ReflectBits<DataType, DataBitWidth>(value);
    }

    // Store the value of the data bits into a working register large enough to XOR with the
    // polynomial. Type deduction using this lambda prevents promotion to int (which is signed),
    // e.g. using decltype(accumulator ^ kReversePolynomial).
    auto accumulator = [value] {
      if constexpr (sizeof(DataType) > sizeof(RegisterType)) {
        return value;
      } else {
        return RegisterType{value};
      }
    }();
    static_assert(static_cast<decltype(accumulator)>(-1) > 0, "accumulator must not be signed");

    // The division is performed with mod 2 arithmetic, in which both addition and subtraction are
    // equivalent to XOR and do not carry. Addition and subtraction are still used to draw analogies
    // to lay decimal long division.
    for (size_t i = 0; i < DataBitWidth; i++) {
      // Subtract the divisor from the accumulator (which is the dividend in this long division) if
      // the accumulator is greater or equal. With mod 2 arithmetic, this only requires testing the
      // dividend's highest-power bit (which is rightmost in this formulation).
      const bool subtract = accumulator & 0b1;

      // Shift the accumulator towards the higher-power generator polynomial cofficient direction.
      // If |Traits::kReflect| = 0, then the message bits in |accumulator| (which have been
      // reflected) shift from MSb to to LSb. Otherwise, message bits are evaluated LSb-first.
      // The bit on the right is automatically discarded, which obviates any masking later on.
      accumulator >>= 1;
      if (subtract) {
        // Working in the "reflected world" (Williams) means that both of these variables are
        // automatically right-aligned (and aligned to each other) with left-zero-padding.
        accumulator ^= kReversePolynomial;
      }
    }

    // Unreflected CRCs want results MSb-left, so reflect as necessary.
    if constexpr (!Traits::kReflect) {
      accumulator = ReflectBits<decltype(accumulator), Traits::kPolynomialBitWidth>(accumulator);
    }
    return static_cast<RegisterType>(accumulator);
  }

  // Reverses the order of the lowest |BitWidth| bits in the integral argument |value|.
  template <typename T, size_t BitWidth>
  [[nodiscard]] static constexpr T ReflectBits(T value) {
    static_assert(BitWidth <= sizeof(T) * 8, "Can not reflect more bits than in type T");
    if constexpr (BitWidth <= 1) {
      return value;
    } else {
      auto mask = static_cast<T>(T{1} << (BitWidth - 1));
      T out = value;
      for (size_t i = 0; i < BitWidth; i++) {
        if (value & T{1}) {
          out = static_cast<T>(out | mask);
        } else {
          out = static_cast<T>(out & ~mask);
        }
        mask >>= 1;
        value >>= 1;
      }
      return out;
    }
  }

  static_assert(static_cast<RegisterType>(-1) > 0, "Accumulation register must not be signed");
  static constexpr size_t kRegisterTypeBitWidth = 8 * sizeof(RegisterType);
  static_assert(Traits::kPolynomialBitWidth <= kRegisterTypeBitWidth,
                "RegisterType can't hold polynomial");

  // Polynomial with highest-power coefficient in the ones position (reference polynomials are
  // written higher-power-left).
  static constexpr RegisterType kReversePolynomial =
      ReflectBits<RegisterType, Traits::kPolynomialBitWidth>(Traits::kPolynomial);

  // Bit mask with the rightmost polynomial coefficient bits (0 or 1) set.
  static constexpr RegisterType kPolynomialMask =
      static_cast<RegisterType>(~(static_cast<RegisterType>(-1) << Traits::kPolynomialBitWidth));

  // The remainder result of the long division is stored right-aligned with its most powerful
  // coefficient in the leftmost position for unreflected CRCs and rightmost (one's) position for
  // reflected CRCs, i.e. in the same orientation as final check value.
  RegisterType remainder_;
};

// CRC model parameters used in the Williams model set out in "A Painless Guide to CRC Error
// Detection Algorithms," with a type parameter and without separate "reflect" parameters.
//
// All classes passed to Crc as its <Traits> template parameter must have these parameters, but do
// not need to be type mays::CrcTraits.
template <typename Type,
          size_t PolynomialBitWidth,
          Type Polynomial,
          Type InitialValue,
          bool Reflect,
          Type OutputXorMask>
class CrcTraits {
 public:
  // Type to hold the shift register representing coefficients of the generator polynomial.
  using RegisterType = Type;

  // Number of bits in the polynomial (and thus the check value).
  static constexpr size_t kPolynomialBitWidth = PolynomialBitWidth;

  // Divisor for each iteration of long division, representing the CRC polynomial coefficients
  // written in higher-power-left form with the (implicit) D**PolynomialBitWidth cofficient omitted.
  static constexpr Type kPolynomial = Polynomial;

  // Initial value added to the accumulator register before long division, also written
  // higher-power-left form. Note that if no message data moves through the CRC, then this will be
  // reflected per the |kReflect| parameter.
  static constexpr Type kInitialValue = InitialValue;

  // True if message data (which comprise the dividend for the CRC polynomial division) are to be
  // fed into the shift register LSb first instead of MSb first. Represents both the "refin" and
  // "refout" parameters of the Williams model, so if true, also causes the output check value to be
  // output with its MSb in the rightmost (one's) position.
  static constexpr bool kReflect = Reflect;

  // Value by which to XOR the final remainder to obtain a check value.
  static constexpr Type kOutputXorMask = OutputXorMask;
};

// Select parameter sets from "Catalogue of parametrised CRC algorithms" (Greg Cook, 2021) accessed
// at https://reveng.sourceforge.io/crc-catalogue/
//
// These were chosen not because of utility but because their instantiation in unit tests provides
// good coverage of the CRC computation branches, e.g. un-/reflected models, polynomial widths
// smaller than data types, initial and output XOR values, etc.
//
// TODO(xo): Replace with NOLINTBEGIN(…)/NOLINTEND(…) after upgrading to Clang-Tidy 14
// NOLINTNEXTLINE(readability-magic-numbers)
using Crc6Darc = CrcTraits<uint8_t, 6, 0b01'1001, 0, true, 0>;
// NOLINTNEXTLINE(readability-magic-numbers)
using Crc7Mmc = CrcTraits<uint8_t, 7, 0b000'1001, 0, false, 0>;
// NOLINTNEXTLINE(readability-magic-numbers)
using Crc8Bluetooth = CrcTraits<uint8_t, 8, 0b1010'0111, 0, true, 0>;
// NOLINTNEXTLINE(readability-magic-numbers)
using Crc15Can = CrcTraits<uint16_t, 15, 0b0100'0101'1001'1001, 0, false, 0>;
// NOLINTNEXTLINE(readability-magic-numbers)
using Crc16Arc = CrcTraits<uint16_t, 16, 0b1000'0000'0000'0101, 0, true, 0>;
// NOLINTNEXTLINE(readability-magic-numbers)
using Crc16Xmodem = CrcTraits<uint16_t, 16, 0b0001'0000'0010'0001, 0, false, 0>;
// NOLINTNEXTLINE(readability-magic-numbers)
using Crc17CanFd = CrcTraits<uint32_t, 17, 0x1685b, 0, false, 0>;
// NOLINTNEXTLINE(readability-magic-numbers)
using Crc21CanFd = CrcTraits<uint32_t, 21, 0x102899, 0, false, 0>;
// NOLINTNEXTLINE(readability-magic-numbers)
using Crc24Ble = CrcTraits<uint32_t, 24, 0x00065b, 0x555555, true, 0>;
// NOLINTNEXTLINE(readability-magic-numbers)
using Crc24Openpgp = CrcTraits<uint32_t, 24, 0x864cfb, 0xb704ce, false, 0>;
// NOLINTNEXTLINE(readability-magic-numbers)
using Crc32IsoHdlc = CrcTraits<uint32_t, 32, 0x04c11db7, 0xffffffff, true, 0xffffffff>;

}  // namespace mays

#endif  // MAYS_CRC_H
