/*
 * varintgb.h
 *
 *  Created on: Jul 25, 2013
 *      Author: lemire
 */

#ifndef VARINTGB_H_
#define VARINTGB_H_

#include "common.h"
#include "codecs.h"
#include "variablebyte.h"

namespace FastPForLib {

using namespace std;

/**
 * Group VarInt.
 *
 * Implemented and designed by D. Lemire based on a talk by Jeff Dean (Google),
 * optimized by N. Kurz.
 */

template <bool delta = false> class VarIntGB : public IntegerCODEC {
public:
  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) {
    uint32_t prev = 0; // for delta

    uint8_t *bout = reinterpret_cast<uint8_t *>(out);
    const uint8_t *const initbout = reinterpret_cast<uint8_t *>(out);
    *out = static_cast<uint32_t>(length);
    bout += 4;

    size_t k = 0;
    for (; k + 3 < length; k += 4) {
      uint8_t *keyp = bout++;
      *keyp = 0;
      {
        const uint32_t val = delta ? in[k] - prev : in[k];
        if (delta)
          prev = in[k];
        if (val < (1U << 8)) {
          *bout++ = static_cast<uint8_t>(val);
        } else if (val < (1U << 16)) {
          *bout++ = static_cast<uint8_t>(val);
          *bout++ = static_cast<uint8_t>(val >> 8);
          *keyp = static_cast<uint8_t>(1);
        } else if (val < (1U << 24)) {
          *bout++ = static_cast<uint8_t>(val);
          *bout++ = static_cast<uint8_t>(val >> 8);
          *bout++ = static_cast<uint8_t>(val >> 16);
          *keyp = static_cast<uint8_t>(2);
        } else {
          // the compiler will do the right thing
          *reinterpret_cast<uint32_t *>(bout) = val;
          bout += 4;
          *keyp = static_cast<uint8_t>(3);
        }
      }
      {
        const uint32_t val = delta ? in[k + 1] - prev : in[k + 1];
        if (delta)
          prev = in[k + 1];
        if (val < (1U << 8)) {
          *bout++ = static_cast<uint8_t>(val);
        } else if (val < (1U << 16)) {
          *bout++ = static_cast<uint8_t>(val);
          *bout++ = static_cast<uint8_t>(val >> 8);
          *keyp |= static_cast<uint8_t>(1 << 2);
        } else if (val < (1U << 24)) {
          *bout++ = static_cast<uint8_t>(val);
          *bout++ = static_cast<uint8_t>(val >> 8);
          *bout++ = static_cast<uint8_t>(val >> 16);
          *keyp |= static_cast<uint8_t>(2 << 2);
        } else {
          // the compiler will do the right thing
          *reinterpret_cast<uint32_t *>(bout) = val;
          bout += 4;
          *keyp |= static_cast<uint8_t>(3 << 2);
        }
      }
      {
        const uint32_t val = delta ? in[k + 2] - prev : in[k + 2];
        if (delta)
          prev = in[k + 2];
        if (val < (1U << 8)) {
          *bout++ = static_cast<uint8_t>(val);
        } else if (val < (1U << 16)) {
          *bout++ = static_cast<uint8_t>(val);
          *bout++ = static_cast<uint8_t>(val >> 8);
          *keyp |= static_cast<uint8_t>(1 << 4);
        } else if (val < (1U << 24)) {
          *bout++ = static_cast<uint8_t>(val);
          *bout++ = static_cast<uint8_t>(val >> 8);
          *bout++ = static_cast<uint8_t>(val >> 16);
          *keyp |= static_cast<uint8_t>(2 << 4);
        } else {
          // the compiler will do the right thing
          *reinterpret_cast<uint32_t *>(bout) = val;
          bout += 4;
          *keyp |= static_cast<uint8_t>(3 << 4);
        }
      }
      {
        const uint32_t val = delta ? in[k + 3] - prev : in[k + 3];
        if (delta)
          prev = in[k + 3];
        if (val < (1U << 8)) {
          *bout++ = static_cast<uint8_t>(val);
        } else if (val < (1U << 16)) {
          *bout++ = static_cast<uint8_t>(val);
          *bout++ = static_cast<uint8_t>(val >> 8);
          *keyp |= static_cast<uint8_t>(1 << 6);
        } else if (val < (1U << 24)) {
          *bout++ = static_cast<uint8_t>(val);
          *bout++ = static_cast<uint8_t>(val >> 8);
          *bout++ = static_cast<uint8_t>(val >> 16);
          *keyp |= static_cast<uint8_t>(2 << 6);
        } else {
          // the compiler will do the right thing
          *reinterpret_cast<uint32_t *>(bout) = val;
          bout += 4;
          *keyp |= static_cast<uint8_t>(3 << 6);
        }
      }
    }

    if (k < length) {
      uint8_t *keyp = bout++;
      *keyp = 0;
      for (int j = 0; k < length && j < 8; j += 2, ++k) {
        const uint32_t val = delta ? in[k] - prev : in[k];
        if (delta)
          prev = in[k];
        if (val < (1U << 8)) {
          *bout++ = static_cast<uint8_t>(val);
        } else if (val < (1U << 16)) {
          *bout++ = static_cast<uint8_t>(val);
          *bout++ = static_cast<uint8_t>(val >> 8);
          *keyp |= static_cast<uint8_t>(1 << j);
        } else if (val < (1U << 24)) {
          *bout++ = static_cast<uint8_t>(val);
          *bout++ = static_cast<uint8_t>(val >> 8);
          *bout++ = static_cast<uint8_t>(val >> 16);
          *keyp |= static_cast<uint8_t>(2 << j);
        } else {
          // the compiler will do the right thing
          *reinterpret_cast<uint32_t *>(bout) = val;
          bout += 4;
          *keyp |= static_cast<uint8_t>(3 << j);
        }
      }
    }
    while (needPaddingTo32Bits(bout)) {
      *bout++ = 0;
    }
    const size_t storageinbytes = bout - initbout;
    assert((storageinbytes % 4) == 0);
    nvalue = storageinbytes / 4;
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t length,
                              uint32_t *out, size_t &nvalue) {
    uint32_t prev = 0; // for delta

    const uint8_t *inbyte = reinterpret_cast<const uint8_t *>(in);
    nvalue = *in;
    inbyte += 4;

    const uint8_t *const endbyte =
        reinterpret_cast<const uint8_t *>(in + length);
    const uint32_t *const endout(out + nvalue);
    uint32_t val;

    while (endbyte > inbyte + 1 + 4 * 4) {
      inbyte = delta ? decodeGroupVarIntDelta(inbyte, &prev, out)
                     : decodeGroupVarInt(inbyte, out);
      out += 4;
    }
    while (endbyte > inbyte) {
      uint8_t key = *inbyte++;
      for (int k = 0; out < endout and k < 4; k++) {
        const uint32_t howmanybyte = key & 3;
        key = static_cast<uint8_t>(key >> 2);
        val = static_cast<uint32_t>(*inbyte++);
        if (howmanybyte >= 1) {
          val |= (static_cast<uint32_t>(*inbyte++) << 8);
          if (howmanybyte >= 2) {
            val |= (static_cast<uint32_t>(*inbyte++) << 16);
            if (howmanybyte >= 3) {
              val |= (static_cast<uint32_t>(*inbyte++) << 24);
            }
          }
        }
        prev = (delta ? prev : 0) + val;
        *out++ = prev;
      }
      assert(inbyte <= endbyte);
    }
    inbyte = padTo32bits(inbyte);
    return reinterpret_cast<const uint32_t *>(inbyte);
  }

  string name() const {
    if (delta)
      return "varintgbdelta";
    else
      return "varintgb";
  }

private:
  static uint32_t mask[4]; // { { 0xFF, 0xFFFF, 0xFFFFFF, 0xFFFFFFFF } };

  const uint8_t *decodeGroupVarInt(const uint8_t *in, uint32_t *out) {
    const uint32_t sel = *in++;
    if (sel == 0) {
      out[0] = static_cast<uint32_t>(in[0]);
      out[1] = static_cast<uint32_t>(in[1]);
      out[2] = static_cast<uint32_t>(in[2]);
      out[3] = static_cast<uint32_t>(in[3]);
      return in + 4;
    }
    const uint32_t sel1 = (sel & 3);
    *out++ = *(reinterpret_cast<const uint32_t *>(in)) & mask[sel1];
    in += sel1 + 1;
    const uint32_t sel2 = ((sel >> 2) & 3);
    *out++ = *(reinterpret_cast<const uint32_t *>(in)) & mask[sel2];
    in += sel2 + 1;
    const uint32_t sel3 = ((sel >> 4) & 3);
    *out++ = *(reinterpret_cast<const uint32_t *>(in)) & mask[sel3];
    in += sel3 + 1;
    const uint32_t sel4 = (sel >> 6);
    *out++ = *(reinterpret_cast<const uint32_t *>(in)) & mask[sel4];
    in += sel4 + 1;
    return in;
  }

  const uint8_t *decodeGroupVarIntDelta(const uint8_t *in, uint32_t *val,
                                        uint32_t *out) {
    const uint32_t sel = *in++;
    if (sel == 0) {
      out[0] = (*val += static_cast<uint32_t>(in[0]));
      out[1] = (*val += static_cast<uint32_t>(in[1]));
      out[2] = (*val += static_cast<uint32_t>(in[2]));
      out[3] = (*val += static_cast<uint32_t>(in[3]));
      return in + 4;
    }
    const uint32_t sel1 = (sel & 3);
    *val += *(reinterpret_cast<const uint32_t *>(in)) & mask[sel1];
    *out++ = *val;
    in += sel1 + 1;
    const uint32_t sel2 = ((sel >> 2) & 3);
    *val += *(reinterpret_cast<const uint32_t *>(in)) & mask[sel2];
    *out++ = *val;
    in += sel2 + 1;
    const uint32_t sel3 = ((sel >> 4) & 3);
    *val += *(reinterpret_cast<const uint32_t *>(in)) & mask[sel3];
    *out++ = *val;
    in += sel3 + 1;
    const uint32_t sel4 = (sel >> 6);
    *val += *(reinterpret_cast<const uint32_t *>(in)) & mask[sel4];
    *out++ = *val;
    in += sel4 + 1;
    return in;
  }
};

template <bool delta>
uint32_t VarIntGB<delta>::mask[4] = {0xFF, 0xFFFF, 0xFFFFFF, 0xFFFFFFFF};
}

#endif // VARINTGB_H_
