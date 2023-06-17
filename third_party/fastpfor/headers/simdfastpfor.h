/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */
#ifndef SIMDFASTPFOR_H_
#define SIMDFASTPFOR_H_

#include "common.h"
#include "codecs.h"
#include "simdbitpacking.h"
#include "usimdbitpacking.h"
#include "memutil.h"
#include "util.h"
#include "simple8b.h"
#include "usimdbitpacking.h"

namespace FastPForLib {

/**
 * SIMDFastPFor
 *
 * In a multithreaded context, you may need one SIMDFastPFor per thread.
 *
 *
 * Reference and documentation:
 *
 * Daniel Lemire and Leonid Boytsov, Decoding billions of integers per second
 * through std::vectorization
 * Software: Practice & Experience
 * http://arxiv.org/abs/1209.2137
 * http://onlinelibrary.wiley.com/doi/10.1002/spe.2203/abstract
 *
 * Note: the algorithms were slightly revised in Sept. 2013 to improve the
 * compression
 * ratios. You can expect the same compression ratios as the scalar FastPFOR (up
 * to a difference of 1%).
 *
 * Designed by D. Lemire with ideas from Leonid Boytsov. This scheme is NOT
 * patented.
 *
 */
template <uint32_t BlockSizeInUnitsOfPackSize =
              8> // BlockSizeInUnitsOfPackSize can have value 4 or 8
class SIMDFastPFor : public IntegerCODEC {
public:
  using IntegerCODEC::encodeArray;
  using IntegerCODEC::decodeArray;

  /**
   * ps (page size) should be a multiple of BlockSize, any "large"
   * value should do.
   */
  SIMDFastPFor(uint32_t ps = 65536)
      : PageSize(ps), bitsPageSize(gccbits(PageSize)), datatobepacked(33),
        bytescontainer(PageSize + 3 * PageSize / BlockSize) {
    assert(ps / BlockSize * BlockSize == ps);
    assert(gccbits(static_cast<uint32_t>(BlockSizeInUnitsOfPackSize * PACKSIZE - 1)) <= 8);
  }
  enum {
    PACKSIZE = 32,
    overheadofeachexcept = 8,
    overheadduetobits = 8,
    overheadduetonmbrexcept = 8,
    BlockSize = BlockSizeInUnitsOfPackSize * PACKSIZE
  };

  static uint32_t *packblockupsimd(const uint32_t *source, uint32_t *out,
                                   const uint32_t bit) {
    for (int k = 0; k < BlockSize; k += 128) {
      SIMD_fastpack_32(source, reinterpret_cast<__m128i *>(out), bit);
      out += 4 * bit;
      source += 128;
    }
    return out;
  }

  static const uint32_t *unpackblocksimd(const uint32_t *source, uint32_t *out,
                                         const uint32_t bit) {
    for (int k = 0; k < BlockSize; k += 128) {
      SIMD_fastunpack_32(reinterpret_cast<const __m128i *>(source), out, bit);
      source += 4 * bit;
      out += 128;
    }
    return source;
  }

  template <class STLContainer>
  static const uint32_t *unpackmesimd(const uint32_t *in, STLContainer &out,
                                      const uint32_t bit) {
    const uint32_t size = *in;
    ++in;
    out.resize((size + 32 - 1) / 32 * 32);
    uint32_t j = 0;
    for (; j + 128 <= size; j += 128) {
      usimdunpack(reinterpret_cast<const __m128i *>(in), &out[j], bit);
      in += 4 * bit;
    }
    for (; j + 31 < size; j += 32) {
      fastunpack(in, &out[j], bit);
      in += bit;
    }
    uint32_t buffer[PACKSIZE];
    uint32_t remaining = size - j;
    memcpy(buffer, in, (remaining * bit + 31) / 32 * sizeof(uint32_t));
    uint32_t *bpointer = buffer;
    in += (out.size() - j) / 32 * bit;
    for (; j < size; j += 32) {
      fastunpack(bpointer, &out[j], bit);
      bpointer += bit;
    }
    in -= (j - size) * bit / 32;
    out.resize(size);
    return in;
  }

  template <class STLContainer>
  static uint32_t *packmeupwithoutmasksimd(STLContainer &source, uint32_t *out,
                                           const uint32_t bit) {
    const uint32_t size = static_cast<uint32_t>(source.size());
    *out = size;
    out++;
    if (source.size() == 0)
      return out;
    source.resize((source.size() + 32 - 1) / 32 * 32);
    uint32_t j = 0;
    for (; j + 128 <= size; j += 128) {
      usimdpackwithoutmask(&source[j], reinterpret_cast<__m128i *>(out), bit);
      out += 4 * bit;
    }
    for (; j < size; j += 32) {
      fastpackwithoutmask(&source[j], out, bit);
      out += bit;
    }
    out -= (j - size) * bit / 32;
    source.resize(size);
    return out;
  }

  // sometimes, mem. usage can grow too much, this clears it up
  void resetBuffer() {
    for (size_t i = 0; i < datatobepacked.size(); ++i) {
      std::vector<uint32_t, cacheallocator>().swap(datatobepacked[i]);
    }
  }

  const uint32_t PageSize;
  const uint32_t bitsPageSize;

  std::vector<std::vector<uint32_t, cacheallocator>> datatobepacked;
  std::vector<uint8_t> bytescontainer;

#ifndef NDEBUG
  const uint32_t *decodeArray(const uint32_t *in, const size_t length,
#else
  const uint32_t *decodeArray(const uint32_t *in, const size_t,
#endif
                              uint32_t *out, size_t &nvalue) override {
#ifndef NDEBUG
    const uint32_t *const initin(in);
#endif
    const size_t mynvalue = *in;
    ++in;
    if (mynvalue > nvalue)
      throw NotEnoughStorage(mynvalue);
    nvalue = mynvalue;
    const uint32_t *const finalout(out + nvalue);
    while (out != finalout) {
      size_t thisnvalue(0);
      size_t thissize = static_cast<size_t>(
          finalout > PageSize + out ? PageSize : (finalout - out));

      __decodeArray(in, thisnvalue, out, thissize);
      in += thisnvalue;
      out += thissize;
    }
    assert(initin + length >= in);
    resetBuffer(); // if you don't do this, the codec has a "memory".
    return in;
  }

  /**
   * If you save the output and recover it in memory, you are
   * responsible to ensure that the alignment is preserved.
   *
   * The input size (length) should be a multiple of
   * BlockSizeInUnitsOfPackSize * PACKSIZE. (This was done
   * to simplify slightly the implementation.)
   */
  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) override {
    checkifdivisibleby(length, BlockSize);
#ifndef NDEBUG
    const uint32_t *const initout(out);
#endif
    const uint32_t *const finalin(in + length);

    *out++ = static_cast<uint32_t>(length);
    const size_t oldnvalue = nvalue;
    nvalue = 1;
    while (in != finalin) {
      size_t thissize = static_cast<size_t>(
          finalin > PageSize + in ? PageSize : (finalin - in));
      size_t thisnvalue(0);
      __encodeArray(in, thissize, out, thisnvalue);
      nvalue += thisnvalue;
      out += thisnvalue;
      in += thissize;
    }
    assert(out == nvalue + initout);
		if (oldnvalue < nvalue)
			std::cerr
					<< "It is possible we have a buffer overrun. You reported having allocated "
					<< oldnvalue * sizeof(uint32_t)
					<< " bytes for the compressed data but we needed "
					<< nvalue * sizeof(uint32_t)
					<< " bytes. Please increase the available memory"
							" for compressed data or check the value of the last parameter provided "
							" to the encodeArray method." << std::endl;
    resetBuffer(); // if you don't do this, the buffer has a memory
  }

  void getBestBFromData(const uint32_t *in, uint8_t &bestb,
                        uint8_t &bestcexcept, uint8_t &maxb) {
    uint32_t freqs[33];
    for (uint32_t k = 0; k <= 32; ++k)
      freqs[k] = 0;
    for (uint32_t k = 0; k < BlockSize; ++k) {
      freqs[asmbits(in[k])]++;
    }
    bestb = 32;
    while (freqs[bestb] == 0)
      bestb--;
    maxb = bestb;
    uint32_t bestcost = bestb * BlockSize;
    uint32_t cexcept = 0;
    bestcexcept = static_cast<uint8_t>(cexcept);
    for (uint32_t b = bestb - 1; b < 32; --b) {
      cexcept += freqs[b + 1];
      uint32_t thiscost = cexcept * overheadofeachexcept +
                          cexcept * (maxb - b) + b * BlockSize +
                          8; // the  extra 8 is the cost of storing maxbits
      if (thiscost < bestcost) {
        bestcost = thiscost;
        bestb = static_cast<uint8_t>(b);
        bestcexcept = static_cast<uint8_t>(cexcept);
      }
    }
  }

  void __encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                     size_t &nvalue) {
    uint32_t *const initout = out; // keep track of this
    checkifdivisibleby(length, BlockSize);
    uint32_t *const headerout = out++; // keep track of this
    for (uint32_t k = 0; k < 32 + 1; ++k)
      datatobepacked[k].clear();
    uint8_t *bc = &bytescontainer[0];
    out = padTo128bits(out);
    assert(!needPaddingTo128Bits(in));
    for (const uint32_t *const final = in + length; (in + BlockSize <= final);
         in += BlockSize) {
      uint8_t bestb, bestcexcept, maxb;
      getBestBFromData(in, bestb, bestcexcept, maxb);
      *bc++ = bestb;
      *bc++ = bestcexcept;
      if (bestcexcept > 0) {
        *bc++ = maxb;
        std::vector<uint32_t, cacheallocator> &thisexceptioncontainer =
            datatobepacked[maxb - bestb];
        const uint32_t maxval = 1U << bestb;
        for (uint32_t k = 0; k < BlockSize; ++k) {
          if (in[k] >= maxval) {
            // we have an exception
            thisexceptioncontainer.push_back(in[k] >> bestb);
            *bc++ = static_cast<uint8_t>(k);
          }
        }
      }
      out = packblockupsimd(in, out, bestb);
    }
    headerout[0] = static_cast<uint32_t>(out - headerout);
    const uint32_t bytescontainersize =
        static_cast<uint32_t>(bc - &bytescontainer[0]);
    *(out++) = bytescontainersize;
    memcpy(out, &bytescontainer[0], bytescontainersize);
    out += (bytescontainersize + sizeof(uint32_t) - 1) / sizeof(uint32_t);
    uint32_t bitmap = 0;
    for (uint32_t k = 2; k <= 32; ++k) {
      if (datatobepacked[k].size() != 0)
        bitmap |= (1U << (k - 1));
    }
    *(out++) = bitmap;
    for (uint32_t k = 2; k <= 32; ++k) {
      if (datatobepacked[k].size() > 0)
        out = packmeupwithoutmasksimd(datatobepacked[k], out, k);
    }
    nvalue = out - initout;
  }

  void __decodeArray(const uint32_t *in, size_t &length, uint32_t *out,
                     const size_t nvalue) {
    const uint32_t *const initin = in;
    const uint32_t *const headerin = in++;
    const uint32_t wheremeta = headerin[0];
    const uint32_t *inexcept = headerin + wheremeta;
    const uint32_t bytesize = *inexcept++;
    const uint8_t *bytep = reinterpret_cast<const uint8_t *>(inexcept);
    inexcept += (bytesize + sizeof(uint32_t) - 1) / sizeof(uint32_t);
    const uint32_t bitmap = *(inexcept++);
    for (uint32_t k = 2; k <= 32; ++k) {
      if ((bitmap & (1U << (k - 1))) != 0) {
        inexcept = unpackmesimd(inexcept, datatobepacked[k], k);
      }
    }
    length = inexcept - initin;
    std::vector<uint32_t, cacheallocator>::const_iterator
        unpackpointers[32 + 1];
    for (uint32_t k = 1; k <= 32; ++k) {
      unpackpointers[k] = datatobepacked[k].begin();
    }
    in = padTo128bits(in);
    assert(!needPaddingTo128Bits(out));
    for (uint32_t run = 0; run < nvalue / BlockSize; ++run, out += BlockSize) {
      const uint8_t b = *bytep++;
      const uint8_t cexcept = *bytep++;
      in = unpackblocksimd(in, out, b);
      if (cexcept > 0) {
        const uint8_t maxbits = *bytep++;
        if (maxbits - b == 1) {
          for (uint32_t k = 0; k < cexcept; ++k) {
            const uint8_t pos = *(bytep++);
            out[pos] |= static_cast<uint32_t>(1) << b;
          }
        } else {
          std::vector<uint32_t, cacheallocator>::const_iterator &exceptionsptr =
              unpackpointers[maxbits - b];
          for (uint32_t k = 0; k < cexcept; ++k) {
            const uint8_t pos = *(bytep++);
            out[pos] |= (*(exceptionsptr++)) << b;
          }
        }
      }
    }
    assert(in == headerin + wheremeta);
  }

  std::string name() const override {
    return std::string("SIMDFastPFor") + std::to_string(BlockSize);
  }
};

/**
 * SIMDSimplePFor: similar to SimplePFor but with SIMD acceleration.
 *
 *
 * Designed by D. Lemire. This scheme is NOT patented.
 *
 * Reference and documentation:
 *
 * Daniel Lemire and Leonid Boytsov, Decoding billions of integers per second
 * through std::vectorization
 * http://arxiv.org/abs/1209.2137
 *
 */
template <class EXCEPTIONCODER = Simple8b<true>>
class SIMDSimplePFor : public IntegerCODEC {
public:
  using IntegerCODEC::encodeArray;
  using IntegerCODEC::decodeArray;

  EXCEPTIONCODER ecoder;
  /**
   * ps (page size) should be a multiple of BlockSize, any "large"
   * value should do.
   */
  SIMDSimplePFor(uint32_t ps = 65536)
      : ecoder(), PageSize(ps), bitsPageSize(gccbits(PageSize)),
        datatobepacked(PageSize),
        bytescontainer(PageSize + 3 * PageSize / BlockSize) {
    assert(ps / BlockSize * BlockSize == ps);
    assert(gccbits(static_cast<uint32_t>(BlockSizeInUnitsOfPackSize * PACKSIZE - 1)) <= 8);
  }
  enum {
    BlockSizeInUnitsOfPackSize = 4,
    PACKSIZE = 32,
    overheadofeachexcept = 8,
    overheadduetobits = 8,
    overheadduetonmbrexcept = 8,
    BlockSize = BlockSizeInUnitsOfPackSize * PACKSIZE
  };

  const uint32_t PageSize;
  const uint32_t bitsPageSize;

  std::vector<uint32_t> datatobepacked;
  std::vector<uint8_t> bytescontainer;

  const uint32_t *decodeArray(const uint32_t *in, const size_t length,
                              uint32_t *out, size_t &nvalue) override {
    const uint32_t *const initin(in);
    const size_t mynvalue = *in;
    ++in;
    if (mynvalue > nvalue)
      throw NotEnoughStorage(mynvalue);
    nvalue = mynvalue;
    const uint32_t *const finalout(out + nvalue);
    while (out != finalout) {
      size_t thisnvalue = length - (in - initin);
      size_t thissize = static_cast<size_t>(
          finalout > PageSize + out ? PageSize : (finalout - out));

      __decodeArray(in, thisnvalue, out, thissize);
      in += thisnvalue;
      out += thissize;
    }
    assert(initin + length >= in);
    return in;
  }

  /*
   * The input size (length) should be a multiple of
   * BlockSizeInUnitsOfPackSize * PACKSIZE. (This was done
   * to simplify slightly the implementation.)
   */
  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) override {
    checkifdivisibleby(length, BlockSize);
    const uint32_t *const initout(out);
    const uint32_t *const finalin(in + length);

    *out++ = static_cast<uint32_t>(length);
    const size_t oldnvalue = nvalue;
    nvalue = 1;
    while (in != finalin) {
      size_t thissize = static_cast<size_t>(
          finalin > PageSize + in ? PageSize : (finalin - in));
      size_t thisnvalue = oldnvalue - (out - initout);
      __encodeArray(in, thissize, out, thisnvalue);
      nvalue += thisnvalue;
      out += thisnvalue;
      in += thissize;
    }
    assert(out == nvalue + initout);
    if (oldnvalue < nvalue)
			std::cerr
					<< "It is possible we have a buffer overrun. You reported having allocated "
					<< oldnvalue * sizeof(uint32_t)
					<< " bytes for the compressed data but we needed "
					<< nvalue * sizeof(uint32_t)
					<< " bytes. Please increase the available memory"
							" for compressed data or check the value of the last parameter provided "
							" to the encodeArray method." << std::endl;
  }

  void getBestBFromData(const uint32_t *in, uint8_t &bestb,
                        uint8_t &bestcexcept, uint8_t &maxb) {
    uint32_t freqs[33];
    for (uint32_t k = 0; k <= 32; ++k)
      freqs[k] = 0;
    for (uint32_t k = 0; k < BlockSize; ++k) {
      freqs[asmbits(in[k])]++;
    }
    bestb = 32;
    while (freqs[bestb] == 0)
      bestb--;
    maxb = bestb;
    uint32_t bestcost = bestb * BlockSize;
    uint32_t cexcept = 0;
    bestcexcept = static_cast<uint8_t>(cexcept);
    for (uint32_t b = bestb - 1; b < 32; --b) {
      cexcept += freqs[b + 1];
      uint32_t thiscost =
          cexcept * overheadofeachexcept + cexcept * (maxb - b) + b * BlockSize;
      if (thiscost < bestcost) {
        bestcost = thiscost;
        bestb = static_cast<uint8_t>(b);
        bestcexcept = static_cast<uint8_t>(cexcept);
      }
    }
  }

  void __encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                     size_t &nvalue) {
    uint32_t *const initout = out; // keep track of this
    checkifdivisibleby(length, BlockSize);
    uint32_t *const headerout = out++; // keep track of this
    datatobepacked.clear();
    uint8_t *bc = &bytescontainer[0];
    for (const uint32_t *const final = in + length; (in + BlockSize <= final);
         in += BlockSize) {
      uint8_t bestb, bestcexcept, maxb;
      getBestBFromData(in, bestb, bestcexcept, maxb);
      *bc++ = bestb;
      *bc++ = bestcexcept;
      if (bestcexcept > 0) {
        assert(bestb < 32);
        const uint32_t maxval = 1U << bestb;
        for (uint32_t k = 0; k < BlockSize; ++k) {
          if (in[k] >= maxval) {
            datatobepacked.push_back(in[k] >> bestb);
            *bc++ = static_cast<uint8_t>(k);
          }
        }
      }
      assert(BlockSize == 128);
      usimdpack(in, reinterpret_cast<__m128i *>(out), bestb);
      out += BlockSizeInUnitsOfPackSize * bestb;
      // out = packblockup<BlockSize>(in, out, bestb);
    }
    headerout[0] = static_cast<uint32_t>(out - headerout);
    const uint32_t bytescontainersize =
        static_cast<uint32_t>(bc - &bytescontainer[0]);
    *(out++) = bytescontainersize;
    memcpy(out, &bytescontainer[0], bytescontainersize);
    out += (bytescontainersize + sizeof(uint32_t) - 1) / sizeof(uint32_t);
    size_t outcap = 0;
    ecoder.encodeArray(datatobepacked.data(), datatobepacked.size(), out,
                       outcap);
    out += outcap;
    nvalue = out - initout;
  }

  void __decodeArray(const uint32_t *in, size_t &length, uint32_t *out,
                     const size_t nvalue) {
    const uint32_t *const initin = in;
    const uint32_t *const headerin = in++;
    const uint32_t wheremeta = headerin[0];
    const uint32_t *inexcept = headerin + wheremeta;
    const uint32_t bytesize = *inexcept++;
    const uint8_t *bytep = reinterpret_cast<const uint8_t *>(inexcept);
    inexcept += (bytesize + sizeof(uint32_t) - 1) / sizeof(uint32_t);
    datatobepacked.resize(datatobepacked.capacity());
    size_t cap = datatobepacked.size();
    size_t le = initin + length - inexcept;
    inexcept = ecoder.decodeArray(inexcept, le, &datatobepacked[0], cap);

    length = inexcept - initin;

    auto exceptionsptr = datatobepacked.begin();
    for (uint32_t run = 0; run < nvalue / BlockSize; ++run, out += BlockSize) {
      const uint8_t b = *bytep++;
      const uint8_t cexcept = *bytep++;
      // in = unpackblock<BlockSize>(in, out, b);
      assert(BlockSize == 128);
      usimdunpack(reinterpret_cast<const __m128i *>(in), out, b);
      in += BlockSizeInUnitsOfPackSize * b;
      for (uint32_t k = 0; k < cexcept; ++k) {
        const uint8_t pos = *(bytep++);
        out[pos] |= (*(exceptionsptr++)) << b;
      }
    }
    assert(in == headerin + wheremeta);
  }

  std::string name() const override { return "SIMDSimplePFor"; }
};

} // namespace FastPFor

#endif /* SIMDFASTPFOR_H_ */
