/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 */
/*  Based on code by
 *      Takeshi Yamamuro <linguin.m.s_at_gmail.com>
 *      Fabrizio Silvestri <fabrizio.silvestri_at_isti.cnr.it>
 *      Rossano Venturini <rossano.venturini_at_isti.cnr.it>
 *   which was available  under the Apache License, Version 2.0.
 */

#ifndef SIMPLE16_H_
#define SIMPLE16_H_

#include "common.h"
#include "codecs.h"

namespace FastPForLib {

/**
 * This is an implementation of the popular Simple16 scheme. It is limited to
 * 28-bit integers (between 0 and 2^28-1).
 *
 * If MarkLength is true, than the number of symbols is written
 * in the stream. Otherwise you need to specify it using the nvalue
 * parameter decodeArray.
 */
template <bool MarkLength> class Simple16 : public IntegerCODEC {
public:
  template <uint32_t num1, uint32_t log1>
  static bool trymefull(const uint32_t *n) {
    const uint32_t min = num1;
    for (uint32_t i = 0; i < min; i++) {
      if (n[i] >= (1U << log1))
        return false;
    }
    return true;
  }

  template <uint32_t num1, uint32_t log1, uint32_t num2, uint32_t log2>
  __attribute__((pure)) static bool trymefull(const uint32_t *n) {
    const uint32_t min1 = num1;
    for (uint32_t i = 0; i < min1; i++) {
      if (n[i] >= (1U << log1))
        return false;
    }
    const uint32_t min2 = num2;
    for (uint32_t i = min1; i < min1 + min2; i++) {
      if (n[i] >= (1U << log2))
        return false;
    }
    return true;
  }

  template <uint32_t num1, uint32_t log1, uint32_t num2, uint32_t log2,
            uint32_t num3, uint32_t log3>
  __attribute__((pure)) static bool trymefull(const uint32_t *n) {
    const uint32_t min1 = num1;
    for (uint32_t i = 0; i < min1; i++) {
      if (n[i] >= (1U << log1))
        return false;
    }
    const uint32_t min2 = num2;

    for (uint32_t i = min1; i < min1 + min2; i++) {
      if (n[i] >= (1U << log2))
        return false;
    }
    const uint32_t min3 = num3;
    for (uint32_t i = min1 + min2; i < min1 + min2 + min3; i++) {
      if (n[i] >= (1U << log3))
        return false;
    }
    return true;
  }

  template <uint32_t num1, uint32_t log1>
  static bool tryme(const uint32_t *n, size_t len) {
    const uint32_t min = (len < num1) ? static_cast<uint32_t>(len) : num1;
    for (uint32_t i = 0; i < min; i++) {
      if ((n[i]) >= (1U << log1))
        return false;
    }
    return true;
  }

  template <uint32_t num1, uint32_t log1, uint32_t num2, uint32_t log2>
  static bool tryme(const uint32_t *n, size_t len) {
    const uint32_t min1 = (len < num1) ? static_cast<uint32_t>(len) : num1;
    for (uint32_t i = 0; i < min1; i++) {
      if ((n[i]) >= (1U << log1))
        return false;
    }
    const uint32_t min2 =
        (len - min1 < num2) ? static_cast<uint32_t>(len - min1) : num2;
    for (uint32_t i = min1; i < min1 + min2; i++) {
      if ((n[i]) >= (1U << log2))
        return false;
    }
    return true;
  }

  template <uint32_t num1, uint32_t log1, uint32_t num2, uint32_t log2,
            uint32_t num3, uint32_t log3>
  static bool tryme(const uint32_t *n, size_t len) {
    const uint32_t min1 = (len < num1) ? static_cast<uint32_t>(len) : num1;
    for (uint32_t i = 0; i < min1; i++) {
      if ((n[i]) >= (1U << log1))
        return false;
    }
    len -= min1;
    const uint32_t min2 = (len < num2) ? static_cast<uint32_t>(len) : num2;

    for (uint32_t i = min1; i < min1 + min2; i++) {
      if ((n[i]) >= (1U << log2))
        return false;
    }
    len -= min2;
    const uint32_t min3 = static_cast<uint32_t>((len < num3) ? len : num3);
    for (uint32_t i = min1 + min2; i < min1 + min2 + min3; i++) {
      if ((n[i]) >= (1U << log3))
        return false;
    }
    return true;
  }

  static void bit_writer(uint32_t *out, const uint32_t value,
                         const uint32_t bits) {
    *out = (*out << bits) | value;
  }

  enum { SIMPLE16_LOGDESC = 4, SIMPLE16_LEN = (1U << SIMPLE16_LOGDESC) };

  static uint32_t which(const uint32_t *const in) {
    return (*in) >> (32 - SIMPLE16_LOGDESC);
  }
  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue);
  // like encodeArray, but does not actually write out the data
  void fakeencodeArray(const uint32_t *in, const size_t length, size_t &nvalue);

  const uint32_t *decodeArray(const uint32_t *in, const size_t len,
                              uint32_t *out, size_t &nvalue);
  std::string name() const { return "Simple16"; }
  Simple16() {}

  /* A set of unpacking functions */
  static void unpack1_28(uint32_t **out, const uint32_t **in);
  static void unpack2_7_1_14(uint32_t **out, const uint32_t **in);
  static void unpack1_7_2_7_1_7(uint32_t **out, const uint32_t **in);
  static void unpack1_14_2_7(uint32_t **out, const uint32_t **in);
  static void unpack2_14(uint32_t **out, const uint32_t **in);
  static void unpack4_1_3_8(uint32_t **out, const uint32_t **in);
  static void unpack3_1_4_4_3_3(uint32_t **out, const uint32_t **in);
  static void unpack4_7(uint32_t **out, const uint32_t **in);
  static void unpack5_4_4_2(uint32_t **out, const uint32_t **in);
  static void unpack4_2_5_4(uint32_t **out, const uint32_t **in);
  static void unpack6_3_5_2(uint32_t **out, const uint32_t **in);
  static void unpack5_2_6_3(uint32_t **out, const uint32_t **in);
  static void unpack7_4(uint32_t **out, const uint32_t **in);
  static void unpack10_1_9_2(uint32_t **out, const uint32_t **in);
  static void unpack14_2(uint32_t **out, const uint32_t **in);
  static void unpack28_1(uint32_t **out, const uint32_t **in);

  typedef void (*unpacker)(uint32_t **out, const uint32_t **in);

  static const unpacker unpackarray[SIMPLE16_LEN];
};

template <bool MarkLength>
const typename Simple16<MarkLength>::unpacker
    Simple16<MarkLength>::unpackarray[SIMPLE16_LEN] = {
        unpack1_28,    unpack2_7_1_14, unpack1_7_2_7_1_7, unpack1_14_2_7,
        unpack2_14,    unpack4_1_3_8,  unpack3_1_4_4_3_3, unpack4_7,
        unpack5_4_4_2, unpack4_2_5_4,  unpack6_3_5_2,     unpack5_2_6_3,
        unpack7_4,     unpack10_1_9_2, unpack14_2,        unpack28_1};

template <bool MarkLength>
void Simple16<MarkLength>::encodeArray(const uint32_t *in, const size_t length,
                                       uint32_t *out, size_t &nvalue) {
  uint32_t NumberOfValuesCoded;
#ifndef NDEBUG
  const uint32_t *const initin(in);
#endif
  const uint32_t *const initout(out);
  if (MarkLength)
    *(out++) = static_cast<uint32_t>(length);
  size_t ValuesRemaining(length);
  const bool becareful = false;
  while (ValuesRemaining >= 28) {
    if (trymefull<28, 1>(in)) {
      out[0] = 0;
      NumberOfValuesCoded = 28;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 1);
      *out <<= 28 - NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 0);
    } else if (trymefull<7, 2, 14, 1>(in)) {
      out[0] = 1;
      NumberOfValuesCoded = 7;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++) {
        bit_writer(out, *in++, 2);
      }
      const uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = 14;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++) {
        bit_writer(out, *in++, 1);
      }
      *out <<= 28 - (base * 2 + NumberOfValuesCoded);
      NumberOfValuesCoded += base;
      if (becareful)
        assert(which(out) == 1);
    } else if (trymefull<7, 1, 7, 2, 7, 1>(in)) {
      out[0] = 2;
      NumberOfValuesCoded = 7;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 1);
      uint32_t fill = NumberOfValuesCoded;
      uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = 7;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 2);
      fill += 2 * NumberOfValuesCoded;
      base += NumberOfValuesCoded;
      NumberOfValuesCoded = 7;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 1);
      fill += NumberOfValuesCoded;
      NumberOfValuesCoded += base;
      *out <<= 28 - fill;
      if (becareful)
        assert(which(out) == 2);
    } else if (trymefull<14, 1, 7, 2>(in)) {
      out[0] = 3;
      NumberOfValuesCoded = 14;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 1);
      const uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = 7;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 2);
      *out <<= 28 - base - 2 * NumberOfValuesCoded;
      NumberOfValuesCoded += base;
      if (becareful)
        assert(which(out) == 3);
    } else if (trymefull<14, 2>(in)) {
      out[0] = 4;
      NumberOfValuesCoded = 14;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 2);
      *out <<= 28 - 2 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 4);
    } else if (trymefull<1, 4, 8, 3>(in)) {
      out[0] = 5;
      NumberOfValuesCoded = 1;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 4);
      const uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = 8;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 3);
      *out <<= 28 - 4 * base - 3 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 5);
      NumberOfValuesCoded += base;
    } else if (trymefull<1, 3, 4, 4, 3, 3>(in)) {
      out[0] = 6;
      NumberOfValuesCoded = 1;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 3);
      uint32_t fill = 3 * NumberOfValuesCoded;
      uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = 4;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 4);
      base += NumberOfValuesCoded;
      fill += 4 * NumberOfValuesCoded;
      NumberOfValuesCoded = 3;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 3);
      fill += 3 * NumberOfValuesCoded;
      *out <<= 28 - fill;
      if (becareful)
        assert(which(out) == 6);
      NumberOfValuesCoded += base;
    } else if (trymefull<7, 4>(in)) {
      out[0] = 7;
      NumberOfValuesCoded = 7;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 4);
      *out <<= 28 - 4 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 7);
    } else if (trymefull<4, 5, 2, 4>(in)) {
      out[0] = 8;
      NumberOfValuesCoded = 4;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 5);
      const uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = 2;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 4);
      *out <<= 28 - 5 * base - 4 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 8);
      NumberOfValuesCoded += base;
    } else if (trymefull<2, 4, 4, 5>(in)) {
      out[0] = 9;
      NumberOfValuesCoded = 2;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 4);
      const uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = 4;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 5);
      *out <<= 28 - 4 * base - 5 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 9);
      NumberOfValuesCoded += base;
    } else if (trymefull<3, 6, 2, 5>(in)) {
      out[0] = 10;
      NumberOfValuesCoded = 3;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 6);
      const uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = 2;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 5);
      *out <<= 28 - 6 * base - 5 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 10);
      NumberOfValuesCoded += base;
    } else if (trymefull<2, 5, 3, 6>(in)) {
      out[0] = 11;
      NumberOfValuesCoded = 2;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 5);
      const uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = 3;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 6);
      *out <<= 28 - 5 * base - 6 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 11);
      NumberOfValuesCoded += base;
    } else if (trymefull<4, 7>(in)) {
      out[0] = 12;
      NumberOfValuesCoded = 4;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 7);
      *out <<= 28 - 7 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 12);
    } else if (trymefull<1, 10, 2, 9>(in)) {
      out[0] = 13;
      NumberOfValuesCoded = 1;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 10);
      const uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = 2;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 9);
      *out <<= 28 - 10 * base - 9 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 13);
      NumberOfValuesCoded += base;
    } else if (trymefull<2, 14>(in)) {
      out[0] = 14;
      NumberOfValuesCoded = 2;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 14);
      *out <<= 28 - 14 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 14);
    } else {
      if ((*in >> 28) > 0) {
        std::cerr << "Input's out of range: " << *in << std::endl;
        throw std::runtime_error(
            "You tried to apply Simple16 to an incompatible set of integers: they should be in [0,2^28).");
      }
      out[0] = 15;
      NumberOfValuesCoded = 1;
      bit_writer(out, *in++, 28);
      if (becareful)
        assert(which(out) == 15);
    }
    ++out;
    ValuesRemaining -= NumberOfValuesCoded;
  }

  while (ValuesRemaining > 0) {
    if (tryme<28, 1>(in, ValuesRemaining)) {
      out[0] = 0;
      NumberOfValuesCoded =
          (ValuesRemaining < 28) ? static_cast<uint32_t>(ValuesRemaining) : 28;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 1);
      *out <<= 28 - NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 0);
    } else if (tryme<7, 2, 14, 1>(in, ValuesRemaining)) {
      out[0] = 1;
      NumberOfValuesCoded =
          (ValuesRemaining < 7) ? static_cast<uint32_t>(ValuesRemaining) : 7;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++) {
        bit_writer(out, *in++, 2);
      }
      const uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = ((ValuesRemaining - base) < 14)
                                ? static_cast<uint32_t>(ValuesRemaining - base)
                                : 14;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++) {
        bit_writer(out, *in++, 1);
      }
      *out <<= 28 - (base * 2 + NumberOfValuesCoded);

      NumberOfValuesCoded += base;
      if (becareful)
        assert(which(out) == 1);
    } else if (tryme<7, 1, 7, 2, 7, 1>(in, ValuesRemaining)) {
      out[0] = 2;
      NumberOfValuesCoded =
          (ValuesRemaining < 7) ? static_cast<uint32_t>(ValuesRemaining) : 7;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 1);
      uint32_t fill = NumberOfValuesCoded;
      uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = (ValuesRemaining - base < 7)
                                ? static_cast<uint32_t>(ValuesRemaining - base)
                                : 7;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 2);
      fill += 2 * NumberOfValuesCoded;
      base += NumberOfValuesCoded;
      NumberOfValuesCoded = (ValuesRemaining - base < 7)
                                ? static_cast<uint32_t>(ValuesRemaining - base)
                                : 7;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 1);
      fill += NumberOfValuesCoded;
      NumberOfValuesCoded += base;
      *out <<= 28 - fill;
      if (becareful)
        assert(which(out) == 2);
    } else if (tryme<14, 1, 7, 2>(in, ValuesRemaining)) {
      out[0] = 3;
      NumberOfValuesCoded =
          (ValuesRemaining < 14) ? static_cast<uint32_t>(ValuesRemaining) : 14;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 1);
      const uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = ((ValuesRemaining - base) < 7)
                                ? static_cast<uint32_t>(ValuesRemaining - base)
                                : 7;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 2);
      *out <<= 28 - base - 2 * NumberOfValuesCoded;
      NumberOfValuesCoded += base;
      if (becareful)
        assert(which(out) == 3);
    } else if (tryme<14, 2>(in, ValuesRemaining)) {
      out[0] = 4;
      NumberOfValuesCoded =
          (ValuesRemaining < 14) ? static_cast<uint32_t>(ValuesRemaining) : 14;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 2);
      *out <<= 28 - 2 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 4);
    } else if (tryme<1, 4, 8, 3>(in, ValuesRemaining)) {
      out[0] = 5;
      NumberOfValuesCoded =
          (ValuesRemaining < 1) ? static_cast<uint32_t>(ValuesRemaining) : 1;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 4);
      const uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = ((ValuesRemaining - base) < 8)
                                ? static_cast<uint32_t>(ValuesRemaining - base)
                                : 8;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 3);
      *out <<= 28 - 4 * base - 3 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 5);
      NumberOfValuesCoded += base;
    } else if (tryme<1, 3, 4, 4, 3, 3>(in, ValuesRemaining)) {
      out[0] = 6;
      NumberOfValuesCoded = (ValuesRemaining < 1) ? ValuesRemaining : 1;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 3);
      uint32_t fill = 3 * NumberOfValuesCoded;
      uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = (ValuesRemaining - base < 4)
                                ? static_cast<uint32_t>(ValuesRemaining - base)
                                : 4;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 4);
      base += NumberOfValuesCoded;
      fill += 4 * NumberOfValuesCoded;
      NumberOfValuesCoded = (ValuesRemaining - base < 3)
                                ? static_cast<uint32_t>(ValuesRemaining - base)
                                : 3;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 3);
      fill += 3 * NumberOfValuesCoded;
      *out <<= 28 - fill;
      if (becareful)
        assert(which(out) == 6);
      NumberOfValuesCoded += base;
    } else if (tryme<7, 4>(in, ValuesRemaining)) {
      out[0] = 7;
      NumberOfValuesCoded =
          (ValuesRemaining < 7) ? static_cast<uint32_t>(ValuesRemaining) : 7;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 4);
      *out <<= 28 - 4 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 7);
    } else if (tryme<4, 5, 2, 4>(in, ValuesRemaining)) {
      out[0] = 8;
      NumberOfValuesCoded =
          (ValuesRemaining < 4) ? static_cast<uint32_t>(ValuesRemaining) : 4;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 5);
      const uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = ((ValuesRemaining - base) < 2)
                                ? static_cast<uint32_t>(ValuesRemaining - base)
                                : 2;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 4);
      *out <<= 28 - 5 * base - 4 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 8);
      NumberOfValuesCoded += base;
    } else if (tryme<2, 4, 4, 5>(in, ValuesRemaining)) {
      out[0] = 9;
      NumberOfValuesCoded =
          (ValuesRemaining < 2) ? static_cast<uint32_t>(ValuesRemaining) : 2;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 4);
      const uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = ((ValuesRemaining - base) < 4)
                                ? static_cast<uint32_t>(ValuesRemaining - base)
                                : 4;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 5);
      *out <<= 28 - 4 * base - 5 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 9);
      NumberOfValuesCoded += base;
    } else if (tryme<3, 6, 2, 5>(in, ValuesRemaining)) {
      out[0] = 10;
      NumberOfValuesCoded =
          (ValuesRemaining < 3) ? static_cast<uint32_t>(ValuesRemaining) : 3;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 6);
      const uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = ((ValuesRemaining - base) < 2)
                                ? static_cast<uint32_t>(ValuesRemaining - base)
                                : 2;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 5);
      *out <<= 28 - 6 * base - 5 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 10);
      NumberOfValuesCoded += base;
    } else if (tryme<2, 5, 3, 6>(in, ValuesRemaining)) {
      out[0] = 11;
      NumberOfValuesCoded =
          (ValuesRemaining < 2) ? static_cast<uint32_t>(ValuesRemaining) : 2;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 5);
      const uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = ((ValuesRemaining - base) < 3)
                                ? static_cast<uint32_t>(ValuesRemaining - base)
                                : 3;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 6);
      *out <<= 28 - 5 * base - 6 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 11);
      NumberOfValuesCoded += base;
    } else if (tryme<4, 7>(in, ValuesRemaining)) {
      out[0] = 12;
      NumberOfValuesCoded =
          (ValuesRemaining < 4) ? static_cast<uint32_t>(ValuesRemaining) : 4;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 7);
      *out <<= 28 - 7 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 12);
    } else if (tryme<1, 10, 2, 9>(in, ValuesRemaining)) {
      out[0] = 13;
      NumberOfValuesCoded = (ValuesRemaining < 1) ? ValuesRemaining : 1;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 10);
      const uint32_t base = NumberOfValuesCoded;
      NumberOfValuesCoded = ((ValuesRemaining - base) < 2)
                                ? static_cast<uint32_t>(ValuesRemaining - base)
                                : 2;
      for (uint32_t i = base; i < base + NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 9);
      *out <<= 28 - 10 * base - 9 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 13);
      NumberOfValuesCoded += base;
    } else if (tryme<2, 14>(in, ValuesRemaining)) {
      out[0] = 14;
      NumberOfValuesCoded =
          (ValuesRemaining < 2) ? static_cast<uint32_t>(ValuesRemaining) : 2;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 14);
      *out <<= 28 - 14 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 14);
    } else {
      if ((*in >> 28) > 0) {
        std::cerr << "Input's out of range: " << *in << std::endl;
        throw std::runtime_error(
            "You tried to apply Simple16 to an incompatible set of integers.");
      }
      out[0] = 15;
      NumberOfValuesCoded = 1;
      bit_writer(out, *in++, 28);
      if (becareful)
        assert(which(out) == 15);
    }
    ValuesRemaining -= NumberOfValuesCoded;
    ++out;
  }
  assert(ValuesRemaining == 0);
  assert(initin + length == in);
  nvalue = out - initout;
}

template <bool MarkLength>
void Simple16<MarkLength>::fakeencodeArray(const uint32_t *in,
                                           const size_t length,
                                           size_t &nvalue) {
  uint32_t NumberOfValuesCoded = 0;
  size_t writtenout = 0;
  size_t ValuesRemaining(length);
  while (ValuesRemaining > 0) {
    if (tryme<28, 1>(in, ValuesRemaining)) {
      NumberOfValuesCoded =
          (ValuesRemaining < 28) ? static_cast<uint32_t>(ValuesRemaining) : 28;
    } else if (tryme<7, 2, 14, 1>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 7 + 14)
                                ? static_cast<uint32_t>(ValuesRemaining)
                                : 7 + 14;
    } else if (tryme<7, 1, 7, 2, 7, 1>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 7 * 3)
                                ? static_cast<uint32_t>(ValuesRemaining)
                                : 7 * 3;
    } else if (tryme<14, 1, 7, 2>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 14 + 7)
                                ? static_cast<uint32_t>(ValuesRemaining)
                                : 14 + 7;
    } else if (tryme<14, 2>(in, ValuesRemaining)) {
      NumberOfValuesCoded =
          (ValuesRemaining < 14) ? static_cast<uint32_t>(ValuesRemaining) : 14;
    } else if (tryme<1, 4, 8, 3>(in, ValuesRemaining)) {
      NumberOfValuesCoded =
          (ValuesRemaining < 9) ? static_cast<uint32_t>(ValuesRemaining) : 9;
    } else if (tryme<1, 3, 4, 4, 3, 3>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 1 + 4 + 3)
                                ? static_cast<uint32_t>(ValuesRemaining)
                                : 1 + 4 + 3;
    } else if (tryme<7, 4>(in, ValuesRemaining)) {
      NumberOfValuesCoded =
          (ValuesRemaining < 7) ? static_cast<uint32_t>(ValuesRemaining) : 7;
    } else if (tryme<4, 5, 2, 4>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 4 + 2)
                                ? static_cast<uint32_t>(ValuesRemaining)
                                : 4 + 2;
    } else if (tryme<2, 4, 4, 5>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 2 + 4)
                                ? static_cast<uint32_t>(ValuesRemaining)
                                : 2 + 4;
    } else if (tryme<3, 6, 2, 5>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 3 + 2)
                                ? static_cast<uint32_t>(ValuesRemaining)
                                : 3 + 2;
    } else if (tryme<2, 5, 3, 6>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 2 + 3)
                                ? static_cast<uint32_t>(ValuesRemaining)
                                : 2 + 3;
    } else if (tryme<4, 7>(in, ValuesRemaining)) {
      NumberOfValuesCoded =
          (ValuesRemaining < 4) ? static_cast<uint32_t>(ValuesRemaining) : 4;
    } else if (tryme<1, 10, 2, 9>(in, ValuesRemaining)) {
      NumberOfValuesCoded =
          (ValuesRemaining < 3) ? static_cast<uint32_t>(ValuesRemaining) : 3;
    } else if (tryme<2, 14>(in, ValuesRemaining)) {
      NumberOfValuesCoded =
          (ValuesRemaining < 2) ? static_cast<uint32_t>(ValuesRemaining) : 2;
    } else {
      NumberOfValuesCoded = 1;
    }
    in += NumberOfValuesCoded;
    ValuesRemaining -= NumberOfValuesCoded;
    ++writtenout;
  }
  nvalue = writtenout;
}

template <bool MarkLength>
const uint32_t *Simple16<MarkLength>::decodeArray(const uint32_t *in,
#ifndef NDEBUG
                                                  const size_t len,
                                                  uint32_t *out,
                                                  size_t &nvalue) {
#else
                                                  const size_t, uint32_t *out,
                                                  size_t &nvalue) {
#endif
#ifndef NDEBUG
  const uint32_t *const endin = in + len;
#endif
  if (MarkLength)
    if ((*in) > nvalue)
      throw NotEnoughStorage(*in);
  const uint32_t actualvalue =
      MarkLength ? *(in++) : static_cast<uint32_t>(nvalue);
  if (nvalue < actualvalue)
    std::cerr << " possible overrun" << std::endl;
  nvalue = actualvalue;
#ifdef STATS
  cout << "simple16 decode " << len << endl;
  vector<uint32_t> stats(16, 0);
#endif
  const uint32_t *const end = out + nvalue;
  while (end > out) {
#ifdef STATS
    stats[which(in)]++;
#endif
    (unpackarray[which(in)])(&out, &in);
  }

#ifdef STATS
  uint32_t sum = std::accumulate(stats.begin(), stats.end(), 0);
  for (uint32_t k = 0; k < stats.size(); ++k) {
    cout << "simple16 stats[" << k << "]=" << stats[k] * 1.0 / sum << endl;
  }
#endif
  ASSERT(in <= endin, in - endin);
  return in;
}

/* --- Intra functions below --- */

template <bool MarkLength>
void Simple16<MarkLength>::unpack1_28(uint32_t **out, const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 27) & 0x01;
  pout[1] = (pin[0] >> 26) & 0x01;
  pout[2] = (pin[0] >> 25) & 0x01;
  pout[3] = (pin[0] >> 24) & 0x01;
  pout[4] = (pin[0] >> 23) & 0x01;
  pout[5] = (pin[0] >> 22) & 0x01;
  pout[6] = (pin[0] >> 21) & 0x01;
  pout[7] = (pin[0] >> 20) & 0x01;
  pout[8] = (pin[0] >> 19) & 0x01;
  pout[9] = (pin[0] >> 18) & 0x01;
  pout[10] = (pin[0] >> 17) & 0x01;
  pout[11] = (pin[0] >> 16) & 0x01;
  pout[12] = (pin[0] >> 15) & 0x01;
  pout[13] = (pin[0] >> 14) & 0x01;
  pout[14] = (pin[0] >> 13) & 0x01;
  pout[15] = (pin[0] >> 12) & 0x01;
  pout[16] = (pin[0] >> 11) & 0x01;
  pout[17] = (pin[0] >> 10) & 0x01;
  pout[18] = (pin[0] >> 9) & 0x01;
  pout[19] = (pin[0] >> 8) & 0x01;
  pout[20] = (pin[0] >> 7) & 0x01;
  pout[21] = (pin[0] >> 6) & 0x01;
  pout[22] = (pin[0] >> 5) & 0x01;
  pout[23] = (pin[0] >> 4) & 0x01;
  pout[24] = (pin[0] >> 3) & 0x01;
  pout[25] = (pin[0] >> 2) & 0x01;
  pout[26] = (pin[0] >> 1) & 0x01;
  pout[27] = pin[0] & 0x01;

  *in = pin + 1;
  *out = pout + 28;
}

template <bool MarkLength>
void Simple16<MarkLength>::unpack2_7_1_14(uint32_t **out, const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 26) & 0x03;
  pout[1] = (pin[0] >> 24) & 0x03;
  pout[2] = (pin[0] >> 22) & 0x03;
  pout[3] = (pin[0] >> 20) & 0x03;
  pout[4] = (pin[0] >> 18) & 0x03;
  pout[5] = (pin[0] >> 16) & 0x03;
  pout[6] = (pin[0] >> 14) & 0x03;

  pout[7] = (pin[0] >> 13) & 0x01;
  pout[8] = (pin[0] >> 12) & 0x01;
  pout[9] = (pin[0] >> 11) & 0x01;
  pout[10] = (pin[0] >> 10) & 0x01;
  pout[11] = (pin[0] >> 9) & 0x01;
  pout[12] = (pin[0] >> 8) & 0x01;
  pout[13] = (pin[0] >> 7) & 0x01;
  pout[14] = (pin[0] >> 6) & 0x01;
  pout[15] = (pin[0] >> 5) & 0x01;
  pout[16] = (pin[0] >> 4) & 0x01;
  pout[17] = (pin[0] >> 3) & 0x01;
  pout[18] = (pin[0] >> 2) & 0x01;
  pout[19] = (pin[0] >> 1) & 0x01;
  pout[20] = pin[0] & 0x01;

  *in = pin + 1;
  *out = pout + 21;
}

template <bool MarkLength>
void Simple16<MarkLength>::unpack1_7_2_7_1_7(uint32_t **out,
                                             const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 27) & 0x01;
  pout[1] = (pin[0] >> 26) & 0x01;
  pout[2] = (pin[0] >> 25) & 0x01;
  pout[3] = (pin[0] >> 24) & 0x01;
  pout[4] = (pin[0] >> 23) & 0x01;
  pout[5] = (pin[0] >> 22) & 0x01;
  pout[6] = (pin[0] >> 21) & 0x01;

  pout[7] = (pin[0] >> 19) & 0x03;
  pout[8] = (pin[0] >> 17) & 0x03;
  pout[9] = (pin[0] >> 15) & 0x03;
  pout[10] = (pin[0] >> 13) & 0x03;
  pout[11] = (pin[0] >> 11) & 0x03;
  pout[12] = (pin[0] >> 9) & 0x03;
  pout[13] = (pin[0] >> 7) & 0x03;

  pout[14] = (pin[0] >> 6) & 0x01;
  pout[15] = (pin[0] >> 5) & 0x01;
  pout[16] = (pin[0] >> 4) & 0x01;
  pout[17] = (pin[0] >> 3) & 0x01;
  pout[18] = (pin[0] >> 2) & 0x01;
  pout[19] = (pin[0] >> 1) & 0x01;
  pout[20] = pin[0] & 0x01;

  *in = pin + 1;
  *out = pout + 21;
}

template <bool MarkLength>
void Simple16<MarkLength>::unpack1_14_2_7(uint32_t **out, const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 27) & 0x01;
  pout[1] = (pin[0] >> 26) & 0x01;
  pout[2] = (pin[0] >> 25) & 0x01;
  pout[3] = (pin[0] >> 24) & 0x01;
  pout[4] = (pin[0] >> 23) & 0x01;
  pout[5] = (pin[0] >> 22) & 0x01;
  pout[6] = (pin[0] >> 21) & 0x01;
  pout[7] = (pin[0] >> 20) & 0x01;
  pout[8] = (pin[0] >> 19) & 0x01;
  pout[9] = (pin[0] >> 18) & 0x01;
  pout[10] = (pin[0] >> 17) & 0x01;
  pout[11] = (pin[0] >> 16) & 0x01;
  pout[12] = (pin[0] >> 15) & 0x01;
  pout[13] = (pin[0] >> 14) & 0x01;

  pout[14] = (pin[0] >> 12) & 0x03;
  pout[15] = (pin[0] >> 10) & 0x03;
  pout[16] = (pin[0] >> 8) & 0x03;
  pout[17] = (pin[0] >> 6) & 0x03;
  pout[18] = (pin[0] >> 4) & 0x03;
  pout[19] = (pin[0] >> 2) & 0x03;
  pout[20] = pin[0] & 0x03;

  *in = pin + 1;
  *out = pout + 21;
}

template <bool MarkLength>
void Simple16<MarkLength>::unpack2_14(uint32_t **out, const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 26) & 0x03;
  pout[1] = (pin[0] >> 24) & 0x03;
  pout[2] = (pin[0] >> 22) & 0x03;
  pout[3] = (pin[0] >> 20) & 0x03;
  pout[4] = (pin[0] >> 18) & 0x03;
  pout[5] = (pin[0] >> 16) & 0x03;
  pout[6] = (pin[0] >> 14) & 0x03;
  pout[7] = (pin[0] >> 12) & 0x03;
  pout[8] = (pin[0] >> 10) & 0x03;
  pout[9] = (pin[0] >> 8) & 0x03;
  pout[10] = (pin[0] >> 6) & 0x03;
  pout[11] = (pin[0] >> 4) & 0x03;
  pout[12] = (pin[0] >> 2) & 0x03;
  pout[13] = pin[0] & 0x03;

  *in = pin + 1;
  *out = pout + 14;
}

template <bool MarkLength>
void Simple16<MarkLength>::unpack4_1_3_8(uint32_t **out, const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 24) & 0x0f;

  pout[1] = (pin[0] >> 21) & 0x07;
  pout[2] = (pin[0] >> 18) & 0x07;
  pout[3] = (pin[0] >> 15) & 0x07;
  pout[4] = (pin[0] >> 12) & 0x07;
  pout[5] = (pin[0] >> 9) & 0x07;
  pout[6] = (pin[0] >> 6) & 0x07;
  pout[7] = (pin[0] >> 3) & 0x07;
  pout[8] = pin[0] & 0x07;

  *in = pin + 1;
  *out = pout + 9;
}

template <bool MarkLength>
void Simple16<MarkLength>::unpack3_1_4_4_3_3(uint32_t **out,
                                             const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 25) & 0x07;

  pout[1] = (pin[0] >> 21) & 0x0f;
  pout[2] = (pin[0] >> 17) & 0x0f;
  pout[3] = (pin[0] >> 13) & 0x0f;
  pout[4] = (pin[0] >> 9) & 0x0f;

  pout[5] = (pin[0] >> 6) & 0x07;
  pout[6] = (pin[0] >> 3) & 0x07;
  pout[7] = pin[0] & 0x07;

  *in = pin + 1;
  *out = pout + 8;
}

template <bool MarkLength>
void Simple16<MarkLength>::unpack4_7(uint32_t **out, const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 24) & 0x0f;
  pout[1] = (pin[0] >> 20) & 0x0f;
  pout[2] = (pin[0] >> 16) & 0x0f;
  pout[3] = (pin[0] >> 12) & 0x0f;
  pout[4] = (pin[0] >> 8) & 0x0f;
  pout[5] = (pin[0] >> 4) & 0x0f;
  pout[6] = pin[0] & 0x0f;

  *in = pin + 1;
  *out = pout + 7;
}

template <bool MarkLength>
void Simple16<MarkLength>::unpack5_4_4_2(uint32_t **out, const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 23) & 0x1f;
  pout[1] = (pin[0] >> 18) & 0x1f;
  pout[2] = (pin[0] >> 13) & 0x1f;
  pout[3] = (pin[0] >> 8) & 0x1f;

  pout[4] = (pin[0] >> 4) & 0x0f;
  pout[5] = pin[0] & 0x0f;

  *in = pin + 1;
  *out = pout + 6;
}

template <bool MarkLength>
void Simple16<MarkLength>::unpack4_2_5_4(uint32_t **out, const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 24) & 0x0f;
  pout[1] = (pin[0] >> 20) & 0x0f;

  pout[2] = (pin[0] >> 15) & 0x1f;
  pout[3] = (pin[0] >> 10) & 0x1f;
  pout[4] = (pin[0] >> 5) & 0x1f;
  pout[5] = pin[0] & 0x1f;

  *in = pin + 1;
  *out = pout + 6;
}

template <bool MarkLength>
void Simple16<MarkLength>::unpack6_3_5_2(uint32_t **out, const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 22) & 0x3f;
  pout[1] = (pin[0] >> 16) & 0x3f;
  pout[2] = (pin[0] >> 10) & 0x3f;

  pout[3] = (pin[0] >> 5) & 0x1f;
  pout[4] = pin[0] & 0x1f;

  *in = pin + 1;
  *out = pout + 5;
}

template <bool MarkLength>
void Simple16<MarkLength>::unpack5_2_6_3(uint32_t **out, const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 23) & 0x1f;
  pout[1] = (pin[0] >> 18) & 0x1f;

  pout[2] = (pin[0] >> 12) & 0x3f;
  pout[3] = (pin[0] >> 6) & 0x3f;
  pout[4] = pin[0] & 0x3f;

  *in = pin + 1;
  *out = pout + 5;
}

template <bool MarkLength>
void Simple16<MarkLength>::unpack7_4(uint32_t **out, const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 21) & 0x7f;
  pout[1] = (pin[0] >> 14) & 0x7f;
  pout[2] = (pin[0] >> 7) & 0x7f;
  pout[3] = pin[0] & 0x7f;

  *in = pin + 1;
  *out = pout + 4;
}

template <bool MarkLength>
void Simple16<MarkLength>::unpack10_1_9_2(uint32_t **out, const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 18) & 0x03ff;

  pout[1] = (pin[0] >> 9) & 0x01ff;
  pout[2] = pin[0] & 0x01ff;

  *in = pin + 1;
  *out = pout + 3;
}

template <bool MarkLength>
void Simple16<MarkLength>::unpack14_2(uint32_t **out, const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 14) & 0x3fff;
  pout[1] = pin[0] & 0x3fff;

  *in = pin + 1;
  *out = pout + 2;
}

template <bool MarkLength>
void Simple16<MarkLength>::unpack28_1(uint32_t **out, const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = pin[0] & 0x0fffffff;

  *in = pin + 1;
  *out = pout + 1;
}

} // namespace FastPFor

#endif /* SIMPLE16_H_ */
