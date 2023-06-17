/**
 * (c) Part of the copyright is to Indeed.com
 * Licensed under the Apache License Version 2.0
 */
/*
 * Based on an initial design by Jeff Plaisance and
 * improved by Nathan Kurz.
 */

#ifndef SIMDVARIABLEBYTE_H_
#define SIMDVARIABLEBYTE_H_

#include "common.h"
#include "codecs.h"

#ifdef __cplusplus
extern "C" {
#endif
size_t masked_vbyte_read_loop_fromcompressedsize(const uint8_t *in,
                                                 uint32_t *out,
                                                 size_t inputsize);
#ifdef __cplusplus
}
#endif

/**
 * SIMD-accelerated version of VariableByteAlt.
 */
class MaskedVByte : public FastPForLib::IntegerCODEC {
public:
  MaskedVByte() {}

  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) {
    const uint8_t *const initbout = reinterpret_cast<uint8_t *>(out);
    uint8_t *bout = reinterpret_cast<uint8_t *>(out);
    for (size_t k = 0; k < length; ++k) {
      const uint32_t val(in[k]);
      /**
       * Code below could be shorter. Whether it could be faster
       * depends on your compiler and machine.
       */
      if (val < (1U << 7)) {
        *bout = val & 0x7F;
        ++bout;
      } else if (val < (1U << 14)) {
        *bout = static_cast<uint8_t>((val & 0x7F) | (1U << 7));
        ++bout;
        *bout = static_cast<uint8_t>(val >> 7);
        ++bout;
      } else if (val < (1U << 21)) {
        *bout = static_cast<uint8_t>((val & 0x7F) | (1U << 7));
        ++bout;
        *bout = static_cast<uint8_t>(((val >> 7) & 0x7F) | (1U << 7));
        ++bout;
        *bout = static_cast<uint8_t>(val >> 14);
        ++bout;
      } else if (val < (1U << 28)) {
        *bout = static_cast<uint8_t>((val & 0x7F) | (1U << 7));
        ++bout;
        *bout = static_cast<uint8_t>(((val >> 7) & 0x7F) | (1U << 7));
        ++bout;
        *bout = static_cast<uint8_t>(((val >> 14) & 0x7F) | (1U << 7));
        ++bout;
        *bout = static_cast<uint8_t>(val >> 21);
        ++bout;
      } else {
        *bout = static_cast<uint8_t>((val & 0x7F) | (1U << 7));
        ++bout;
        *bout = static_cast<uint8_t>(((val >> 7) & 0x7F) | (1U << 7));
        ++bout;
        *bout = static_cast<uint8_t>(((val >> 14) & 0x7F) | (1U << 7));
        ++bout;
        *bout = static_cast<uint8_t>(((val >> 21) & 0x7F) | (1U << 7));
        ++bout;
        *bout = static_cast<uint8_t>(val >> 28);
        ++bout;
      }
    }
    while (FastPForLib::needPaddingTo32Bits(bout)) {
      *bout++ = 0xFFU;
    }
    const size_t storageinbytes = bout - initbout;
    nvalue = storageinbytes / 4;
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t length,
                              uint32_t *out, size_t &nvalue) {
    const uint8_t *inbyte = reinterpret_cast<const uint8_t *>(in);
    nvalue = masked_vbyte_read_loop_fromcompressedsize(inbyte, out, length * 4);
    return reinterpret_cast<const uint32_t *>(inbyte);
    return in + length;
  }

  std::string name() const { return "MaskedVByte"; }
};
#endif /* SIMDVARIABLEBYTE_H_ */
