/**
*  This code is released under the
*  Apache License Version 2.0 http://www.apache.org/licenses/.
*
*  Implemented by Daniel Lemire, http://lemire.me/en/
*  (c) Daniel Lemire, http://lemire.me/en/
*
*  Follows Vo Ngoc Anh, Alistair Moffat: Index compression using 64-bit words.
*  Softw., Pract. Exper. 40(2): 131-147 (2010)
**/

/*****************************************************
***** Simple8b-like encoder/decoder, RLE variant *****
******************************************************
*
*  Instead of using selector values 0 and 1 for encoding 240 and 120 integers,
*  a single selector value 15 is used to run-length encode up to 2^28 repeated
*  32-bit integers. The rest of the selector values are shifted down by one to
*  their natural position (selector values 1-14 will encode bit lengths 1-60).
*
*  This also frees up selector value 0, so 64-bit output value 0 can now be
*  used as end-of-stream marker as it cannot be produced by the compressor.
*
*  Selector value: 0 |  1  2  3  4  5  6  7  8  9 10 11 12 13 14 | 15 (RLE)
*  Integers coded: 0 | 60 30 20 15 12 10  8  7  6  5  4  3  2  1 | up to 2^28
*  Bits/integer:   0 |  1  2  3  4  5  6  7  8 10 12 15 20 30 60 | 32 bits
*
*  If larger than 32-bit integers need to be RL-encoded, the 60 bits allocated
*  for selector value 15 can be shifted more towards storage and less towards
*  quantity, e.g. 2^12 48-bit integers, or the other way, e.g. 2^44 16-bit ints.
*
*  Performance:
*
*  Decoding is somewhat faster than Simple8b most probably due to less
*branching.
*  Encoding is somewhat slower than Simple8b mostly due to no optimization at
*all.
*  The extra check for run-length encoding is optional, cheap and not a big
*factor.
*  Code is mostly unoptimized to keep it small, portable and facilitate easy
*reuse.
*
*  Since the encoding is entirely unoptimized to keep the code simple, there are
*  some other bit packing schemes that can improve encoding speed, while costing
*  only a minor increase in the packed size (packed bits per integer) metric.
*  If your dataset does not have a lot of 1 bit integers, these may work better:
*
*  Selector value: 0 |  1  2  3  4  5  6  7  8  9 10 11 12 13 14 | 15 (RLE)
*  Integers coded: 0 | 30 20 15 12 10  9  8  7  6  6  5  4  3  2 | up to 2^28
*  Bits/integer:   0 |  2  3  4  5  6  6  7  8  9 10 12 15 20 30 | 32 bits
*
*  Selector value: 0 |  1  2  3  4  5  6  7  8  9 10 11 12 13 14 | 15 (RLE)
*  Integers coded: 0 | 20 15 12 11 10  9  8  7  6  6  5  5  4  3 | up to 2^28
*  Bits/integer:   0 |  3  4  5  5  6  6  7  8  9 10 11 12 15 20 | 32 bits
*
**/

#ifndef SIMPLE8B_RLE_H_
#define SIMPLE8B_RLE_H_

#include "common.h"
#include "codecs.h"

#define _SIMPLE8B_USE_RLE // optional, comment this line if run-length encoding
                          // is not needed

namespace FastPForLib {

/****************************************
********** Simple8b-like codec **********
*****************************************/
class Simple8b_Codec {
  static const uint32_t bitLength[];

  static const uint32_t SIMPLE8B_BITSIZE = 60;
  static const uint32_t SIMPLE8B_MAXCODE = 15;
  static const uint32_t SIMPLE8B_MINCODE = 1;

#if defined(_SIMPLE8B_USE_RLE)
  static const uint32_t RLE_MAX_VALUE_BITS = 32;
  static const uint32_t RLE_MAX_COUNT_BITS =
      SIMPLE8B_BITSIZE - RLE_MAX_VALUE_BITS;
  static const uint64_t RLE_MAX_VALUE_MASK = (1ULL << RLE_MAX_VALUE_BITS) - 1;
  static const uint64_t RLE_MAX_COUNT_MASK = (1ULL << RLE_MAX_COUNT_BITS) - 1;

  // check if next integer repeats, return count if packs better, otherwize 0
  static uint32_t tryRunLength(const uint32_t *input, uint32_t pos,
                               uint32_t count) {
    uint32_t startPos = pos;
    uint32_t endPos = pos + count;
    uint32_t test = input[pos++];
    if (test > RLE_MAX_VALUE_MASK)
      return 0;
    while (pos < endPos && test == input[pos]) {
      pos++;
    }
    uint32_t bitLen = bits(test | 1U);
    uint32_t repeatCount = (pos - startPos);
    return (bitLen * repeatCount >= SIMPLE8B_BITSIZE) ? repeatCount : 0;
  }
#endif

public:
  static int32_t Compress(const uint32_t *input, int32_t inOffset,
                          uint32_t inCount, uint64_t *output,
                          int32_t outOffset) {
    uint32_t inPos = inOffset;
    uint32_t inEnd = inOffset + inCount;
    uint32_t outPos = outOffset;

    while (inPos < inEnd) {
      uint32_t remainingCount = inEnd - inPos;
      uint64_t outVal = 0;

#if defined(_SIMPLE8B_USE_RLE)
      // try if run-length encoding packs better than bit packing
      uint64_t repeatCount = tryRunLength(input, inPos, remainingCount);
      if (repeatCount > 0) {
        repeatCount = (repeatCount < RLE_MAX_COUNT_MASK) ? repeatCount
                                                         : RLE_MAX_COUNT_MASK;
        outVal = (static_cast<uint64_t>(SIMPLE8B_MAXCODE) << SIMPLE8B_BITSIZE) |
                 (repeatCount << RLE_MAX_VALUE_BITS) | input[inPos];
        inPos += static_cast<uint32_t>(repeatCount);
      } else
#endif
      {
        // otherwise try all the bit packing possibilities
        uint32_t code = SIMPLE8B_MINCODE;
        for (; code < SIMPLE8B_MAXCODE; code++) {
          uint32_t intNum = bitLength[SIMPLE8B_MAXCODE - code];
          uint32_t bitLen = bitLength[code];
          intNum = (intNum < remainingCount) ? intNum : remainingCount;

          uint64_t maxVal = (1ULL << bitLen) - 1;
          uint64_t val = static_cast<uint64_t>(code) << SIMPLE8B_BITSIZE;
          uint32_t j = 0;
          for (; j < intNum; j++) {
            uint64_t inputVal = static_cast<uint64_t>(input[inPos + j]);
            if (inputVal > maxVal) {
              break;
            }
            val |= inputVal << (j * bitLen);
          }
          if (j == intNum) {
            outVal = val;
            inPos += intNum;
            break;
          }
        }
        // if no bit packing possible, encode just one value
        if (code == SIMPLE8B_MAXCODE) {
          outVal = (static_cast<uint64_t>(code) << SIMPLE8B_BITSIZE) |
                   input[inPos++];
        }
      }
      output[outPos++] = outVal;
    }
    return outPos - outOffset;
  }

  static uint32_t Decompress(const uint64_t *input, uint32_t inOffset,
                             uint32_t *output, uint32_t outOffset,
                             uint32_t outCount) {
    uint32_t inPos = 0;
    uint32_t outPos = outOffset;
    uint32_t outEnd = outOffset + outCount;

    while (outPos < outEnd) {
      uint32_t remainingCount = outEnd - outPos;

      uint64_t val = input[inPos++];
      uint32_t code = static_cast<uint32_t>(val >> SIMPLE8B_BITSIZE);

      // optional check for end-of-stream
      if (code == 0) {
        break; // end of stream
      }

#if defined(_SIMPLE8B_USE_RLE)
      else if (code == SIMPLE8B_MAXCODE) {
        // decode rle-encoded integers
        uint32_t repeatValue = static_cast<uint32_t>(val & RLE_MAX_VALUE_MASK);
        uint32_t repeatCount = static_cast<uint32_t>(
            (val >> RLE_MAX_VALUE_BITS) & RLE_MAX_COUNT_MASK);
        repeatCount =
            (repeatCount < remainingCount) ? repeatCount : remainingCount;

        for (uint32_t i = 0; i < repeatCount; ++i) {
          output[outPos++] = repeatValue;
        }
      }
#endif
      else {
        // decode bit-packed integers
        uint32_t intNum = bitLength[SIMPLE8B_MAXCODE - code];
        uint32_t bitLen = bitLength[code];
        uint64_t bitMask = (1ULL << bitLen) - 1;
        intNum = (intNum < remainingCount)
                     ? intNum
                     : remainingCount; // optional buffer end check

#if defined(_DONT_UNROLL_LOOP)
        for (uint32_t i = 0; i < intNum; ++i) {
          output[outPos++] = static_cast<uint32_t>(val & bitMask);
          val >>= bitLen;
        }
#else
        uint32_t i = 0;
        for (; i < intNum; i += 8) {
          auto out = output + outPos + i;
          out[0] = static_cast<uint32_t>(val & bitMask);
          val >>= bitLen;
          out[1] = static_cast<uint32_t>(val & bitMask);
          val >>= bitLen;
          out[2] = static_cast<uint32_t>(val & bitMask);
          val >>= bitLen;
          out[3] = static_cast<uint32_t>(val & bitMask);
          val >>= bitLen;
          out[4] = static_cast<uint32_t>(val & bitMask);
          val >>= bitLen;
          out[5] = static_cast<uint32_t>(val & bitMask);
          val >>= bitLen;
          out[6] = static_cast<uint32_t>(val & bitMask);
          val >>= bitLen;
          out[7] = static_cast<uint32_t>(val & bitMask);
          val >>= bitLen;
        }
        for (; i < intNum; ++i) {
          auto out = output + outPos + i;
          out[0] = static_cast<uint32_t>(val & bitMask);
          val >>= bitLen;
        }
        outPos += intNum;
#endif
      }
    }
    return inPos;
  }
};

const uint32_t Simple8b_Codec::bitLength[] = {1, 1,  2,  3,  4,  5,  6,  7,
                                              8, 10, 12, 15, 20, 30, 60, 32};
// const uint32_t Simple8b_Codec::bitLength[] = { 1, 2, 3, 4, 5, 6, 6, 7, 8, 9,
// 10, 12, 15, 20, 30, 32 };
// const uint32_t Simple8b_Codec::bitLength[] = { 1, 3, 4, 5, 5, 6, 6, 7, 8, 9,
// 10, 11, 12, 15, 20, 32 };
// const uint32_t Simple8b_Codec::bitLength[] = { 1, 4, 4, 5, 5, 6, 6, 7, 8, 9,
// 10, 11, 12, 14, 15, 32 };

/**
* If MarkLength is true, than the number of symbols is written
* in the stream. Otherwise you need to specify it using the nvalue
* parameter decodeArray.
*
* Note that when MarkLength is false, some unaligned (64-bit vs. 32-bit)
* access are possible. This may fail on non-x86 platforms.
*/
template <bool MarkLength> class Simple8b_RLE : public IntegerCODEC {

public:
  std::string name() const { return "Simple8b_RLE"; }

  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) {

    if (MarkLength) {
      *out++ = static_cast<uint32_t>(length);
    }
    // this may lead to unaligned access. Performance may be affected.
    // not much of an effect in practice on recent Intel processors.
    uint64_t *out64 = reinterpret_cast<uint64_t *>(out);
    auto count = Simple8b_Codec::Compress(in, 0, length, out64, 0);
    nvalue = count * 2;
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t length,
                              uint32_t *out, size_t &nvalue) {

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
    const uint32_t *const endin(in + length);
    const uint64_t *finalin64 = reinterpret_cast<const uint64_t *>(endin);
#endif
    if (nvalue < actualvalue) {
      std::cerr << " possible overrun" << std::endl;
    }
    nvalue = actualvalue;

    uint32_t pos = 0;

    pos = Simple8b_Codec::Decompress(in64, 0, out, 0, nvalue);

    assert(in64 + pos <= finalin64);
    in = reinterpret_cast<const uint32_t *>(in64 + pos);
    assert(in <= endin);
    nvalue = MarkLength ? actualvalue : nvalue;
    return in;
  }
};

#undef _SIMPLE8B_USE_RLE

} // namespace FastPFor

#endif /* SIMPLE8B_RLE_H_ */
