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
#ifndef SIMPLE9_H_
#define SIMPLE9_H_

#include "common.h"
#include "codecs.h"

namespace FastPForLib {

/**
 * If MarkLength is true, than the number of symbols is written
 * in the stream. Otherwise you need to specify it using the nvalue
 * parameter decodeArray.
 */
template <bool MarkLength, bool hacked = false>
class Simple9 : public IntegerCODEC {
public:
  template <uint32_t num1, uint32_t log1>
  static bool trymefull(const uint32_t *n) {
    for (uint32_t i = 0; i < num1; i++) {
      if ((n[i]) >= (1U << log1))
        return false;
    }
    return true;
  }
  template <uint32_t num1, uint32_t log1>
  static bool tryme(const uint32_t *n, size_t len) {
    const uint32_t min = (len < num1) ? len : num1;
    for (uint32_t i = 0; i < min; i++) {
      if ((n[i]) >= (1U << log1))
        return false;
    }
    return true;
  }
  static void bit_writer(uint32_t *out, const uint32_t value,
                         const uint32_t bits) {
    *out = (*out << bits) | value;
  }

  enum { SIMPLE9_LOGDESC = 4, SIMPLE9_LEN = (1U << SIMPLE9_LOGDESC) };

  static uint32_t which(const uint32_t *const in) {
    return (*in) >> (32 - SIMPLE9_LOGDESC);
  }
  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue);
  const uint32_t *decodeArray(const uint32_t *in, const size_t len,
                              uint32_t *out, size_t &nvalue);
  std::string name() const {
    if (hacked)
      return "Simple9hacked";
    return "Simple9";
  }
  Simple9() {}

  static void unpack0_28(uint32_t **out, const uint32_t **in);
  static void unpack1_28(uint32_t **out, const uint32_t **in);
  static void unpack2_14(uint32_t **out, const uint32_t **in);
  static void unpack4_7(uint32_t **out, const uint32_t **in);
  static void unpack7_4(uint32_t **out, const uint32_t **in);
  static void unpack14_2(uint32_t **out, const uint32_t **in);
  static void unpack28_1(uint32_t **out, const uint32_t **in);
  static void unpack3_9(uint32_t **out, const uint32_t **in);
  static void unpack9_3(uint32_t **out, const uint32_t **in);
  static void unpack5_5(uint32_t **out, const uint32_t **in);
  typedef void (*unpacker)(uint32_t **out, const uint32_t **in);

  static const unpacker unpackarray[SIMPLE9_LEN];
};

template <bool MarkLength, bool hacked>
const typename Simple9<MarkLength, hacked>::unpacker
    Simple9<MarkLength, hacked>::unpackarray[SIMPLE9_LEN] = {
        unpack1_28, unpack2_14, unpack3_9,  unpack4_7,  unpack5_5, unpack7_4,
        unpack9_3,  unpack14_2, unpack28_1, unpack0_28, NULL,      NULL,
        NULL,       NULL,       NULL,       NULL};

template <bool MarkLength, bool hacked>
void Simple9<MarkLength, hacked>::encodeArray(const uint32_t *in,
                                              const size_t length,
                                              uint32_t *out, size_t &nvalue) {
  uint32_t NumberOfValuesCoded;
  const uint32_t *const initout(out);
  if (MarkLength)
    *(out++) = length;
  size_t ValuesRemaining(length);
  // precompute
  const bool becareful = false;

  while (ValuesRemaining >= 28) {
    if (hacked && trymefull<28, 0>(in)) {
      out[0] = 9 << (32 - SIMPLE9_LOGDESC);
      NumberOfValuesCoded = 28;
      in += 28;
    } else if (trymefull<28, 1>(in)) {
      out[0] = 0;
      NumberOfValuesCoded = 28;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 1);
      if (becareful)
        assert(which(out) == 0);
    } else if (trymefull<14, 2>(in)) {
      out[0] = 1;
      NumberOfValuesCoded = 14;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 2);
      if (becareful)
        assert(which(out) == 1);
    } else if (trymefull<9, 3>(in)) {
      out[0] = 2;
      NumberOfValuesCoded = 9;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 3);
      *out <<= 28 - 3 * 9;
      if (becareful)
        assert(which(out) == 2);
    } else if (trymefull<7, 4>(in)) {
      out[0] = 3;

      NumberOfValuesCoded = 7;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 4);
      if (becareful)
        assert(which(out) == 7);
    } else if (trymefull<5, 5>(in)) {
      out[0] = 4;

      NumberOfValuesCoded = 5;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 5);
      *out <<= 28 - 5 * 5;
      if (becareful)
        assert(which(out) == 4);
    } else if (trymefull<4, 7>(in)) {
      out[0] = 5;
      NumberOfValuesCoded = 4;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 7);

      if (becareful)
        assert(which(out) == 5);
    } else if (trymefull<3, 9>(in)) {
      out[0] = 6;
      NumberOfValuesCoded = 3;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 9);
      *out <<= 28 - 9 * 3;
      if (becareful)
        assert(which(out) == 6);
    } else if (trymefull<2, 14>(in)) {
      out[0] = 7;

      NumberOfValuesCoded = 2;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 14);

      if (becareful)
        assert(which(out) == 14);
    } else {
      if ((*in >> 28) > 0) {
        std::cerr << "Input's out of range: " << *in << std::endl;
        throw std::runtime_error(
            "You tried to apply Simple9 to an incompatible set of integers.");
      }

      out[0] = 8;

      NumberOfValuesCoded = 1;
      bit_writer(out, *in++, 28);
      if (becareful)
        assert(which(out) == 15);
    }

    ++out;

    ValuesRemaining -= NumberOfValuesCoded;
  }
  while (ValuesRemaining > 0) {
    if (hacked && trymefull<28, 0>(in)) {
      out[0] = 9 << (32 - SIMPLE9_LOGDESC);
      NumberOfValuesCoded = (ValuesRemaining < 28) ? ValuesRemaining : 28;
      in += NumberOfValuesCoded;
    } else if (tryme<28, 1>(in, ValuesRemaining)) {
      out[0] = 0;

      NumberOfValuesCoded = (ValuesRemaining < 28) ? ValuesRemaining : 28;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 1);
      *out <<= 28 - NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 0);
    } else if (tryme<14, 2>(in, ValuesRemaining)) {
      out[0] = 1;

      NumberOfValuesCoded = (ValuesRemaining < 14) ? ValuesRemaining : 14;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 2);
      *out <<= 28 - 2 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 1);
    } else if (tryme<9, 3>(in, ValuesRemaining)) {
      out[0] = 2;
      NumberOfValuesCoded = (ValuesRemaining < 9) ? ValuesRemaining : 9;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 3);
      *out <<= 28 - 3 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 2);
    } else if (tryme<7, 4>(in, ValuesRemaining)) {
      out[0] = 3;

      NumberOfValuesCoded = (ValuesRemaining < 7) ? ValuesRemaining : 7;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 4);
      *out <<= 28 - 4 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 7);
    } else if (tryme<5, 5>(in, ValuesRemaining)) {
      out[0] = 4;

      NumberOfValuesCoded = (ValuesRemaining < 5) ? ValuesRemaining : 5;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 5);
      *out <<= 28 - 5 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 4);
    } else if (tryme<4, 7>(in, ValuesRemaining)) {
      out[0] = 5;
      NumberOfValuesCoded = (ValuesRemaining < 4) ? ValuesRemaining : 4;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 7);

      *out <<= 28 - 7 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 5);
    } else if (tryme<3, 9>(in, ValuesRemaining)) {
      out[0] = 6;
      NumberOfValuesCoded = (ValuesRemaining < 3) ? ValuesRemaining : 3;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 9);
      *out <<= 28 - 9 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 6);
    } else if (tryme<2, 14>(in, ValuesRemaining)) {
      out[0] = 7;
      NumberOfValuesCoded = (ValuesRemaining < 2) ? ValuesRemaining : 2;
      for (uint32_t i = 0; i < NumberOfValuesCoded; i++)
        bit_writer(out, *in++, 14);
      *out <<= 28 - 14 * NumberOfValuesCoded;
      if (becareful)
        assert(which(out) == 14);
    } else {
      if ((*in >> 28) > 0) {
        std::cerr << "Input's out of range: " << *in << std::endl;
        throw std::runtime_error(
            "You tried to apply Simple9 to an incompatible set of integers.");
      }
      out[0] = 8;
      NumberOfValuesCoded = 1;
      bit_writer(out, *in++, 28);
      if (becareful)
        assert(which(out) == 15);
    }
    ++out;
    ValuesRemaining -= NumberOfValuesCoded;
  }
  nvalue = out - initout;
}
#ifdef STATS
template <bool MarkLength, bool hacked>
const uint32_t *
Simple9<MarkLength, hacked>::decodeArray(const uint32_t *in, const size_t len,
                                         uint32_t *out, size_t &nvalue) {
  size_t lengths[] = {28, 14, 9, 7, 5, 4, 3, 2, 1};
  vector<uint32_t> stats(16, 0);
  size_t expectedlength = 0;
#else
template <bool MarkLength, bool hacked>
const uint32_t *
Simple9<MarkLength, hacked>::decodeArray(const uint32_t *in, const size_t /* len */,
                                         uint32_t *out, size_t &nvalue) {
#endif
  if (MarkLength)
    if ((*in) > nvalue)
      throw NotEnoughStorage(*in);
  const uint32_t actualvalue = MarkLength ? *(in++) : nvalue;
  if (nvalue < actualvalue)
    std::cerr << " possible overrun" << std::endl;
  nvalue = actualvalue;
  const uint32_t *const end = out + nvalue;
  while (end > out) {
#ifdef STATS
    stats[which(in)]++;
    expectedlength += lengths[which(in)];
#endif
    // while (in < endin) {
    (unpackarray[which(in)])(&out, &in);
  }

#ifdef STATS
  uint32_t sum = std::accumulate(stats.begin(), stats.end(), 0);

  for (uint32_t k = 0; k < stats.size(); ++k) {
    std::cout << "k=" << k << std::endl;
    std::cout << "simple9 stats[" << k << "]=" << (stats[k] * 1.0 / sum)
              << std::endl;
  }
  std::cout << "alt computed length" << sum << std::endl;
  std::cout << "computed length = " << expectedlength << std::endl;
  std::cout << "we compressed " << nvalue << " integers down to " << len
            << " 32-bit words" << std::endl;
#endif
  return in;
}

/* --- Intra functions below --- */

template <bool MarkLength, bool hacked>
void Simple9<MarkLength, hacked>::unpack1_28(uint32_t **out,
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

template <bool MarkLength, bool hacked>
void Simple9<MarkLength, hacked>::unpack0_28(uint32_t **out,
                                             const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;
  for (int k = 0; k < 28; ++k)
    pout[k] = 0;
  *in = pin + 1;
  *out = pout + 28;
}

template <bool MarkLength, bool hacked>
void Simple9<MarkLength, hacked>::unpack2_14(uint32_t **out,
                                             const uint32_t **in) {
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

template <bool MarkLength, bool hacked>
void Simple9<MarkLength, hacked>::unpack4_7(uint32_t **out,
                                            const uint32_t **in) {
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

template <bool MarkLength, bool hacked>
void Simple9<MarkLength, hacked>::unpack7_4(uint32_t **out,
                                            const uint32_t **in) {
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

template <bool MarkLength, bool hacked>
void Simple9<MarkLength, hacked>::unpack14_2(uint32_t **out,
                                             const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 14) & 0x3fff;
  pout[1] = pin[0] & 0x3fff;

  *in = pin + 1;
  *out = pout + 2;
}

template <bool MarkLength, bool hacked>
void Simple9<MarkLength, hacked>::unpack28_1(uint32_t **out,
                                             const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = pin[0] & 0x0fffffff;

  *in = pin + 1;
  *out = pout + 1;
}

template <bool MarkLength, bool hacked>
void Simple9<MarkLength, hacked>::unpack3_9(uint32_t **out,
                                            const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 25) & 0x07;
  pout[1] = (pin[0] >> 22) & 0x07;
  pout[2] = (pin[0] >> 19) & 0x07;
  pout[3] = (pin[0] >> 16) & 0x07;
  pout[4] = (pin[0] >> 13) & 0x07;
  pout[5] = (pin[0] >> 10) & 0x07;
  pout[6] = (pin[0] >> 7) & 0x07;
  pout[7] = (pin[0] >> 4) & 0x07;
  pout[8] = (pin[0] >> 1) & 0x07;

  *in = pin + 1;
  *out = pout + 9;
}

template <bool MarkLength, bool hacked>
void Simple9<MarkLength, hacked>::unpack5_5(uint32_t **out,
                                            const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 23) & 0x1f;
  pout[1] = (pin[0] >> 18) & 0x1f;
  pout[2] = (pin[0] >> 13) & 0x1f;
  pout[3] = (pin[0] >> 8) & 0x1f;
  pout[4] = (pin[0] >> 3) & 0x1f;

  *in = pin + 1;
  *out = pout + 5;
}

template <bool MarkLength, bool hacked>
void Simple9<MarkLength, hacked>::unpack9_3(uint32_t **out,
                                            const uint32_t **in) {
  uint32_t *pout;
  const uint32_t *pin;

  pout = *out;
  pin = *in;

  pout[0] = (pin[0] >> 19) & 0x01ff;
  pout[1] = (pin[0] >> 10) & 0x01ff;
  pout[2] = (pin[0] >> 1) & 0x01ff;

  *in = pin + 1;
  *out = pout + 3;
}

} // namespace FastPFor

#endif /* SIMPLE9_H_ */
