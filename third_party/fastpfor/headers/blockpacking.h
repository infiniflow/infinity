/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 * and Owen Kaser
 */

#ifndef BLOCKPACKING_H_
#define BLOCKPACKING_H_

#include "codecs.h"
#include "bitpackingunaligned.h"
#include "bitpackingaligned.h"
#include "util.h"

namespace FastPForLib {

/**
 * This is 32-bit *aligned* binary packing, designed from the
 * ground up.
 *
 * For all purposes, it has performance equal to ByteAlignedPacking
 * with the align template parameter set to true, but it is slightly
 * more elegant.
 */
template <uint32_t MiniBlockSize> class BinaryPacking : public IntegerCODEC {
public:
  static const uint32_t HowManyMiniBlocks = 16;
  static const uint32_t BlockSize = HowManyMiniBlocks * MiniBlockSize;
  static const uint32_t bits32 = 5; // constexprbits(32);

  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) {
    checkifdivisibleby(length, BlockSize);
    const uint32_t *const initout(out);
    *out++ = length;
    uint32_t Bs[HowManyMiniBlocks];
    for (const uint32_t *const final = in + length; in + BlockSize <= final;
         in += BlockSize) {
      for (uint32_t i = 0; i < HowManyMiniBlocks; ++i)
        Bs[i] = maxbits(in + i * MiniBlockSize, in + (i + 1) * MiniBlockSize);
      if (HowManyMiniBlocks == 16)
        out = fastpackwithoutmask_16(&Bs[0], out, bits32);
      else
        throw std::logic_error("unsupported HowManyMiniBlocks");
      for (uint32_t i = 0; i < HowManyMiniBlocks; ++i) {
        if (MiniBlockSize == 8)
          out = fastpackwithoutmask_8(in + i * MiniBlockSize, out, Bs[i]);
        else if (MiniBlockSize == 16)
          out = fastpackwithoutmask_16(in + i * MiniBlockSize, out, Bs[i]);
        else if (MiniBlockSize == 24)
          out = fastpackwithoutmask_24(in + i * MiniBlockSize, out, Bs[i]);
        else if (MiniBlockSize == 32)
          out = fastpackwithoutmask_32(in + i * MiniBlockSize, out, Bs[i]);

        else
          throw std::logic_error("unsupported MiniBlockSize");
      }
    }
    nvalue = out - initout;
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t /*length*/,
                              uint32_t *out, size_t &nvalue) {
    const uint32_t actuallength = *in++;
    const uint32_t *const initout(out);
    uint32_t Bs[HowManyMiniBlocks];
    for (; out < initout + actuallength;) {
      if (HowManyMiniBlocks == 16)
        in = fastunpack_16(in, &Bs[0], bits32);
      else
        throw std::logic_error("unsupported HowManyMiniBlocks");
      for (uint32_t i = 0; i < HowManyMiniBlocks; ++i, out += MiniBlockSize) {
        if (MiniBlockSize == 8)
          in = fastunpack_8(in, out, Bs[i]);
        else if (MiniBlockSize == 16)
          in = fastunpack_16(in, out, Bs[i]);
        else if (MiniBlockSize == 24)
          in = fastunpack_24(in, out, Bs[i]);
        else if (MiniBlockSize == 32)
          in = fastunpack_32(in, out, Bs[i]);
        else
          throw std::logic_error("unsupported MiniBlockSize");
      }
    }
    nvalue = out - initout;
    return in;
  }

  std::string name() const {
    std::ostringstream convert;
    convert << "BinaryPacking" << MiniBlockSize;
    return convert.str();
  }
};

/**
 * This is an attempt to make BinaryPacking faster, at the expense
 * of some compression.
 */
template <uint32_t MiniBlockSize>
class FastBinaryPacking : public IntegerCODEC {
public:
  using IntegerCODEC::encodeArray;
  using IntegerCODEC::decodeArray;

  static const uint32_t HowManyMiniBlocks = 4;
  static const uint32_t BlockSize = HowManyMiniBlocks * MiniBlockSize;
  static const uint32_t bits32 = 8; // 8 > gccbits(32);

  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) override {
    checkifdivisibleby(length, BlockSize);
    const uint32_t *const initout(out);
    *out++ = static_cast<uint32_t>(length);
    uint32_t Bs[HowManyMiniBlocks];
    for (const uint32_t *const final = in + length; in + BlockSize <= final;
         in += BlockSize) {
      for (uint32_t i = 0; i < HowManyMiniBlocks; ++i)
        Bs[i] = maxbits(in + i * MiniBlockSize, in + (i + 1) * MiniBlockSize);
      *out++ = (Bs[0] << 24) | (Bs[1] << 16) | (Bs[2] << 8) | Bs[3];
      for (uint32_t i = 0; i < HowManyMiniBlocks; ++i) {
        if (MiniBlockSize == 8)
          out = fastpackwithoutmask_8(in + i * MiniBlockSize, out, Bs[i]);
        else if (MiniBlockSize == 16)
          out = fastpackwithoutmask_16(in + i * MiniBlockSize, out, Bs[i]);
        else if (MiniBlockSize == 24)
          out = fastpackwithoutmask_24(in + i * MiniBlockSize, out, Bs[i]);
        else if (MiniBlockSize == 32) {
          fastpackwithoutmask(in + i * MiniBlockSize, out, Bs[i]);
          out += Bs[i];

        } else
          throw std::logic_error("unsupported MiniBlockSize");
      }
    }
    nvalue = out - initout;
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t /*length*/,
                              uint32_t *out, size_t &nvalue) override {
    const uint32_t actuallength = *in++;
    const uint32_t *const initout(out);
    uint32_t Bs[HowManyMiniBlocks];
    for (; out < initout + actuallength; out += BlockSize) {
      Bs[0] = static_cast<uint8_t>(in[0] >> 24);
      Bs[1] = static_cast<uint8_t>(in[0] >> 16);
      Bs[2] = static_cast<uint8_t>(in[0] >> 8);
      Bs[3] = static_cast<uint8_t>(in[0]);
      ++in;
      for (uint32_t i = 0; i < HowManyMiniBlocks; ++i) {
        if (MiniBlockSize == 8)
          in = fastunpack_8(in, out + i * MiniBlockSize, Bs[i]);
        else if (MiniBlockSize == 16)
          in = fastunpack_16(in, out + i * MiniBlockSize, Bs[i]);
        else if (MiniBlockSize == 24)
          in = fastunpack_24(in, out + i * MiniBlockSize, Bs[i]);
        else if (MiniBlockSize == 32) {
          fastunpack(in, out + i * MiniBlockSize, Bs[i]);
          in += Bs[i];
        } else
          throw std::logic_error("unsupported MiniBlockSize");
      }
    }
    nvalue = out - initout;
    return in;
  }

  std::string name() const override {
    std::ostringstream convert;
    convert << "FastBinaryPacking" << MiniBlockSize;
    return convert.str();
  }
};

// A simpler version of FastBinaryPacking32. (For sanity testing.)
class BP32 : public IntegerCODEC {
public:
  using IntegerCODEC::encodeArray;
  using IntegerCODEC::decodeArray;

  static const uint32_t MiniBlockSize = 32;
  static const uint32_t HowManyMiniBlocks = 4;
  static const uint32_t BlockSize = HowManyMiniBlocks * MiniBlockSize;

  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) override {
    checkifdivisibleby(length, BlockSize);
    const uint32_t *const initout(out);
    *out++ = static_cast<uint32_t>(length);
    uint32_t Bs[HowManyMiniBlocks];
    for (const uint32_t *const final = in + length; in + BlockSize <= final;
         in += BlockSize) {
      for (uint32_t i = 0; i < HowManyMiniBlocks; ++i)
        Bs[i] = maxbits(in + i * MiniBlockSize, in + (i + 1) * MiniBlockSize);
      *out++ = (Bs[0] << 24) | (Bs[1] << 16) | (Bs[2] << 8) | Bs[3];
      for (uint32_t i = 0; i < HowManyMiniBlocks; ++i) {
        fastpackwithoutmask(in + i * MiniBlockSize, out, Bs[i]);
        out += Bs[i];
      }
    }
    nvalue = out - initout;
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t /*length*/,
                              uint32_t *out, size_t &nvalue) override {
    const uint32_t actuallength = *in++;
    const uint32_t *const initout(out);
    uint32_t Bs[HowManyMiniBlocks];
    for (; out < initout + actuallength;) {
      Bs[0] = static_cast<uint8_t>(in[0] >> 24);
      Bs[1] = static_cast<uint8_t>(in[0] >> 16);
      Bs[2] = static_cast<uint8_t>(in[0] >> 8);
      Bs[3] = static_cast<uint8_t>(in[0]);
      ++in;
      for (uint32_t i = 0; i < HowManyMiniBlocks; ++i, out += MiniBlockSize) {
        fastunpack(in, out, Bs[i]);
        in += Bs[i];
      }
    }
    nvalue = out - initout;
    return in;
  }

  std::string name() const override { return "BP32"; }
};

/**
 * This is the original unaligned binary packing. You can
 * force alignment on 32-bit boundaries with the "align" template
 *  parameter however.
 * The packing is aligned on byte boundaries only.
 */
template <uint32_t MiniBlockSize, bool align = false, bool prescan = false>
class ByteAlignedPacking : public IntegerCODEC {
public:
  static const uint32_t HowManyMiniBlocks = 16;
  static const uint32_t BlockSize = HowManyMiniBlocks * MiniBlockSize;
  static const uint32_t bits32 = 5; // constexprbits(32);

  void encodeArray(const uint32_t *in, const size_t length, uint32_t *out,
                   size_t &nvalue) {
    checkifdivisibleby(length, BlockSize);
    const uint32_t *const initout(out);
    *out++ = length;
    uint8_t *outbyte = reinterpret_cast<uint8_t *>(out);
    uint32_t Bs[HowManyMiniBlocks];
    const uint32_t storageforbitwidth =
        prescan ? bits(maxbits(in, in + length)) : bits32;
    if (prescan)
      *outbyte++ = storageforbitwidth;
    if (MiniBlockSize == 32)
      assert((storageforbitwidth * HowManyMiniBlocks) % 32 == 0);

    for (const uint32_t *const final = in + length; in + BlockSize <= final;
         in += BlockSize) {
      for (uint32_t i = 0; i < HowManyMiniBlocks; ++i)
        Bs[i] = maxbits(in + i * MiniBlockSize, in + (i + 1) * MiniBlockSize);
      if (HowManyMiniBlocks == 16)
        outbyte = fastunalignedpackwithoutmask_16(&Bs[0], outbyte,
                                                  storageforbitwidth);
      else
        throw std::logic_error("unsupported HowManyMiniBlocks");
      for (uint32_t i = 0; i < HowManyMiniBlocks; ++i) {
        if (align)
          outbyte = padTo32bits(outbyte);
        if (MiniBlockSize == 8)
          outbyte = fastunalignedpackwithoutmask_8(in + i * MiniBlockSize,
                                                   outbyte, Bs[i]);

        else if (MiniBlockSize == 16)
          outbyte = fastunalignedpackwithoutmask_16(in + i * MiniBlockSize,
                                                    outbyte, Bs[i]);

        else if (MiniBlockSize == 32) {
          fastpackwithoutmask(in + i * MiniBlockSize,
                              reinterpret_cast<uint32_t *>(outbyte), Bs[i]);
          outbyte += sizeof(uint32_t) * Bs[i];

        } else
          throw std::logic_error("unsupported MiniBlockSize");
      }
    }
    outbyte = padTo32bits(outbyte);
    const uint32_t storageinbytes =
        outbyte - reinterpret_cast<const uint8_t *>(initout);
    nvalue = storageinbytes / 4;
  }

  const uint32_t *decodeArray(const uint32_t *in, const size_t length,
                              uint32_t *out, size_t &nvalue) {
    const uint32_t actuallength = *in++;
    const uint8_t *inbyte = reinterpret_cast<const uint8_t *>(in);
    const uint32_t *const initout(out);

    const uint32_t storageforbitwidth = prescan ? *inbyte++ : bits32;
    uint32_t Bs[HowManyMiniBlocks];
    for (; out < initout + actuallength;) {
      if (HowManyMiniBlocks == 16)
        inbyte = fastunalignedunpack_16(inbyte, &Bs[0], storageforbitwidth);
      else
        throw std::logic_error("unsupported HowManyMiniBlocks");
      for (uint32_t i = 0; i < HowManyMiniBlocks; ++i, out += MiniBlockSize) {
        if (align)
          inbyte = padTo32bits(inbyte);
        if (MiniBlockSize == 8)
          inbyte = fastunalignedunpack_8(inbyte, out, Bs[i]);
        else if (MiniBlockSize == 16)
          inbyte = fastunalignedunpack_16(inbyte, out, Bs[i]);
        else if (MiniBlockSize == 32) {
          fastunpack(reinterpret_cast<const uint32_t *>(inbyte), out, Bs[i]);
          inbyte += sizeof(uint32_t) * Bs[i];
        } else
          throw std::logic_error("unsupported MiniBlockSize");
      }
    }

    nvalue = out - initout;
    return reinterpret_cast<const uint32_t *>(padTo32bits(inbyte));
  }

  std::string name() const {
    std::ostringstream convert;
    convert << "ByteAlignedPacking" << MiniBlockSize;
    if (prescan or align) {
      convert << "<";
      if (prescan) {
        convert << "prescan";
        if (align)
          convert << ",aligned";
      } else if (align)
        convert << "aligned";
      convert << ">";
    }
    return convert.str();
  }
};

} // namespace FastPFor

#endif /* BLOCKPACKING_H_ */
