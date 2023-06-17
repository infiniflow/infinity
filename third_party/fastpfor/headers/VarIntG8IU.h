/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 */
#if !defined(__SSSE3__) && !(defined(_MSC_VER) && defined(__AVX__))
#ifndef _MSC_VER
#pragma message                                                                \
    "Disabling varintg8iu due to lack of SSSE3 support, try adding -mssse3 or the equivalent on your compiler"
#else
#pragma message("Disabling varintg8iu due to lack of SSSE3 support, try adding -mssse3 or the equivalent on your compiler")
#endif
#else
#ifndef VARINTG8IU_H__
#define VARINTG8IU_H__
#include <emmintrin.h>
#include "codecs.h"
#ifdef __GNUC__
#define PREDICT_FALSE(x) (__builtin_expect(x, 0))
#else
#define PREDICT_FALSE(x) x
#endif

namespace FastPForLib {
/**
 *
 * Implementation of varint-G8IU taken from
 * Stepanov et al., SIMD-Based Decoding of Posting Lists, CIKM 2011
 *
 * Update: D. Lemire believes that this scheme was patented by Rose, Stepanov et
 * al. (patent 20120221539).
 * We wrote this code before the patent was published (August 2012).
 *
 * By Maxime Caron and Daniel Lemire
 *
 * This code was originally written by M. Caron and then
 * optimized by D. Lemire.
 *
 */
class VarIntG8IU : public IntegerCODEC {

public:
  // For all possible values of the
  // descriptor we build a table of any shuffle sequence
  // that might be needed at decode time.
  VarIntG8IU() {
    char mask[256][32];
    for (int desc = 0; desc <= 255; desc++) {
      int bitmask = 0x00000001;
      int bitindex = 0;
      // count number of 0 in the char
      int complete = 0;
      int ithSize[8];
      int lastpos = -1;
      while (bitindex < 8) {
        if ((desc & bitmask) == 0) {
          ithSize[complete] = bitindex - lastpos;
          lastpos = bitindex;
          complete++;
        }
        bitindex++;
        bitmask = bitmask << 1;
      }
      maskOutputSize[desc] = complete;

      int j = 0;
      int k = 0;
      for (int i = 0; i < complete; i++) {
        for (int n = 0; n < 4; n++) {
          if (n < ithSize[i]) {
            mask[desc][k] = static_cast<unsigned char>(j);
            j = j + 1;
          } else {
            mask[desc][k] = -1;
          }
          k = k + 1;
        }
      }
    }
    for (int desc = 0; desc <= 255; desc++) {
      vecmask[desc][0] =
          _mm_lddqu_si128(reinterpret_cast<__m128i const *>(mask[desc]));
      vecmask[desc][1] =
          _mm_lddqu_si128(reinterpret_cast<__m128i const *>(mask[desc] + 16));
    }
  }

  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) {
    const uint32_t *src = in;
    size_t srclength = length * 4;

    unsigned char *dst = reinterpret_cast<unsigned char *>(out);
    nvalue = nvalue * 4;

    size_t compressed_size = 0;
    while (srclength > 0 && nvalue >= 9) {
      compressed_size += encodeBlock(src, srclength, dst, nvalue);
    }
    // Ouput might not be a multiple of 4 so we make it so
    nvalue = ((compressed_size + 3) / 4);
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t length,
                              uint32_t *out, size_t &nvalue) {

    const unsigned char *src = reinterpret_cast<const unsigned char *>(in);
    const uint32_t *const initdst = out;

    uint32_t *dst = out;
    size_t srclength = length * 4;
    for (; srclength >= 22; srclength -= 8, src += 8) {
      unsigned char desc = *src;
      src += 1;
      srclength -= 1;
      const __m128i data =
          _mm_lddqu_si128(reinterpret_cast<__m128i const *>(src));
      const __m128i result = _mm_shuffle_epi8(data, vecmask[desc][0]);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(dst), result);
      int readSize = maskOutputSize[desc];

      if (readSize > 4) {
        const __m128i result2 = _mm_shuffle_epi8(
            data, vecmask[desc][1]); //__builtin_ia32_pshufb128(data, shf2);
        _mm_storeu_si128(
            reinterpret_cast<__m128i *>(dst + 4),
            result2); //__builtin_ia32_storedqu(dst + (16), result2);
      }
      dst += readSize;
    }
    while (srclength >= 9) {
      unsigned char desc = *src;
      src += 1;
      srclength -= 1;
      char buff[32];
      memcpy(buff, src, 8);
      const __m128i data =
          _mm_lddqu_si128(reinterpret_cast<__m128i const *>(buff));
      const __m128i result = _mm_shuffle_epi8(data, vecmask[desc][0]);
      _mm_storeu_si128(reinterpret_cast<__m128i *>(buff), result);
      int readSize = maskOutputSize[desc];
      if (readSize > 4) {
        const __m128i result2 = _mm_shuffle_epi8(data, vecmask[desc][1]);
        _mm_storeu_si128(reinterpret_cast<__m128i *>(buff + 16), result2);
      }
      memcpy(dst, buff, 4 * readSize);
      dst += readSize;
      srclength -= 8;
      src += 8;
    }

    nvalue = (dst - initdst);
    return reinterpret_cast<uint32_t *>((reinterpret_cast<uintptr_t>(src) + 3) &
                                        ~3);
  }

  virtual std::string name() const { return std::string("VarIntG8IU"); }

  int encodeBlock(const uint32_t *&src, size_t &srclength, unsigned char *&dest,
                  size_t &dstlength) {
    unsigned char desc = 0xFF;
    unsigned char bitmask = 0x01;
    uint32_t buffer[8];
    int ithSize[8];
    int length = 0;
    int numInt = 0;

    while (srclength > 0) {
      const uint32_t *temp = src;
      int byteNeeded = getNumByteNeeded(*temp);

      if (PREDICT_FALSE(length + byteNeeded > 8)) {
        break;
      }

      // flip the correct bit in desc
      bitmask = static_cast<unsigned char>(bitmask << (byteNeeded - 1));
      desc = desc ^ bitmask;
      bitmask = static_cast<unsigned char>(bitmask << 1);

      ithSize[numInt] = byteNeeded;
      length += byteNeeded;
      buffer[numInt] = *temp;
      src = src + 1;
      srclength -= 4;
      numInt++;
    }

    dest[0] = desc;
    int written = 1;
    for (int i = 0; i < numInt; i++) {
      int size = ithSize[i];
      uint32_t value = buffer[i];
      for (int j = 0; j < size; j++) {
        dest[written] = static_cast<unsigned char>(value >> (j * 8));
        written++;
      }
    }
    dest += 9;
    dstlength -= 9;
    return 9;
  }

private:
  int maskOutputSize[256];
  __m128i vecmask[256][2];

  int getNumByteNeeded(const uint32_t val) {
    return ((__builtin_clz(val | 255) ^ 31) >> 3) + 1;
  }
};

} // namespace FastPFor

#endif // VARINTG8IU_H__
#endif // __SSSE3__
