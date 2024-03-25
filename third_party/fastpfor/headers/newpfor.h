/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

/*  Based on code by
 *      Takeshi Yamamuro <linguin.m.s_at_gmail.com>
 *      Fabrizio Silvestri <fabrizio.silvestri_at_isti.cnr.it>
 *      Rossano Venturini <rossano.venturini_at_isti.cnr.it>
 *   which was available  under the Apache License, Version 2.0.
 */

#ifndef NEWPFOR_H_
#define NEWPFOR_H_

#include "common.h"
#include "util.h"
#include "codecs.h"
#include "simple16.h"

namespace FastPForLib {

/**
 * NewPFD also known as NewPFOR.
 *
 * In a multithreaded context, you may need one NewPFor per thread.
 *
 * Follows
 *
 * H. Yan, S. Ding, T. Suel, Inverted index compression and query processing
 * with
 * optimized document ordering, in: WWW '09, 2009, pp. 401-410.
 */
template <uint32_t BlockSizeInUnitsOfPackSize,
          class ExceptionCoder = Simple16<false>>
class NewPFor : public IntegerCODEC {
public:
  using IntegerCODEC::encodeArray;
  using IntegerCODEC::decodeArray;

  enum {
    PFORDELTA_B = 6,
    PFORDELTA_NEXCEPT = 10,
    PFORDELTA_EXCEPTSZ = 16,
    TAIL_MERGIN = 1024,
    PFORDELTA_INVERSERATIO = 10,
    PACKSIZE = 32,
    BlockSize = BlockSizeInUnitsOfPackSize * PACKSIZE
  };

  NewPFor()
      : ecoder(), exceptionsPositions(BlockSize), exceptionsValues(BlockSize),
        exceptions(4 * BlockSize + TAIL_MERGIN + 1), tobecoded(BlockSize) {}
  static uint32_t tryB(uint32_t b, const uint32_t *in, uint32_t len);
  virtual uint32_t findBestB(const uint32_t *in, uint32_t len);

  virtual void encodeBlock(const uint32_t *in, uint32_t *out, size_t &nvalue);
  virtual const uint32_t *decodeBlock(const uint32_t *in, uint32_t *out,
                                      size_t &nvalue);

  virtual void encodeArray(const uint32_t *in, const size_t len, uint32_t *out,
                           size_t &nvalue);
  virtual const uint32_t *decodeArray(const uint32_t *in, const size_t len,
                                      uint32_t *out, size_t &nvalue);
  virtual std::string name() const {
    return "NewPFor<" + std::to_string(BlockSizeInUnitsOfPackSize) + "," + ecoder.name() + ">";
  }
  ExceptionCoder ecoder;
  std::vector<uint32_t> exceptionsPositions;
  std::vector<uint32_t> exceptionsValues;
  std::vector<uint32_t> exceptions;
  std::vector<uint32_t> tobecoded;
  static std::vector<uint32_t> possLogs;
};

/// nice compilers support this
// template<uint32_t BlockSizeInUnitsOfPackSize, class ExceptionCoder>
// std::vector<uint32_t> NewPFor<BlockSizeInUnitsOfPackSize,
// ExceptionCoder>::possLogs =
//        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 16, 20, 32 };

/// this is for brain dead compilers:
static inline std::vector<uint32_t> __ihatestupidcompilers() {
  std::vector<uint32_t> ans;
  ans.push_back(0); // I
  ans.push_back(1); // hate
  ans.push_back(2); // stupid
  ans.push_back(3); // compilers
  ans.push_back(4);
  ans.push_back(5);
  ans.push_back(6);
  ans.push_back(7);
  ans.push_back(8);
  ans.push_back(9);
  ans.push_back(10);
  ans.push_back(11);
  ans.push_back(12);
  ans.push_back(13);
  ans.push_back(16);
  ans.push_back(20);
  ans.push_back(32);
  return ans;
}

template <uint32_t BlockSizeInUnitsOfPackSize, class ExceptionCoder>
std::vector<uint32_t>
    NewPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::possLogs =
        __ihatestupidcompilers();

template <uint32_t BlockSizeInUnitsOfPackSize, class ExceptionCoder>
__attribute__((pure)) uint32_t
NewPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::tryB(uint32_t b,
                                                          const uint32_t *in,
                                                          uint32_t len) {
  uint32_t i;
  uint32_t curExcept;

  assert(b <= 32);

  if (b == 32)
    return 0;

  for (i = 0, curExcept = 0; i < len; i++) {
    if (in[i] >= (1ULL << b))
      curExcept++;
  }

  return curExcept;
}

template <uint32_t BlockSizeInUnitsOfPackSize, class ExceptionCoder>
__attribute__((pure)) uint32_t
NewPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::findBestB(
    const uint32_t *in, uint32_t len) {
  const uint32_t mb = maxbits(in, in + len);
  uint32_t i = 0;
  while (mb > 28 + possLogs[i])
    ++i; // some schemes such as Simple16 don't code numbers greater than 28

  for (; i < possLogs.size() - 1; i++) {

    const uint32_t nExceptions = tryB(possLogs[i], in, len);
    if (nExceptions * PFORDELTA_INVERSERATIO <= len)
      return possLogs[i];
  }
  return possLogs.back();
}

template <uint32_t BlockSizeInUnitsOfPackSize, class ExceptionCoder>
void NewPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::encodeBlock(
    const uint32_t *in, uint32_t *out, size_t &nvalue) {
  const uint32_t len = BlockSize;

  uint32_t b = findBestB(in, len);
  if (b < 32) {

    uint32_t nExceptions = 0;
    size_t encodedExceptions_sz = 0;

    const uint32_t *const initout(out);
    for (uint32_t i = 0; i < len; i++) {

      if (in[i] >= (1U << b)) {
        tobecoded[i] = in[i] & ((1U << b) - 1);
        exceptionsPositions[nExceptions] = i;
        exceptionsValues[nExceptions] = (in[i] >> b);
        nExceptions++;
      } else {
        tobecoded[i] = in[i];
      }
    }

    if (nExceptions > 0) {

      for (uint32_t i = nExceptions - 1; i > 0; i--) {
        const uint32_t cur = exceptionsPositions[i];
        const uint32_t prev = exceptionsPositions[i - 1];

        exceptionsPositions[i] = cur - prev;
      }

      for (uint32_t i = 0; i < nExceptions; i++) {

        exceptions[i] =
            (i > 0) ? exceptionsPositions[i] - 1 : exceptionsPositions[i];
        exceptions[i + nExceptions] = exceptionsValues[i] - 1;
      }
    }

    if (nExceptions > 0)
      ecoder.encodeArray(&exceptions[0], 2 * nExceptions, out + 1,
                         encodedExceptions_sz);
    *out++ = (b << (PFORDELTA_NEXCEPT + PFORDELTA_EXCEPTSZ)) |
             (nExceptions << PFORDELTA_EXCEPTSZ) |
             static_cast<uint32_t>(encodedExceptions_sz);
    /* Write exceptional values */

    out += static_cast<uint32_t>(encodedExceptions_sz);
    for (uint32_t i = 0; i < len; i += 32) {
      fastpackwithoutmask(&tobecoded[i], out, b);
      out += b;
    }
    nvalue = out - initout;

  } else {
    *out++ = (b << (PFORDELTA_NEXCEPT + PFORDELTA_EXCEPTSZ));
    for (uint32_t i = 0; i < len; i++)
      out[i] = in[i];
    nvalue = len + 1;
  }
}

template <uint32_t BlockSizeInUnitsOfPackSize, class ExceptionCoder>
void NewPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::encodeArray(
    const uint32_t *in, const size_t len, uint32_t *out, size_t &nvalue) {
  size_t csize;
#ifndef NDEBUG
  const uint32_t *const initin(in);
  const uint32_t *const initout(out);
#endif
  checkifdivisibleby(len, BlockSize);
  const size_t initnvalue = nvalue;

  uint32_t numBlocks = div_roundup(static_cast<uint32_t>(len), BlockSize);

  /* Output the number of blocks */
  *out++ = numBlocks;
  nvalue = 1;
#ifdef STATS
  std::vector<uint32_t> stats(33, 0);
#endif
  for (uint32_t i = 0; i < numBlocks; i++) {
#ifdef STATS
    stats[findBestB(in, BlockSize)]++;
#endif

    encodeBlock(in, out, csize);
    in += BlockSize;
    out += csize;
    nvalue += csize;
  }
#ifdef STATS
  for (uint32_t k = 0; k < 33; ++k)
    printf("newpfor b=%u %u\n", k, stats[k]);
#endif
  if (nvalue > initnvalue) {
    fprintf(stderr, "we have a possible buffer overrun\n");
  }
  ASSERT(len == static_cast<size_t>(in - initin),
      std::to_string(len) + " " + std::to_string(in - initin));
  ASSERT(nvalue == static_cast<size_t>(out - initout),
      std::to_string(nvalue) + " " + std::to_string(out - initout));
}

template <uint32_t BlockSizeInUnitsOfPackSize, class ExceptionCoder>
const uint32_t *
NewPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::decodeBlock(
    const uint32_t *in, uint32_t *out, size_t &nvalue) {
  const uint32_t *const initout(out);
  const uint32_t b = *in >> (32 - PFORDELTA_B);
  const size_t nExceptions = (*in >> (32 - (PFORDELTA_B + PFORDELTA_NEXCEPT))) &
                             ((1 << PFORDELTA_NEXCEPT) - 1);
  const uint32_t encodedExceptionsSize = *in & ((1 << PFORDELTA_EXCEPTSZ) - 1);

  size_t twonexceptions = 2 * nExceptions;
  ++in;
  if (encodedExceptionsSize > 0)
    ecoder.decodeArray(in, encodedExceptionsSize, &exceptions[0],
                       twonexceptions);
  assert(twonexceptions >= 2 * nExceptions);
  in += encodedExceptionsSize;

  uint32_t *beginout(out); // we use this later

  for (uint32_t j = 0; j < BlockSize; j += 32) {
    fastunpack(in, out, b);
    in += b;
    out += 32;
  }

  for (uint32_t e = 0, lpos = -1; e < nExceptions; e++) {
    lpos += exceptions[e] + 1;
    beginout[lpos] |= (exceptions[e + nExceptions] + 1) << b;
  }

  nvalue = out - initout;
  return in;
}

template <uint32_t BlockSizeInUnitsOfPackSize, class ExceptionCoder>
const uint32_t *
NewPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::decodeArray(
#ifndef NDEBUG
    const uint32_t *in, const size_t len, uint32_t *out, size_t &nvalue) {
#else
    const uint32_t *in, const size_t, uint32_t *out, size_t &nvalue) {
#endif
#ifndef NDEBUG
  const uint32_t *const initin(in);
#endif
  const uint32_t *const initout(out);

  if (BlockSize * (*in) > nvalue)
    throw NotEnoughStorage(*in);
  const uint32_t numBlocks = *in++;
  size_t out_len = 0;
  for (uint32_t i = 0; i < numBlocks; i++) {
    in = decodeBlock(in, out, out_len);
    out += out_len;
  }

  if (static_cast<size_t>(out - initout) > nvalue) {
    fprintf(stderr, "possible buffer overrun\n");
  }
  ASSERT(in <= len + initin,
      std::to_string(in - initin) + " " + std::to_string(len));

  nvalue = out - initout;
  assert(nvalue == numBlocks * BlockSize);
  return in;
}

} // namespace FastPForLib

#endif /* NEWPFOR_H_ */
