/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 */

/*  Based on code by
 *      Takeshi Yamamuro <linguin.m.s_at_gmail.com>
 *      Fabrizio Silvestri <fabrizio.silvestri_at_isti.cnr.it>
 *      Rossano Venturini <rossano.venturini_at_isti.cnr.it>
 *    which was available under the Apache License, Version 2.0.
 */

#ifndef VSENCODING_H_
#define VSENCODING_H_
#if !defined(__clang__) && !defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#pragma GCC diagnostic ignored "-Wunsafe-loop-optimizations"
#endif

#include <exception>
#include "common.h"

/***
 * D. Lemire: for my purposes, I just took the code I needed from
 * http://integerencoding.isti.cnr.it/ after adapting the API somewhat
 * and some minor cleaning up/optimization.
 *
 * I put it inside a namespace because there are many global variables.
 * My minor changes did not affect performances in my tests, in fact,
 * I noticed a small gain in decompression speed in one test.
 *
 * Note: This code is not what I would consider model C++ code.
 *
 * The code below follows :
 * F. Silvestri and R. Venturini, �VSEncoding: efficient coding and fast
 * decoding of integer lists via dynamic programming,� in CIKM, 2010, pp.
 * 1219-1228.
 */

namespace vsencoding {

// better to explicitly mark these dependencies
using FastPForLib::div_roundup;
using FastPForLib::IntegerCODEC;
using FastPForLib::asmbits;
using FastPForLib::NotEnoughStorage;

class BitsWriter {
private:
  uint32_t *data;
  uint32_t Fill;
  uint64_t buffer;

public:
  uint32_t written;

  BitsWriter(uint32_t *out);
  void bit_flush();

  void bit_writer(uint32_t value, uint32_t bits);
};

BitsWriter::BitsWriter(uint32_t *out)
    : data(out), Fill(0), buffer(0), written(0) {}

void BitsWriter::bit_flush() {
  if (Fill == 0)
    return;

  if (Fill > 32) {
    buffer <<= 64 - Fill;
    *data++ = static_cast<uint32_t>(buffer >> 32);
    *data++ = buffer & ((1ULL << 32) - 1);
    written += 2;
    Fill = 0;
  }

  if (Fill > 0) {
    *data++ = buffer << (32 - Fill) & ((1ULL << 32) - 1);
    written++;
  }

  buffer = 0;
  Fill = 0;
}
void BitsWriter::bit_writer(uint32_t value, uint32_t bits) {
  if (bits == 0)
    return;

  buffer = (buffer << bits) | (value & ((1ULL << bits) - 1));

  Fill += bits;

  if (Fill >= 32) {
    *data++ = (buffer >> (Fill - 32)) & ((1ULL << 32) - 1);
    written++;
    Fill -= 32;
  }
}

/*
 * If a list is larger that this parameter, then
 * it is split into subblocks that are compressed
 * separatelly. Compression is slightly worse, but
 * the decompression is more cache-friendly.
 */
//#define VSENCODING_BLOCKSZ      65536U

class VSEncoding {
private:
  /*
   * In some cases, there might be asymmetry between possible
   * lenghts of blocks if they are formed by zeros(posszLens)
   * or larger numbers(possLens).
   */
  bool aligned;
  uint32_t *possLens;
  uint32_t *posszLens;
  uint32_t poss_sz;
  uint32_t maxBlk;

public:
  VSEncoding(uint32_t *lens, uint32_t *zlens, uint32_t size, bool cflag);

  /*
   * Compute the optimal sub-lists from lists.
   *      len: The length of the sequence of lists
   *      fixCost: The fix cost in bits that we pay for  each block
   */
  uint32_t *compute_OptPartition(uint32_t *seq, uint32_t len, uint32_t fixCost,
                                 uint32_t &pSize);
};

VSEncoding::VSEncoding(uint32_t *lens, uint32_t *zlens, uint32_t size,
                       bool cflag)
    :

      aligned(cflag),
      possLens(lens), posszLens(zlens), poss_sz(size),
      maxBlk(possLens[poss_sz - 1]) {

  if (posszLens != NULL && maxBlk < posszLens[poss_sz - 1])
    maxBlk = posszLens[poss_sz - 1];
}

uint32_t *VSEncoding::compute_OptPartition(uint32_t *seq, uint32_t len,
                                           uint32_t fixCost, uint32_t &pSize) {
  int *SSSP;
  uint32_t i;
  uint32_t maxB;
  uint32_t *part;
  uint64_t curCost;
  uint64_t *cost;

  /* It will store the shortest path */
  SSSP = new int[len + 1];

  /* cost[i] will contain the cost of encoding up to i-th position */
  cost = new uint64_t[len + 1];

  if (SSSP == NULL || cost == NULL)
    std::cerr << "Can't allocate memory" << std::endl;

  for (i = 0; i < len + 1U; ++i) {
    SSSP[i] = -1;
    cost[i] = 0;
  }

  /*
   * This loop computes the cost of the optimal partition.
   * The computation of the max log in each block is done
   * by scanning. Probably we could obtain a faster solution
   * by using RMQ data structures. We use this trivial
   * solution since construction time is not our main concern.
   */
  {
    int mleft;
    int j;
    int g;
    int l;

    for (i = 0U; i < len;) {
      ++i; // rewrite to avoid possibly infinite loop warning
      mleft = (static_cast<int>(i - maxBlk) > 0) ? i - maxBlk : 0;

      for (maxB = 0, l = 0, g = 0, j = i - 1; j >= mleft; j--) {
        if (maxB < seq[j])
          maxB = seq[j];

        if (posszLens == NULL) {
          /*
           * FIXME: If the gaps of elements in possLens[] are
           * sparse, a process below is more efficient to hop
           * these gaps using the elements rather than
           * decrementing j.
           */
          if (i - j != possLens[l])
            continue;
          else
            l++;
        } else {
          /*
           * Treat runs of 0 in a different way.
           * They could form larger blocks!
           */
          if (maxB != 0) {
            mleft = (static_cast<int>(i - maxBlk) > 0)
                        ? i - possLens[poss_sz - 1]
                        : 0;

            if (i - j != possLens[l])
              continue;

            if (i - j == possLens[l])
              l++;
          } else {
            if (i - j == possLens[l])
              l++;

            if (i - j != posszLens[g])
              continue;

            if (i - j == posszLens[g])
              g++;
          }
        }

        /* Caluculate costs */
        if (aligned)
          curCost = cost[j] + div_roundup((i - j) * maxB, 32) + fixCost;
        else
          curCost = cost[j] + (i - j) * maxB + fixCost;

        if (SSSP[i] == -1)
          cost[i] = curCost + 1;

        if (curCost <= cost[i]) {
          cost[i] = curCost;
          SSSP[i] = j;
        }
      }
    }
  }

  /* Compute number of nodes in the path */
  {
    int next;

    pSize = 0;
    next = len;

    while (next != 0) {
      next = SSSP[next];
      pSize++;
    }

    /*
     * Obtain the optimal partition starting
     * from the last block.
     */
    part = new uint32_t[pSize + 1];

    if (part == NULL) {
      /* Finalization */
      delete[] SSSP;
      delete[] cost;
      throw std::runtime_error("Can't allocate memory");
    }

    i = pSize;
    next = len;

    while (next != 0) {
      part[i--] = next;
      next = SSSP[next];
    }

    part[0] = 0;
  }

  /* Finalization */
  delete[] SSSP;
  delete[] cost;

  return part;
}

class VSEncodingBlocks : public IntegerCODEC {
public:
  enum {
    TAIL_MERGIN = 2048, // was 128 but I want to be safe (D. Lemire)
    VSEBLOCKS_LOGLEN = 4,
    VSEBLOCKS_LOGLOG = 4,
    VSEBLOCKS_LOGDESC = (VSEBLOCKS_LOGLEN + VSEBLOCKS_LOGLOG),
    VSEBLOCKS_LENS_LEN = (1 << VSEBLOCKS_LOGLEN),
    VSEBLOCKS_LOGS_LEN = (1 << VSEBLOCKS_LOGLOG)
  };
  VSEncodingBlocks(const uint32_t mVSENCODING_BLOCKSZ = 65536U)
      : VSENCODING_BLOCKSZ(mVSENCODING_BLOCKSZ),
        __tmp(VSENCODING_BLOCKSZ * 4 + VSEncodingBlocks::TAIL_MERGIN) {}

  void encodeVS(uint32_t len, const uint32_t *in, uint32_t &size,
                uint32_t *out);

  const uint32_t *decodeVS(uint32_t len, const uint32_t *in, uint32_t *out,
                           uint32_t *aux);
  std::string name() const { return "VSEncoding"; }

  /*
   * It assumes that values start form 0.
   *  - *in: points to the first d-gap to be encoded
   *  - *out: points to the first int that will countain the compress
   * It returns the compress size in number of int
   *
   * Note: *out must be large enough to contain the compress.
   */
  void encodeArray(const uint32_t *in, const size_t len, uint32_t *out,
                   size_t &nvalue);

  const uint32_t *decodeArray(const uint32_t *in, const size_t len,
                              uint32_t *out, size_t &nvalue);
  uint32_t VSENCODING_BLOCKSZ; //     = 65536U

  std::vector<uint32_t> __tmp; // = new uint32_t[VSENCODING_BLOCKSZ * 2 +
                               // VSEncodingBlocks::TAIL_MERGIN];
};

#ifdef _MSC_VER
inline void __vseblocks_copy16(const uint32_t *src, uint32_t *dest) {
  memcpy(dest, src, 16 * sizeof(uint32_t));
}

inline void __vseblocks_zero32(uint32_t *dest) {
  memset(dest, 0, 32 * sizeof(uint32_t));
}
#else

#define __vseblocks_copy16(src, dest)                                          \
  __asm__ __volatile__("movdqu %4, %%xmm0\n\t"                                 \
                       "movdqu %5, %%xmm1\n\t"                                 \
                       "movdqu %6, %%xmm2\n\t"                                 \
                       "movdqu %7, %%xmm3\n\t"                                 \
                       "movdqu %%xmm0, %0\n\t"                                 \
                       "movdqu %%xmm1, %1\n\t"                                 \
                       "movdqu %%xmm2, %2\n\t"                                 \
                       "movdqu %%xmm3, %3\n\t"                                 \
                       : "=m"(dest[0]), "=m"(dest[4]), "=m"(dest[8]),          \
                         "=m"(dest[12])                                        \
                       : "m"(src[0]), "m"(src[4]), "m"(src[8]), "m"(src[12])   \
                       : "memory", "%xmm0", "%xmm1", "%xmm2", "%xmm3")

#define __vseblocks_zero32(dest)                                               \
  __asm__ __volatile__("pxor   %%xmm0, %%xmm0\n\t"                             \
                       "movdqu %%xmm0, %0\n\t"                                 \
                       "movdqu %%xmm0, %1\n\t"                                 \
                       "movdqu %%xmm0, %2\n\t"                                 \
                       "movdqu %%xmm0, %3\n\t"                                 \
                       "movdqu %%xmm0, %4\n\t"                                 \
                       "movdqu %%xmm0, %5\n\t"                                 \
                       "movdqu %%xmm0, %6\n\t"                                 \
                       "movdqu %%xmm0, %7\n\t"                                 \
                       : "=m"(dest[0]), "=m"(dest[4]), "=m"(dest[8]),          \
                         "=m"(dest[12]), "=m"(dest[16]), "=m"(dest[20]),       \
                         "=m"(dest[24]), "=m"(dest[28])::"memory", "%xmm0")
#endif

/* A set of unpacking functions */
static void __vseblocks_unpack1(uint32_t *out, const uint32_t *in, uint32_t bs);
static void __vseblocks_unpack2(uint32_t *out, const uint32_t *in, uint32_t bs);
static void __vseblocks_unpack3(uint32_t *out, const uint32_t *in, uint32_t bs);
static void __vseblocks_unpack4(uint32_t *out, const uint32_t *in, uint32_t bs);
static void __vseblocks_unpack5(uint32_t *out, const uint32_t *in, uint32_t bs);
static void __vseblocks_unpack6(uint32_t *out, const uint32_t *in, uint32_t bs);
static void __vseblocks_unpack7(uint32_t *out, const uint32_t *in, uint32_t bs);
static void __vseblocks_unpack8(uint32_t *out, const uint32_t *in, uint32_t bs);
static void __vseblocks_unpack9(uint32_t *out, const uint32_t *in, uint32_t bs);
static void __vseblocks_unpack10(uint32_t *out, const uint32_t *in,
                                 uint32_t bs);
static void __vseblocks_unpack11(uint32_t *out, const uint32_t *in,
                                 uint32_t bs);
static void __vseblocks_unpack12(uint32_t *out, const uint32_t *in,
                                 uint32_t bs);
static void __vseblocks_unpack16(uint32_t *out, const uint32_t *in,
                                 uint32_t bs);
static void __vseblocks_unpack20(uint32_t *out, const uint32_t *in,
                                 uint32_t bs);
static void __vseblocks_unpack32(uint32_t *out, const uint32_t *in,
                                 uint32_t bs);

/* A interface of unpacking functions above */
typedef void (*__vseblocks_unpacker)(uint32_t *out, const uint32_t *in,
                                     uint32_t bs);

static __vseblocks_unpacker __vseblocks_unpack[] = {NULL,
                                                    __vseblocks_unpack1,
                                                    __vseblocks_unpack2,
                                                    __vseblocks_unpack3,
                                                    __vseblocks_unpack4,
                                                    __vseblocks_unpack5,
                                                    __vseblocks_unpack6,
                                                    __vseblocks_unpack7,
                                                    __vseblocks_unpack8,
                                                    __vseblocks_unpack9,
                                                    __vseblocks_unpack10,
                                                    __vseblocks_unpack11,
                                                    __vseblocks_unpack12,
                                                    __vseblocks_unpack16,
                                                    __vseblocks_unpack20,
                                                    __vseblocks_unpack32};

/*
 * There is asymmetry between possible lenghts ofblocks
 * if they are formed by zeros or larger numbers.
 */
static uint32_t __vseblocks_possLens[] = {1, 2,  3,  4,  5,  6,  7,  8,
                                          9, 10, 11, 12, 13, 14, 15, 16};

static uint32_t __vseblocks_posszLens[] = {1, 2,  3,  4,  5,  6,  7,  8,
                                           9, 10, 11, 12, 13, 14, 16, 32};

static uint32_t __vseblocks_remapLogs[] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 16, 16, 16, 16,
    20, 20, 20, 20, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32};

static uint32_t __vseblocks_codeLogs[] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 13, 13, 13,
    14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15};

static uint32_t __vseblocks_possLogs[] = {0, 1, 2,  3,  4,  5,  6,  7,
                                          8, 9, 10, 11, 12, 16, 20, 32};

#ifdef USE_BOOST_SHAREDPTR
static VSEncodingPtr __vseblocks = VSEncodingPtr(
    new VSEncoding(&__vseblocks_possLens[0], &__vseblocks_posszLens[0],
                   VSEncodingBlocks::VSEBLOCKS_LENS_LEN, false));
#else
static VSEncoding *__vseblocks =
    new VSEncoding(&__vseblocks_possLens[0], &__vseblocks_posszLens[0],
                   VSEncodingBlocks::VSEBLOCKS_LENS_LEN, false);
#endif /* USE_BOOST_SHAREDPTR */

void VSEncodingBlocks::encodeVS(uint32_t len, const uint32_t *in,
                                uint32_t &size, uint32_t *out) {
  uint32_t i;
  uint32_t j;
  uint32_t *logs;
  uint32_t numBlocks;
  uint32_t maxB;
  uint32_t ntotal;
  uint32_t *part;
  uint32_t *blocks[VSEBLOCKS_LOGS_LEN];
  uint32_t blockCur[VSEBLOCKS_LOGS_LEN];
  uint32_t countBlocksLogs[VSEBLOCKS_LOGS_LEN];
  BitsWriter *wt;

  logs = new uint32_t[len];

  if (logs == NULL)
    std::cerr << "Can't allocate memory" << std::endl;

  /* Compute logs of all numbers */
  for (i = 0; i < len; i++)
    logs[i] = __vseblocks_remapLogs[1 + asmbits(in[i])];

  /* Compute optimal partition */
  part = __vseblocks->compute_OptPartition(
      logs, len, VSEBLOCKS_LOGLEN + VSEBLOCKS_LOGLOG, numBlocks);

  /* Ready to write */
  wt = new BitsWriter(out);

  if (wt == NULL)
    std::cerr << "Can't initialize a class" << std::endl;

  /* countBlocksLogs[i] says how many blocks uses i bits */
  for (i = 0; i < VSEBLOCKS_LOGS_LEN; i++) {
    countBlocksLogs[i] = 0;
    blockCur[i] = 0;
  }

  /* Count number of occs of each log */
  for (i = 0; i < numBlocks; i++) {
    /* Compute max B in the block */
    for (maxB = 0, j = part[i]; j < part[i + 1]; j++)
      if (maxB < logs[j])
        maxB = logs[j];

    countBlocksLogs[__vseblocks_codeLogs[maxB]] += part[i + 1] - part[i];
  }

  for (ntotal = 0, i = 1; i < VSEBLOCKS_LOGS_LEN; i++)
    if (countBlocksLogs[i] > 0)
      ntotal++;

  /* Write occs. zero is assumed to be present */
  wt->bit_writer(ntotal, 32);

  /* For each logs write it and the number of its occurrences */
  for (i = 1; i < VSEBLOCKS_LOGS_LEN; i++) {
    if (countBlocksLogs[i] > 0) {
      wt->bit_writer(countBlocksLogs[i], 28);
      wt->bit_writer(i, 4);
    }
  }

  /* Prepare arrays to store groups of elements */
  for (blocks[0] = 0, i = 1; i < VSEBLOCKS_LOGS_LEN; i++) {
    if (countBlocksLogs[i] > 0) {
      blocks[i] = new uint32_t[countBlocksLogs[i]];

      if (blocks[i] == NULL)
        std::cerr << "Can't allocate memory" << std::endl;
    } else {
      blocks[i] = NULL;
    }
  }

  /* Permute the elements based on their values of B */
  for (i = 0; i < numBlocks; i++) {
    for (maxB = 0, j = part[i]; j < part[i + 1]; j++)
      if (maxB < logs[j])
        maxB = logs[j];

    if (!maxB)
      continue;

    for (j = part[i]; j < part[i + 1]; j++) {
      /* Save current element in its bucket */
      blocks[__vseblocks_codeLogs[maxB]][blockCur[__vseblocks_codeLogs[maxB]]] =
          in[j];
      blockCur[__vseblocks_codeLogs[maxB]]++;
    }
  }

  /* Write each bucket ... keeping byte alligment */
  for (i = 1; i < VSEBLOCKS_LOGS_LEN; i++) {
    for (j = 0; j < countBlocksLogs[i]; j++)
      wt->bit_writer(blocks[i][j], __vseblocks_possLogs[i]);

    /* Align to next word */
    if (countBlocksLogs[i] > 0)
      wt->bit_flush();
  }

  wt->bit_flush();

  /* write block codes... a byte each */
  for (i = 0; i < numBlocks; i++) {
    /* Compute max B in the block */
    for (j = part[i], maxB = 0; j < part[i + 1]; j++) {
      if (maxB < logs[j])
        maxB = logs[j];
    }

    if (maxB) {
      /* Compute the code for the block length */
      for (j = 0; j < VSEBLOCKS_LENS_LEN; j++) {
        if (part[i + 1] - part[i] == __vseblocks_possLens[j])
          break;
      }
    } else {
      /*
       * Treat runs of 0 in a different way.
       * Compute the code for the block length.
       */
      for (j = 0; j < VSEBLOCKS_LENS_LEN; j++) {
        if (part[i + 1] - part[i] == __vseblocks_posszLens[j])
          break;
      }
    }

    /* Writes the value of B and K */
    wt->bit_writer(__vseblocks_codeLogs[maxB], VSEBLOCKS_LOGLOG);
    wt->bit_writer(j, VSEBLOCKS_LOGLEN);
  }

  /* Align to 32-bit */
  wt->bit_flush();

  /* Finalization */
  for (i = 0; i < VSEBLOCKS_LOGS_LEN; i++)
    delete[] blocks[i];

  delete[] part;
  delete[] logs;

  size = wt->written;

  delete wt;
}

const uint32_t *VSEncodingBlocks::decodeVS(uint32_t len, const uint32_t *in,
                                           uint32_t *out, uint32_t *aux) {
  int ntotal;
  uint32_t nblk;
  const uint32_t *addr;
  uint32_t *pblk[VSEBLOCKS_LOGS_LEN];
  uint32_t B;
  uint32_t K;
  uint32_t *end;

  //__validate(in, len);

  ntotal = *in++;
  addr = in + ntotal;

  while (ntotal-- > 0) {
    B = (*in) & (VSEBLOCKS_LOGS_LEN - 1);
    nblk = *(in++) >> VSEBLOCKS_LOGLEN;

    /* Do unpacking */
    (__vseblocks_unpack[B])(aux, addr, nblk);

    pblk[B] = aux;
    aux += nblk;
    addr += (nblk * __vseblocks_possLogs[B] + 31) / 32;
  }

  /*
   * FIXME: We assume that a 32-bit block is processed in a loop here.
   * I might think some amount of 32-bit blocks are processed simutaneously.
   */
  end = out + len;

  do {
    /* Permuting integers with a first 8-bit */
    B = (*addr) >> (VSEBLOCKS_LOGDESC * 3 + VSEBLOCKS_LOGLEN);
    K = (((*addr) >> (VSEBLOCKS_LOGDESC * 3)) & (VSEBLOCKS_LENS_LEN - 1));

    if (B) {
      /*
             * NOTICE: If a max value in __vseblocks_possLens[] is
             * over 16, a code below needs to be fixed.
             * __vseblocks_copy16() just copies 16 values each.
             */ __vseblocks_copy16(pblk[B], out);
      pblk[B] += __vseblocks_possLens[K];
      out += __vseblocks_possLens[K];
    } else {
      /* FIXME: Is it better to do memcpy() firstly? */ __vseblocks_zero32(out);
      out += __vseblocks_posszLens[K];
    }

    /* Permuting integers with a second 8-bit */
    B = ((*addr) >> (VSEBLOCKS_LOGDESC * 2 + VSEBLOCKS_LOGLEN)) &
        (VSEBLOCKS_LOGS_LEN - 1);
    K = (((*addr) >> (VSEBLOCKS_LOGDESC * 2)) & (VSEBLOCKS_LENS_LEN - 1));

    if (B) {
      __vseblocks_copy16(pblk[B], out);
      pblk[B] += __vseblocks_possLens[K];
      out += __vseblocks_possLens[K];
    } else {
      __vseblocks_zero32(out);
      out += __vseblocks_posszLens[K];
    }

    /* Permuting integers with a third 8-bit */
    B = ((*addr) >> (VSEBLOCKS_LOGDESC + VSEBLOCKS_LOGLEN)) &
        (VSEBLOCKS_LOGS_LEN - 1);
    K = (((*addr) >> VSEBLOCKS_LOGDESC) & (VSEBLOCKS_LENS_LEN - 1));

    if (B) {
      __vseblocks_copy16(pblk[B], out);
      pblk[B] += __vseblocks_possLens[K];
      out += __vseblocks_possLens[K];
    } else {
      __vseblocks_zero32(out);
      out += __vseblocks_posszLens[K];
    }

    /* Permuting integers with a fourth 8-bit */
    B = ((*addr) >> VSEBLOCKS_LOGLEN) & (VSEBLOCKS_LOGS_LEN - 1);
    K = (*addr++) & (VSEBLOCKS_LENS_LEN - 1);

    if (B) {
      __vseblocks_copy16(pblk[B], out);
      pblk[B] += __vseblocks_possLens[K];
      out += __vseblocks_possLens[K];
    } else {
      __vseblocks_zero32(out);
      out += __vseblocks_posszLens[K];
    }
  } while (end > out);
  return addr;
}

void VSEncodingBlocks::encodeArray(const uint32_t *in, const size_t len,
                                   uint32_t *out, size_t &nvalue) {
#ifndef NDEBUG
  const uint32_t *const initout(out);
#endif
  *(out++) = static_cast<uint32_t>(len);

  uint32_t res;
  const uint32_t *lin;
  uint32_t *lout;
  uint32_t csize;

  for (nvalue = 0, res = static_cast<uint32_t>(len), lin = in, lout = out;
       res > VSENCODING_BLOCKSZ; res -= VSENCODING_BLOCKSZ,
      lin += VSENCODING_BLOCKSZ, lout += csize, nvalue += csize + 1) {
    encodeVS(VSENCODING_BLOCKSZ, lin, csize, &__tmp[0]);
    *lout++ = csize;
    memcpy(lout, &__tmp[0], csize * sizeof(uint32_t));
  }

  encodeVS(res, lin, csize, lout);
  lout += csize;
  nvalue += csize;
  ++nvalue;
  ASSERT(nvalue + initout == lout, (lout - initout) << " " << nvalue << " "
                                                    << csize);
}

const uint32_t *VSEncodingBlocks::decodeArray(const uint32_t *in,
#ifndef NDEBUG
                                              const size_t len, uint32_t *out,
                                              size_t &nvalue) {
#else
                                              const size_t, uint32_t *out,
                                              size_t &nvalue) {
#endif
#ifndef NDEBUG
  const uint32_t *const initin(in);
#endif
  const uint32_t *const initout(out);
  const size_t orignvalue = nvalue;
  if ((*in) > orignvalue)
    throw NotEnoughStorage(*in);
  nvalue = *(in++);
  uint32_t res;
  uint32_t sum;

  // __validate(in, (len << 2));
  //__validate(out, ((nvalue + TAIL_MERGIN) << 2));

  for (res = static_cast<uint32_t>(nvalue); res > VSENCODING_BLOCKSZ;
       out += VSENCODING_BLOCKSZ, in += sum, res -= VSENCODING_BLOCKSZ) {
    sum = *in++;
    decodeVS(VSENCODING_BLOCKSZ, in, out, &__tmp[0]);
  }

  const uint32_t *ans = decodeVS(res, in, out, &__tmp[0]);
  assert(initin + len >= in);
  if (initout + orignvalue < out)
    std::cerr << "possible overrun" << std::endl;
  return ans;
}

/* --- Intra functions below --- */

void __vseblocks_unpack1(uint32_t *__restrict__ out,
                         const uint32_t *__restrict__ in, uint32_t bs) {
  // for (uint32_t i = 0; i < bs; i += 32, out += 32, in += 1) {
  for (bs = (bs + 31U) / 32U; bs > 0; out += 32, in += 1,
      --bs) { // remove inf-loop gcc warning (ovflw-related)
    out[0] = in[0] >> 31;
    out[1] = (in[0] >> 30) & 0x01;
    out[2] = (in[0] >> 29) & 0x01;
    out[3] = (in[0] >> 28) & 0x01;
    out[4] = (in[0] >> 27) & 0x01;
    out[5] = (in[0] >> 26) & 0x01;
    out[6] = (in[0] >> 25) & 0x01;
    out[7] = (in[0] >> 24) & 0x01;
    out[8] = (in[0] >> 23) & 0x01;
    out[9] = (in[0] >> 22) & 0x01;
    out[10] = (in[0] >> 21) & 0x01;
    out[11] = (in[0] >> 20) & 0x01;
    out[12] = (in[0] >> 19) & 0x01;
    out[13] = (in[0] >> 18) & 0x01;
    out[14] = (in[0] >> 17) & 0x01;
    out[15] = (in[0] >> 16) & 0x01;
    out[16] = (in[0] >> 15) & 0x01;
    out[17] = (in[0] >> 14) & 0x01;
    out[18] = (in[0] >> 13) & 0x01;
    out[19] = (in[0] >> 12) & 0x01;
    out[20] = (in[0] >> 11) & 0x01;
    out[21] = (in[0] >> 10) & 0x01;
    out[22] = (in[0] >> 9) & 0x01;
    out[23] = (in[0] >> 8) & 0x01;
    out[24] = (in[0] >> 7) & 0x01;
    out[25] = (in[0] >> 6) & 0x01;
    out[26] = (in[0] >> 5) & 0x01;
    out[27] = (in[0] >> 4) & 0x01;
    out[28] = (in[0] >> 3) & 0x01;
    out[29] = (in[0] >> 2) & 0x01;
    out[30] = (in[0] >> 1) & 0x01;
    out[31] = in[0] & 0x01;
  }
}

void __vseblocks_unpack2(uint32_t *__restrict__ out,
                         const uint32_t *__restrict__ in, uint32_t bs) {
  // for (uint32_t i = 0; i < bs; i += 32, out += 32, in += 2) {
  for (bs = (bs + 31U) / 32U; bs > 0; out += 32, in += 2, --bs) {
    out[0] = in[0] >> 30;
    out[1] = (in[0] >> 28) & 0x03;
    out[2] = (in[0] >> 26) & 0x03;
    out[3] = (in[0] >> 24) & 0x03;
    out[4] = (in[0] >> 22) & 0x03;
    out[5] = (in[0] >> 20) & 0x03;
    out[6] = (in[0] >> 18) & 0x03;
    out[7] = (in[0] >> 16) & 0x03;
    out[8] = (in[0] >> 14) & 0x03;
    out[9] = (in[0] >> 12) & 0x03;
    out[10] = (in[0] >> 10) & 0x03;
    out[11] = (in[0] >> 8) & 0x03;
    out[12] = (in[0] >> 6) & 0x03;
    out[13] = (in[0] >> 4) & 0x03;
    out[14] = (in[0] >> 2) & 0x03;
    out[15] = in[0] & 0x03;
    out[16] = in[1] >> 30;
    out[17] = (in[1] >> 28) & 0x03;
    out[18] = (in[1] >> 26) & 0x03;
    out[19] = (in[1] >> 24) & 0x03;
    out[20] = (in[1] >> 22) & 0x03;
    out[21] = (in[1] >> 20) & 0x03;
    out[22] = (in[1] >> 18) & 0x03;
    out[23] = (in[1] >> 16) & 0x03;
    out[24] = (in[1] >> 14) & 0x03;
    out[25] = (in[1] >> 12) & 0x03;
    out[26] = (in[1] >> 10) & 0x03;
    out[27] = (in[1] >> 8) & 0x03;
    out[28] = (in[1] >> 6) & 0x03;
    out[29] = (in[1] >> 4) & 0x03;
    out[30] = (in[1] >> 2) & 0x03;
    out[31] = in[1] & 0x03;
  }
}

void __vseblocks_unpack3(uint32_t *__restrict__ out,
                         const uint32_t *__restrict__ in, uint32_t bs) {
  for (bs = (bs + 31U) / 32U; bs > 0; out += 32, in += 3, --bs) {
    out[0] = in[0] >> 29;
    out[1] = (in[0] >> 26) & 0x07;
    out[2] = (in[0] >> 23) & 0x07;
    out[3] = (in[0] >> 20) & 0x07;
    out[4] = (in[0] >> 17) & 0x07;
    out[5] = (in[0] >> 14) & 0x07;
    out[6] = (in[0] >> 11) & 0x07;
    out[7] = (in[0] >> 8) & 0x07;
    out[8] = (in[0] >> 5) & 0x07;
    out[9] = (in[0] >> 2) & 0x07;
    out[10] = (in[0] << 1) & 0x07;
    out[10] |= in[1] >> 31;
    out[11] = (in[1] >> 28) & 0x07;
    out[12] = (in[1] >> 25) & 0x07;
    out[13] = (in[1] >> 22) & 0x07;
    out[14] = (in[1] >> 19) & 0x07;
    out[15] = (in[1] >> 16) & 0x07;
    out[16] = (in[1] >> 13) & 0x07;
    out[17] = (in[1] >> 10) & 0x07;
    out[18] = (in[1] >> 7) & 0x07;
    out[19] = (in[1] >> 4) & 0x07;
    out[20] = (in[1] >> 1) & 0x07;
    out[21] = (in[1] << 2) & 0x07;
    out[21] |= in[2] >> 30;
    out[22] = (in[2] >> 27) & 0x07;
    out[23] = (in[2] >> 24) & 0x07;
    out[24] = (in[2] >> 21) & 0x07;
    out[25] = (in[2] >> 18) & 0x07;
    out[26] = (in[2] >> 15) & 0x07;
    out[27] = (in[2] >> 12) & 0x07;
    out[28] = (in[2] >> 9) & 0x07;
    out[29] = (in[2] >> 6) & 0x07;
    out[30] = (in[2] >> 3) & 0x07;
    out[31] = in[2] & 0x07;
  }
}

void __vseblocks_unpack4(uint32_t *__restrict__ out,
                         const uint32_t *__restrict__ in, uint32_t bs) {
  for (bs = (bs + 31U) / 32U; bs > 0; out += 32, in += 4, --bs) {
    out[0] = in[0] >> 28;
    out[1] = (in[0] >> 24) & 0x0f;
    out[2] = (in[0] >> 20) & 0x0f;
    out[3] = (in[0] >> 16) & 0x0f;
    out[4] = (in[0] >> 12) & 0x0f;
    out[5] = (in[0] >> 8) & 0x0f;
    out[6] = (in[0] >> 4) & 0x0f;
    out[7] = in[0] & 0x0f;
    out[8] = in[1] >> 28;
    out[9] = (in[1] >> 24) & 0x0f;
    out[10] = (in[1] >> 20) & 0x0f;
    out[11] = (in[1] >> 16) & 0x0f;
    out[12] = (in[1] >> 12) & 0x0f;
    out[13] = (in[1] >> 8) & 0x0f;
    out[14] = (in[1] >> 4) & 0x0f;
    out[15] = in[1] & 0x0f;
    out[16] = in[2] >> 28;
    out[17] = (in[2] >> 24) & 0x0f;
    out[18] = (in[2] >> 20) & 0x0f;
    out[19] = (in[2] >> 16) & 0x0f;
    out[20] = (in[2] >> 12) & 0x0f;
    out[21] = (in[2] >> 8) & 0x0f;
    out[22] = (in[2] >> 4) & 0x0f;
    out[23] = in[2] & 0x0f;
    out[24] = in[3] >> 28;
    out[25] = (in[3] >> 24) & 0x0f;
    out[26] = (in[3] >> 20) & 0x0f;
    out[27] = (in[3] >> 16) & 0x0f;
    out[28] = (in[3] >> 12) & 0x0f;
    out[29] = (in[3] >> 8) & 0x0f;
    out[30] = (in[3] >> 4) & 0x0f;
    out[31] = in[3] & 0x0f;
  }
}

void __vseblocks_unpack5(uint32_t *__restrict__ out,
                         const uint32_t *__restrict__ in, uint32_t bs) {
  for (bs = (bs + 31U) / 32U; bs > 0; out += 32, in += 5, --bs) {
    out[0] = in[0] >> 27;
    out[1] = (in[0] >> 22) & 0x1f;
    out[2] = (in[0] >> 17) & 0x1f;
    out[3] = (in[0] >> 12) & 0x1f;
    out[4] = (in[0] >> 7) & 0x1f;
    out[5] = (in[0] >> 2) & 0x1f;
    out[6] = (in[0] << 3) & 0x1f;
    out[6] |= in[1] >> 29;
    out[7] = (in[1] >> 24) & 0x1f;
    out[8] = (in[1] >> 19) & 0x1f;
    out[9] = (in[1] >> 14) & 0x1f;
    out[10] = (in[1] >> 9) & 0x1f;
    out[11] = (in[1] >> 4) & 0x1f;
    out[12] = (in[1] << 1) & 0x1f;
    out[12] |= in[2] >> 0x1f;
    out[13] = (in[2] >> 26) & 0x1f;
    out[14] = (in[2] >> 21) & 0x1f;
    out[15] = (in[2] >> 16) & 0x1f;
    out[16] = (in[2] >> 11) & 0x1f;
    out[17] = (in[2] >> 6) & 0x1f;
    out[18] = (in[2] >> 1) & 0x1f;
    out[19] = (in[2] << 4) & 0x1f;
    out[19] |= in[3] >> 28;
    out[20] = (in[3] >> 23) & 0x1f;
    out[21] = (in[3] >> 18) & 0x1f;
    out[22] = (in[3] >> 13) & 0x1f;
    out[23] = (in[3] >> 8) & 0x1f;
    out[24] = (in[3] >> 3) & 0x1f;
    out[25] = (in[3] << 2) & 0x1f;
    out[25] |= in[4] >> 30;
    out[26] = (in[4] >> 25) & 0x1f;
    out[27] = (in[4] >> 20) & 0x1f;
    out[28] = (in[4] >> 15) & 0x1f;
    out[29] = (in[4] >> 10) & 0x1f;
    out[30] = (in[4] >> 5) & 0x1f;
    out[31] = in[4] & 0x1f;
  }
}

void __vseblocks_unpack6(uint32_t *__restrict__ out,
                         const uint32_t *__restrict__ in, uint32_t bs) {
  for (bs = (bs + 31U) / 32U; bs > 0; out += 32, in += 6, --bs) {
    out[0] = in[0] >> 26;
    out[1] = (in[0] >> 20) & 0x3f;
    out[2] = (in[0] >> 14) & 0x3f;
    out[3] = (in[0] >> 8) & 0x3f;
    out[4] = (in[0] >> 2) & 0x3f;
    out[5] = (in[0] << 4) & 0x3f;
    out[5] |= in[1] >> 28;
    out[6] = (in[1] >> 22) & 0x3f;
    out[7] = (in[1] >> 16) & 0x3f;
    out[8] = (in[1] >> 10) & 0x3f;
    out[9] = (in[1] >> 4) & 0x3f;
    out[10] = (in[1] << 2) & 0x3f;
    out[10] |= in[2] >> 30;
    out[11] = (in[2] >> 24) & 0x3f;
    out[12] = (in[2] >> 18) & 0x3f;
    out[13] = (in[2] >> 12) & 0x3f;
    out[14] = (in[2] >> 6) & 0x3f;
    out[15] = in[2] & 0x3f;
    out[16] = in[3] >> 26;
    out[17] = (in[3] >> 20) & 0x3f;
    out[18] = (in[3] >> 14) & 0x3f;
    out[19] = (in[3] >> 8) & 0x3f;
    out[20] = (in[3] >> 2) & 0x3f;
    out[21] = (in[3] << 4) & 0x3f;
    out[21] |= in[4] >> 28;
    out[22] = (in[4] >> 22) & 0x3f;
    out[23] = (in[4] >> 16) & 0x3f;
    out[24] = (in[4] >> 10) & 0x3f;
    out[25] = (in[4] >> 4) & 0x3f;
    out[26] = (in[4] << 2) & 0x3f;
    out[26] |= in[5] >> 30;
    out[27] = (in[5] >> 24) & 0x3f;
    out[28] = (in[5] >> 18) & 0x3f;
    out[29] = (in[5] >> 12) & 0x3f;
    out[30] = (in[5] >> 6) & 0x3f;
    out[31] = in[5] & 0x3f;
  }
}

void __vseblocks_unpack7(uint32_t *__restrict__ out,
                         const uint32_t *__restrict__ in, uint32_t bs) {
  for (bs = (bs + 31U) / 32U; bs > 0; out += 32, in += 7, --bs) {
    out[0] = in[0] >> 25;
    out[1] = (in[0] >> 18) & 0x7f;
    out[2] = (in[0] >> 11) & 0x7f;
    out[3] = (in[0] >> 4) & 0x7f;
    out[4] = (in[0] << 3) & 0x7f;
    out[4] |= in[1] >> 29;
    out[5] = (in[1] >> 22) & 0x7f;
    out[6] = (in[1] >> 15) & 0x7f;
    out[7] = (in[1] >> 8) & 0x7f;
    out[8] = (in[1] >> 1) & 0x7f;
    out[9] = (in[1] << 6) & 0x7f;
    out[9] |= in[2] >> 26;
    out[10] = (in[2] >> 19) & 0x7f;
    out[11] = (in[2] >> 12) & 0x7f;
    out[12] = (in[2] >> 5) & 0x7f;
    out[13] = (in[2] << 2) & 0x7f;
    out[13] |= in[3] >> 30;
    out[14] = (in[3] >> 23) & 0x7f;
    out[15] = (in[3] >> 16) & 0x7f;
    out[16] = (in[3] >> 9) & 0x7f;
    out[17] = (in[3] >> 2) & 0x7f;
    out[18] = (in[3] << 5) & 0x7f;
    out[18] |= in[4] >> 27;
    out[19] = (in[4] >> 20) & 0x7f;
    out[20] = (in[4] >> 13) & 0x7f;
    out[21] = (in[4] >> 6) & 0x7f;
    out[22] = (in[4] << 1) & 0x7f;
    out[22] |= in[5] >> 31;
    out[23] = (in[5] >> 24) & 0x7f;
    out[24] = (in[5] >> 17) & 0x7f;
    out[25] = (in[5] >> 10) & 0x7f;
    out[26] = (in[5] >> 3) & 0x7f;
    out[27] = (in[5] << 4) & 0x7f;
    out[27] |= in[6] >> 28;
    out[28] = (in[6] >> 21) & 0x7f;
    out[29] = (in[6] >> 14) & 0x7f;
    out[30] = (in[6] >> 7) & 0x7f;
    out[31] = in[6] & 0x7f;
  }
}

void __vseblocks_unpack8(uint32_t *__restrict__ out,
                         const uint32_t *__restrict__ in, uint32_t bs) {
  for (bs = (bs + 31U) / 32U; bs > 0; out += 32, in += 8, --bs) {
    out[0] = in[0] >> 24;
    out[1] = (in[0] >> 16) & 0xff;
    out[2] = (in[0] >> 8) & 0xff;
    out[3] = in[0] & 0xff;
    out[4] = in[1] >> 24;
    out[5] = (in[1] >> 16) & 0xff;
    out[6] = (in[1] >> 8) & 0xff;
    out[7] = in[1] & 0xff;
    out[8] = in[2] >> 24;
    out[9] = (in[2] >> 16) & 0xff;
    out[10] = (in[2] >> 8) & 0xff;
    out[11] = in[2] & 0xff;
    out[12] = in[3] >> 24;
    out[13] = (in[3] >> 16) & 0xff;
    out[14] = (in[3] >> 8) & 0xff;
    out[15] = in[3] & 0xff;
    out[16] = in[4] >> 24;
    out[17] = (in[4] >> 16) & 0xff;
    out[18] = (in[4] >> 8) & 0xff;
    out[19] = in[4] & 0xff;
    out[20] = in[5] >> 24;
    out[21] = (in[5] >> 16) & 0xff;
    out[22] = (in[5] >> 8) & 0xff;
    out[23] = in[5] & 0xff;
    out[24] = in[6] >> 24;
    out[25] = (in[6] >> 16) & 0xff;
    out[26] = (in[6] >> 8) & 0xff;
    out[27] = in[6] & 0xff;
    out[28] = in[7] >> 24;
    out[29] = (in[7] >> 16) & 0xff;
    out[30] = (in[7] >> 8) & 0xff;
    out[31] = in[7] & 0xff;
  }
}

void __vseblocks_unpack9(uint32_t *__restrict__ out,
                         const uint32_t *__restrict__ in, uint32_t bs) {
  for (bs = (bs + 31U) / 32U; bs > 0; out += 32, in += 9, --bs) {
    out[0] = in[0] >> 23;
    out[1] = (in[0] >> 14) & 0x01ff;
    out[2] = (in[0] >> 5) & 0x01ff;
    out[3] = (in[0] << 4) & 0x01ff;
    out[3] |= in[1] >> 28;
    out[4] = (in[1] >> 19) & 0x01ff;
    out[5] = (in[1] >> 10) & 0x01ff;
    out[6] = (in[1] >> 1) & 0x01ff;
    out[7] = (in[1] << 8) & 0x01ff;
    out[7] |= in[2] >> 24;
    out[8] = (in[2] >> 15) & 0x01ff;
    out[9] = (in[2] >> 6) & 0x01ff;
    out[10] = (in[2] << 3) & 0x01ff;
    out[10] |= in[3] >> 29;
    out[11] = (in[3] >> 20) & 0x01ff;
    out[12] = (in[3] >> 11) & 0x01ff;
    out[13] = (in[3] >> 2) & 0x01ff;
    out[14] = (in[3] << 7) & 0x01ff;
    out[14] |= in[4] >> 25;
    out[15] = (in[4] >> 16) & 0x01ff;
    out[16] = (in[4] >> 7) & 0x01ff;
    out[17] = (in[4] << 2) & 0x01ff;
    out[17] |= in[5] >> 30;
    out[18] = (in[5] >> 21) & 0x01ff;
    out[19] = (in[5] >> 12) & 0x01ff;
    out[20] = (in[5] >> 3) & 0x01ff;
    out[21] = (in[5] << 6) & 0x01ff;
    out[21] |= in[6] >> 26;
    out[22] = (in[6] >> 17) & 0x01ff;
    out[23] = (in[6] >> 8) & 0x01ff;
    out[24] = (in[6] << 1) & 0x01ff;
    out[24] |= in[7] >> 31;
    out[25] = (in[7] >> 22) & 0x01ff;
    out[26] = (in[7] >> 13) & 0x01ff;
    out[27] = (in[7] >> 4) & 0x01ff;
    out[28] = (in[7] << 5) & 0x01ff;
    out[28] |= in[8] >> 27;
    out[29] = (in[8] >> 18) & 0x01ff;
    out[30] = (in[8] >> 9) & 0x01ff;
    out[31] = in[8] & 0x01ff;
  }
}

void __vseblocks_unpack10(uint32_t *__restrict__ out,
                          const uint32_t *__restrict__ in, uint32_t bs) {
  for (bs = (bs + 31U) / 32U; bs > 0; out += 32, in += 10, --bs) {
    out[0] = in[0] >> 22;
    out[1] = (in[0] >> 12) & 0x03ff;
    out[2] = (in[0] >> 2) & 0x03ff;
    out[3] = (in[0] << 8) & 0x03ff;
    out[3] |= in[1] >> 24;
    out[4] = (in[1] >> 14) & 0x03ff;
    out[5] = (in[1] >> 4) & 0x03ff;
    out[6] = (in[1] << 6) & 0x03ff;
    out[6] |= in[2] >> 26;
    out[7] = (in[2] >> 16) & 0x03ff;
    out[8] = (in[2] >> 6) & 0x03ff;
    out[9] = (in[2] << 4) & 0x03ff;
    out[9] |= in[3] >> 28;
    out[10] = (in[3] >> 18) & 0x03ff;
    out[11] = (in[3] >> 8) & 0x03ff;
    out[12] = (in[3] << 2) & 0x03ff;
    out[12] |= in[4] >> 30;
    out[13] = (in[4] >> 20) & 0x03ff;
    out[14] = (in[4] >> 10) & 0x03ff;
    out[15] = in[4] & 0x03ff;
    out[16] = in[5] >> 22;
    out[17] = (in[5] >> 12) & 0x03ff;
    out[18] = (in[5] >> 2) & 0x03ff;
    out[19] = (in[5] << 8) & 0x03ff;
    out[19] |= in[6] >> 24;
    out[20] = (in[6] >> 14) & 0x03ff;
    out[21] = (in[6] >> 4) & 0x03ff;
    out[22] = (in[6] << 6) & 0x03ff;
    out[22] |= in[7] >> 26;
    out[23] = (in[7] >> 16) & 0x03ff;
    out[24] = (in[7] >> 6) & 0x03ff;
    out[25] = (in[7] << 4) & 0x03ff;
    out[25] |= in[8] >> 28;
    out[26] = (in[8] >> 18) & 0x03ff;
    out[27] = (in[8] >> 8) & 0x03ff;
    out[28] = (in[8] << 2) & 0x03ff;
    out[28] |= in[9] >> 30;
    out[29] = (in[9] >> 20) & 0x03ff;
    out[30] = (in[9] >> 10) & 0x03ff;
    out[31] = in[9] & 0x03ff;
  }
}

void __vseblocks_unpack11(uint32_t *__restrict__ out,
                          const uint32_t *__restrict__ in, uint32_t bs) {
  for (bs = (bs + 31U) / 32U; bs > 0; out += 32, in += 11, --bs) {
    out[0] = in[0] >> 21;
    out[1] = (in[0] >> 10) & 0x07ff;
    out[2] = (in[0] << 1) & 0x07ff;
    out[2] |= in[1] >> 31;
    out[3] = (in[1] >> 20) & 0x07ff;
    out[4] = (in[1] >> 9) & 0x07ff;
    out[5] = (in[1] << 2) & 0x07ff;
    out[5] |= in[2] >> 30;
    out[6] = (in[2] >> 19) & 0x07ff;
    out[7] = (in[2] >> 8) & 0x07ff;
    out[8] = (in[2] << 3) & 0x07ff;
    out[8] |= in[3] >> 29;
    out[9] = (in[3] >> 18) & 0x07ff;
    out[10] = (in[3] >> 7) & 0x07ff;
    out[11] = (in[3] << 4) & 0x07ff;
    out[11] |= in[4] >> 28;
    out[12] = (in[4] >> 17) & 0x07ff;
    out[13] = (in[4] >> 6) & 0x07ff;
    out[14] = (in[4] << 5) & 0x07ff;
    out[14] |= in[5] >> 27;
    out[15] = (in[5] >> 16) & 0x07ff;
    out[16] = (in[5] >> 5) & 0x07ff;
    out[17] = (in[5] << 6) & 0x07ff;
    out[17] |= in[6] >> 26;
    out[18] = (in[6] >> 15) & 0x07ff;
    out[19] = (in[6] >> 4) & 0x07ff;
    out[20] = (in[6] << 7) & 0x07ff;
    out[20] |= in[7] >> 25;
    out[21] = (in[7] >> 14) & 0x07ff;
    out[22] = (in[7] >> 3) & 0x07ff;
    out[23] = (in[7] << 8) & 0x07ff;
    out[23] |= in[8] >> 24;
    out[24] = (in[8] >> 13) & 0x07ff;
    out[25] = (in[8] >> 2) & 0x07ff;
    out[26] = (in[8] << 9) & 0x07ff;
    out[26] |= in[9] >> 23;
    out[27] = (in[9] >> 12) & 0x07ff;
    out[28] = (in[9] >> 1) & 0x07ff;
    out[29] = (in[9] << 10) & 0x07ff;
    out[29] |= in[10] >> 22;
    out[30] = (in[10] >> 11) & 0x07ff;
    out[31] = in[10] & 0x07ff;
  }
}

void __vseblocks_unpack12(uint32_t *__restrict__ out,
                          const uint32_t *__restrict__ in, uint32_t bs) {
  for (bs = (bs + 31U) / 32U; bs > 0; out += 32, in += 12, --bs) {
    out[0] = in[0] >> 20;
    out[1] = (in[0] >> 8) & 0x0fff;
    out[2] = (in[0] << 4) & 0x0fff;
    out[2] |= in[1] >> 28;
    out[3] = (in[1] >> 16) & 0x0fff;
    out[4] = (in[1] >> 4) & 0x0fff;
    out[5] = (in[1] << 8) & 0x0fff;
    out[5] |= in[2] >> 24;
    out[6] = (in[2] >> 12) & 0x0fff;
    out[7] = in[2] & 0x0fff;
    out[8] = in[3] >> 20;
    out[9] = (in[3] >> 8) & 0x0fff;
    out[10] = (in[3] << 4) & 0x0fff;
    out[10] |= in[4] >> 28;
    out[11] = (in[4] >> 16) & 0x0fff;
    out[12] = (in[4] >> 4) & 0x0fff;
    out[13] = (in[4] << 8) & 0x0fff;
    out[13] |= in[5] >> 24;
    out[14] = (in[5] >> 12) & 0x0fff;
    out[15] = in[5] & 0x0fff;
    out[16] = in[6] >> 20;
    out[17] = (in[6] >> 8) & 0x0fff;
    out[18] = (in[6] << 4) & 0x0fff;
    out[18] |= in[7] >> 28;
    out[19] = (in[7] >> 16) & 0x0fff;
    out[20] = (in[7] >> 4) & 0x0fff;
    out[21] = (in[7] << 8) & 0x0fff;
    out[21] |= in[8] >> 24;
    out[22] = (in[8] >> 12) & 0x0fff;
    out[23] = in[8] & 0x0fff;
    out[24] = in[9] >> 20;
    out[25] = (in[9] >> 8) & 0x0fff;
    out[26] = (in[9] << 4) & 0x0fff;
    out[26] |= in[10] >> 28;
    out[27] = (in[10] >> 16) & 0x0fff;
    out[28] = (in[10] >> 4) & 0x0fff;
    out[29] = (in[10] << 8) & 0x0fff;
    out[29] |= in[11] >> 24;
    out[30] = (in[11] >> 12) & 0x0fff;
    out[31] = in[11] & 0x0fff;
  }
}

void __vseblocks_unpack16(uint32_t *__restrict__ out,
                          const uint32_t *__restrict__ in, uint32_t bs) {
  for (bs = (bs + 31U) / 32U; bs > 0; out += 32, in += 16, --bs) {
    out[0] = in[0] >> 16;
    out[1] = in[0] & 0xffff;
    out[2] = in[1] >> 16;
    out[3] = in[1] & 0xffff;
    out[4] = in[2] >> 16;
    out[5] = in[2] & 0xffff;
    out[6] = in[3] >> 16;
    out[7] = in[3] & 0xffff;
    out[8] = in[4] >> 16;
    out[9] = in[4] & 0xffff;
    out[10] = in[5] >> 16;
    out[11] = in[5] & 0xffff;
    out[12] = in[6] >> 16;
    out[13] = in[6] & 0xffff;
    out[14] = in[7] >> 16;
    out[15] = in[7] & 0xffff;
    out[16] = in[8] >> 16;
    out[17] = in[8] & 0xffff;
    out[18] = in[9] >> 16;
    out[19] = in[9] & 0xffff;
    out[20] = in[10] >> 16;
    out[21] = in[10] & 0xffff;
    out[22] = in[11] >> 16;
    out[23] = in[11] & 0xffff;
    out[24] = in[12] >> 16;
    out[25] = in[12] & 0xffff;
    out[26] = in[13] >> 16;
    out[27] = in[13] & 0xffff;
    out[28] = in[14] >> 16;
    out[29] = in[14] & 0xffff;
    out[30] = in[15] >> 16;
    out[31] = in[15] & 0xffff;
  }
}

void __vseblocks_unpack20(uint32_t *__restrict__ out,
                          const uint32_t *__restrict__ in, uint32_t bs) {
  for (bs = (bs + 31U) / 32U; bs > 0; out += 32, in += 20, --bs) {
    out[0] = in[0] >> 12;
    out[1] = (in[0] << 8) & 0x0fffff;
    out[1] |= in[1] >> 24;
    out[2] = (in[1] >> 4) & 0x0fffff;
    out[3] = (in[1] << 16) & 0x0fffff;
    out[3] |= in[2] >> 16;
    out[4] = (in[2] << 4) & 0x0fffff;
    out[4] |= in[3] >> 28;
    out[5] = (in[3] >> 8) & 0x0fffff;
    out[6] = (in[3] << 12) & 0x0fffff;
    out[6] |= in[4] >> 20;
    out[7] = in[4] & 0x0fffff;
    out[8] = in[5] >> 12;
    out[9] = (in[5] << 8) & 0x0fffff;
    out[9] |= in[6] >> 24;
    out[10] = (in[6] >> 4) & 0x0fffff;
    out[11] = (in[6] << 16) & 0x0fffff;
    out[11] |= in[7] >> 16;
    out[12] = (in[7] << 4) & 0x0fffff;
    out[12] |= in[8] >> 28;
    out[13] = (in[8] >> 8) & 0x0fffff;
    out[14] = (in[8] << 12) & 0x0fffff;
    out[14] |= in[9] >> 20;
    out[15] = in[9] & 0x0fffff;
    out[16] = in[10] >> 12;
    out[17] = (in[10] << 8) & 0x0fffff;
    out[17] |= in[11] >> 24;
    out[18] = (in[11] >> 4) & 0x0fffff;
    out[19] = (in[11] << 16) & 0x0fffff;
    out[19] |= in[12] >> 16;
    out[20] = (in[12] << 4) & 0x0fffff;
    out[20] |= in[13] >> 28;
    out[21] = (in[13] >> 8) & 0x0fffff;
    out[22] = (in[13] << 12) & 0x0fffff;
    out[22] |= in[14] >> 20;
    out[23] = in[14] & 0x0fffff;
    out[24] = in[15] >> 12;
    out[25] = (in[15] << 8) & 0x0fffff;
    out[25] |= in[16] >> 24;
    out[26] = (in[16] >> 4) & 0x0fffff;
    out[27] = (in[16] << 16) & 0x0fffff;
    out[27] |= in[17] >> 16;
    out[28] = (in[17] << 4) & 0x0fffff;
    out[28] |= in[18] >> 28;
    out[29] = (in[18] >> 8) & 0x0fffff;
    out[30] = (in[18] << 12) & 0x0fffff;
    out[30] |= in[19] >> 20;
    out[31] = in[19] & 0x0fffff;
  }
}

void __vseblocks_unpack32(uint32_t *__restrict__ out,
                          const uint32_t *__restrict__ in, uint32_t bs) {
  for (bs = (bs + 15U) / 16U; bs > 0U; out += 16, in += 16, --bs) {
    __vseblocks_copy16(in, out);
  }
}

} // namespace vsencoding

#if !defined(__clang__) && !defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#pragma GCC diagnostic pop
#endif
#endif /* VSENCODING_H_ */
