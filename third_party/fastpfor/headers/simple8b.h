/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#ifndef SIMPLE8B_H_
#define SIMPLE8B_H_

#include "common.h"
#include "codecs.h"

namespace FastPForLib {

/**
 * Follows Vo Ngoc Anh, Alistair Moffat: Index compression using 64-bit words.
 *  Softw., Pract. Exper. 40(2): 131-147 (2010)
 *
 *  Implemented by D. Lemire
 *
 */
/**
 * If MarkLength is true, than the number of symbols is written
 * in the stream. Otherwise you need to specify it using the nvalue
 * parameter decodeArray.
 *
 * Note that when MarkLength is false, some unaligned (64-bit vs. 32-bit)
 * access are possible. This may fail on non-x86 platforms.
 */
template <bool MarkLength> class Simple8b : public IntegerCODEC {
public:
  template <uint32_t num1, uint32_t log1>
  static bool tryme(const uint32_t *n, size_t len) {
    if (log1 >= 32)
      return true;
    const uint32_t minv =
        (static_cast<uint32_t>(len) < num1) ? static_cast<uint32_t>(len) : num1;
    for (uint32_t i = 0; i < minv; i++) {
      if (n[i] >= (1U << (log1 % 32)))
        return false;
    }
    return true;
  }
  template <uint32_t num1, uint32_t log1>
  static bool trymefull(const uint32_t *n) {
    if (log1 >= 32)
      return true;
    for (uint32_t i = 0; i < num1; i++) {
      if (n[i] >= (1U << (log1 % 32)))
        return false;
    }
    return true;
  }
  static void bit_writer(uint64_t *out, const uint32_t value,
                         const uint32_t bits) {
    *out = (*out << bits) | value;
  }

  enum { SIMPLE8B_LOGDESC = 4, SIMPLE8B_LEN = (1U << SIMPLE8B_LOGDESC) };

  static uint32_t which(const uint64_t *const in) {
    return static_cast<uint32_t>((*in) >> (64 - SIMPLE8B_LOGDESC));
  }
  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue);

  // like encode Array, but does not write the data
  void fakeencodeArray(const uint32_t *in, const size_t length, size_t &nvalue);
  /*
   * we allow a buffer overrun of 240 elements, so out should be
   * able to accomodate the full uncompress data plus 240 elements.
   */
  const uint32_t *decodeArray(const uint32_t *in, const size_t len,
                              uint32_t *out, size_t &nvalue);
  std::string name() const { return "Simple8b"; }
  Simple8b() {}

  template <uint32_t num1, uint32_t log1>
  static void unpack(uint32_t *&out, const uint64_t *&in) {
    if (log1 < 32) {
      const uint32_t actuallog1 =
          log1 < 32 ? log1 : 31; // bogus to silence compiler warnings
      const uint32_t mask = ((static_cast<uint32_t>(1) << actuallog1) - 1);
      for (uint32_t k = 0; k < num1; ++k) {
        *(out++) = static_cast<uint32_t>(
                       in[0] >> (64 - SIMPLE8B_LOGDESC - log1 - k * log1)) &
                   mask;
      }
    } else {
      for (uint32_t k = 0; k < num1; ++k) {
        *(out++) = static_cast<uint32_t>(
            in[0] >> (64 - SIMPLE8B_LOGDESC - log1 - k * log1));
      }
    }
    ++in;
  }
  template <uint32_t log1>
  static void carefulunpack(uint32_t num1, uint32_t *&out,
                            const uint64_t *&in) {
    if (log1 < 32) {
      const uint32_t actuallog1 =
          log1 < 32 ? log1 : 31; // bogus to silence compiler warnings
      const uint32_t mask = ((static_cast<uint32_t>(1) << actuallog1) - 1);
      for (uint32_t k = 0; k < num1; ++k) {
        *(out++) = static_cast<uint32_t>(
                       in[0] >> (64 - SIMPLE8B_LOGDESC - log1 - k * log1)) &
                   mask;
      }
    } else {
      for (uint32_t k = 0; k < num1; ++k) {
        *(out++) = static_cast<uint32_t>(
            in[0] >> (64 - SIMPLE8B_LOGDESC - log1 - k * log1));
      }
    }
    ++in;
  }
};

template <bool MarkLength>
void Simple8b<MarkLength>::encodeArray(const uint32_t *in, const size_t length,
                                       uint32_t *out, size_t &nvalue) {
#ifndef NDEBUG
  const uint32_t *const initin(in);
#endif
  uint32_t NumberOfValuesCoded = 0;
  const uint32_t *const initout(out);
  if (MarkLength) {
    *out = static_cast<uint32_t>(length);
    ++out;
  }
  // this may lead to unaligned access. Performance may be affected.
  // not much of an effect in practice on recent Intel processors.
  uint64_t *out64 = reinterpret_cast<uint64_t *>(out);
  size_t ValuesRemaining(length);
  // precompute
  const bool becareful(false);
  while (ValuesRemaining >= 240) {
    if (trymefull<120, 0>(in)) {
      if (trymefull<120, 0>(in + 120)) {
        NumberOfValuesCoded = 240;
        out64[0] = 0;
        in += NumberOfValuesCoded;
        if (becareful)
          assert(which(out64) == 0);

      } else {
        NumberOfValuesCoded = 120;
        out64[0] = 1ULL << (64 - SIMPLE8B_LOGDESC);
        in += NumberOfValuesCoded;
        if (becareful)
          assert(which(out64) == 1);
      }
    } else if (trymefull<60, 1>(in)) {
      out64[0] = 2;
      NumberOfValuesCoded = 60;
      for (uint32_t i = 0; i < 60; i++) {
        bit_writer(out64, *in++, 1);
      }
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 1 * 60;
      if (becareful)
        assert(which(out64) == 2);
    } else if (trymefull<30, 2>(in)) {
      out64[0] = 3;
      NumberOfValuesCoded = 30;
      for (uint32_t i = 0; i < 30; i++)
        bit_writer(out64, *in++, 2);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 2 * 30;
      if (becareful)
        assert(which(out64) == 3);
    } else if (trymefull<20, 3>(in)) {
      out64[0] = 4;
      NumberOfValuesCoded = 20;
      for (uint32_t i = 0; i < 20; i++)
        bit_writer(out64, *in++, 3);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 3 * 20;
      if (becareful)
        assert(which(out64) == 4);
    } else if (trymefull<15, 4>(in)) {
      out64[0] = 5;
      NumberOfValuesCoded = 15;
      for (uint32_t i = 0; i < 15; i++)
        bit_writer(out64, *in++, 4);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 4 * 15;
      if (becareful)
        assert(which(out64) == 5);
    } else if (trymefull<12, 5>(in)) {
      out64[0] = 6;
      NumberOfValuesCoded = 12;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 5);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 5 * 12;
      if (becareful)
        assert(which(out64) == 6);
    } else if (trymefull<10, 6>(in)) {
      out64[0] = 7;
      NumberOfValuesCoded = 10;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 6);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 6 * 10;
      if (becareful)
        assert(which(out64) == 7);
    } else if (trymefull<8, 7>(in)) {
      out64[0] = 8;
      NumberOfValuesCoded = 8;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 7);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 7 * 8;
      if (becareful)
        assert(which(out64) == 8);
    } else if (trymefull<7, 8>(in)) {
      out64[0] = 9;
      NumberOfValuesCoded = 7;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 8);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 8 * 7;
      if (becareful)
        assert(which(out64) == 9);
    } else if (trymefull<6, 10>(in)) {
      out64[0] = 10;
      NumberOfValuesCoded = 6;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 10);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 10 * 6;
      if (becareful)
        assert(which(out64) == 10);
    } else if (trymefull<5, 12>(in)) {
      out64[0] = 11;
      NumberOfValuesCoded =
          (ValuesRemaining < 5) ? static_cast<uint32_t>(ValuesRemaining) : 5;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 12);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 12 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 11);
    } else if (trymefull<4, 15>(in)) {
      out64[0] = 12;
      NumberOfValuesCoded = 4;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 15);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 15 * 4;
      if (becareful)
        assert(which(out64) == 12);
    } else if (trymefull<3, 20>(in)) {
      out64[0] = 13;
      NumberOfValuesCoded = 3;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 20);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 20 * 3;
      if (becareful)
        assert(which(out64) == 13);
    } else if (trymefull<2, 30>(in)) {
      out64[0] = 14;
      NumberOfValuesCoded = 2;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 30);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 30 * 2;
      if (becareful)
        assert(which(out64) == 14);
    } else if (trymefull<1, 60>(in)) {
      out64[0] = 15;
      NumberOfValuesCoded = 1;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 60);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 60 * 1;
      if (becareful)
        assert(which(out64) == 15);
    } else {
      throw std::logic_error("shouldn't happen");
    }
    ++out64;

    ValuesRemaining -= NumberOfValuesCoded;
  }

  while (ValuesRemaining > 0) {
    if (tryme<240, 0>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 240)
                                ? static_cast<uint32_t>(ValuesRemaining)
                                : 240;
      out64[0] = 0;
      in += NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 0);
    } else if (tryme<120, 0>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 120)
                                ? static_cast<uint32_t>(ValuesRemaining)
                                : 120;
      out64[0] = 1ULL << (64 - SIMPLE8B_LOGDESC);
      in += NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 1);
    } else if (tryme<60, 1>(in, ValuesRemaining)) {
      out64[0] = 2;
      NumberOfValuesCoded =
          (ValuesRemaining < 60) ? static_cast<uint32_t>(ValuesRemaining) : 60;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++) {
        bit_writer(out64, *in++, 1);
      }
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 1 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 2);
    } else if (tryme<30, 2>(in, ValuesRemaining)) {
      out64[0] = 3;
      NumberOfValuesCoded =
          (ValuesRemaining < 30) ? static_cast<uint32_t>(ValuesRemaining) : 30;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 2);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 2 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 3);
    } else if (tryme<20, 3>(in, ValuesRemaining)) {
      out64[0] = 4;
      NumberOfValuesCoded =
          (ValuesRemaining < 20) ? static_cast<uint32_t>(ValuesRemaining) : 20;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 3);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 3 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 4);
    } else if (tryme<15, 4>(in, ValuesRemaining)) {
      out64[0] = 5;
      NumberOfValuesCoded =
          (ValuesRemaining < 15) ? static_cast<uint32_t>(ValuesRemaining) : 15;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 4);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 4 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 5);
    } else if (tryme<12, 5>(in, ValuesRemaining)) {
      out64[0] = 6;
      NumberOfValuesCoded =
          (ValuesRemaining < 12) ? static_cast<uint32_t>(ValuesRemaining) : 12;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 5);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 5 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 6);
    } else if (tryme<10, 6>(in, ValuesRemaining)) {
      out64[0] = 7;
      NumberOfValuesCoded =
          (ValuesRemaining < 10) ? static_cast<uint32_t>(ValuesRemaining) : 10;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 6);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 6 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 7);
    } else if (tryme<8, 7>(in, ValuesRemaining)) {
      out64[0] = 8;
      NumberOfValuesCoded =
          (ValuesRemaining < 8) ? static_cast<uint32_t>(ValuesRemaining) : 8;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 7);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 7 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 8);
    } else if (tryme<7, 8>(in, ValuesRemaining)) {
      out64[0] = 9;
      NumberOfValuesCoded =
          (ValuesRemaining < 7) ? static_cast<uint32_t>(ValuesRemaining) : 7;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 8);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 8 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 9);
    } else if (tryme<6, 10>(in, ValuesRemaining)) {
      out64[0] = 10;
      NumberOfValuesCoded =
          (ValuesRemaining < 6) ? static_cast<uint32_t>(ValuesRemaining) : 6;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 10);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 10 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 10);
    } else if (tryme<5, 12>(in, ValuesRemaining)) {
      out64[0] = 11;
      NumberOfValuesCoded =
          (ValuesRemaining < 5) ? static_cast<uint32_t>(ValuesRemaining) : 5;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 12);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 12 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 11);
    } else if (tryme<4, 15>(in, ValuesRemaining)) {
      out64[0] = 12;
      NumberOfValuesCoded =
          (ValuesRemaining < 4) ? static_cast<uint32_t>(ValuesRemaining) : 4;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 15);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 15 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 12);
    } else if (tryme<3, 20>(in, ValuesRemaining)) {
      out64[0] = 13;
      NumberOfValuesCoded =
          (ValuesRemaining < 3) ? static_cast<uint32_t>(ValuesRemaining) : 3;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 20);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 20 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 13);
    } else if (tryme<2, 30>(in, ValuesRemaining)) {
      out64[0] = 14;
      NumberOfValuesCoded =
          (ValuesRemaining < 2) ? static_cast<uint32_t>(ValuesRemaining) : 2;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 30);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 30 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 14);
    } else if (tryme<1, 60>(in, ValuesRemaining)) {
      out64[0] = 15;
      NumberOfValuesCoded = (ValuesRemaining < 1) ? ValuesRemaining : 1;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out64, *in++, 60);
      out64[0] <<= 64 - SIMPLE8B_LOGDESC - 60 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out64) == 15);
    } else {
      throw std::logic_error("shouldn't happen");
    }
    if (becareful)
      ASSERT(initin + length - ValuesRemaining + NumberOfValuesCoded == in,
             which(out64));
    ++out64;

    ValuesRemaining -= NumberOfValuesCoded;
  }
  assert(initin + length == in);
  nvalue = reinterpret_cast<uint32_t *>(out64) - initout;
}

template <bool MarkLength>
void Simple8b<MarkLength>::fakeencodeArray(const uint32_t *in,
                                           const size_t length,
                                           size_t &nvalue) {
  uint32_t NumberOfValuesCoded = 0;
  size_t writtenout = 0;
  size_t ValuesRemaining(length);
  while (ValuesRemaining > 0) {
    if (tryme<240, 0>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 240) ? ValuesRemaining : 240;
    } else if (tryme<120, 0>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 120) ? ValuesRemaining : 120;
    } else if (tryme<60, 1>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 60) ? ValuesRemaining : 60;
    } else if (tryme<30, 2>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 30) ? ValuesRemaining : 30;
    } else if (tryme<20, 3>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 20) ? ValuesRemaining : 20;
    } else if (tryme<15, 4>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 15) ? ValuesRemaining : 15;
    } else if (tryme<12, 5>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 12) ? ValuesRemaining : 12;
    } else if (tryme<10, 6>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 10) ? ValuesRemaining : 10;
    } else if (tryme<8, 7>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 8) ? ValuesRemaining : 8;
    } else if (tryme<7, 8>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 7) ? ValuesRemaining : 7;
    } else if (tryme<6, 10>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 6) ? ValuesRemaining : 6;
    } else if (tryme<5, 12>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 5) ? ValuesRemaining : 5;
    } else if (tryme<4, 15>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 4) ? ValuesRemaining : 4;
    } else if (tryme<3, 20>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 3) ? ValuesRemaining : 3;
    } else if (tryme<2, 30>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 2) ? ValuesRemaining : 2;
    } else if (tryme<1, 60>(in, ValuesRemaining)) {
      NumberOfValuesCoded = (ValuesRemaining < 1) ? ValuesRemaining : 1;
    } else {
      throw std::logic_error("shouldn't happen");
    }
    in += NumberOfValuesCoded;
    ValuesRemaining -= NumberOfValuesCoded;
    ++writtenout;
  }
  nvalue = writtenout * 2;
}

template <bool MarkLength>
const uint32_t *Simple8b<MarkLength>::decodeArray(const uint32_t *in,
#ifndef NDEBUG
                                                  const size_t len,
                                                  uint32_t *out,
                                                  size_t &nvalue) {
#else
                                                  const size_t, uint32_t *out,
                                                  size_t &nvalue) {
#endif
#ifndef NDEBUG
  // const uint32_t * const initin(in);
  const uint32_t *const endin(in + len);
#endif
#ifdef STATS
  vector<uint32_t> stats(16, 0);
#endif
  uint32_t markednvalue;
  if (MarkLength) {
    markednvalue = *in++;
    if (markednvalue > nvalue)
      throw NotEnoughStorage(markednvalue);
  }
  const size_t actualvalue = MarkLength ? markednvalue : nvalue;
  // this may lead to unaligned access. Performance may be affected.
  // not much of an effect in practice on recent Intel processors.
  const uint64_t *in64 = reinterpret_cast<const uint64_t *>(in);
#ifndef NDEBUG
  const uint64_t *finalin64 = reinterpret_cast<const uint64_t *>(endin);
#endif

  if (nvalue < actualvalue)
    std::cerr << " possible overrun" << std::endl;
  nvalue = actualvalue;
  const uint32_t *const end = out + nvalue;
  const uint32_t *const initout(out);
  while (end > out + 240) {
#ifdef STATS
    stats[which(in64)]++;
#endif
    switch (which(in64)) {
    case 0:
      unpack<240, 0>(out, in64);
      break;
    case 1:
      unpack<120, 0>(out, in64);
      break;
    case 2:
      unpack<60, 1>(out, in64);
      break;
    case 3:
      unpack<30, 2>(out, in64);
      break;
    case 4:
      unpack<20, 3>(out, in64);
      break;
    case 5:
      unpack<15, 4>(out, in64);
      break;
    case 6:
      unpack<12, 5>(out, in64);
      break;
    case 7:
      unpack<10, 6>(out, in64);
      break;
    case 8:
      unpack<8, 7>(out, in64);
      break;
    case 9:
      unpack<7, 8>(out, in64);
      break;
    case 10:
      unpack<6, 10>(out, in64);
      break;
    case 11:
      unpack<5, 12>(out, in64);
      break;
    case 12:
      unpack<4, 15>(out, in64);
      break;
    case 13:
      unpack<3, 20>(out, in64);
      break;
    case 14:
      unpack<2, 30>(out, in64);
      break;
    case 15:
      unpack<1, 60>(out, in64);
      break;
    default:
      throw std::logic_error("hmmm how could that happen?");
    }
  }
  while (end > out) {
#ifdef STATS
    stats[which(in64)]++;
#endif
    switch (which(in64)) {
    case 0:
      carefulunpack<0>(std::min<uint32_t>(static_cast<uint32_t>(end - out), 240), out, in64);
      break;
    case 1:
      carefulunpack<0>(std::min<uint32_t>(static_cast<uint32_t>(end - out), 120), out, in64);
      break;
    case 2:
      carefulunpack<1>(std::min<uint32_t>(static_cast<uint32_t>(end - out), 60), out, in64);
      break;
    case 3:
      carefulunpack<2>(std::min<uint32_t>(static_cast<uint32_t>(end - out), 30), out, in64);
      break;
    case 4:
      carefulunpack<3>(std::min<uint32_t>(static_cast<uint32_t>(end - out), 20), out, in64);
      break;
    case 5:
      carefulunpack<4>(std::min<uint32_t>(static_cast<uint32_t>(end - out), 15), out, in64);
      break;
    case 6:
      carefulunpack<5>(std::min<uint32_t>(static_cast<uint32_t>(end - out), 12), out, in64);
      break;
    case 7:
      carefulunpack<6>(std::min<uint32_t>(static_cast<uint32_t>(end - out), 10), out, in64);
      break;
    case 8:
      carefulunpack<7>(std::min<uint32_t>(static_cast<uint32_t>(end - out), 8), out, in64);
      break;
    case 9:
      carefulunpack<8>(std::min<uint32_t>(static_cast<uint32_t>(end - out), 7), out, in64);
      break;
    case 10:
      carefulunpack<10>(std::min<uint32_t>(static_cast<uint32_t>(end - out), 6), out, in64);
      break;
    case 11:
      carefulunpack<12>(std::min<uint32_t>(static_cast<uint32_t>(end - out), 5), out, in64);
      break;
    case 12:
      carefulunpack<15>(std::min<uint32_t>(static_cast<uint32_t>(end - out), 4), out, in64);
      break;
    case 13:
      carefulunpack<20>(std::min<uint32_t>(static_cast<uint32_t>(end - out), 3), out, in64);
      break;
    case 14:
      carefulunpack<30>(std::min<uint32_t>(static_cast<uint32_t>(end - out), 2), out, in64);
      break;
    case 15:
      carefulunpack<60>(1, out, in64);
      break;
    default:
      throw std::logic_error("hmmm how could that happen?");
    }
  }

#ifdef STATS
  uint32_t sum = std::accumulate(stats.begin(), stats.end(), 0);
  for (uint32_t k = 0; k < stats.size(); ++k) {
    cout << "simple8b stats[" << k << "]=" << stats[k] * 1.0 / sum << std::endl;
  }
#endif
  assert(in64 <= finalin64);
  in = reinterpret_cast<const uint32_t *>(in64);
  assert(in <= endin);
  // check that we don't overrun the buffer too much?
  ASSERT(out < end + 240, out - end);
  nvalue = MarkLength ? actualvalue : out - initout;
  return in;
}

} // namespace FastPFor

#endif /* SIMPLE8B_H_ */
