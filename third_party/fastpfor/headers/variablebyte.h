/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#ifndef VARIABLEBYTE_H_
#define VARIABLEBYTE_H_
#include "codecs.h"
#include "common.h"

namespace FastPForLib {

class VariableByte : public IntegerCODEC {
 public:
  template <uint32_t i>
  uint8_t extract7bits(const uint64_t val) {
    return static_cast<uint8_t>((val >> (7 * i)) & ((1U << 7) - 1));
  }

  template <uint32_t i>
  uint8_t extract7bitsmaskless(const uint64_t val) {
    return static_cast<uint8_t>((val >> (7 * i)));
  }

  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) override {
    _encodeArray<uint32_t>(in, length, out, nvalue);
  }

  void encodeArray(const uint64_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) override {
    _encodeArray<uint64_t>(in, length, out, nvalue);
  }

  template <typename T>
  void _encodeArray(const T *in, const size_t length, uint32_t *out,
                    size_t &nvalue) {
    uint8_t *bout = reinterpret_cast<uint8_t *>(out);
    const uint8_t *const initbout = reinterpret_cast<uint8_t *>(out);
    size_t bytenvalue = nvalue * sizeof(uint32_t);
    encodeToByteArray(in, length, bout, bytenvalue);
    bout += bytenvalue;
    while (needPaddingTo32Bits(bout)) {
      *bout++ = 0;
    }
    const size_t storageinbytes = bout - initbout;
    assert((storageinbytes % 4) == 0);
    nvalue = storageinbytes / 4;
  }

  template <typename T>
  void encodeToByteArray(const T *in, const size_t length, uint8_t *bout,
                         size_t &nvalue) {
    const uint8_t *const initbout = bout;
    for (size_t k = 0; k < length; ++k) {
      const uint64_t val = in[k];
      /**
       * Code below could be shorter. Whether it could be faster
       * depends on your compiler and machine.
       */
      if (val < (1ULL << 7)) {
        *bout = static_cast<uint8_t>(val | 0x80);
        ++bout;
      } else if (val < (1ULL << 14)) {
        *bout = extract7bits<0>(val);
        ++bout;
        *bout = extract7bitsmaskless<1>(val) | 0x80;
        ++bout;
      } else if (val < (1ULL << 21)) {
        *bout = extract7bits<0>(val);
        ++bout;
        *bout = extract7bits<1>(val);
        ++bout;
        *bout = extract7bitsmaskless<2>(val) | 0x80;
        ++bout;
      } else if (val < (1ULL << 28)) {
        *bout = extract7bits<0>(val);
        ++bout;
        *bout = extract7bits<1>(val);
        ++bout;
        *bout = extract7bits<2>(val);
        ++bout;
        *bout = extract7bitsmaskless<3>(val) | 0x80;
        ++bout;
      } else if (val < (1ULL << 35)) {
        *bout = extract7bits<0>(val);
        ++bout;
        *bout = extract7bits<1>(val);
        ++bout;
        *bout = extract7bits<2>(val);
        ++bout;
        *bout = extract7bits<3>(val);
        ++bout;
        *bout = extract7bitsmaskless<4>(val) | 0x80;
        ++bout;
      } else if (val < (1ULL << 42)) {
        *bout = extract7bits<0>(val);
        ++bout;
        *bout = extract7bits<1>(val);
        ++bout;
        *bout = extract7bits<2>(val);
        ++bout;
        *bout = extract7bits<3>(val);
        ++bout;
        *bout = extract7bits<4>(val);
        ++bout;
        *bout = extract7bitsmaskless<5>(val) | 0x80;
        ++bout;
      } else if (val < (1ULL << 49)) {
        *bout = extract7bits<0>(val);
        ++bout;
        *bout = extract7bits<1>(val);
        ++bout;
        *bout = extract7bits<2>(val);
        ++bout;
        *bout = extract7bits<3>(val);
        ++bout;
        *bout = extract7bits<4>(val);
        ++bout;
        *bout = extract7bits<5>(val);
        ++bout;
        *bout = extract7bitsmaskless<6>(val) | 0x80;
        ++bout;
      } else if (val < (1ULL << 56)) {
        *bout = extract7bits<0>(val);
        ++bout;
        *bout = extract7bits<1>(val);
        ++bout;
        *bout = extract7bits<2>(val);
        ++bout;
        *bout = extract7bits<3>(val);
        ++bout;
        *bout = extract7bits<4>(val);
        ++bout;
        *bout = extract7bits<5>(val);
        ++bout;
        *bout = extract7bits<6>(val);
        ++bout;
        *bout = extract7bitsmaskless<7>(val) | 0x80;
        ++bout;
      } else if (val < (1ULL << 63)) {
        *bout = extract7bits<0>(val);
        ++bout;
        *bout = extract7bits<1>(val);
        ++bout;
        *bout = extract7bits<2>(val);
        ++bout;
        *bout = extract7bits<3>(val);
        ++bout;
        *bout = extract7bits<4>(val);
        ++bout;
        *bout = extract7bits<5>(val);
        ++bout;
        *bout = extract7bits<6>(val);
        ++bout;
        *bout = extract7bits<7>(val);
        ++bout;
        *bout = extract7bitsmaskless<8>(val) | 0x80;
        ++bout;
      } else {
        *bout = extract7bits<0>(val);
        ++bout;
        *bout = extract7bits<1>(val);
        ++bout;
        *bout = extract7bits<2>(val);
        ++bout;
        *bout = extract7bits<3>(val);
        ++bout;
        *bout = extract7bits<4>(val);
        ++bout;
        *bout = extract7bits<5>(val);
        ++bout;
        *bout = extract7bits<6>(val);
        ++bout;
        *bout = extract7bits<7>(val);
        ++bout;
        *bout = extract7bits<8>(val);
        ++bout;
        *bout = extract7bitsmaskless<9>(val) | 0x80;
        ++bout;
      }
    }
    nvalue = bout - initbout;
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t length,
                              uint32_t *out, size_t &nvalue) override {
    decodeFromByteArray<uint32_t>((const uint8_t *)in,
                                  length * sizeof(uint32_t), out, nvalue);
    return in + length;
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t length,
                              uint64_t *out, size_t &nvalue) override {
    decodeFromByteArray<uint64_t>((const uint8_t *)in,
                                  length * sizeof(uint32_t), out, nvalue);
    return in + length;
  }

  template <typename T>
  const uint8_t *decodeFromByteArray(const uint8_t *inbyte, const size_t length,
                                     T *out, size_t &nvalue) {
    if (length == 0) {
      nvalue = 0;
      return inbyte;  // abort
    }
    const uint8_t *const endbyte = inbyte + length;
    const T *const initout(out);
    // this assumes that there is a value to be read

    while (endbyte > inbyte + 10) {
      uint64_t c;
      T v = 0;

      c = inbyte[0];
      v = c & 0x7F;
      if (c >= 128) {
        inbyte += 1;
        *out++ = v;
        continue;
      }

      c = inbyte[1];
      v |= (c & 0x7F) << 7;
      if (c >= 128) {
        inbyte += 2;
        *out++ = v;
        continue;
      }

      c = inbyte[2];
      v |= (c & 0x7F) << 14;
      if (c >= 128) {
        inbyte += 3;
        *out++ = v;
        continue;
      }

      c = inbyte[3];
      v |= (c & 0x7F) << 21;
      if (c >= 128) {
        inbyte += 4;
        *out++ = v;
        continue;
      }

      c = inbyte[4];
      v |= (c & 0x7F) << 28;
      if (c >= 128) {
        inbyte += 5;
        *out++ = v;
        continue;
      }

      c = inbyte[5];
      v |= static_cast<uint64_t>(c & 0x7F) << 35;
      if (c >= 128) {
        inbyte += 6;
        *out++ = v;
        continue;
      }

      c = inbyte[6];
      v |= static_cast<uint64_t>(c & 0x7F) << 42;
      if (c >= 128) {
        inbyte += 7;
        *out++ = v;
        continue;
      }

      c = inbyte[7];
      v |= static_cast<uint64_t>(c & 0x7F) << 49;
      if (c >= 128) {
        inbyte += 8;
        *out++ = v;
        continue;
      }

      c = inbyte[8];
      v |= static_cast<uint64_t>(c & 0x7F) << 56;
      if (c >= 128) {
        inbyte += 9;
        *out++ = v;
        continue;
      }

      c = inbyte[9];
      inbyte += 10;
      v |= static_cast<uint64_t>(c & 0x1) << 63;
      *out++ = v;
    }

    while (endbyte > inbyte) {
      unsigned int shift = 0;
      for (T v = 0; endbyte > inbyte; shift += 7) {
        uint64_t c = *inbyte++;
        v += ((c & 127) << shift);
        if ((c & 128)) {
          *out++ = v;
          break;
        }
      }
    }
    nvalue = out - initout;
    return inbyte;
  }

  std::string name() const override { return "VariableByte"; }
};

class VByte : public IntegerCODEC {
 public:
  template <uint32_t i>
  uint8_t extract7bits(const uint32_t val) {
    return static_cast<uint8_t>((val >> (7 * i)) & ((1U << 7) - 1));
  }

  template <uint32_t i>
  uint8_t extract7bitsmaskless(const uint32_t val) {
    return static_cast<uint8_t>((val >> (7 * i)));
  }

  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) {
    uint8_t *bout = reinterpret_cast<uint8_t *>(out);
    const uint8_t *const initbout = reinterpret_cast<uint8_t *>(out);
    size_t bytenvalue = nvalue * sizeof(uint32_t);
    encodeToByteArray(in, length, bout, bytenvalue);
    bout += bytenvalue;
    while (needPaddingTo32Bits(bout)) {
      *bout++ = 0xFF;
    }
    const size_t storageinbytes = bout - initbout;
    assert((storageinbytes % 4) == 0);
    nvalue = storageinbytes / 4;
  }

  void encodeToByteArray(const uint32_t *in, const size_t length, uint8_t *bout,
                         size_t &nvalue) {
    const uint8_t *const initbout = bout;
    for (size_t k = 0; k < length; ++k) {
      const uint32_t val = in[k];
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
    nvalue = bout - initbout;
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t length,
                              uint32_t *out, size_t &nvalue) {
    decodeFromByteArray((const uint8_t *)in, length * sizeof(uint32_t), out,
                        nvalue);
    return in + length;
  }

  const uint8_t *decodeFromByteArray(const uint8_t *inbyte, const size_t length,
                                     uint32_t *out, size_t &nvalue) {
    if (length == 0) {
      nvalue = 0;
      return inbyte;  // abort
    }
    const uint8_t *const endbyte = inbyte + length;
    const uint32_t *const initout(out);
    // this assumes that there is a value to be read

    while (endbyte > inbyte + 5) {
      uint8_t c;
      uint32_t v;

      c = inbyte[0];
      v = c & 0x7F;
      if (c < 128) {
        inbyte += 1;
        *out++ = v;
        continue;
      }

      c = inbyte[1];
      v |= (c & 0x7F) << 7;
      if (c < 128) {
        inbyte += 2;
        *out++ = v;
        continue;
      }

      c = inbyte[2];
      v |= (c & 0x7F) << 14;
      if (c < 128) {
        inbyte += 3;
        *out++ = v;
        continue;
      }

      c = inbyte[3];
      v |= (c & 0x7F) << 21;
      if (c < 128) {
        inbyte += 4;
        *out++ = v;
        continue;
      }

      c = inbyte[4];
      inbyte += 5;
      v |= (c & 0x0F) << 28;
      *out++ = v;
    }
    while (endbyte > inbyte) {
      unsigned int shift = 0;
      for (uint32_t v = 0; endbyte > inbyte; shift += 7) {
        uint8_t c = *inbyte++;
        v += ((c & 127) << shift);
        if ((c < 128)) {
          *out++ = v;
          break;
        }
      }
    }
    nvalue = out - initout;
    return inbyte;
  }

  std::string name() const { return "VByte"; }
};

}  // namespace FastPForLib

#endif /* VARIABLEBYTE_H_ */
