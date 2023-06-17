/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 */

/*  Based on code by
 *      Takeshi Yamamuro <linguin.m.s_at_gmail.com>
 *      Fabrizio Silvestri <fabrizio.silvestri_at_isti.cnr.it>
 *      Rossano Venturini <rossano.venturini_at_isti.cnr.it>
 *   which was available  under the Apache License, Version 2.0.
 */

#ifndef SIMDOPTPFOR_H_
#define SIMDOPTPFOR_H_

#include "common.h"
#include "util.h"
#include "simple16.h"
#include "simdnewpfor.h"

namespace FastPForLib {

/**
 * SIMDOptPFor, a SIMD-version of OPTPFor designed by D. Lemire
 *
 * In a multithreaded context, you may need one OPTPFor per thread.
 *
 * Combines results from the following papers:
 *
 * H. Yan, S. Ding, T. Suel, Inverted index compression and query processing
 * with
 * optimized document ordering, in: WWW '09, 2009, pp. 401-410.
 *
 * Daniel Lemire and Leonid Boytsov, Decoding billions of integers per second
 * through std::vectorization
 * Software: Practice & Experience
 * http://arxiv.org/abs/1209.2137
 * http://onlinelibrary.wiley.com/doi/10.1002/spe.2203/abstract
 */
template <uint32_t BlockSizeInUnitsOfPackSize,
          class ExceptionCoder = Simple16<false>>
class SIMDOPTPFor
    : public SIMDNewPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder> {
public:
  SIMDOPTPFor() {}
  uint32_t tryB(uint32_t b, const uint32_t *in, uint32_t len);
  uint32_t findBestB(const uint32_t *in, uint32_t len);

  virtual std::string name() const {
    std::ostringstream convert;
    convert << "SIMDOPTPFor<" << BlockSizeInUnitsOfPackSize << ","
            << SIMDNewPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::ecoder
                   .name()
            << ">";
    return convert.str();
  }
};

template <uint32_t BlockSizeInUnitsOfPackSize, class ExceptionCoder>
__attribute__((pure)) uint32_t
SIMDOPTPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::tryB(
    uint32_t b, const uint32_t *in, uint32_t len) {

  assert(b <= 32);
  if (b == 32) {
    return len;
  }
  uint32_t size = div_roundup(len * b, 32);
  uint32_t curExcept = 0;

  for (uint32_t i = 0; i < len; i++) {
    if (in[i] >= (1U << b)) {
      const uint32_t e = in[i] >> b;
      SIMDNewPFor<BlockSizeInUnitsOfPackSize,
                  ExceptionCoder>::exceptionsPositions[curExcept] = i;
      SIMDNewPFor<BlockSizeInUnitsOfPackSize,
                  ExceptionCoder>::exceptionsValues[curExcept] = e;
      curExcept++;
    }
  }

  if (curExcept > 0) {

    for (uint32_t i = curExcept - 1; i > 0; i--) {
      const uint32_t cur = SIMDNewPFor<BlockSizeInUnitsOfPackSize,
                                       ExceptionCoder>::exceptionsPositions[i];
      const uint32_t prev =
          SIMDNewPFor<BlockSizeInUnitsOfPackSize,
                      ExceptionCoder>::exceptionsPositions[i - 1];
      const uint32_t gap = cur - prev;
      SIMDNewPFor<BlockSizeInUnitsOfPackSize,
                  ExceptionCoder>::exceptionsPositions[i] = gap;
    }

    for (uint32_t i = 0; i < curExcept; i++) {
      const uint32_t excPos =
          (i > 0)
              ? SIMDNewPFor<BlockSizeInUnitsOfPackSize,
                            ExceptionCoder>::exceptionsPositions[i] -
                    1
              : SIMDNewPFor<BlockSizeInUnitsOfPackSize,
                            ExceptionCoder>::exceptionsPositions[i];
      const uint32_t excVal = SIMDNewPFor<BlockSizeInUnitsOfPackSize,
                                          ExceptionCoder>::exceptionsValues[i] -
                              1;
      SIMDNewPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::exceptions[i] =
          excPos;
      SIMDNewPFor<BlockSizeInUnitsOfPackSize,
                  ExceptionCoder>::exceptions[i + curExcept] = excVal;
    }
    size_t encodedExceptions_sz;
    SIMDNewPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::ecoder
        .fakeencodeArray(&SIMDNewPFor<BlockSizeInUnitsOfPackSize,
                                      ExceptionCoder>::exceptions[0],
                         2 * curExcept,
                         /*&encodedExceptions[0], */ encodedExceptions_sz);
    size += static_cast<uint32_t>(encodedExceptions_sz);
  }
  return size;
}
template <uint32_t BlockSizeInUnitsOfPackSize, class ExceptionCoder>
__attribute__((pure)) uint32_t
SIMDOPTPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::findBestB(
    const uint32_t *in, uint32_t len) {
  uint32_t b =
      SIMDNewPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::possLogs.back();
  assert(b == 32);
  uint32_t bsize = tryB(b, in, len);
  const uint32_t mb = maxbits(in, in + len);
  uint32_t i = 0;
  while (
      mb >
      28 + SIMDNewPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::possLogs[i])
    ++i; // some schemes such as Simple16 don't code numbers greater than 28

  for (; i < SIMDNewPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::possLogs
                     .size() -
                 1;
       i++) {
    const uint32_t csize = tryB(
        SIMDNewPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::possLogs[i],
        in, len);

    if (csize <= bsize) {
      b = SIMDNewPFor<BlockSizeInUnitsOfPackSize, ExceptionCoder>::possLogs[i];
      bsize = csize;
    }
  }
  return b;
}

} // namespace FastPFor

#endif /* SIMDOPTPFOR_H_ */
