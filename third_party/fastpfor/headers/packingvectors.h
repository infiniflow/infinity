/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#ifndef PACKINGVECTORS_H_
#define PACKINGVECTORS_H_

#include "common.h"
#include "bitpacking.h"

namespace FastPForLib {

template <uint32_t PACKSIZE = 32> class packingvector {
public:
  template <class STLContainer>
  static const uint32_t *unpackme(const uint32_t *in, STLContainer &out,
                                  const uint32_t bit) {
    const uint32_t size = *in;
    ++in;
    out.resize((size + PACKSIZE - 1) / PACKSIZE * PACKSIZE);
    for (uint32_t j = 0; j != out.size(); j += PACKSIZE) {
      fastunpack(in, &out[j], bit);
      in += bit;
    }
    out.resize(size);
    return in;
  }

  template <typename IntType>
  static const uint32_t *unpackmetight(
                                       const uint32_t *in,
                                       IntType * out,
                                       size_t outSize,
                                       const uint32_t bit) {
    const uint32_t size = *in;
    ++in;
    uint32_t j = 0;
    for (; j + PACKSIZE - 1 < size; j += PACKSIZE) {
      fastunpack(in, &out[j], bit);
      in += bit;
    }
    uint32_t buffer[PACKSIZE * 2];
    uint32_t remaining = size - j;
    memcpy(buffer, in, (remaining * bit + 31) / 32 * sizeof(uint32_t));
    uint32_t *bpointer = buffer;
    in += (outSize - j) / PACKSIZE * bit;
    for (; j != outSize; j += PACKSIZE) {
      fastunpack(bpointer, &out[j], bit);
      bpointer += bit;
    }
    in -= (j - size) * bit / 32;
    return in;
  }

  template <class STLContainer>
  static uint32_t *packmeup(STLContainer &source, uint32_t *out,
                            const uint32_t bit) {
    const size_t size = source.size();
    *out = static_cast<uint32_t>(size);
    out++;
    if (source.size() == 0)
      return out;
    source.resize((source.size() + PACKSIZE - 1) / PACKSIZE * PACKSIZE);
    for (uint32_t j = 0; j != source.size(); j += PACKSIZE) {
      fastpack(&source[j], out, bit);
      out += bit;
    }
    source.resize(size);
    return out;
  }

  template <class STLContainer>
  static uint32_t *packmeupwithoutmask(STLContainer &source, uint32_t *out,
                                       const uint32_t bit) {
    const size_t size = source.size();
    *out = static_cast<uint32_t>(size);
    out++;
    if (source.size() == 0)
      return out;
    source.resize((source.size() + PACKSIZE - 1) / PACKSIZE * PACKSIZE);
    for (uint32_t j = 0; j != source.size(); j += PACKSIZE) {
      fastpackwithoutmask(&source[j], out, bit);
      out += bit;
    }
    source.resize(size);
    return out;
  }

  template <typename IntType>
  static uint32_t *packmeuptightwithoutmask(
                                            const IntType * source,
                                            size_t size,
                                            uint32_t *out,
                                            const uint32_t bit) {
    *out = static_cast<uint32_t>(size);
    out++;
    uint32_t j = 0;
    for (; j < size; j += PACKSIZE) {
      fastpackwithoutmask(&source[j], out, bit);
      out += bit;
    }
    out -= (j - size) * bit / 32;
    return out;
  }
};

} // namespace FastPFor

#endif /* PACKINGVECTORS_H_ */
