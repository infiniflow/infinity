/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#ifndef EPFOR_H_
#define EPFOR_H_
#include "blockpacking.h"
#include "codecs.h"
#include "common.h"
#include "cpubenchmark.h"
#include "packingvectors.h"
#include "simple8b.h"

namespace FastPForLib {

/**
 * FastPFor
 *
 * In a multithreaded context, you may need one FastPFor per thread.
 *
 * Designed by D. Lemire. This scheme is NOT patented.
 *
 * Reference and documentation:
 *
 * Daniel Lemire and Leonid Boytsov, Decoding billions of integers per second
 * through std::vectorization
 * Software: Practice & Experience
 * http://arxiv.org/abs/1209.2137
 * http://onlinelibrary.wiley.com/doi/10.1002/spe.2203/abstract
 *
 */
template <uint32_t BlockSizeInUnitsOfPackSize =
              8, /* BlockSizeInUnitsOfPackSize can have value 4 or 8 */
          typename IntType =
              uint32_t /* Integer type, currently support uint32_t or uint64_t */>
class FastPForImpl {
 public:
  /**
   * ps (page size) should be a multiple of BlockSize, any "large"
   * value should do.
   */
  FastPForImpl(uint32_t ps = 65536)
      : PageSize(ps),
        bitsPageSize(gccbits(PageSize)),
        datatobepacked(sizeof(IntType) * 8 + 1),
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

  // sometimes, mem. usage can grow too much, this clears it up
  void resetBuffer() {
    for (size_t i = 0; i < datatobepacked.size(); ++i) {
      std::vector<IntType>().swap(datatobepacked[i]);
    }
  }

  const uint32_t PageSize;
  const uint32_t bitsPageSize;

  std::vector<std::vector<IntType>> datatobepacked;
  std::vector<uint8_t> bytescontainer;

  const uint32_t *decodeArray(const uint32_t *in, const size_t length,
                              IntType *out, size_t &nvalue) {
    const uint32_t *const initin(in);
    const size_t mynvalue = *in;
    ++in;
    if (mynvalue > nvalue) throw NotEnoughStorage(mynvalue);
    nvalue = mynvalue;
    const IntType *const finalout(out + nvalue);
    while (out != finalout) {
      size_t thisnvalue(0);
      size_t thissize = static_cast<size_t>(
          finalout > PageSize + out ? PageSize : (finalout - out));

      __decodeArray(in, thisnvalue, out, thissize);
      in += thisnvalue;
      out += thissize;
    }

    if (initin + length < in) {
      throw std::logic_error(
          "Decode run over output buffer. Potential buffer overflow!");
    }

    resetBuffer();  // if you don't do this, the codec has a "memory".
    return in;
  }

  /*
   * The input size (length) should be a multiple of
   * BlockSizeInUnitsOfPackSize * PACKSIZE. (This was done
   * to simplify slightly the implementation.)
   */
  void encodeArray(const IntType *in, const size_t length, uint32_t *out,
                   size_t &nvalue) {
    checkifdivisibleby(length, BlockSize);
    const IntType *const finalin(in + length);

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
    if (oldnvalue < nvalue) {
      std::ostringstream os;
      os << "It is possible we have a buffer overrun. You reported having "
            "allocated "
         << oldnvalue * sizeof(uint32_t)
         << " bytes for the compressed data but we needed "
         << nvalue * sizeof(uint32_t)
         << " bytes. Please increase the available memory"
            " for compressed data or check the value of the last parameter "
            "provided "
            " to the encodeArray method.";
      throw std::logic_error(os.str());
    }
    resetBuffer();  // if you don't do this, the buffer has a memory
  }

  void getBestBFromData(const IntType *in, uint8_t &bestb, uint8_t &bestcexcept,
                        uint8_t &maxb) {
    uint8_t bits = sizeof(IntType) * 8;
    uint32_t freqs[65];
    for (uint32_t k = 0; k <= bits; ++k) freqs[k] = 0;
    for (uint32_t k = 0; k < BlockSize; ++k) {
      freqs[asmbits(in[k])]++;
    }
    bestb = bits;
    while (freqs[bestb] == 0) bestb--;
    maxb = bestb;
    uint32_t bestcost = bestb * BlockSize;
    uint32_t cexcept = 0;
    bestcexcept = static_cast<uint8_t>(cexcept);
    for (uint32_t b = bestb - 1; b < bits; --b) {
      cexcept += freqs[b + 1];
      uint32_t thiscost = cexcept * overheadofeachexcept +
                          cexcept * (maxb - b) + b * BlockSize +
                          8;  // the  extra 8 is the cost of storing maxbits
      if (maxb - b == 1) thiscost -= cexcept;
      if (thiscost < bestcost) {
        bestcost = thiscost;
        bestb = static_cast<uint8_t>(b);
        bestcexcept = static_cast<uint8_t>(cexcept);
      }
    }
  }

  void __encodeArray(const IntType *in, const size_t length, uint32_t *out,
                     size_t &nvalue) {
    uint32_t *const initout = out;  // keep track of this
    checkifdivisibleby(length, BlockSize);
    uint32_t *const headerout = out++;  // keep track of this
    for (uint32_t k = 0; k < sizeof(IntType) * 8 + 1; ++k) {
      datatobepacked[k].clear();
    }
    uint8_t *bc = &bytescontainer[0];
    for (const IntType *const final = in + length; (in + BlockSize <= final);
         in += BlockSize) {
      uint8_t bestb, bestcexcept, maxb;
      getBestBFromData(in, bestb, bestcexcept, maxb);
      *bc++ = bestb;
      *bc++ = bestcexcept;
      if (bestcexcept > 0) {
        *bc++ = maxb;
        std::vector<IntType> &thisexceptioncontainer =
            datatobepacked[maxb - bestb];
        const IntType maxval = static_cast<IntType>(1ULL << bestb);
        for (uint32_t k = 0; k < BlockSize; ++k) {
          if (in[k] >= maxval) {
            // we have an exception
            thisexceptioncontainer.push_back(in[k] >> bestb);
            *bc++ = static_cast<uint8_t>(k);
          }
        }
      }
      out = packblockup<BlockSize>(in, out, bestb);
    }
    headerout[0] = static_cast<uint32_t>(out - headerout);
    const uint32_t bytescontainersize =
        static_cast<uint32_t>(bc - &bytescontainer[0]);
    *(out++) = bytescontainersize;
    memcpy(out, &bytescontainer[0], bytescontainersize);
    out += (bytescontainersize + sizeof(uint32_t) - 1) / sizeof(uint32_t);

    IntType bitmap = 0;
    for (uint32_t k = 2; k <= sizeof(IntType) * 8; ++k) {
      if (datatobepacked[k].size() != 0) bitmap |= (1ULL << (k - 1));
    }
    *(reinterpret_cast<IntType *>(out)) = bitmap;
    out += (sizeof(IntType) + sizeof(uint32_t) - 1) / sizeof(uint32_t);

    for (uint32_t k = 2; k <= sizeof(IntType) * 8; ++k) {
      if (datatobepacked[k].size() > 0) {
        size_t nValue = datatobepacked[k].size();
        datatobepacked[k].resize((datatobepacked[k].size() + 32 - 1) / 32 * 32);
        out = packingvector<32>::packmeuptightwithoutmask(
            datatobepacked[k].data(), nValue, out, k);
      }
    }
    nvalue = out - initout;
  }

  void __decodeArray(const uint32_t *in, size_t &length, IntType *out,
                     const size_t nvalue) {
    const uint32_t *const initin = in;
    const uint32_t *const headerin = in++;
    const uint32_t wheremeta = headerin[0];
    const uint32_t *inexcept = headerin + wheremeta;
    const uint32_t bytesize = *inexcept++;
    const uint8_t *bytep = reinterpret_cast<const uint8_t *>(inexcept);
    inexcept += (bytesize + sizeof(uint32_t) - 1) / sizeof(uint32_t);
    IntType bitmap = *(reinterpret_cast<const IntType *>(inexcept));
    inexcept += (sizeof(IntType) + sizeof(uint32_t) - 1) / sizeof(uint32_t);
    for (uint32_t k = 2; k <= sizeof(IntType) * 8; ++k) {
      if ((bitmap & (1ULL << (k - 1))) != 0) {
        uint32_t nvalue = *inexcept;
        datatobepacked[k].resize((nvalue + PACKSIZE - 1) / PACKSIZE * PACKSIZE);
        inexcept = packingvector<32>::unpackmetight(
            inexcept, datatobepacked[k].data(), datatobepacked[k].size(), k);
        datatobepacked[k].resize(nvalue);
      }
    }
    length = inexcept - initin;
    typename std::vector<IntType>::const_iterator
        unpackpointers[sizeof(IntType) * 8 + 1];
    for (uint32_t k = 1; k <= sizeof(IntType) * 8; ++k) {
      unpackpointers[k] = datatobepacked[k].begin();
    }
    for (uint32_t run = 0; run < nvalue / BlockSize; ++run, out += BlockSize) {
      const uint8_t b = *bytep++;
      const uint8_t cexcept = *bytep++;
      in = unpackblock<BlockSize>(in, out, b);
      if (cexcept > 0) {
        const uint8_t maxbits = *bytep++;
        if (maxbits - b == 1) {
          for (uint32_t k = 0; k < cexcept; ++k) {
            const uint8_t pos = *(bytep++);
            out[pos] |= static_cast<uint64_t>(1) << b;
          }
        } else {
          typename std::vector<IntType>::const_iterator &exceptionsptr =
              unpackpointers[maxbits - b];
          for (uint32_t k = 0; k < cexcept; ++k) {
            const uint8_t pos = *(bytep++);
            out[pos] |= (static_cast<uint64_t>(*(exceptionsptr++))) << b;
          }
        }
      }
    }
    assert(in == headerin + wheremeta);
  }
};

template <uint32_t BlockSizeInUnitsOfPackSize = 8>
class FastPFor : public IntegerCODEC {
 public:
  FastPFor(uint32_t pageSize = 65536) : pfor32(pageSize) {}

  enum {
    PACKSIZE = 32,
    overheadofeachexcept = 8,
    overheadduetobits = 8,
    overheadduetonmbrexcept = 8,
    BlockSize = BlockSizeInUnitsOfPackSize * PACKSIZE
  };

  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) override {
    pfor32.encodeArray(in, length, out, nvalue);
  }

  void encodeArray(const uint64_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) override {
    pfor64.encodeArray(in, length, out, nvalue);
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t length,
                              uint32_t *out, size_t &nvalue) override {
    return pfor32.decodeArray(in, length, out, nvalue);
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t length,
                              uint64_t *out, size_t &nvalue) override {
    return pfor64.decodeArray(in, length, out, nvalue);
  }

  std::string name() const override {
    return std::string("FastPFor") + std::to_string(BlockSize);
  }

 private:
  FastPForImpl<BlockSizeInUnitsOfPackSize, uint32_t> pfor32;
  FastPForImpl<BlockSizeInUnitsOfPackSize, uint64_t> pfor64;
};

/**
 * SimplePFor
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
class SimplePFor : public IntegerCODEC {
public:
  using IntegerCODEC::encodeArray;
  using IntegerCODEC::decodeArray;

  EXCEPTIONCODER ecoder;
  /**
   * ps (page size) should be a multiple of BlockSize, any "large"
   * value should do.
   */
  SimplePFor(uint32_t ps = 65536)
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
      out = packblockup<BlockSize>(in, out, bestb);
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
      in = unpackblock<BlockSize>(in, out, b);
      for (uint32_t k = 0; k < cexcept; ++k) {
        const uint8_t pos = *(bytep++);
        out[pos] |= (*(exceptionsptr++)) << b;
      }
    }
    assert(in == headerin + wheremeta);
  }

  std::string name() const override { return "SimplePFor"; }
};

}  // namespace FastPForLib

#endif /* EPFOR_H_ */
