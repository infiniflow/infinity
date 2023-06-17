/**
*  This code is released under the
*  Apache License Version 2.0 http://www.apache.org/licenses/.
*
*  Implemented by Daniel Lemire, http://lemire.me/en/
*
*  Based on code by
*      Takeshi Yamamuro <linguin.m.s_at_gmail.com>
*      Fabrizio Silvestri <fabrizio.silvestri_at_isti.cnr.it>
*      Rossano Venturini <rossano.venturini_at_isti.cnr.it>
*  which was available under the Apache License, Version 2.0.
*/

/****************************************************
***** Simple9-like encoder/decoder, RLE variant *****
*****************************************************
*
*  Improves upon Simple9 by being able to encode up to 31-bit non-negative
*integers
*  (instead of only 28-bit integers with Simple9), optionally can run-length
*encode
*  up to 2^12 16-bit repeated integers, while retaining comparable speed to
*Simple9.
*
*  Possible targets may include 32-bit or branch-constrained environments like
*GPUs.
*  The main goal is to have a small, simple and portable 32-bit integer
*compressor.
*
*  Selector values 1-7 encode betweeen 2 and 14 packed integers. Selector value
*0 is
*  used to either encode 28 packed 1-bit integers, optionally mark the
*end-of-stream,
*  or run-length encode up to 2^12 repeated 16-bit integers (whichever packs
*better).
*  Selector values 8-15 (i.e. the most significant bit set) encode one 31-bit
*integer.
*  If input value is bigger than 31 bits, only the first 31 bits will be
*retained.
*
*  Selector value: | 0 (RLE) |  0 |  1  2  3  4  5  6  7 | 8 to 15 |
*  Integers coded: | 1..2^12 | 28 | 14  9  7  5  4  3  2 | 1       |
*  Bits / integer: | 16 bits |  1 |  2  3  4  5  7  9 14 | 31 bits |
*
*  If larger than 16-bit integers need to be RL-encoded, the 28 bits allocated
*for
*  selector value 0 (RLE) can be shifted more towards integer size and less
*towards
*  integer quantity, for example encoding up to 2^8 20-bit repeated integers.
*
*  Optionally, if run-length encoding and 28x1-bit integer packing are not
*needed,
*  then selector code 0 can be left unused. This way 32-bit output value 0
*cannot be
*  produced by the compressor and so it can be used as end-of-stream marker
*instead.
*
*  Performance:
*
*  Decoding is somewhat faster than Simple9 most probably due to less branching.
*  Encoding is somewhat slower than Simple9 mostly due to no optimizations at
*all.
*  The extra check for run-length encoding is optional, cheap and not a big
*factor.
*  Code is mostly unoptimized to keep it small, portable and facilitate easy
*reuse.
*
**/

#ifndef SIMPLE9_RLE_H_
#define SIMPLE9_RLE_H_

#include "common.h"
#include "codecs.h"

//#define _SIMPLE9_USE_RLE // uncomment this line if run-length encoding is
//needed

namespace FastPForLib {

/***************************************
********** Simple9-like codec **********
****************************************/
class Simple9_Codec {

  static const uint32_t intNumber[];
  static const uint32_t bitLength[];

  static const uint32_t SIMPLE9_BITSIZE = 28;
  static const uint32_t SIMPLE9_MAXCODE = 8;

#if !defined(_SIMPLE9_USE_RLE)
  static const uint32_t SIMPLE9_MINCODE =
      0; // set to 1 to use zero as end-of-stream marker.
#else
  static const uint32_t SIMPLE9_MINCODE = 1;

  static const uint32_t RLE_SELECTOR_VALUE = 0;
  static const uint32_t RLE_MAX_VALUE_BITS = 16;
  static const uint32_t RLE_MAX_COUNT_BITS =
      SIMPLE9_BITSIZE - RLE_MAX_VALUE_BITS;
  static const uint32_t RLE_MAX_VALUE_MASK = (1U << RLE_MAX_VALUE_BITS) - 1;
  static const uint32_t RLE_MAX_COUNT_MASK = (1U << RLE_MAX_COUNT_BITS) - 1;

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
    uint32_t bitLen = bits(test | 1u);
    uint32_t repeatCount = (pos - startPos);
    return (bitLen * repeatCount >= SIMPLE9_BITSIZE) ? repeatCount : 0;
  }
#endif

public:
  static int32_t Compress(const uint32_t *input, int32_t inOffset,
                          uint32_t inCount, uint32_t *output,
                          int32_t outOffset) {
    uint32_t inPos = inOffset;
    uint32_t inEnd = inOffset + inCount;
    uint32_t outPos = outOffset;

    while (inPos < inEnd) {
      uint32_t remainingCount = inEnd - inPos;
      uint32_t outVal = 0;

#if defined(_SIMPLE9_USE_RLE)
      // try if run-length encoding packs better than bit packing
      uint32_t repeatCount = tryRunLength(input, inPos, remainingCount);
      if (repeatCount > 0) {
        repeatCount = (repeatCount < RLE_MAX_COUNT_MASK) ? repeatCount
                                                         : RLE_MAX_COUNT_MASK;
        outVal = (RLE_SELECTOR_VALUE << SIMPLE9_BITSIZE) |
                 (repeatCount << RLE_MAX_VALUE_BITS) | input[inPos];
        inPos += repeatCount;
      } else
#endif
      {
        // try all the bit packing possibilities
        uint32_t code = SIMPLE9_MINCODE;
        for (; code < SIMPLE9_MAXCODE; code++) {
          uint32_t intNum = intNumber[code];
          uint32_t bitLen = bitLength[code];
          intNum = (intNum < remainingCount) ? intNum : remainingCount;

          uint32_t maxVal = (1U << bitLen) - 1;
          uint32_t val = code << SIMPLE9_BITSIZE;
          uint32_t j = 0;
          for (; j < intNum; j++) {
            uint32_t inputVal = input[inPos + j];
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
        // if no bit packing possible, encode just one value (up to INT_MAX)
        if (code == SIMPLE9_MAXCODE) {
          outVal = (code << SIMPLE9_BITSIZE) | input[inPos++];
        }
      }
      output[outPos++] = outVal;
    }
    return outPos - outOffset;
  }

  static uint32_t Decompress(const uint32_t *input, uint32_t inOffset,
                             uint32_t *output, uint32_t outOffset,
                             uint32_t outCount) {
    uint32_t inPos = inOffset;
    uint32_t outPos = outOffset;
    uint32_t outEnd = outOffset + outCount;

    while (outPos < outEnd) {
      uint32_t remainingCount = outEnd - outPos;

      uint32_t val = input[inPos++];
      uint32_t code = val >> SIMPLE9_BITSIZE;

#if defined(_SIMPLE9_USE_RLE)
      if (code == RLE_SELECTOR_VALUE) {
        // decode rle-encoded integers
        uint32_t repeatValue = val & RLE_MAX_VALUE_MASK;
        uint32_t repeatCount = (val >> RLE_MAX_VALUE_BITS) & RLE_MAX_COUNT_MASK;
        repeatCount =
            (repeatCount < remainingCount) ? repeatCount : remainingCount;

        for (uint32_t i = 0; i < repeatCount; ++i) {
          output[outPos++] = repeatValue;
        }
      }
#else
      // optional check for end-of-stream
      if (SIMPLE9_MINCODE == 1 && code == 0) {
        break; // end of stream
      }
#endif
      else {
        // decode bit-packed integers
        uint32_t intNum = intNumber[code];
        uint32_t bitLen = bitLength[code];
        uint32_t bitMask = (1U << bitLen) - 1;
        intNum = (intNum < remainingCount)
                     ? intNum
                     : remainingCount; // optional buffer end check

#if defined(_DONT_UNROLL_LOOP)
        for (uint32_t i = 0; i < intNum; ++i) {
          output[outPos++] = val & bitMask;
          val >>= bitLen;
        }
#else
        uint32_t i = 0;
        for (; i < intNum; i += 7) {
          auto out = output + outPos + i;
          out[0] = val & bitMask;
          val >>= bitLen;
          out[1] = val & bitMask;
          val >>= bitLen;
          out[2] = val & bitMask;
          val >>= bitLen;
          out[3] = val & bitMask;
          val >>= bitLen;
          out[4] = val & bitMask;
          val >>= bitLen;
          out[5] = val & bitMask;
          val >>= bitLen;
          out[6] = val & bitMask;
          val >>= bitLen;
        }
        for (; i < intNum; ++i) {
          auto out = output + outPos + i;
          out[0] = val & bitMask;
          val >>= bitLen;
        }
        outPos += intNum;
#endif
      }
    }
    return inPos;
  }
};

// --------------------------- selector code:  0,  1,  2,  3,  4,  5,  6,  7,
// 8,  9,  A,  B,  C,  D,  E,  F
const uint32_t Simple9_Codec::intNumber[] = {28, 14, 9, 7, 5, 4, 3, 2,
                                             1,  1,  1, 1, 1, 1, 1, 1};
const uint32_t Simple9_Codec::bitLength[] = {1,  2,  3,  4,  5,  7,  9,  14,
                                             31, 31, 31, 31, 31, 31, 31, 31};

/**
* If MarkLength is true, than the number of symbols is written
* in the stream. Otherwise you need to specify it using the nvalue
* parameter decodeArray.
*/
template <bool MarkLength> class Simple9_RLE : public IntegerCODEC {

public:
  std::string name() const { return "Simple9_RLE"; }

  void encodeArray(const uint32_t *input, const size_t length, uint32_t *out,
                   size_t &nvalue) {
    if (MarkLength) {
      *out++ = static_cast<uint32_t>(length);
    }
    auto count = Simple9_Codec::Compress(input, 0, length, out, 0);
    nvalue = count;
  }

  const uint32_t *decodeArray(const uint32_t *input, const size_t length,
                              uint32_t *out, size_t &nvalue) {
    uint32_t markednvalue;
    if (MarkLength) {
      markednvalue = *input++;
      if (markednvalue > nvalue)
        throw NotEnoughStorage(markednvalue);
    }
    const size_t actualvalue = MarkLength ? markednvalue : nvalue;
    if (nvalue < actualvalue) {
      std::cerr << " possible overrun" << std::endl;
    }
    auto count = actualvalue;
    Simple9_Codec::Decompress(input, 0, out, 0, count);
    nvalue = MarkLength ? actualvalue : count;
    input += count;
    return input;
  }
};

#undef _SIMPLE9_USE_RLE

} // namespace FastPFor

#endif /* SIMPLE9_RLE_H_ */
