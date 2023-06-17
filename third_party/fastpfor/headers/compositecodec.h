/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */
#ifndef COMPOSITECODEC_H_
#define COMPOSITECODEC_H_

#include "codecs.h"
#include "common.h"
#include "util.h"

namespace FastPForLib {

/**
 * This is a useful class for CODEC that only compress
 * data having length a multiple of some unit length.
 */
template <class Codec1, class Codec2>
class CompositeCodec : public IntegerCODEC {
 public:
  CompositeCodec() : codec1(), codec2() {}
  Codec1 codec1;
  Codec2 codec2;

  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) override {
    _encodeArray<uint32_t>(in, length, out, nvalue);
  }

  void encodeArray(const uint64_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) override {
    _encodeArray<uint64_t>(in, length, out, nvalue);
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t length,
                              uint32_t *out, size_t &nvalue) override {
    return _decodeArray<uint32_t>(in, length, out, nvalue);
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t length,
                              uint64_t *out, size_t &nvalue) override {
    return _decodeArray<uint64_t>(in, length, out, nvalue);
  }

  std::string name() const override {
    std::ostringstream convert;
    convert << codec1.name() << "+" << codec2.name();
    return convert.str();
  }

 private:
  template <typename IntType>
  void _encodeArray(const IntType *in, const size_t length, uint32_t *out,
                    size_t &nvalue) {
    if (nvalue == 0) {
      return;
    }
    const size_t roundedlength = length / Codec1::BlockSize * Codec1::BlockSize;
    size_t nvalue1 = nvalue;
    codec1.encodeArray(in, roundedlength, out, nvalue1);

    if (roundedlength < length) {
      if (nvalue1 > nvalue) {
        throw std::logic_error(
            "Encode run over output buffer. Potential buffer overflow!");
      }
      size_t nvalue2 = nvalue - nvalue1;
      codec2.encodeArray(in + roundedlength, length - roundedlength,
                         out + nvalue1, nvalue2);
      nvalue = nvalue1 + nvalue2;
    } else {
      nvalue = nvalue1;
    }
  }

  template <typename IntType>
  const uint32_t *_decodeArray(const uint32_t *in, const size_t length,
                               IntType *out, size_t &nvalue) {
    if (nvalue == 0) {
      return in;
    }
    const uint32_t *const initin(in);
    size_t mynvalue1 = nvalue;
    const uint32_t *in2 = codec1.decodeArray(in, length, out, mynvalue1);
    if (length + in > in2) {
      if (nvalue <= mynvalue1) {
        throw std::logic_error("Buffer contains more data than requested!");
      }
      size_t nvalue2 = nvalue - mynvalue1;
      const uint32_t *in3 = codec2.decodeArray(in2, length - (in2 - in),
                                               out + mynvalue1, nvalue2);
      nvalue = mynvalue1 + nvalue2;
      if (initin + length < in3) {
        throw std::logic_error(
            "Decode run over output buffer. Potential buffer overflow!");
      }
      return in3;
    }
    nvalue = mynvalue1;
    if (initin + length < in2) {
      throw std::logic_error(
          "Decode run over output buffer. Potential buffer overflow!");
    }
    return in2;
  }
};

}  // namespace FastPForLib

#endif /* COMPOSITECODEC_H_ */
