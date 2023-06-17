/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/ and Owen Kaser
 */

#ifndef ROLLEDBITPACKING_H_
#define ROLLEDBITPACKING_H_

#include <exception> // logic_error, runtime_error
#include "common.h"
#include "util.h"

namespace FastPForLib {

/***
 * The idea here is not to unroll the loops and to let the
 * compiler decide how to best do it. It turns out to be
 * very difficult to get the same performance as manually
 * generated functions. Still, we leave this code here to
 * show that we made a significant effort to do it the
 * "proper" way from a software engineering perspective.
 */

#if !defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#pragma GCC push_options
#pragma GCC optimize("O3")
#pragma GCC optimize(                                                          \
    "unroll-loops") // use cmd-line --param max-unroll-times=32?
#endif

// crazy function that basically just does
// x -> x % (1U<<bit)
// should be nearly optimized away by the compiler.
template <uint32_t bit, bool mask> constexpr uint32_t __bitmask(uint32_t val) {
  return ((mask == false) || (bit == 32))
             ? val
             : (bit == 0
                    ? 0
                    : val & ((1U << (bit >= 31 ? 31 : bit)) -
                             1)); // trust compiler to optimize away the modulo
}

/**
 * This code does not assume that the output has been zeroed.
 */
template <uint32_t bit, bool mask>
void __pack(const uint32_t *__restrict__ in, uint32_t *__restrict__ out) {
  uint32_t inwordpointer = 0; // indicates where we are reading
  // k iterates over the output
  for (uint32_t k = 0; k < bit; ++k, ++out) {
    if (inwordpointer == 0) {
      *out = __bitmask<bit, mask>(*in++);
      inwordpointer += bit;
    }
    // compiler (gcc 4.6.3) had trouble with this, cannot unroll
    // while(inwordpointer <= 32-bit) {
    //    *out |= __bitmask<bit, mask> (*in++) << inwordpointer;
    //    inwordpointer += bit;
    //}
    // attempt (grungy!)

    // loop cannot iterate much
    for (uint32_t i = 0; i < 1 + 32 / bit; ++i) {
      if (inwordpointer > 32 - bit)
        break;
      *out |= __bitmask<bit, mask>(*in++) << inwordpointer;
      inwordpointer += bit;
    }
    // end ofk grunge

    if (inwordpointer < 32) {
      *out |= __bitmask<bit, mask>(*in) << inwordpointer;
      inwordpointer += bit - 32;
      *(out + 1) = __bitmask<bit, mask>(*in++) >> (bit - inwordpointer);

    } else
      // inwordpointer==32
      inwordpointer = 0;
  }
}

/**
 * Alternative to __pack
 */
template <uint32_t bit, bool mask>
void __pack_tight(const uint32_t *__restrict__ in, uint32_t *__restrict__ out) {
  assert(bit <= 32);
  if (bit == 32) {
    for (int k = 0; k < 32; ++k) {
      out[k] = in[k];
    }
    return;
  }

  if (bit == 16) {
    for (uint32_t outer = 0; outer < 16; ++outer) {
      for (uint32_t inwordpointer = 0; inwordpointer < 32; inwordpointer += 16)
        *out |= __bitmask<bit, mask>(*(in++)) << inwordpointer;
      ++out;
    }
    return;
  }
  if (bit == 8) {
    for (uint32_t outer = 0; outer < 8; ++outer) {
      for (uint32_t inwordpointer = 0; inwordpointer < 32; inwordpointer += 8)
        *out |= __bitmask<bit, mask>(*(in++)) << inwordpointer;
      ++out;
    }
    return;
  }
#if !defined(_MSC_VER) && !defined(__INTEL_COMPILER)
  enum { mygcd = gcd(bit, 32) };
#else
  const uint32_t mygcd = gcd(bit, 32);
#endif
  // iterate over bit position of the output
  if ((bit & 1) == 0) {
    for (uint32_t t = 0; t < mygcd; ++t) {
      for (uint32_t pointer = 0; pointer < 32 * bit / mygcd; pointer += bit) {
        uint32_t inword = (pointer % 32);
        const uint32_t v = __bitmask<bit, mask>(*in++);
        out[pointer / 32] |= v << inword;
        if (inword > 32 - bit)
          out[pointer / 32 + 1] = v >> (32 - inword);
      }
      out += bit / mygcd;
    }
  } else {
    for (uint32_t pointer = 0; pointer < 32 * bit;) {
      for (int i = 0; i < 2; ++i, pointer += bit) {
        uint32_t inword = (pointer % 32);
        const uint32_t v = __bitmask<bit, mask>(*in++);
        out[pointer / 32] |= v << inword;
        if (inword > 32 - bit)
          out[pointer / 32 + 1] = v >> (32 - inword);
      }
    }
    out += bit / mygcd;
  }
}

/**
 * Alternative to __unpack
 */
template <uint32_t bit, bool mask>
void __unpack_tight(const uint32_t *__restrict__ in,
                    uint32_t *__restrict__ out) {
  assert(bit <= 32);
  if (bit == 32) {
    for (int k = 0; k < 32; ++k) {
      out[k] = in[k];
    }
    return;
  }
  if (bit == 16) {
    for (uint32_t outer = 0; outer < 16; ++outer) {
      for (uint32_t inwordpointer = 0; inwordpointer < 32; inwordpointer += 16)
        *(out++) = __bitmask<bit, mask>((*in) >> inwordpointer);
      ++in;
    }
    return;
  }
  if (bit == 8) {
    for (uint32_t outer = 0; outer < 8; ++outer) {
      for (uint32_t inwordpointer = 0; inwordpointer < 32; inwordpointer += 8)
        *(out++) = __bitmask<bit, mask>((*in) >> inwordpointer);
      ++in;
    }
    return;
  }

#if !defined(_MSC_VER) && !defined(__INTEL_COMPILER)
  enum { mygcd = gcd(bit, 32) };
#else
  const uint32_t mygcd = gcd(bit, 32);
#endif

  for (uint32_t t = 0; t < mygcd; ++t) {
    for (uint32_t pointer = 0; pointer < 32 * bit / mygcd; pointer += bit) {
      const uint32_t inword = (pointer % 32);
      *out = in[pointer / 32] >> inword;
      if (inword > 32 - bit)
        *out |= (in[pointer / 32 + 1] << (32 - inword));
      *out = __bitmask<bit, mask>(*out);
      ++out;
    }
    in += bit / mygcd;
  }
  return;
}

// from bit offset compute byte offset +
uint32_t byteoffset(uint32_t offset) {
  if (offset < 8)
    return 0;
  if (offset < 16)
    return 1;
  if (offset < 24)
    return 2;
  return 3;
}

/**
 * variable length
 */
template <uint32_t bit, bool mask, uint32_t length>
uint8_t *__pack_vl(const uint32_t *__restrict__ in,
                   uint8_t *__restrict__ outbyte) {
  assert(bit <= 32);
  uint32_t counter = 0;
  uint32_t *out = reinterpret_cast<uint32_t *>(outbyte);
#if !defined(_MSC_VER) && !defined(__INTEL_COMPILER)
  enum { mygcd = gcd(bit, 32) };
#else
  const uint32_t mygcd = gcd(bit, 32);
#endif

  // iterate over bit position of the output
  for (uint32_t t = 0; t < mygcd; ++t) {
    out[0] = __bitmask<bit, mask>(*in++);
    if (++counter == length)
      return reinterpret_cast<uint8_t *>(&out[0]) + 1 + byteoffset(bit);
    for (uint32_t pointer = bit; pointer < 32 * bit / mygcd; pointer += bit) {
      const uint32_t inword = (pointer % 32);
      out[pointer / 32] |= __bitmask<bit, mask>(*in) << inword;
      if (inword > 32 - bit) {
        out[pointer / 32 + 1] = __bitmask<bit, mask>(*in) >> (32 - inword);
        if (++counter == length)
          return reinterpret_cast<uint8_t *>(&out[pointer / 32 + 1]) + 1 +
                 byteoffset(bit + 32 - inword - 1);
      } else {
        if (++counter == length)
          return reinterpret_cast<uint8_t *>(&out[pointer / 32]) + 1 +
                 byteoffset(bit + inword - 1);
      }
      ++in;
    }
    out += bit / mygcd;
  }
  throw std::logic_error("should never get here");
}

/**
 * variable length
 */
template <uint32_t bit, uint32_t length, bool mask>
const uint8_t *__unpack_vl(const uint8_t *__restrict__ inbyte,
                           uint32_t *__restrict__ out) {
  assert(bit <= 32);
  uint32_t counter = 0;

#if !defined(_MSC_VER) && !defined(__INTEL_COMPILER)
  enum { mygcd = gcd(bit, 32) };
#else
  const uint32_t mygcd = gcd(bit, 32);
#endif
  const uint32_t *in = reinterpret_cast<const uint32_t *>(inbyte);
  for (uint32_t t = 0; t < mygcd; ++t) {
    for (uint32_t pointer = 0; pointer < 32 * bit / mygcd; pointer += bit) {
      const uint32_t inword = (pointer % 32);
      if (inword > 32 - bit) {
        *out++ = __bitmask<bit, mask>((in[pointer / 32] >> inword) |
                                      (in[pointer / 32 + 1] << (32 - inword)));
        if (++counter == length)
          return reinterpret_cast<const uint8_t *>(&in[pointer / 32 + 1]) + 1 +
                 byteoffset(bit + 32 - inword - 1);
      } else {
        *out++ = __bitmask<bit, mask>(in[pointer / 32] >> inword);
        if (++counter == length)
          return reinterpret_cast<const uint8_t *>(&in[pointer / 32]) + 1 +
                 byteoffset(bit + inword - 1);
      }
    }
    in += bit / mygcd;
  }
  throw std::logic_error("should never get here");
}

template <uint32_t bit>
void __unpack(const uint32_t *__restrict__ in, uint32_t *__restrict__ out) {
  uint32_t inwordpointer = 0; // indicates where we are writing
  // k iterates over the input
  for (uint32_t k = 0; k < bit; ++k, ++in) {
    // did not unroll; compiler did not detect invariant that inwordpointer is
    // always
    // small positive number
    /* for (; inwordpointer   /\*+   bit <= 32 *\/ <= 32-bit; inwordpointer +=
     * bit) { */
    /*       *out++ = __bitmask<bit> ((*in) >> inwordpointer); */
    /*   } */
    // grungy replacement to help with unrolling
    for (uint32_t i = 0; i < 1 + 32 / bit; ++i) {
      if (inwordpointer > 32 - bit)
        break;
      *out++ = __bitmask<bit, true>((*in) >> inwordpointer);
      inwordpointer += bit;
    }
    // end ofk grunge

    if (inwordpointer < 32) {
      *out++ = (in[0] >> inwordpointer) |
               __bitmask<bit, true>(in[1] << (32 - inwordpointer));
      inwordpointer += bit - 32;
    } else
      // inwordpointer==32
      inwordpointer = 0;
  }
}
#if !defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#pragma GCC pop_options
#endif

void unpack(const uint32_t *__restrict__ in, uint32_t *__restrict__ out,
            const uint32_t bit) {

  switch (bit) {
  case 0:
    for (uint32_t k = 0; k < 32; ++k)
      out[k] = 0;
    break;
  case 1:
    __unpack<1>(in, out);
    break;
  case 2:
    __unpack<2>(in, out);
    break;
  case 3:
    __unpack<3>(in, out);
    break;
  case 4:
    __unpack<4>(in, out);
    break;
  case 5:
    __unpack<5>(in, out);
    break;
  case 6:
    __unpack<6>(in, out);
    break;
  case 7:
    __unpack<7>(in, out);
    break;
  case 8:
    __unpack<8>(in, out);
    break;
  case 9:
    __unpack<9>(in, out);
    break;
  case 10:
    __unpack<10>(in, out);
    break;
  case 11:
    __unpack<11>(in, out);
    break;
  case 12:
    __unpack<12>(in, out);
    break;
  case 13:
    __unpack<13>(in, out);
    break;
  case 14:
    __unpack<14>(in, out);
    break;
  case 15:
    __unpack<15>(in, out);
    break;
  case 16:
    __unpack<16>(in, out);
    break;
  case 17:
    __unpack<17>(in, out);
    break;
  case 18:
    __unpack<18>(in, out);
    break;
  case 19:
    __unpack<19>(in, out);
    break;
  case 20:
    __unpack<20>(in, out);
    break;
  case 21:
    __unpack<21>(in, out);
    break;
  case 22:
    __unpack<22>(in, out);
    break;
  case 23:
    __unpack<23>(in, out);
    break;
  case 24:
    __unpack<24>(in, out);
    break;
  case 25:
    __unpack<25>(in, out);
    break;
  case 26:
    __unpack<26>(in, out);
    break;
  case 27:
    __unpack<27>(in, out);
    break;
  case 28:
    __unpack<28>(in, out);
    break;
  case 29:
    __unpack<29>(in, out);
    break;
  case 30:
    __unpack<30>(in, out);
    break;
  case 31:
    __unpack<31>(in, out);
    break;
  case 32:
    __unpack<32>(in, out);
    break;
  default:
    throw std::runtime_error("Unsupported number of bits");
  }
}

template <bool mask>
void pack(const uint32_t *__restrict__ in, uint32_t *__restrict__ out,
          const uint32_t bit) {
  switch (bit) {
  case 0:
    // nothing to do
    break;
  case 1:
    __pack<1, mask>(in, out);
    break;
  case 2:
    __pack<2, mask>(in, out);
    break;
  case 3:
    __pack<3, mask>(in, out);
    break;
  case 4:
    __pack<4, mask>(in, out);
    break;
  case 5:
    __pack<5, mask>(in, out);
    break;
  case 6:
    __pack<6, mask>(in, out);
    break;
  case 7:
    __pack<7, mask>(in, out);
    break;
  case 8:
    __pack<8, mask>(in, out);
    break;
  case 9:
    __pack<9, mask>(in, out);
    break;
  case 10:
    __pack<10, mask>(in, out);
    break;
  case 11:
    __pack<11, mask>(in, out);
    break;
  case 12:
    __pack<12, mask>(in, out);
    break;
  case 13:
    __pack<13, mask>(in, out);
    break;
  case 14:
    __pack<14, mask>(in, out);
    break;
  case 15:
    __pack<15, mask>(in, out);
    break;
  case 16:
    __pack<16, mask>(in, out);
    break;
  case 17:
    __pack<17, mask>(in, out);
    break;
  case 18:
    __pack<18, mask>(in, out);
    break;
  case 19:
    __pack<19, mask>(in, out);
    break;
  case 20:
    __pack<20, mask>(in, out);
    break;
  case 21:
    __pack<21, mask>(in, out);
    break;
  case 22:
    __pack<22, mask>(in, out);
    break;
  case 23:
    __pack<23, mask>(in, out);
    break;
  case 24:
    __pack<24, mask>(in, out);
    break;
  case 25:
    __pack<25, mask>(in, out);
    break;
  case 26:
    __pack<26, mask>(in, out);
    break;
  case 27:
    __pack<27, mask>(in, out);
    break;
  case 28:
    __pack<28, mask>(in, out);
    break;
  case 29:
    __pack<29, mask>(in, out);
    break;
  case 30:
    __pack<30, mask>(in, out);
    break;
  case 31:
    __pack<31, mask>(in, out);
    break;
  case 32:
    __pack<32, mask>(in, out);
    break;
  default:
    throw std::runtime_error("Unsupported number of bits");
  }
}

template <bool mask>
void pack_tight(const uint32_t *__restrict__ in, uint32_t *__restrict__ out,
                const uint32_t bit) {
  switch (bit) {
  case 0:
    // nothing to do
    break;
  case 1:
    __pack_tight<1, mask>(in, out);
    break;
  case 2:
    __pack_tight<2, mask>(in, out);
    break;
  case 3:
    __pack_tight<3, mask>(in, out);
    break;
  case 4:
    __pack_tight<4, mask>(in, out);
    break;
  case 5:
    __pack_tight<5, mask>(in, out);
    break;
  case 6:
    __pack_tight<6, mask>(in, out);
    break;
  case 7:
    __pack_tight<7, mask>(in, out);
    break;
  case 8:
    __pack_tight<8, mask>(in, out);
    break;
  case 9:
    __pack_tight<9, mask>(in, out);
    break;
  case 10:
    __pack_tight<10, mask>(in, out);
    break;
  case 11:
    __pack_tight<11, mask>(in, out);
    break;
  case 12:
    __pack_tight<12, mask>(in, out);
    break;
  case 13:
    __pack_tight<13, mask>(in, out);
    break;
  case 14:
    __pack_tight<14, mask>(in, out);
    break;
  case 15:
    __pack_tight<15, mask>(in, out);
    break;
  case 16:
    __pack_tight<16, mask>(in, out);
    break;
  case 17:
    __pack_tight<17, mask>(in, out);
    break;
  case 18:
    __pack_tight<18, mask>(in, out);
    break;
  case 19:
    __pack_tight<19, mask>(in, out);
    break;
  case 20:
    __pack_tight<20, mask>(in, out);
    break;
  case 21:
    __pack_tight<21, mask>(in, out);
    break;
  case 22:
    __pack_tight<22, mask>(in, out);
    break;
  case 23:
    __pack_tight<23, mask>(in, out);
    break;
  case 24:
    __pack_tight<24, mask>(in, out);
    break;
  case 25:
    __pack_tight<25, mask>(in, out);
    break;
  case 26:
    __pack_tight<26, mask>(in, out);
    break;
  case 27:
    __pack_tight<27, mask>(in, out);
    break;
  case 28:
    __pack_tight<28, mask>(in, out);
    break;
  case 29:
    __pack_tight<29, mask>(in, out);
    break;
  case 30:
    __pack_tight<30, mask>(in, out);
    break;
  case 31:
    __pack_tight<31, mask>(in, out);
    break;
  case 32:
    __pack_tight<32, mask>(in, out);
    break;
  default:
    throw std::runtime_error("Unsupported number of bits");
  }
}

void unpack_tight(const uint32_t *__restrict__ in, uint32_t *__restrict__ out,
                  const uint32_t bit) {

  switch (bit) {
  case 0:
    for (uint32_t k = 0; k < 32; ++k)
      out[k] = 0;
    break;
  case 1:
    __unpack_tight<1, true>(in, out);
    break;
  case 2:
    __unpack_tight<2, true>(in, out);
    break;
  case 3:
    __unpack_tight<3, true>(in, out);
    break;
  case 4:
    __unpack_tight<4, true>(in, out);
    break;
  case 5:
    __unpack_tight<5, true>(in, out);
    break;
  case 6:
    __unpack_tight<6, true>(in, out);
    break;
  case 7:
    __unpack_tight<7, true>(in, out);
    break;
  case 8:
    __unpack_tight<8, true>(in, out);
    break;
  case 9:
    __unpack_tight<9, true>(in, out);
    break;
  case 10:
    __unpack_tight<10, true>(in, out);
    break;
  case 11:
    __unpack_tight<11, true>(in, out);
    break;
  case 12:
    __unpack_tight<12, true>(in, out);
    break;
  case 13:
    __unpack_tight<13, true>(in, out);
    break;
  case 14:
    __unpack_tight<14, true>(in, out);
    break;
  case 15:
    __unpack_tight<15, true>(in, out);
    break;
  case 16:
    __unpack_tight<16, true>(in, out);
    break;
  case 17:
    __unpack_tight<17, true>(in, out);
    break;
  case 18:
    __unpack_tight<18, true>(in, out);
    break;
  case 19:
    __unpack_tight<19, true>(in, out);
    break;
  case 20:
    __unpack_tight<20, true>(in, out);
    break;
  case 21:
    __unpack_tight<21, true>(in, out);
    break;
  case 22:
    __unpack_tight<22, true>(in, out);
    break;
  case 23:
    __unpack_tight<23, true>(in, out);
    break;
  case 24:
    __unpack_tight<24, true>(in, out);
    break;
  case 25:
    __unpack_tight<25, true>(in, out);
    break;
  case 26:
    __unpack_tight<26, true>(in, out);
    break;
  case 27:
    __unpack_tight<27, true>(in, out);
    break;
  case 28:
    __unpack_tight<28, true>(in, out);
    break;
  case 29:
    __unpack_tight<29, true>(in, out);
    break;
  case 30:
    __unpack_tight<30, true>(in, out);
    break;
  case 31:
    __unpack_tight<31, true>(in, out);
    break;
  case 32:
    __unpack_tight<32, true>(in, out);
    break;
  default:
    throw std::runtime_error("Unsupported number of bits");
  }
}

template <bool mask, uint32_t length>
uint8_t *pack_vl(const uint32_t *__restrict__ in, uint8_t *__restrict__ out,
                 const uint32_t bit) {
  switch (bit) {
  case 0:
    // nothing to do
    return out;
  case 1:
    return __pack_vl<1, mask, length>(in, out);

  case 2:
    return __pack_vl<2, mask, length>(in, out);

  case 3:
    return __pack_vl<3, mask, length>(in, out);

  case 4:
    return __pack_vl<4, mask, length>(in, out);

  case 5:
    return __pack_vl<5, mask, length>(in, out);

  case 6:
    return __pack_vl<6, mask, length>(in, out);

  case 7:
    return __pack_vl<7, mask, length>(in, out);

  case 8:
    return __pack_vl<8, mask, length>(in, out);

  case 9:
    return __pack_vl<9, mask, length>(in, out);

  case 10:
    return __pack_vl<10, mask, length>(in, out);

  case 11:
    return __pack_vl<11, mask, length>(in, out);

  case 12:
    return __pack_vl<12, mask, length>(in, out);

  case 13:
    return __pack_vl<13, mask, length>(in, out);

  case 14:
    return __pack_vl<14, mask, length>(in, out);

  case 15:
    return __pack_vl<15, mask, length>(in, out);

  case 16:
    return __pack_vl<16, mask, length>(in, out);

  case 17:
    return __pack_vl<17, mask, length>(in, out);

  case 18:
    return __pack_vl<18, mask, length>(in, out);

  case 19:
    return __pack_vl<19, mask, length>(in, out);

  case 20:
    return __pack_vl<20, mask, length>(in, out);

  case 21:
    return __pack_vl<21, mask, length>(in, out);

  case 22:
    return __pack_vl<22, mask, length>(in, out);

  case 23:
    return __pack_vl<23, mask, length>(in, out);

  case 24:
    return __pack_vl<24, mask, length>(in, out);

  case 25:
    return __pack_vl<25, mask, length>(in, out);

  case 26:
    return __pack_vl<26, mask, length>(in, out);

  case 27:
    return __pack_vl<27, mask, length>(in, out);

  case 28:
    return __pack_vl<28, mask, length>(in, out);

  case 29:
    return __pack_vl<29, mask, length>(in, out);

  case 30:
    return __pack_vl<30, mask, length>(in, out);

  case 31:
    return __pack_vl<31, mask, length>(in, out);

  case 32:
    return __pack_vl<32, mask, length>(in, out);

  default:
    throw std::runtime_error("Unsupported number of bits");
  }
}

template <uint32_t length>
const uint8_t *unpack_vl(const uint8_t *__restrict__ in,
                         uint32_t *__restrict__ out, const uint32_t bit) {

  switch (bit) {
  case 0:
    for (uint32_t k = 0; k < length; ++k)
      out[k] = 0;
    return in;

  case 1:
    return __unpack_vl<1, length>(in, out);

  case 2:
    return __unpack_vl<2, length>(in, out);

  case 3:
    return __unpack_vl<3, length>(in, out);

  case 4:
    return __unpack_vl<4, length>(in, out);

  case 5:
    return __unpack_vl<5, length>(in, out);

  case 6:
    return __unpack_vl<6, length>(in, out);

  case 7:
    return __unpack_vl<7, length>(in, out);

  case 8:
    return __unpack_vl<8, length>(in, out);

  case 9:
    return __unpack_vl<9, length>(in, out);

  case 10:
    return __unpack_vl<10, length>(in, out);

  case 11:
    return __unpack_vl<11, length>(in, out);

  case 12:
    return __unpack_vl<12, length>(in, out);

  case 13:
    return __unpack_vl<13, length>(in, out);

  case 14:
    return __unpack_vl<14, length>(in, out);

  case 15:
    return __unpack_vl<15, length>(in, out);

  case 16:
    return __unpack_vl<16, length>(in, out);

  case 17:
    return __unpack_vl<17, length>(in, out);

  case 18:
    return __unpack_vl<18, length>(in, out);

  case 19:
    return __unpack_vl<19, length>(in, out);

  case 20:
    return __unpack_vl<20, length>(in, out);

  case 21:
    return __unpack_vl<21, length>(in, out);

  case 22:
    return __unpack_vl<22, length>(in, out);

  case 23:
    return __unpack_vl<23, length>(in, out);

  case 24:
    return __unpack_vl<24, length>(in, out);

  case 25:
    return __unpack_vl<25, length>(in, out);

  case 26:
    return __unpack_vl<26, length>(in, out);

  case 27:
    return __unpack_vl<27, length>(in, out);

  case 28:
    return __unpack_vl<28, length>(in, out);

  case 29:
    return __unpack_vl<29, length>(in, out);

  case 30:
    return __unpack_vl<30, length>(in, out);

  case 31:
    return __unpack_vl<31, length>(in, out);

  case 32:
    return __unpack_vl<32, length>(in, out);

  default:
    throw std::runtime_error("Unsupported number of bits");
  }
}

} // namespace FastPFor

#endif /* ROLLEDBITPACKING_H_ */
