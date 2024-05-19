// StreamVByte data format:
//   [count] [count * 2-bits per key] [count * vbyte ints]
//   (out)   | (key)->              | (data)->           |
//   4 bytes | (count+3)/4 bytes    | max of count * 4B  |
// each 8-bit key has four 2-bit lengths: 00=1B, 01=2B, 10=3B, 11=4B
// no particular alignment is assumed or guaranteed for any elements

#if defined(_MSC_VER)
    /* Microsoft C/C++-compatible compiler */
    #if (defined(_M_IX86) || defined(_M_AMD64))
    #include <intrin.h>
    #elif defined(_M_ARM64)
    #include <simde/x86/sse4.1.h>
    #endif

    #include <iso646.h>
    #include <stdint.h>
    #define __restrict__ __restrict
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
    /* GCC-compatible compiler, targeting x86/x86-64 */
    #include <x86intrin.h>
#elif defined(__aarch64__)
    /* GCC-compatible compiler, targeting ARM with NEON */
    #include <simde/x86/sse4.1.h>
#elif defined(__GNUC__) && defined(__IWMMXT__)
    /* GCC-compatible compiler, targeting ARM with WMMX */
    #include <mmintrin.h>
#elif (defined(__GNUC__) || defined(__xlC__)) && (defined(__VEC__) || defined(__ALTIVEC__))
    /* XLC or GCC-compatible compiler, targeting PowerPC with VMX/VSX */
    #include <altivec.h>
#elif defined(__GNUC__) && defined(__SPE__)
    /* GCC-compatible compiler, targeting PowerPC with SPE */
    #include <spe.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#if defined(_MSC_VER)
#define ALIGNED(x) __declspec(align(x))
#else
#if defined(__GNUC__)
#define ALIGNED(x) __attribute__((aligned(x)))
#endif
#endif

typedef __m128i xmm_t;

#ifdef __AVX2__
typedef __m256i ymm_t;
#define YMM(x) _mm256_castsi128_si256(x)
#define XMM(y) _mm256_castsi256_si128(y)
#endif // __AVX2__

#ifdef IACA
#include </opt/intel/iaca/include/iacaMarks.h>
#else // not IACA
#undef IACA_START
#define IACA_START
#undef IACA_END
#define IACA_END
#endif // not IACA

#if defined(_MSC_VER) && !defined(__clang__)
static inline int __builtin_ctz(uint32_t x) {
  unsigned long ret;
  _BitScanForward(&ret, x);
  return (int)ret;
}
#endif

static inline uint8_t _encoded_length(uint32_t val) {
  if (val < (1 << 8)) {
    return 1;
  }
  if (val < (1 << 16)) {
    return 2;
  }
  if (val < (1 << 24)) {
    return 3;
  }
  return 4;
}

static inline uint8_t _encode_data(uint32_t val,
                                   uint8_t *__restrict__ *dataPtrPtr) {
  uint8_t *dataPtr = *dataPtrPtr;
  uint8_t code;

  if (val < (1 << 8)) { // 1 byte
    *dataPtr = (uint8_t)(val);
    *dataPtrPtr += 1;
    code = 0;
  } else if (val < (1 << 16)) { // 2 bytes
    *(uint16_t *)dataPtr = (uint16_t)(val);
    *dataPtrPtr += 2;
    code = 1;
  } else if (val < (1 << 24)) { // 3 bytes
    *(uint16_t *)dataPtr = (uint16_t)(val);
    *(dataPtr + 2) = (uint8_t)(val >> 16);
    *dataPtrPtr += 3;
    code = 2;
  } else { // 4 bytes
    *(uint32_t *)dataPtr = val;
    *dataPtrPtr += 4;
    code = 3;
  }

  return code;
}

uint8_t *svb_encode_scalar_d1_init(const uint32_t *in,
                                   uint8_t *__restrict__ keyPtr,
                                   uint8_t *__restrict__ dataPtr,
                                   uint32_t count, uint32_t prev) {
  if (count == 0)
    return dataPtr; // exit immediately if no data

  uint8_t shift = 0; // cycles 0, 2, 4, 6, 0, 2, 4, 6, ...
  uint8_t key = 0;
  for (uint32_t c = 0; c < count; c++) {
    if (shift == 8) {
      shift = 0;
      *keyPtr++ = key;
      key = 0;
    }
    uint32_t val = in[c] - prev;
    prev = in[c];
    uint8_t code = _encode_data(val, &dataPtr);
    key |= code << shift;
    shift += 2;
  }

  *keyPtr = key;  // write last key (no increment needed)
  return dataPtr; // pointer to first unused data byte
}

uint8_t *svb_encode_scalar_d1(const uint32_t *in, uint8_t *__restrict__ keyPtr,
                              uint8_t *__restrict__ dataPtr, uint32_t count) {
  return svb_encode_scalar_d1_init(in, keyPtr, dataPtr, count, 0);
}

uint8_t *svb_encode_scalar(const uint32_t *in, uint8_t *__restrict__ keyPtr,
                           uint8_t *__restrict__ dataPtr, uint32_t count) {
  if (count == 0)
    return dataPtr; // exit immediately if no data

  uint8_t shift = 0; // cycles 0, 2, 4, 6, 0, 2, 4, 6, ...
  uint8_t key = 0;
  for (uint32_t c = 0; c < count; c++) {
    if (shift == 8) {
      shift = 0;
      *keyPtr++ = key;
      key = 0;
    }
    uint32_t val = in[c];
    uint8_t code = _encode_data(val, &dataPtr);
    key |= code << shift;
    shift += 2;
  }

  *keyPtr = key;  // write last key (no increment needed)
  return dataPtr; // pointer to first unused data byte
}

static inline uint32_t _decode_data(uint8_t **dataPtrPtr, uint8_t code) {
  uint8_t *dataPtr = *dataPtrPtr;
  uint32_t val;

  if (code == 0) { // 1 byte
    val = (uint32_t)*dataPtr;
    dataPtr += 1;
  } else if (code == 1) { // 2 bytes
    val = (uint32_t) * (uint16_t *)dataPtr;
    dataPtr += 2;
  } else if (code == 2) { // 3 bytes
    val = (uint32_t) * (uint16_t *)dataPtr;
    val |= *(dataPtr + 2) << 16;
    dataPtr += 3;
  } else {                      // code == 3
    val = *(uint32_t *)dataPtr; // 4 bytes
    dataPtr += 4;
  }

  *dataPtrPtr = dataPtr;
  return val;
}

uint8_t *svb_append_scalar_d1(uint8_t *keyPtr, uint8_t *dataPtr,
                              size_t sizebytes, size_t count, uint32_t delta) {
  uint32_t keyLen = (uint32_t)(count + 3) / 4; // 2-bits rounded to full byte

  // make space for additional keys?
  if (count >= keyLen * 4) {
    memmove(dataPtr + 1, dataPtr, sizebytes - keyLen);
    *dataPtr = 0;
    dataPtr++;
    sizebytes++;
    keyLen++;
  }

  keyPtr += count / 4;
  dataPtr += sizebytes - keyLen;
  int shift = (count % 4) * 2;

  uint8_t code = _encode_data(delta, &dataPtr);
  *keyPtr |= code << shift;
  return dataPtr;
}

uint8_t *svb_insert_scalar_d1_init(uint8_t *keyPtr, uint8_t *dataPtr,
                                   size_t dataSize, uint32_t count,
                                   uint32_t prev, uint32_t new_key,
                                   uint32_t *position) {
  if (count == 0) {
    *position = 0;
    return svb_encode_scalar_d1_init(&new_key, keyPtr, dataPtr, 1, prev);
  }

  uint8_t shift = 0;
  uint32_t key = *keyPtr;
  uint8_t *dataPtrBegin = dataPtr;

  for (uint32_t c = 0; c < count; c++) {
    uint8_t *dataPtrPrev = dataPtr;

    if (shift == 8) {
      shift = 0;
      key = *(++keyPtr);
    }

    uint8_t current_key_code = (key >> shift) & 0x3;
    uint32_t current_key = prev + _decode_data(&dataPtr, current_key_code);
    if (current_key >= new_key) {
      // rewind to the insertion position
      dataPtr = dataPtrPrev;

      // shift keys 2 bits "to the right"
      uint32_t mask_hi = key & (~0u << shift);
      uint32_t mask_lo = key & ((1 << shift) - 1);
      key = (mask_hi << 2) | mask_lo;
      uint32_t carry_bits, prev_carry_bits = (key & (3 << 8)) >> 8;

      for (uint8_t *p = keyPtr + 1; p < dataPtrBegin; p++) {
        carry_bits = (*p & (3 << 6)) >> 6;
        *p <<= 2;
        *p |= prev_carry_bits;
        prev_carry_bits = carry_bits;
      }

      // shift values n bytes "to the right", then insert the new key
      // and the updated delta of the next key
      int gap = _encoded_length(new_key - prev) +
                _encoded_length(current_key - new_key) - (current_key_code + 1);
      assert(gap >= 0);
      if (gap > 0)
        memmove(dataPtr + gap, dataPtr, dataSize - (dataPtr - dataPtrBegin));

      // first insert the new key
      uint8_t code = _encode_data(new_key - prev, &dataPtr);
      *keyPtr = key | (code << shift);

      // then update the current key
      shift += 2;
      if (shift == 8) {
        shift = 0;
        keyPtr++;
      }
      code = _encode_data(current_key - new_key, &dataPtr);
      *keyPtr &= ~(3 << shift);
      *keyPtr |= (code << shift);

      *position = c;
      return dataPtrBegin + dataSize + gap;
    }

    prev = current_key;
    shift += 2;
  }

  // append the new key at the end
  if (shift == 8) {
    shift = 0;
    keyPtr++;
  }

  uint8_t code = _encode_data(new_key - prev, &dataPtr);
  key &= ~(3 << shift);
  key |= code << shift;
  *keyPtr = key; // write last key (no increment needed)

  *position = count;
  return dataPtrBegin + dataSize + code + 1;
}

uint8_t *svb_decode_scalar_d1_init(uint32_t *outPtr, const uint8_t *keyPtr,
                                   uint8_t *dataPtr, uint32_t count,
                                   uint32_t prev) {
  if (count == 0)
    return dataPtr; // no reads or writes if no data

  uint8_t shift = 0;
  uint32_t key = *keyPtr++;

  for (uint32_t c = 0; c < count; c++) {
    if (shift == 8) {
      shift = 0;
      key = *keyPtr++;
    }
    uint32_t val = _decode_data(&dataPtr, (key >> shift) & 0x3);
    val += prev;
    *outPtr++ = val;
    prev = val;
    shift += 2;
  }

  return dataPtr; // pointer to first unused byte after end
}

uint8_t *svb_decode_scalar_d1(uint32_t *outPtr, const uint8_t *keyPtr,
                              uint8_t *dataPtr, uint32_t count) {
  return svb_decode_scalar_d1_init(outPtr, keyPtr, dataPtr, count, 0);
}

uint8_t *svb_decode_scalar(uint32_t *outPtr, const uint8_t *keyPtr,
                           uint8_t *dataPtr, uint32_t count) {
  if (count == 0)
    return dataPtr; // no reads or writes if no data

  uint8_t shift = 0;
  uint32_t key = *keyPtr++;
  for (uint32_t c = 0; c < count; c++) {
    if (shift == 8) {
      shift = 0;
      key = *keyPtr++;
    }
    uint32_t val = _decode_data(&dataPtr, (key >> shift) & 0x3);
    *outPtr++ = val;
    shift += 2;
  }

  return dataPtr; // pointer to first unused byte after end
}

static const uint8_t lengthTable[256] = {
    4,  5,  6,  7,  5,  6,  7,  8,  6,  7,  8,  9,  7,  8,  9,  10, 5,  6,  7,
    8,  6,  7,  8,  9,  7,  8,  9,  10, 8,  9,  10, 11, 6,  7,  8,  9,  7,  8,
    9,  10, 8,  9,  10, 11, 9,  10, 11, 12, 7,  8,  9,  10, 8,  9,  10, 11, 9,
    10, 11, 12, 10, 11, 12, 13, 5,  6,  7,  8,  6,  7,  8,  9,  7,  8,  9,  10,
    8,  9,  10, 11, 6,  7,  8,  9,  7,  8,  9,  10, 8,  9,  10, 11, 9,  10, 11,
    12, 7,  8,  9,  10, 8,  9,  10, 11, 9,  10, 11, 12, 10, 11, 12, 13, 8,  9,
    10, 11, 9,  10, 11, 12, 10, 11, 12, 13, 11, 12, 13, 14, 6,  7,  8,  9,  7,
    8,  9,  10, 8,  9,  10, 11, 9,  10, 11, 12, 7,  8,  9,  10, 8,  9,  10, 11,
    9,  10, 11, 12, 10, 11, 12, 13, 8,  9,  10, 11, 9,  10, 11, 12, 10, 11, 12,
    13, 11, 12, 13, 14, 9,  10, 11, 12, 10, 11, 12, 13, 11, 12, 13, 14, 12, 13,
    14, 15, 7,  8,  9,  10, 8,  9,  10, 11, 9,  10, 11, 12, 10, 11, 12, 13, 8,
    9,  10, 11, 9,  10, 11, 12, 10, 11, 12, 13, 11, 12, 13, 14, 9,  10, 11, 12,
    10, 11, 12, 13, 11, 12, 13, 14, 12, 13, 14, 15, 10, 11, 12, 13, 11, 12, 13,
    14, 12, 13, 14, 15, 13, 14, 15, 16};

static const int8_t shuffleTable[256][16] = {
    {0, -1, -1, -1, 1, -1, -1, -1, 2, -1, -1, -1, 3, -1, -1, -1}, // 1111
    {0, 1, -1, -1, 2, -1, -1, -1, 3, -1, -1, -1, 4, -1, -1, -1},  // 2111
    {0, 1, 2, -1, 3, -1, -1, -1, 4, -1, -1, -1, 5, -1, -1, -1},   // 3111
    {0, 1, 2, 3, 4, -1, -1, -1, 5, -1, -1, -1, 6, -1, -1, -1},    // 4111
    {0, -1, -1, -1, 1, 2, -1, -1, 3, -1, -1, -1, 4, -1, -1, -1},  // 1211
    {0, 1, -1, -1, 2, 3, -1, -1, 4, -1, -1, -1, 5, -1, -1, -1},   // 2211
    {0, 1, 2, -1, 3, 4, -1, -1, 5, -1, -1, -1, 6, -1, -1, -1},    // 3211
    {0, 1, 2, 3, 4, 5, -1, -1, 6, -1, -1, -1, 7, -1, -1, -1},     // 4211
    {0, -1, -1, -1, 1, 2, 3, -1, 4, -1, -1, -1, 5, -1, -1, -1},   // 1311
    {0, 1, -1, -1, 2, 3, 4, -1, 5, -1, -1, -1, 6, -1, -1, -1},    // 2311
    {0, 1, 2, -1, 3, 4, 5, -1, 6, -1, -1, -1, 7, -1, -1, -1},     // 3311
    {0, 1, 2, 3, 4, 5, 6, -1, 7, -1, -1, -1, 8, -1, -1, -1},      // 4311
    {0, -1, -1, -1, 1, 2, 3, 4, 5, -1, -1, -1, 6, -1, -1, -1},    // 1411
    {0, 1, -1, -1, 2, 3, 4, 5, 6, -1, -1, -1, 7, -1, -1, -1},     // 2411
    {0, 1, 2, -1, 3, 4, 5, 6, 7, -1, -1, -1, 8, -1, -1, -1},      // 3411
    {0, 1, 2, 3, 4, 5, 6, 7, 8, -1, -1, -1, 9, -1, -1, -1},       // 4411
    {0, -1, -1, -1, 1, -1, -1, -1, 2, 3, -1, -1, 4, -1, -1, -1},  // 1121
    {0, 1, -1, -1, 2, -1, -1, -1, 3, 4, -1, -1, 5, -1, -1, -1},   // 2121
    {0, 1, 2, -1, 3, -1, -1, -1, 4, 5, -1, -1, 6, -1, -1, -1},    // 3121
    {0, 1, 2, 3, 4, -1, -1, -1, 5, 6, -1, -1, 7, -1, -1, -1},     // 4121
    {0, -1, -1, -1, 1, 2, -1, -1, 3, 4, -1, -1, 5, -1, -1, -1},   // 1221
    {0, 1, -1, -1, 2, 3, -1, -1, 4, 5, -1, -1, 6, -1, -1, -1},    // 2221
    {0, 1, 2, -1, 3, 4, -1, -1, 5, 6, -1, -1, 7, -1, -1, -1},     // 3221
    {0, 1, 2, 3, 4, 5, -1, -1, 6, 7, -1, -1, 8, -1, -1, -1},      // 4221
    {0, -1, -1, -1, 1, 2, 3, -1, 4, 5, -1, -1, 6, -1, -1, -1},    // 1321
    {0, 1, -1, -1, 2, 3, 4, -1, 5, 6, -1, -1, 7, -1, -1, -1},     // 2321
    {0, 1, 2, -1, 3, 4, 5, -1, 6, 7, -1, -1, 8, -1, -1, -1},      // 3321
    {0, 1, 2, 3, 4, 5, 6, -1, 7, 8, -1, -1, 9, -1, -1, -1},       // 4321
    {0, -1, -1, -1, 1, 2, 3, 4, 5, 6, -1, -1, 7, -1, -1, -1},     // 1421
    {0, 1, -1, -1, 2, 3, 4, 5, 6, 7, -1, -1, 8, -1, -1, -1},      // 2421
    {0, 1, 2, -1, 3, 4, 5, 6, 7, 8, -1, -1, 9, -1, -1, -1},       // 3421
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, 10, -1, -1, -1},       // 4421
    {0, -1, -1, -1, 1, -1, -1, -1, 2, 3, 4, -1, 5, -1, -1, -1},   // 1131
    {0, 1, -1, -1, 2, -1, -1, -1, 3, 4, 5, -1, 6, -1, -1, -1},    // 2131
    {0, 1, 2, -1, 3, -1, -1, -1, 4, 5, 6, -1, 7, -1, -1, -1},     // 3131
    {0, 1, 2, 3, 4, -1, -1, -1, 5, 6, 7, -1, 8, -1, -1, -1},      // 4131
    {0, -1, -1, -1, 1, 2, -1, -1, 3, 4, 5, -1, 6, -1, -1, -1},    // 1231
    {0, 1, -1, -1, 2, 3, -1, -1, 4, 5, 6, -1, 7, -1, -1, -1},     // 2231
    {0, 1, 2, -1, 3, 4, -1, -1, 5, 6, 7, -1, 8, -1, -1, -1},      // 3231
    {0, 1, 2, 3, 4, 5, -1, -1, 6, 7, 8, -1, 9, -1, -1, -1},       // 4231
    {0, -1, -1, -1, 1, 2, 3, -1, 4, 5, 6, -1, 7, -1, -1, -1},     // 1331
    {0, 1, -1, -1, 2, 3, 4, -1, 5, 6, 7, -1, 8, -1, -1, -1},      // 2331
    {0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, -1, 9, -1, -1, -1},       // 3331
    {0, 1, 2, 3, 4, 5, 6, -1, 7, 8, 9, -1, 10, -1, -1, -1},       // 4331
    {0, -1, -1, -1, 1, 2, 3, 4, 5, 6, 7, -1, 8, -1, -1, -1},      // 1431
    {0, 1, -1, -1, 2, 3, 4, 5, 6, 7, 8, -1, 9, -1, -1, -1},       // 2431
    {0, 1, 2, -1, 3, 4, 5, 6, 7, 8, 9, -1, 10, -1, -1, -1},       // 3431
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, -1, 11, -1, -1, -1},       // 4431
    {0, -1, -1, -1, 1, -1, -1, -1, 2, 3, 4, 5, 6, -1, -1, -1},    // 1141
    {0, 1, -1, -1, 2, -1, -1, -1, 3, 4, 5, 6, 7, -1, -1, -1},     // 2141
    {0, 1, 2, -1, 3, -1, -1, -1, 4, 5, 6, 7, 8, -1, -1, -1},      // 3141
    {0, 1, 2, 3, 4, -1, -1, -1, 5, 6, 7, 8, 9, -1, -1, -1},       // 4141
    {0, -1, -1, -1, 1, 2, -1, -1, 3, 4, 5, 6, 7, -1, -1, -1},     // 1241
    {0, 1, -1, -1, 2, 3, -1, -1, 4, 5, 6, 7, 8, -1, -1, -1},      // 2241
    {0, 1, 2, -1, 3, 4, -1, -1, 5, 6, 7, 8, 9, -1, -1, -1},       // 3241
    {0, 1, 2, 3, 4, 5, -1, -1, 6, 7, 8, 9, 10, -1, -1, -1},       // 4241
    {0, -1, -1, -1, 1, 2, 3, -1, 4, 5, 6, 7, 8, -1, -1, -1},      // 1341
    {0, 1, -1, -1, 2, 3, 4, -1, 5, 6, 7, 8, 9, -1, -1, -1},       // 2341
    {0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, 9, 10, -1, -1, -1},       // 3341
    {0, 1, 2, 3, 4, 5, 6, -1, 7, 8, 9, 10, 11, -1, -1, -1},       // 4341
    {0, -1, -1, -1, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1},       // 1441
    {0, 1, -1, -1, 2, 3, 4, 5, 6, 7, 8, 9, 10, -1, -1, -1},       // 2441
    {0, 1, 2, -1, 3, 4, 5, 6, 7, 8, 9, 10, 11, -1, -1, -1},       // 3441
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, -1, -1, -1},       // 4441
    {0, -1, -1, -1, 1, -1, -1, -1, 2, -1, -1, -1, 3, 4, -1, -1},  // 1112
    {0, 1, -1, -1, 2, -1, -1, -1, 3, -1, -1, -1, 4, 5, -1, -1},   // 2112
    {0, 1, 2, -1, 3, -1, -1, -1, 4, -1, -1, -1, 5, 6, -1, -1},    // 3112
    {0, 1, 2, 3, 4, -1, -1, -1, 5, -1, -1, -1, 6, 7, -1, -1},     // 4112
    {0, -1, -1, -1, 1, 2, -1, -1, 3, -1, -1, -1, 4, 5, -1, -1},   // 1212
    {0, 1, -1, -1, 2, 3, -1, -1, 4, -1, -1, -1, 5, 6, -1, -1},    // 2212
    {0, 1, 2, -1, 3, 4, -1, -1, 5, -1, -1, -1, 6, 7, -1, -1},     // 3212
    {0, 1, 2, 3, 4, 5, -1, -1, 6, -1, -1, -1, 7, 8, -1, -1},      // 4212
    {0, -1, -1, -1, 1, 2, 3, -1, 4, -1, -1, -1, 5, 6, -1, -1},    // 1312
    {0, 1, -1, -1, 2, 3, 4, -1, 5, -1, -1, -1, 6, 7, -1, -1},     // 2312
    {0, 1, 2, -1, 3, 4, 5, -1, 6, -1, -1, -1, 7, 8, -1, -1},      // 3312
    {0, 1, 2, 3, 4, 5, 6, -1, 7, -1, -1, -1, 8, 9, -1, -1},       // 4312
    {0, -1, -1, -1, 1, 2, 3, 4, 5, -1, -1, -1, 6, 7, -1, -1},     // 1412
    {0, 1, -1, -1, 2, 3, 4, 5, 6, -1, -1, -1, 7, 8, -1, -1},      // 2412
    {0, 1, 2, -1, 3, 4, 5, 6, 7, -1, -1, -1, 8, 9, -1, -1},       // 3412
    {0, 1, 2, 3, 4, 5, 6, 7, 8, -1, -1, -1, 9, 10, -1, -1},       // 4412
    {0, -1, -1, -1, 1, -1, -1, -1, 2, 3, -1, -1, 4, 5, -1, -1},   // 1122
    {0, 1, -1, -1, 2, -1, -1, -1, 3, 4, -1, -1, 5, 6, -1, -1},    // 2122
    {0, 1, 2, -1, 3, -1, -1, -1, 4, 5, -1, -1, 6, 7, -1, -1},     // 3122
    {0, 1, 2, 3, 4, -1, -1, -1, 5, 6, -1, -1, 7, 8, -1, -1},      // 4122
    {0, -1, -1, -1, 1, 2, -1, -1, 3, 4, -1, -1, 5, 6, -1, -1},    // 1222
    {0, 1, -1, -1, 2, 3, -1, -1, 4, 5, -1, -1, 6, 7, -1, -1},     // 2222
    {0, 1, 2, -1, 3, 4, -1, -1, 5, 6, -1, -1, 7, 8, -1, -1},      // 3222
    {0, 1, 2, 3, 4, 5, -1, -1, 6, 7, -1, -1, 8, 9, -1, -1},       // 4222
    {0, -1, -1, -1, 1, 2, 3, -1, 4, 5, -1, -1, 6, 7, -1, -1},     // 1322
    {0, 1, -1, -1, 2, 3, 4, -1, 5, 6, -1, -1, 7, 8, -1, -1},      // 2322
    {0, 1, 2, -1, 3, 4, 5, -1, 6, 7, -1, -1, 8, 9, -1, -1},       // 3322
    {0, 1, 2, 3, 4, 5, 6, -1, 7, 8, -1, -1, 9, 10, -1, -1},       // 4322
    {0, -1, -1, -1, 1, 2, 3, 4, 5, 6, -1, -1, 7, 8, -1, -1},      // 1422
    {0, 1, -1, -1, 2, 3, 4, 5, 6, 7, -1, -1, 8, 9, -1, -1},       // 2422
    {0, 1, 2, -1, 3, 4, 5, 6, 7, 8, -1, -1, 9, 10, -1, -1},       // 3422
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, 10, 11, -1, -1},       // 4422
    {0, -1, -1, -1, 1, -1, -1, -1, 2, 3, 4, -1, 5, 6, -1, -1},    // 1132
    {0, 1, -1, -1, 2, -1, -1, -1, 3, 4, 5, -1, 6, 7, -1, -1},     // 2132
    {0, 1, 2, -1, 3, -1, -1, -1, 4, 5, 6, -1, 7, 8, -1, -1},      // 3132
    {0, 1, 2, 3, 4, -1, -1, -1, 5, 6, 7, -1, 8, 9, -1, -1},       // 4132
    {0, -1, -1, -1, 1, 2, -1, -1, 3, 4, 5, -1, 6, 7, -1, -1},     // 1232
    {0, 1, -1, -1, 2, 3, -1, -1, 4, 5, 6, -1, 7, 8, -1, -1},      // 2232
    {0, 1, 2, -1, 3, 4, -1, -1, 5, 6, 7, -1, 8, 9, -1, -1},       // 3232
    {0, 1, 2, 3, 4, 5, -1, -1, 6, 7, 8, -1, 9, 10, -1, -1},       // 4232
    {0, -1, -1, -1, 1, 2, 3, -1, 4, 5, 6, -1, 7, 8, -1, -1},      // 1332
    {0, 1, -1, -1, 2, 3, 4, -1, 5, 6, 7, -1, 8, 9, -1, -1},       // 2332
    {0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, -1, 9, 10, -1, -1},       // 3332
    {0, 1, 2, 3, 4, 5, 6, -1, 7, 8, 9, -1, 10, 11, -1, -1},       // 4332
    {0, -1, -1, -1, 1, 2, 3, 4, 5, 6, 7, -1, 8, 9, -1, -1},       // 1432
    {0, 1, -1, -1, 2, 3, 4, 5, 6, 7, 8, -1, 9, 10, -1, -1},       // 2432
    {0, 1, 2, -1, 3, 4, 5, 6, 7, 8, 9, -1, 10, 11, -1, -1},       // 3432
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, -1, 11, 12, -1, -1},       // 4432
    {0, -1, -1, -1, 1, -1, -1, -1, 2, 3, 4, 5, 6, 7, -1, -1},     // 1142
    {0, 1, -1, -1, 2, -1, -1, -1, 3, 4, 5, 6, 7, 8, -1, -1},      // 2142
    {0, 1, 2, -1, 3, -1, -1, -1, 4, 5, 6, 7, 8, 9, -1, -1},       // 3142
    {0, 1, 2, 3, 4, -1, -1, -1, 5, 6, 7, 8, 9, 10, -1, -1},       // 4142
    {0, -1, -1, -1, 1, 2, -1, -1, 3, 4, 5, 6, 7, 8, -1, -1},      // 1242
    {0, 1, -1, -1, 2, 3, -1, -1, 4, 5, 6, 7, 8, 9, -1, -1},       // 2242
    {0, 1, 2, -1, 3, 4, -1, -1, 5, 6, 7, 8, 9, 10, -1, -1},       // 3242
    {0, 1, 2, 3, 4, 5, -1, -1, 6, 7, 8, 9, 10, 11, -1, -1},       // 4242
    {0, -1, -1, -1, 1, 2, 3, -1, 4, 5, 6, 7, 8, 9, -1, -1},       // 1342
    {0, 1, -1, -1, 2, 3, 4, -1, 5, 6, 7, 8, 9, 10, -1, -1},       // 2342
    {0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, 9, 10, 11, -1, -1},       // 3342
    {0, 1, 2, 3, 4, 5, 6, -1, 7, 8, 9, 10, 11, 12, -1, -1},       // 4342
    {0, -1, -1, -1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, -1, -1},       // 1442
    {0, 1, -1, -1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, -1, -1},       // 2442
    {0, 1, 2, -1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, -1, -1},       // 3442
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, -1, -1},       // 4442
    {0, -1, -1, -1, 1, -1, -1, -1, 2, -1, -1, -1, 3, 4, 5, -1},   // 1113
    {0, 1, -1, -1, 2, -1, -1, -1, 3, -1, -1, -1, 4, 5, 6, -1},    // 2113
    {0, 1, 2, -1, 3, -1, -1, -1, 4, -1, -1, -1, 5, 6, 7, -1},     // 3113
    {0, 1, 2, 3, 4, -1, -1, -1, 5, -1, -1, -1, 6, 7, 8, -1},      // 4113
    {0, -1, -1, -1, 1, 2, -1, -1, 3, -1, -1, -1, 4, 5, 6, -1},    // 1213
    {0, 1, -1, -1, 2, 3, -1, -1, 4, -1, -1, -1, 5, 6, 7, -1},     // 2213
    {0, 1, 2, -1, 3, 4, -1, -1, 5, -1, -1, -1, 6, 7, 8, -1},      // 3213
    {0, 1, 2, 3, 4, 5, -1, -1, 6, -1, -1, -1, 7, 8, 9, -1},       // 4213
    {0, -1, -1, -1, 1, 2, 3, -1, 4, -1, -1, -1, 5, 6, 7, -1},     // 1313
    {0, 1, -1, -1, 2, 3, 4, -1, 5, -1, -1, -1, 6, 7, 8, -1},      // 2313
    {0, 1, 2, -1, 3, 4, 5, -1, 6, -1, -1, -1, 7, 8, 9, -1},       // 3313
    {0, 1, 2, 3, 4, 5, 6, -1, 7, -1, -1, -1, 8, 9, 10, -1},       // 4313
    {0, -1, -1, -1, 1, 2, 3, 4, 5, -1, -1, -1, 6, 7, 8, -1},      // 1413
    {0, 1, -1, -1, 2, 3, 4, 5, 6, -1, -1, -1, 7, 8, 9, -1},       // 2413
    {0, 1, 2, -1, 3, 4, 5, 6, 7, -1, -1, -1, 8, 9, 10, -1},       // 3413
    {0, 1, 2, 3, 4, 5, 6, 7, 8, -1, -1, -1, 9, 10, 11, -1},       // 4413
    {0, -1, -1, -1, 1, -1, -1, -1, 2, 3, -1, -1, 4, 5, 6, -1},    // 1123
    {0, 1, -1, -1, 2, -1, -1, -1, 3, 4, -1, -1, 5, 6, 7, -1},     // 2123
    {0, 1, 2, -1, 3, -1, -1, -1, 4, 5, -1, -1, 6, 7, 8, -1},      // 3123
    {0, 1, 2, 3, 4, -1, -1, -1, 5, 6, -1, -1, 7, 8, 9, -1},       // 4123
    {0, -1, -1, -1, 1, 2, -1, -1, 3, 4, -1, -1, 5, 6, 7, -1},     // 1223
    {0, 1, -1, -1, 2, 3, -1, -1, 4, 5, -1, -1, 6, 7, 8, -1},      // 2223
    {0, 1, 2, -1, 3, 4, -1, -1, 5, 6, -1, -1, 7, 8, 9, -1},       // 3223
    {0, 1, 2, 3, 4, 5, -1, -1, 6, 7, -1, -1, 8, 9, 10, -1},       // 4223
    {0, -1, -1, -1, 1, 2, 3, -1, 4, 5, -1, -1, 6, 7, 8, -1},      // 1323
    {0, 1, -1, -1, 2, 3, 4, -1, 5, 6, -1, -1, 7, 8, 9, -1},       // 2323
    {0, 1, 2, -1, 3, 4, 5, -1, 6, 7, -1, -1, 8, 9, 10, -1},       // 3323
    {0, 1, 2, 3, 4, 5, 6, -1, 7, 8, -1, -1, 9, 10, 11, -1},       // 4323
    {0, -1, -1, -1, 1, 2, 3, 4, 5, 6, -1, -1, 7, 8, 9, -1},       // 1423
    {0, 1, -1, -1, 2, 3, 4, 5, 6, 7, -1, -1, 8, 9, 10, -1},       // 2423
    {0, 1, 2, -1, 3, 4, 5, 6, 7, 8, -1, -1, 9, 10, 11, -1},       // 3423
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, 10, 11, 12, -1},       // 4423
    {0, -1, -1, -1, 1, -1, -1, -1, 2, 3, 4, -1, 5, 6, 7, -1},     // 1133
    {0, 1, -1, -1, 2, -1, -1, -1, 3, 4, 5, -1, 6, 7, 8, -1},      // 2133
    {0, 1, 2, -1, 3, -1, -1, -1, 4, 5, 6, -1, 7, 8, 9, -1},       // 3133
    {0, 1, 2, 3, 4, -1, -1, -1, 5, 6, 7, -1, 8, 9, 10, -1},       // 4133
    {0, -1, -1, -1, 1, 2, -1, -1, 3, 4, 5, -1, 6, 7, 8, -1},      // 1233
    {0, 1, -1, -1, 2, 3, -1, -1, 4, 5, 6, -1, 7, 8, 9, -1},       // 2233
    {0, 1, 2, -1, 3, 4, -1, -1, 5, 6, 7, -1, 8, 9, 10, -1},       // 3233
    {0, 1, 2, 3, 4, 5, -1, -1, 6, 7, 8, -1, 9, 10, 11, -1},       // 4233
    {0, -1, -1, -1, 1, 2, 3, -1, 4, 5, 6, -1, 7, 8, 9, -1},       // 1333
    {0, 1, -1, -1, 2, 3, 4, -1, 5, 6, 7, -1, 8, 9, 10, -1},       // 2333
    {0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, -1, 9, 10, 11, -1},       // 3333
    {0, 1, 2, 3, 4, 5, 6, -1, 7, 8, 9, -1, 10, 11, 12, -1},       // 4333
    {0, -1, -1, -1, 1, 2, 3, 4, 5, 6, 7, -1, 8, 9, 10, -1},       // 1433
    {0, 1, -1, -1, 2, 3, 4, 5, 6, 7, 8, -1, 9, 10, 11, -1},       // 2433
    {0, 1, 2, -1, 3, 4, 5, 6, 7, 8, 9, -1, 10, 11, 12, -1},       // 3433
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, -1, 11, 12, 13, -1},       // 4433
    {0, -1, -1, -1, 1, -1, -1, -1, 2, 3, 4, 5, 6, 7, 8, -1},      // 1143
    {0, 1, -1, -1, 2, -1, -1, -1, 3, 4, 5, 6, 7, 8, 9, -1},       // 2143
    {0, 1, 2, -1, 3, -1, -1, -1, 4, 5, 6, 7, 8, 9, 10, -1},       // 3143
    {0, 1, 2, 3, 4, -1, -1, -1, 5, 6, 7, 8, 9, 10, 11, -1},       // 4143
    {0, -1, -1, -1, 1, 2, -1, -1, 3, 4, 5, 6, 7, 8, 9, -1},       // 1243
    {0, 1, -1, -1, 2, 3, -1, -1, 4, 5, 6, 7, 8, 9, 10, -1},       // 2243
    {0, 1, 2, -1, 3, 4, -1, -1, 5, 6, 7, 8, 9, 10, 11, -1},       // 3243
    {0, 1, 2, 3, 4, 5, -1, -1, 6, 7, 8, 9, 10, 11, 12, -1},       // 4243
    {0, -1, -1, -1, 1, 2, 3, -1, 4, 5, 6, 7, 8, 9, 10, -1},       // 1343
    {0, 1, -1, -1, 2, 3, 4, -1, 5, 6, 7, 8, 9, 10, 11, -1},       // 2343
    {0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, 9, 10, 11, 12, -1},       // 3343
    {0, 1, 2, 3, 4, 5, 6, -1, 7, 8, 9, 10, 11, 12, 13, -1},       // 4343
    {0, -1, -1, -1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, -1},       // 1443
    {0, 1, -1, -1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, -1},       // 2443
    {0, 1, 2, -1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, -1},       // 3443
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, -1},       // 4443
    {0, -1, -1, -1, 1, -1, -1, -1, 2, -1, -1, -1, 3, 4, 5, 6},    // 1114
    {0, 1, -1, -1, 2, -1, -1, -1, 3, -1, -1, -1, 4, 5, 6, 7},     // 2114
    {0, 1, 2, -1, 3, -1, -1, -1, 4, -1, -1, -1, 5, 6, 7, 8},      // 3114
    {0, 1, 2, 3, 4, -1, -1, -1, 5, -1, -1, -1, 6, 7, 8, 9},       // 4114
    {0, -1, -1, -1, 1, 2, -1, -1, 3, -1, -1, -1, 4, 5, 6, 7},     // 1214
    {0, 1, -1, -1, 2, 3, -1, -1, 4, -1, -1, -1, 5, 6, 7, 8},      // 2214
    {0, 1, 2, -1, 3, 4, -1, -1, 5, -1, -1, -1, 6, 7, 8, 9},       // 3214
    {0, 1, 2, 3, 4, 5, -1, -1, 6, -1, -1, -1, 7, 8, 9, 10},       // 4214
    {0, -1, -1, -1, 1, 2, 3, -1, 4, -1, -1, -1, 5, 6, 7, 8},      // 1314
    {0, 1, -1, -1, 2, 3, 4, -1, 5, -1, -1, -1, 6, 7, 8, 9},       // 2314
    {0, 1, 2, -1, 3, 4, 5, -1, 6, -1, -1, -1, 7, 8, 9, 10},       // 3314
    {0, 1, 2, 3, 4, 5, 6, -1, 7, -1, -1, -1, 8, 9, 10, 11},       // 4314
    {0, -1, -1, -1, 1, 2, 3, 4, 5, -1, -1, -1, 6, 7, 8, 9},       // 1414
    {0, 1, -1, -1, 2, 3, 4, 5, 6, -1, -1, -1, 7, 8, 9, 10},       // 2414
    {0, 1, 2, -1, 3, 4, 5, 6, 7, -1, -1, -1, 8, 9, 10, 11},       // 3414
    {0, 1, 2, 3, 4, 5, 6, 7, 8, -1, -1, -1, 9, 10, 11, 12},       // 4414
    {0, -1, -1, -1, 1, -1, -1, -1, 2, 3, -1, -1, 4, 5, 6, 7},     // 1124
    {0, 1, -1, -1, 2, -1, -1, -1, 3, 4, -1, -1, 5, 6, 7, 8},      // 2124
    {0, 1, 2, -1, 3, -1, -1, -1, 4, 5, -1, -1, 6, 7, 8, 9},       // 3124
    {0, 1, 2, 3, 4, -1, -1, -1, 5, 6, -1, -1, 7, 8, 9, 10},       // 4124
    {0, -1, -1, -1, 1, 2, -1, -1, 3, 4, -1, -1, 5, 6, 7, 8},      // 1224
    {0, 1, -1, -1, 2, 3, -1, -1, 4, 5, -1, -1, 6, 7, 8, 9},       // 2224
    {0, 1, 2, -1, 3, 4, -1, -1, 5, 6, -1, -1, 7, 8, 9, 10},       // 3224
    {0, 1, 2, 3, 4, 5, -1, -1, 6, 7, -1, -1, 8, 9, 10, 11},       // 4224
    {0, -1, -1, -1, 1, 2, 3, -1, 4, 5, -1, -1, 6, 7, 8, 9},       // 1324
    {0, 1, -1, -1, 2, 3, 4, -1, 5, 6, -1, -1, 7, 8, 9, 10},       // 2324
    {0, 1, 2, -1, 3, 4, 5, -1, 6, 7, -1, -1, 8, 9, 10, 11},       // 3324
    {0, 1, 2, 3, 4, 5, 6, -1, 7, 8, -1, -1, 9, 10, 11, 12},       // 4324
    {0, -1, -1, -1, 1, 2, 3, 4, 5, 6, -1, -1, 7, 8, 9, 10},       // 1424
    {0, 1, -1, -1, 2, 3, 4, 5, 6, 7, -1, -1, 8, 9, 10, 11},       // 2424
    {0, 1, 2, -1, 3, 4, 5, 6, 7, 8, -1, -1, 9, 10, 11, 12},       // 3424
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, 10, 11, 12, 13},       // 4424
    {0, -1, -1, -1, 1, -1, -1, -1, 2, 3, 4, -1, 5, 6, 7, 8},      // 1134
    {0, 1, -1, -1, 2, -1, -1, -1, 3, 4, 5, -1, 6, 7, 8, 9},       // 2134
    {0, 1, 2, -1, 3, -1, -1, -1, 4, 5, 6, -1, 7, 8, 9, 10},       // 3134
    {0, 1, 2, 3, 4, -1, -1, -1, 5, 6, 7, -1, 8, 9, 10, 11},       // 4134
    {0, -1, -1, -1, 1, 2, -1, -1, 3, 4, 5, -1, 6, 7, 8, 9},       // 1234
    {0, 1, -1, -1, 2, 3, -1, -1, 4, 5, 6, -1, 7, 8, 9, 10},       // 2234
    {0, 1, 2, -1, 3, 4, -1, -1, 5, 6, 7, -1, 8, 9, 10, 11},       // 3234
    {0, 1, 2, 3, 4, 5, -1, -1, 6, 7, 8, -1, 9, 10, 11, 12},       // 4234
    {0, -1, -1, -1, 1, 2, 3, -1, 4, 5, 6, -1, 7, 8, 9, 10},       // 1334
    {0, 1, -1, -1, 2, 3, 4, -1, 5, 6, 7, -1, 8, 9, 10, 11},       // 2334
    {0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, -1, 9, 10, 11, 12},       // 3334
    {0, 1, 2, 3, 4, 5, 6, -1, 7, 8, 9, -1, 10, 11, 12, 13},       // 4334
    {0, -1, -1, -1, 1, 2, 3, 4, 5, 6, 7, -1, 8, 9, 10, 11},       // 1434
    {0, 1, -1, -1, 2, 3, 4, 5, 6, 7, 8, -1, 9, 10, 11, 12},       // 2434
    {0, 1, 2, -1, 3, 4, 5, 6, 7, 8, 9, -1, 10, 11, 12, 13},       // 3434
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, -1, 11, 12, 13, 14},       // 4434
    {0, -1, -1, -1, 1, -1, -1, -1, 2, 3, 4, 5, 6, 7, 8, 9},       // 1144
    {0, 1, -1, -1, 2, -1, -1, -1, 3, 4, 5, 6, 7, 8, 9, 10},       // 2144
    {0, 1, 2, -1, 3, -1, -1, -1, 4, 5, 6, 7, 8, 9, 10, 11},       // 3144
    {0, 1, 2, 3, 4, -1, -1, -1, 5, 6, 7, 8, 9, 10, 11, 12},       // 4144
    {0, -1, -1, -1, 1, 2, -1, -1, 3, 4, 5, 6, 7, 8, 9, 10},       // 1244
    {0, 1, -1, -1, 2, 3, -1, -1, 4, 5, 6, 7, 8, 9, 10, 11},       // 2244
    {0, 1, 2, -1, 3, 4, -1, -1, 5, 6, 7, 8, 9, 10, 11, 12},       // 3244
    {0, 1, 2, 3, 4, 5, -1, -1, 6, 7, 8, 9, 10, 11, 12, 13},       // 4244
    {0, -1, -1, -1, 1, 2, 3, -1, 4, 5, 6, 7, 8, 9, 10, 11},       // 1344
    {0, 1, -1, -1, 2, 3, 4, -1, 5, 6, 7, 8, 9, 10, 11, 12},       // 2344
    {0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, 9, 10, 11, 12, 13},       // 3344
    {0, 1, 2, 3, 4, 5, 6, -1, 7, 8, 9, 10, 11, 12, 13, 14},       // 4344
    {0, -1, -1, -1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},       // 1444
    {0, 1, -1, -1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},       // 2444
    {0, 1, 2, -1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},       // 3444
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}        // 4444
};

// static char HighTo32[16] = {8, 9, -1, -1, 10, 11, -1, -1, 12, 13, -1, -1, 14,
// 15, -1, -1};
// Byte Order: {0x0706050403020100, 0x0F0E0D0C0B0A0908}
#if !defined(_MSC_VER) || defined(__clang__) || (defined(_MSC_VER) && defined(_M_ARM64))
static const xmm_t High16To32 = { (long long)0xFFFF0B0AFFFF0908, (long long)0xFFFF0F0EFFFF0D0C};
#else
static const xmm_t High16To32 = {8,  9,  -1, -1, 10, 11, -1, -1,
                           12, 13, -1, -1, 14, 15, -1, -1};
#endif

static inline void _write_avx(uint32_t *out, xmm_t Vec) {
  ///////////////////
  // was:
  //
  //_mm_stream_si128((xmm_t *)out, Vec);
  //
  // Daniel: It is not "fair" to use _mm_stream_si128: _mm_stream_si128 has
  // alignment
  // requirements unlike _mm_storeu_si128. In any case, other codecs do not use
  // _mm_stream_si128
  // so any gain specific to _mm_stream_si128 would make comparisons difficult.
  // If the gains
  // are really substantial, then that is are result that should be studied
  // separately.
  ///////////////////
  _mm_storeu_si128((xmm_t *)out, Vec);
}

#define BroadcastLastXMM 0xFF // bits 0-7 all set to choose highest element

static inline xmm_t _write_16bit_avx_d1(uint32_t *out, xmm_t Vec, xmm_t Prev) {
  // vec == [A B C D E F G H] (16 bit values)
  xmm_t Add = _mm_slli_si128(Vec, 2);               // [- A B C D E F G]
  Prev = _mm_shuffle_epi32(Prev, BroadcastLastXMM); // [P P P P] (32-bit)
  Vec = _mm_add_epi32(Vec, Add);                    // [A AB BC CD DE FG GH]
  Add = _mm_slli_si128(Vec, 4);                     // [- - A AB BC CD DE EF]
  Vec = _mm_add_epi32(Vec, Add);      // [A AB ABC ABCD BCDE CDEF DEFG EFGH]
  xmm_t V1 = _mm_cvtepu16_epi32(Vec); // [A AB ABC ABCD] (32-bit)
  V1 = _mm_add_epi32(V1, Prev);       // [PA PAB PABC PABCD] (32-bit)
  xmm_t V2 =
      _mm_shuffle_epi8(Vec, High16To32); // [BCDE CDEF DEFG EFGH] (32-bit)
  V2 = _mm_add_epi32(V1, V2); // [PABCDE PABCDEF PABCDEFG PABCDEFGH] (32-bit)
  _write_avx(out, V1);
  _write_avx(out + 4, V2);
  //_mm_stream_si128((xmm_t *)(out), V1);
  //_mm_stream_si128((xmm_t *)(out + 4), V2);
  return V2;
}

static inline xmm_t _write_avx_d1(uint32_t *out, xmm_t Vec, xmm_t Prev) {
  xmm_t Add = _mm_slli_si128(Vec, 4); // Cycle 1: [- A B C] (already done)
  Prev = _mm_shuffle_epi32(Prev, BroadcastLastXMM); // Cycle 2: [P P P P]
  Vec = _mm_add_epi32(Vec, Add);                    // Cycle 2: [A AB BC CD]
  Add = _mm_slli_si128(Vec, 8);                     // Cycle 3: [- - A AB]
  Vec = _mm_add_epi32(Vec, Prev);                   // Cycle 3: [PA PAB PBC PCD]
  Vec = _mm_add_epi32(Vec, Add); // Cycle 4: [PA PAB PABC PABCD]

  _write_avx(out, Vec);
  //_mm_stream_si128((xmm_t *)out, Vec);
  return Vec;
}

// static xmm_t ExtraDelta[256] = { {0x0000000000000000, 0x0000000000000000} };

static inline xmm_t _decode_avx(uint32_t key,
                                uint8_t *__restrict__ *dataPtrPtr) {
  uint8_t len = lengthTable[key];
  xmm_t Data = _mm_loadu_si128((xmm_t *)*dataPtrPtr);
  xmm_t Shuf = *(xmm_t *)&shuffleTable[key];

  Data = _mm_shuffle_epi8(Data, Shuf);
  *dataPtrPtr += len;

  // Appears feasible to have custom delta per key without slowdown
  //    Data = _mm_add_epi32(Data, ExtraDelta[key]);

  return Data;
}

uint8_t *svb_decode_avx_d1_init(uint32_t *out, uint8_t *__restrict__ keyPtr,
                                uint8_t *__restrict__ dataPtr, uint64_t count,
                                uint32_t prev) {
  uint64_t keybytes = count / 4; // number of key bytes
  if (keybytes >= 8) {
    xmm_t Prev = _mm_set1_epi32(prev);
    xmm_t Data;

    int64_t Offset = -(int64_t)keybytes / 8 + 1;

    const uint64_t *keyPtr64 = (const uint64_t *)keyPtr - Offset;
    uint64_t nextkeys = keyPtr64[Offset];
    for (; Offset != 0; ++Offset) {
      uint64_t keys = nextkeys;
      nextkeys = keyPtr64[Offset + 1];
      // faster 16-bit delta since we only have 8-bit values
      if (!keys) { // 32 1-byte ints in a row

        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr)));
        Prev = _write_16bit_avx_d1(out, Data, Prev);
        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr + 8)));
        Prev = _write_16bit_avx_d1(out + 8, Data, Prev);
        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr + 16)));
        Prev = _write_16bit_avx_d1(out + 16, Data, Prev);
        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr + 24)));
        Prev = _write_16bit_avx_d1(out + 24, Data, Prev);
        out += 32;
        dataPtr += 32;
        continue;
      }

      Data = _decode_avx(keys & 0x00FF, &dataPtr);
      Prev = _write_avx_d1(out, Data, Prev);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      Prev = _write_avx_d1(out + 4, Data, Prev);

      keys >>= 16;
      Data = _decode_avx((keys & 0x00FF), &dataPtr);
      Prev = _write_avx_d1(out + 8, Data, Prev);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      Prev = _write_avx_d1(out + 12, Data, Prev);

      keys >>= 16;
      Data = _decode_avx((keys & 0x00FF), &dataPtr);
      Prev = _write_avx_d1(out + 16, Data, Prev);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      Prev = _write_avx_d1(out + 20, Data, Prev);

      keys >>= 16;
      Data = _decode_avx((keys & 0x00FF), &dataPtr);
      Prev = _write_avx_d1(out + 24, Data, Prev);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      Prev = _write_avx_d1(out + 28, Data, Prev);

      out += 32;
    }
    {
      uint64_t keys = nextkeys;
      // faster 16-bit delta since we only have 8-bit values
      if (!keys) { // 32 1-byte ints in a row
        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr)));
        Prev = _write_16bit_avx_d1(out, Data, Prev);
        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr + 8)));
        Prev = _write_16bit_avx_d1(out + 8, Data, Prev);
        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr + 16)));
        Prev = _write_16bit_avx_d1(out + 16, Data, Prev);
        Data = _mm_cvtepu8_epi16(_mm_loadl_epi64((xmm_t *)(dataPtr + 24)));
        _write_16bit_avx_d1(out + 24, Data, Prev);
        out += 32;
        dataPtr += 32;

      } else {

        Data = _decode_avx(keys & 0x00FF, &dataPtr);
        Prev = _write_avx_d1(out, Data, Prev);
        Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
        Prev = _write_avx_d1(out + 4, Data, Prev);

        keys >>= 16;
        Data = _decode_avx((keys & 0x00FF), &dataPtr);
        Prev = _write_avx_d1(out + 8, Data, Prev);
        Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
        Prev = _write_avx_d1(out + 12, Data, Prev);

        keys >>= 16;
        Data = _decode_avx((keys & 0x00FF), &dataPtr);
        Prev = _write_avx_d1(out + 16, Data, Prev);
        Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
        Prev = _write_avx_d1(out + 20, Data, Prev);

        keys >>= 16;
        Data = _decode_avx((keys & 0x00FF), &dataPtr);
        Prev = _write_avx_d1(out + 24, Data, Prev);
        Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
        _write_avx_d1(out + 28, Data, Prev);

        out += 32;
      }
    }
    prev = out[-1];
  }
  uint64_t consumedkeys = keybytes - (keybytes & 7);
  return svb_decode_scalar_d1_init(out, keyPtr + consumedkeys, dataPtr,
                                   count & 31, prev);
}

uint8_t *svb_decode_avx_d1_simple(uint32_t *out, uint8_t *__restrict__ keyPtr,
                                  uint8_t *__restrict__ dataPtr,
                                  uint64_t count) {
  return svb_decode_avx_d1_init(out, keyPtr, dataPtr, count, 0);
}

uint8_t *svb_decode_avx_simple(uint32_t *out, uint8_t *__restrict__ keyPtr,
                               uint8_t *__restrict__ dataPtr, uint64_t count) {

  uint64_t keybytes = count / 4; // number of key bytes
  xmm_t Data;
  if (keybytes >= 8) {

    int64_t Offset = -(int64_t)keybytes / 8 + 1;

    const uint64_t *keyPtr64 = (const uint64_t *)keyPtr - Offset;
    uint64_t nextkeys = keyPtr64[Offset];
    for (; Offset != 0; ++Offset) {
      uint64_t keys = nextkeys;
      nextkeys = keyPtr64[Offset + 1];

      Data = _decode_avx((keys & 0xFF), &dataPtr);
      _write_avx(out, Data);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      _write_avx(out + 4, Data);

      keys >>= 16;
      Data = _decode_avx((keys & 0xFF), &dataPtr);
      _write_avx(out + 8, Data);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      _write_avx(out + 12, Data);

      keys >>= 16;
      Data = _decode_avx((keys & 0xFF), &dataPtr);
      _write_avx(out + 16, Data);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      _write_avx(out + 20, Data);

      keys >>= 16;
      Data = _decode_avx((keys & 0xFF), &dataPtr);
      _write_avx(out + 24, Data);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      _write_avx(out + 28, Data);

      out += 32;
    }
    {
      uint64_t keys = nextkeys;

      Data = _decode_avx((keys & 0xFF), &dataPtr);
      _write_avx(out, Data);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      _write_avx(out + 4, Data);

      keys >>= 16;
      Data = _decode_avx((keys & 0xFF), &dataPtr);
      _write_avx(out + 8, Data);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      _write_avx(out + 12, Data);

      keys >>= 16;
      Data = _decode_avx((keys & 0xFF), &dataPtr);
      _write_avx(out + 16, Data);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      _write_avx(out + 20, Data);

      keys >>= 16;
      Data = _decode_avx((keys & 0xFF), &dataPtr);
      _write_avx(out + 24, Data);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      _write_avx(out + 28, Data);

      out += 32;
    }
  }
  uint64_t consumedkeys = keybytes - (keybytes & 7);
  return svb_decode_scalar(out, keyPtr + consumedkeys, dataPtr, count & 31);
}

uint64_t svb_encode(uint8_t *out, const uint32_t *in, uint32_t count, int delta,
                    int type) {
  *(uint32_t *)out = count;          // first 4 bytes is number of ints
  uint8_t *keyPtr = out + 4;         // keys come immediately after 32-bit count
  uint32_t keyLen = (count + 3) / 4; // 2-bits rounded to full byte
  uint8_t *dataPtr = keyPtr + keyLen; // variable byte data after all keys

  if (delta == 0 && type == 1) {
    return svb_encode_scalar(in, keyPtr, dataPtr, count) - out;
  }

  if (delta == 1 && type == 1) {
    return svb_encode_scalar_d1(in, keyPtr, dataPtr, count) - out;
  }

  printf("Unknown delta (%d) type (%d) combination.\n", delta, type);
  abort();
}

uint64_t svb_decode(uint32_t *out, uint8_t *in, int delta, int type) {
  uint32_t count = *(uint32_t *)in; // first 4 bytes is number of ints
  if (count == 0)
    return 0;

  uint8_t *keyPtr = in + 4;            // full list of keys is next
  uint32_t keyLen = ((count + 3) / 4); // 2-bits per key (rounded up)
  uint8_t *dataPtr = keyPtr + keyLen;  // data starts at end of keys

  if (delta == 0 && type == 1) {
    return svb_decode_scalar(out, keyPtr, dataPtr, count) - in;
  }

  if (delta == 1 && type == 1) {
    return svb_decode_scalar_d1(out, keyPtr, dataPtr, count) - in;
  }

  if (delta == 0 && type == 5) {
    return svb_decode_avx_simple(out, keyPtr, dataPtr, count) - in;
  }

  if (delta == 1 && type == 5) {
    return svb_decode_avx_d1_simple(out, keyPtr, dataPtr, count) - in;
  }

  printf("Unknown delta (%d) type (%d) combination.\n", delta, type);
  abort();
}

/* should emulate std:lower_bound*/
static int lower_bound(uint32_t *A, uint32_t key, int imin, int imax) {
  int imid;
  imax--;
  while (imin + 1 < imax) {
    imid = imin + ((imax - imin) / 2);

    if (A[imid] >= key) {
      imax = imid;
    } else if (A[imid] < key) {
      imin = imid;
    }
  }
  if (A[imin] >= key)
    return imin;
  return imax;
}

static ALIGNED(16) int8_t shuffle_mask_bytes[16 * 16] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 0,  1,  2,
    3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 4,  5,  6,  7,  4,  5,
    6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 0,  1,  2,  3,  4,  5,  6,  7,  8,
    9,  10, 11, 12, 13, 14, 15, 8,  9,  10, 11, 4,  5,  6,  7,  8,  9,  10, 11,
    12, 13, 14, 15, 0,  1,  2,  3,  8,  9,  10, 11, 8,  9,  10, 11, 12, 13, 14,
    15, 4,  5,  6,  7,  8,  9,  10, 11, 8,  9,  10, 11, 12, 13, 14, 15, 0,  1,
    2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 12, 13, 14, 15, 4,
    5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 0,  1,  2,  3,  12, 13, 14, 15,
    8,  9,  10, 11, 12, 13, 14, 15, 4,  5,  6,  7,  12, 13, 14, 15, 8,  9,  10,
    11, 12, 13, 14, 15, 0,  1,  2,  3,  4,  5,  6,  7,  12, 13, 14, 15, 12, 13,
    14, 15, 8,  9,  10, 11, 12, 13, 14, 15, 8,  9,  10, 11, 12, 13, 14, 15, 0,
    1,  2,  3,  8,  9,  10, 11, 12, 13, 14, 15, 12, 13, 14, 15, 4,  5,  6,  7,
    8,  9,  10, 11, 12, 13, 14, 15, 12, 13, 14, 15, 0,  1,  2,  3,  4,  5,  6,
    7,  8,  9,  10, 11, 12, 13, 14, 15,
};

static const __m128i *streamvbyte_shuffle_mask = (__m128i *)shuffle_mask_bytes;

static inline int find_lower_bound(__m128i *PrevHi, __m128i *PrevLow,
                                   uint32_t key, uint32_t *presult) {
  int offset = 0;
  int s;

#if 0
    // scalar code:
    uint32_t out[8];
    _write_avx(out, *PrevLow);
    _write_avx(out + 4, *PrevHi);
    s = std::lower_bound(out, out + 8, key) - out;
    *presult = out[s];
    return (s);
#endif

  uint32_t mask;
  __m128i key4 = _mm_set1_epi32(key - 2147483648U);
  __m128i conversion = _mm_set1_epi32(2147483648U);
  __m128i tmp = _mm_sub_epi32(*PrevLow, conversion);
  mask = _mm_movemask_ps(_mm_castsi128_ps(_mm_cmplt_epi32(tmp, key4)));
  __m128i *out = PrevLow;
  // not found in PrevLow? then try PrevHi
  if (mask == 15) {
    tmp = _mm_sub_epi32(*PrevHi, conversion);
    mask = _mm_movemask_ps(_mm_castsi128_ps(_mm_cmplt_epi32(tmp, key4)));
    out = PrevHi;
    offset = 4;
  }
  // there MUST be a result!
  assert(mask != 15);
  const __m128i p = _mm_shuffle_epi8(*out, streamvbyte_shuffle_mask[mask ^ 15]);
  s = __builtin_ctz(mask ^ 15);
  *presult = _mm_cvtsi128_si32(p);

  return (offset + s);
}

static inline void _scan_16bit_avx_d1(xmm_t Vec, xmm_t *PrevHi,
                                      xmm_t *PrevLow) {
  xmm_t Prev = *PrevHi;
  // vec == [A B C D E F G H] (16 bit values)
  xmm_t Add = _mm_slli_si128(Vec, 2);               // [- A B C D E F G]
  Prev = _mm_shuffle_epi32(Prev, BroadcastLastXMM); // [P P P P] (32-bit)
  Vec = _mm_add_epi32(Vec, Add);                    // [A AB BC CD DE FG GH]
  Add = _mm_slli_si128(Vec, 4);                     // [- - A AB BC CD DE EF]
  Vec = _mm_add_epi32(Vec, Add);      // [A AB ABC ABCD BCDE CDEF DEFG EFGH]
  *PrevLow = _mm_cvtepu16_epi32(Vec); // [A AB ABC ABCD] (32-bit)
  *PrevLow = _mm_add_epi32(*PrevLow, Prev);    // [PA PAB PABC PABCD] (32-bit)
  *PrevHi = _mm_shuffle_epi8(Vec, High16To32); // [BCDE CDEF DEFG EFGH] (32-bit)
  *PrevHi = _mm_add_epi32(
      *PrevHi, *PrevLow); // [PABCDE PABCDEF PABCDEFG PABCDEFGH] (32-bit)
}

static inline xmm_t _scan_avx_d1(xmm_t Vec, xmm_t Prev) {
  xmm_t Add = _mm_slli_si128(Vec, 4); // Cycle 1: [- A B C] (already done)
  Prev = _mm_shuffle_epi32(Prev, BroadcastLastXMM); // Cycle 2: [P P P P]
  Vec = _mm_add_epi32(Vec, Add);                    // Cycle 2: [A AB BC CD]
  Add = _mm_slli_si128(Vec, 8);                     // Cycle 3: [- - A AB]
  Vec = _mm_add_epi32(Vec, Prev);                   // Cycle 3: [PA PAB PBC PCD]
  return _mm_add_epi32(Vec, Add); // Cycle 4: [PA PAB PABC PABCD]
}

ALIGNED(16)
int8_t streamvbyte_shuffle_mask_bytes[256] = {
    0, 1, 2, 3, 0,  0,  0,  0,  0, 0, 0, 0, 0,  0,  0, 0, 4, 5, 6, 7, 0, 0,
    0, 0, 0, 0, 0,  0,  0,  0,  0, 0, 8, 9, 10, 11, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 12, 13, 14, 15, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0,
};

static const __m128i *shuffle_mask = (__m128i *)streamvbyte_shuffle_mask_bytes;

static inline uint32_t _extract_from_xmm(xmm_t *PrevHi, xmm_t *PrevLow, int i) {
  static const int indices[] = {0, 0, 0, 0, 1, 1, 1, 1};
  xmm_t *prevs[2] = {PrevLow, PrevHi};
  return _mm_cvtsi128_si32(
      _mm_shuffle_epi8(*prevs[indices[i]], shuffle_mask[i & 3]));
}

int svb_find_avx_d1_init(uint8_t *keyPtr, uint8_t *dataPtr, uint64_t count,
                         uint32_t prev, uint32_t key, uint32_t *presult) {
  uint64_t keybytes = count / 4; // number of key bytes
  int consumedInts = 0;

  if (keybytes >= 8) {
    xmm_t PrevHi = _mm_set1_epi32(prev);
    xmm_t PrevLow;
    xmm_t Data;

    int64_t Offset = -(int64_t)keybytes / 8 + 1;

    const uint64_t *keyPtr64 = (const uint64_t *)keyPtr - Offset;
    uint64_t nextkeys = keyPtr64[Offset];
    for (; Offset != 0; ++Offset) {
      uint64_t keys = nextkeys;
      nextkeys = keyPtr64[Offset + 1];
      // faster 16-bit delta since we only have 8-bit values
      if (!keys) { // 32 1-byte ints in a row

        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr)));
        _scan_16bit_avx_d1(Data, &PrevHi, &PrevLow);
        // check 8 ints
        if (key <= (uint32_t)_mm_extract_epi32(PrevHi, 3)) {
          int s = find_lower_bound(&PrevHi, &PrevLow, key, presult);
          return (consumedInts + s);
        }
        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr + 8)));
        _scan_16bit_avx_d1(Data, &PrevHi, &PrevLow);
        // check 8 ints
        if (key <= (uint32_t)_mm_extract_epi32(PrevHi, 3)) {
          int s = find_lower_bound(&PrevHi, &PrevLow, key, presult);
          return (consumedInts + 8 + s);
        }
        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr + 16)));
        _scan_16bit_avx_d1(Data, &PrevHi, &PrevLow);
        // check 8 ints
        if (key <= (uint32_t)_mm_extract_epi32(PrevHi, 3)) {
          int s = find_lower_bound(&PrevHi, &PrevLow, key, presult);
          return (consumedInts + 16 + s);
        }
        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr + 24)));
        _scan_16bit_avx_d1(Data, &PrevHi, &PrevLow);
        // check 8 ints
        if (key <= (uint32_t)_mm_extract_epi32(PrevHi, 3)) {
          int s = find_lower_bound(&PrevHi, &PrevLow, key, presult);
          return (consumedInts + 24 + s);
        }
        dataPtr += 32;
        consumedInts += 32;
        continue;
      }

      Data = _decode_avx(keys & 0x00FF, &dataPtr);
      PrevLow = _scan_avx_d1(Data, PrevHi);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      PrevHi = _scan_avx_d1(Data, PrevLow);
      // check 8 ints
      if (key <= (uint32_t)_mm_extract_epi32(PrevHi, 3)) {
        int s = find_lower_bound(&PrevHi, &PrevLow, key, presult);
        return (consumedInts + s);
      }

      keys >>= 16;
      Data = _decode_avx((keys & 0x00FF), &dataPtr);
      PrevLow = _scan_avx_d1(Data, PrevHi);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      PrevHi = _scan_avx_d1(Data, PrevLow);
      // check 8 ints
      if (key <= (uint32_t)_mm_extract_epi32(PrevHi, 3)) {
        int s = find_lower_bound(&PrevHi, &PrevLow, key, presult);
        return (consumedInts + 8 + s);
      }

      keys >>= 16;
      Data = _decode_avx((keys & 0x00FF), &dataPtr);
      PrevLow = _scan_avx_d1(Data, PrevHi);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      PrevHi = _scan_avx_d1(Data, PrevLow);
      // check 8 ints
      if (key <= (uint32_t)_mm_extract_epi32(PrevHi, 3)) {
        int s = find_lower_bound(&PrevHi, &PrevLow, key, presult);
        return (consumedInts + 16 + s);
      }

      keys >>= 16;
      Data = _decode_avx((keys & 0x00FF), &dataPtr);
      PrevLow = _scan_avx_d1(Data, PrevHi);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      PrevHi = _scan_avx_d1(Data, PrevLow);
      // check 8 ints
      if (key <= (uint32_t)_mm_extract_epi32(PrevHi, 3)) {
        int s = find_lower_bound(&PrevHi, &PrevLow, key, presult);
        return (consumedInts + 24 + s);
      }

      consumedInts += 32;
    }
    {
      uint64_t keys = nextkeys;
      // faster 16-bit delta since we only have 8-bit values
      if (!keys) { // 32 1-byte ints in a row
        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr)));
        _scan_16bit_avx_d1(Data, &PrevHi, &PrevLow);
        // check 8 ints
        if (key <= (uint32_t)_mm_extract_epi32(PrevHi, 3)) {
          int s = find_lower_bound(&PrevHi, &PrevLow, key, presult);
          return (consumedInts + s);
        }
        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr + 8)));
        _scan_16bit_avx_d1(Data, &PrevHi, &PrevLow);
        // check 8 ints
        if (key <= (uint32_t)_mm_extract_epi32(PrevHi, 3)) {
          int s = find_lower_bound(&PrevHi, &PrevLow, key, presult);
          return (consumedInts + 8 + s);
        }
        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr + 16)));
        _scan_16bit_avx_d1(Data, &PrevHi, &PrevLow);
        // check 8 ints
        if (key <= (uint32_t)_mm_extract_epi32(PrevHi, 3)) {
          int s = find_lower_bound(&PrevHi, &PrevLow, key, presult);
          return (consumedInts + 16 + s);
        }
        Data = _mm_cvtepu8_epi16(_mm_loadl_epi64((xmm_t *)(dataPtr + 24)));
        _scan_16bit_avx_d1(Data, &PrevHi, &PrevLow);
        // check 8 ints
        if (key <= (uint32_t)_mm_extract_epi32(PrevHi, 3)) {
          int s = find_lower_bound(&PrevHi, &PrevLow, key, presult);
          return (consumedInts + 24 + s);
        }

        dataPtr += 32;
        consumedInts += 32;

      } else {

        Data = _decode_avx(keys & 0x00FF, &dataPtr);
        PrevLow = _scan_avx_d1(Data, PrevHi);
        Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
        PrevHi = _scan_avx_d1(Data, PrevLow);
        // check 8 ints
        if (key <= (uint32_t)_mm_extract_epi32(PrevHi, 3)) {
          int s = find_lower_bound(&PrevHi, &PrevLow, key, presult);
          return (consumedInts + s);
        }

        keys >>= 16;
        Data = _decode_avx((keys & 0x00FF), &dataPtr);
        PrevLow = _scan_avx_d1(Data, PrevHi);
        Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
        PrevHi = _scan_avx_d1(Data, PrevLow);
        // check 8 ints
        if (key <= (uint32_t)_mm_extract_epi32(PrevHi, 3)) {
          int s = find_lower_bound(&PrevHi, &PrevLow, key, presult);
          return (consumedInts + 8 + s);
        }

        keys >>= 16;
        Data = _decode_avx((keys & 0x00FF), &dataPtr);
        PrevLow = _scan_avx_d1(Data, PrevHi);
        Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
        PrevHi = _scan_avx_d1(Data, PrevLow);
        // check 8 ints
        if (key <= (uint32_t)_mm_extract_epi32(PrevHi, 3)) {
          int s = find_lower_bound(&PrevHi, &PrevLow, key, presult);
          return (consumedInts + 16 + s);
        }

        keys >>= 16;
        Data = _decode_avx((keys & 0x00FF), &dataPtr);
        PrevLow = _scan_avx_d1(Data, PrevHi);
        Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
        PrevHi = _scan_avx_d1(Data, PrevLow);
        // check 8 ints
        if (key <= (uint32_t)_mm_extract_epi32(PrevHi, 3)) {
          int s = find_lower_bound(&PrevHi, &PrevLow, key, presult);
          return (consumedInts + 24 + s);
        }

        consumedInts += 32;
      }
      // extract the highest integer which was stored so far;
      // it will be the "prev" value for the remaining data
      prev = (uint32_t)_mm_extract_epi32(PrevHi, 3);
    }
  }
  uint32_t keysleft = count & 31;
  if (keysleft > 0) {
    uint64_t consumedkeys = keybytes - (keybytes & 7);
    uint32_t out[32];
    svb_decode_scalar_d1_init(out, keyPtr + consumedkeys, dataPtr, keysleft,
                              prev);
    if (key <= out[keysleft - 1]) {
      int s = lower_bound(out, key, 0, keysleft);
      assert(s >= 0 && s < (int)keysleft);
      *presult = out[s];
      return (consumedInts + s);
    }
  }

  // key was not found!
  *presult = key + 1;
  return (int)count;
}

int svb_find_scalar_d1_init(uint8_t *keyPtr, uint8_t *dataPtr, uint64_t count,
                            uint32_t prev, uint32_t searchkey,
                            uint32_t *presult) {
  uint8_t shift = 0;
  uint32_t key = *keyPtr++;

  for (uint32_t c = 0; c < count; c++) {
    if (shift == 8) {
      shift = 0;
      key = *keyPtr++;
    }
    prev += _decode_data(&dataPtr, (key >> shift) & 0x3);
    if (prev >= searchkey) {
      *presult = prev;
      return (c);
    }

    shift += 2;
  }

  *presult = searchkey + 1;
  return (int)count;
}

uint32_t svb_select_scalar_d1_init(uint8_t *keyPtr, uint8_t *dataPtr,
                                   uint64_t count, uint32_t prev, int slot) {
  uint8_t shift = 0;
  uint32_t key = *keyPtr++;

  (void)count;

  // make sure that the loop is run at least once
  for (int c = 0; c <= slot; c++) {
    if (shift == 8) {
      shift = 0;
      key = *keyPtr++;
    }
    prev += _decode_data(&dataPtr, (key >> shift) & 0x3);
    shift += 2;
  }

  return prev;
}

uint32_t svb_select_avx_d1_init(uint8_t *keyPtr, uint8_t *dataPtr,
                                uint64_t count, uint32_t prev, int slot) {
  uint64_t keybytes = count / 4; // number of key bytes
  int consumedInts = 0;

  // make sure that the loop is run at least once
  if (keybytes >= 8) {
    xmm_t PrevHi = _mm_set1_epi32(prev);
    xmm_t PrevLow;
    xmm_t Data;

    int64_t Offset = -(int64_t)keybytes / 8 + 1;

    const uint64_t *keyPtr64 = (const uint64_t *)keyPtr - Offset;
    uint64_t nextkeys = keyPtr64[Offset];
    for (; Offset != 0; ++Offset) {
      uint64_t keys = nextkeys;
      nextkeys = keyPtr64[Offset + 1];
      // faster 16-bit delta since we only have 8-bit values
      if (!keys) { // 32 1-byte ints in a row

        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr)));
        _scan_16bit_avx_d1(Data, &PrevHi, &PrevLow);
        // check 8 ints
        if (slot < consumedInts + 8)
          return _extract_from_xmm(&PrevHi, &PrevLow, slot - consumedInts);

        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr + 8)));
        _scan_16bit_avx_d1(Data, &PrevHi, &PrevLow);
        // check 8 ints
        if (slot < consumedInts + 16)
          return _extract_from_xmm(&PrevHi, &PrevLow,
                                   slot - (consumedInts + 8));

        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr + 16)));
        _scan_16bit_avx_d1(Data, &PrevHi, &PrevLow);
        // check 8 ints
        if (slot < consumedInts + 24)
          return _extract_from_xmm(&PrevHi, &PrevLow,
                                   slot - (consumedInts + 16));

        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr + 24)));
        _scan_16bit_avx_d1(Data, &PrevHi, &PrevLow);
        // check 8 ints
        if (slot < consumedInts + 32)
          return _extract_from_xmm(&PrevHi, &PrevLow,
                                   slot - (consumedInts + 24));

        dataPtr += 32;
        consumedInts += 32;
        continue;
      }

      Data = _decode_avx(keys & 0x00FF, &dataPtr);
      PrevLow = _scan_avx_d1(Data, PrevHi);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      PrevHi = _scan_avx_d1(Data, PrevLow);
      // check 8 ints
      if (slot < consumedInts + 8)
        return _extract_from_xmm(&PrevHi, &PrevLow, slot - consumedInts);

      keys >>= 16;
      Data = _decode_avx((keys & 0x00FF), &dataPtr);
      PrevLow = _scan_avx_d1(Data, PrevHi);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      PrevHi = _scan_avx_d1(Data, PrevLow);
      // check 8 ints
      if (slot < consumedInts + 16)
        return _extract_from_xmm(&PrevHi, &PrevLow, slot - (consumedInts + 8));

      keys >>= 16;
      Data = _decode_avx((keys & 0x00FF), &dataPtr);
      PrevLow = _scan_avx_d1(Data, PrevHi);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      PrevHi = _scan_avx_d1(Data, PrevLow);
      // check 8 ints
      if (slot < consumedInts + 24)
        return _extract_from_xmm(&PrevHi, &PrevLow, slot - (consumedInts + 16));

      keys >>= 16;
      Data = _decode_avx((keys & 0x00FF), &dataPtr);
      PrevLow = _scan_avx_d1(Data, PrevHi);
      Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
      PrevHi = _scan_avx_d1(Data, PrevLow);
      // check 8 ints
      if (slot < consumedInts + 32)
        return _extract_from_xmm(&PrevHi, &PrevLow, slot - (consumedInts + 24));

      consumedInts += 32;
    }

    {
      uint64_t keys = nextkeys;
      // faster 16-bit delta since we only have 8-bit values
      if (!keys) { // 32 1-byte ints in a row
        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr)));
        _scan_16bit_avx_d1(Data, &PrevHi, &PrevLow);
        // check 8 ints
        if (slot < consumedInts + 8)
          return _extract_from_xmm(&PrevHi, &PrevLow, slot - consumedInts);

        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr + 8)));
        _scan_16bit_avx_d1(Data, &PrevHi, &PrevLow);
        // check 8 ints
        if (slot < consumedInts + 16)
          return _extract_from_xmm(&PrevHi, &PrevLow,
                                   slot - (consumedInts + 8));

        Data = _mm_cvtepu8_epi16(_mm_lddqu_si128((xmm_t *)(dataPtr + 16)));
        _scan_16bit_avx_d1(Data, &PrevHi, &PrevLow);
        // check 8 ints
        if (slot < consumedInts + 24)
          return _extract_from_xmm(&PrevHi, &PrevLow,
                                   slot - (consumedInts + 16));

        Data = _mm_cvtepu8_epi16(_mm_loadl_epi64((xmm_t *)(dataPtr + 24)));
        _scan_16bit_avx_d1(Data, &PrevHi, &PrevLow);
        // check 8 ints
        if (slot < consumedInts + 32)
          return _extract_from_xmm(&PrevHi, &PrevLow,
                                   slot - (consumedInts + 24));

        dataPtr += 32;
        consumedInts += 32;

      } else {

        Data = _decode_avx(keys & 0x00FF, &dataPtr);
        PrevLow = _scan_avx_d1(Data, PrevHi);
        Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
        PrevHi = _scan_avx_d1(Data, PrevLow);
        // check 8 ints
        if (slot < consumedInts + 8)
          return _extract_from_xmm(&PrevHi, &PrevLow, slot - consumedInts);

        keys >>= 16;
        Data = _decode_avx((keys & 0x00FF), &dataPtr);
        PrevLow = _scan_avx_d1(Data, PrevHi);
        Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
        PrevHi = _scan_avx_d1(Data, PrevLow);
        // check 8 ints
        if (slot < consumedInts + 16)
          return _extract_from_xmm(&PrevHi, &PrevLow,
                                   slot - (consumedInts + 8));

        keys >>= 16;
        Data = _decode_avx((keys & 0x00FF), &dataPtr);
        PrevLow = _scan_avx_d1(Data, PrevHi);
        Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
        PrevHi = _scan_avx_d1(Data, PrevLow);
        // check 8 ints
        if (slot < consumedInts + 24)
          return _extract_from_xmm(&PrevHi, &PrevLow,
                                   slot - (consumedInts + 16));

        keys >>= 16;
        Data = _decode_avx((keys & 0x00FF), &dataPtr);
        PrevLow = _scan_avx_d1(Data, PrevHi);
        Data = _decode_avx((keys & 0xFF00) >> 8, &dataPtr);
        PrevHi = _scan_avx_d1(Data, PrevLow);
        // check 8 ints
        if (slot < consumedInts + 32)
          return _extract_from_xmm(&PrevHi, &PrevLow,
                                   slot - (consumedInts + 24));

        consumedInts += 32;
      }
      // extract the highest integer which was stored so far;
      // it will be the "prev" value for the remaining data
      prev = (uint32_t)_mm_extract_epi32(PrevHi, 3);
    }
  }
  uint64_t consumedkeys = keybytes - (keybytes & 7);
  uint32_t keysleft = count & 31;
  return svb_select_scalar_d1_init(keyPtr + consumedkeys, dataPtr, keysleft,
                                   prev, slot - consumedInts);
}
