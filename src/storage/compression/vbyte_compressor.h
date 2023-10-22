#pragma once

#include <cstdint>

namespace infinity {

class VByteCompressor {
public:
    static uint32_t EncodeVInt32(uint8_t *output_byte, int32_t value);

    static int32_t DecodeVInt32(uint8_t *&input_byte, uint32_t &input_byte_length);

    static uint32_t GetVInt32Length(int32_t value);

    static void WriteVUInt32(uint32_t value, char *&cursor);

    static uint32_t ReadVUInt32(char *&cursor);

    template <typename T, typename Iterator>
    static T ReadVInt(Iterator &in);

    template <typename T, typename Iterator>
    static void VSkip(Iterator &in);

private:
    static uint8_t ReadByte(const uint8_t *input_byte, uint32_t &pos);
    static void WriteByte(uint8_t *output_byte, uint32_t &pos, uint8_t value);
};

inline uint8_t VByteCompressor::ReadByte(const uint8_t *input_byte, uint32_t &pos) { return input_byte[pos++]; }

inline void VByteCompressor::WriteByte(uint8_t *output_byte, uint32_t &pos, uint8_t value) { output_byte[pos++] = value; }

inline uint32_t VByteCompressor::GetVInt32Length(int32_t value) {
    uint8_t l = 1;
    uint32_t ui = value;
    while ((ui & ~0x7F) != 0) {
        l++;
        ui >>= 7;
    }
    return l;
}

inline void VByteCompressor::WriteVUInt32(uint32_t value, char *&cursor) {
    while (value > 0x7F) {
        *cursor++ = 0x80 | (value & 0x7F);
        value >>= 7;
    }
    *cursor++ = value & 0x7F;
}

inline uint32_t VByteCompressor::ReadVUInt32(char *&cursor) {
    uint8_t byte = *(uint8_t *)cursor++;
    uint32_t value = byte & 0x7F;
    int shift = 7;

    while (byte & 0x80) {
        byte = *(uint8_t *)cursor++;
        value |= ((byte & 0x7F) << shift);
        shift += 7;
    }
    return value;
}

inline uint32_t VByteCompressor::EncodeVInt32(uint8_t *output_byte, int32_t value) {
    uint32_t l = 0;
    uint32_t ui = value;
    while ((ui & ~0x7F) != 0) {
        WriteByte(output_byte, l, (uint8_t)((ui & 0x7f) | 0x80));
        ui >>= 7;
    }
    WriteByte(output_byte, l, (uint8_t)ui);
    return l;
}

inline int32_t VByteCompressor::DecodeVInt32(uint8_t *&input_byte, uint32_t &input_byte_length) {
    uint32_t l = 0;
    auto b = ReadByte(input_byte, l);
    uint32_t i = b & 0x7F;
    for (int32_t shift = 7; (b & 0x80) != 0; shift += 7) {
        b = ReadByte(input_byte, l);
        i |= (b & 0x7FL) << shift;
    }
    input_byte += l;
    input_byte_length -= l;
    return i;
}

template <typename T, typename Iterator>
T VByteCompressor::ReadVInt(Iterator &in) {
    constexpr T MASK = 0x80;
    T out = *in;
    ++in;
    if (!(out & MASK))
        return out;

    T b;
    out -= MASK;
    b = *in;
    ++in;
    out += b << 7;
    if (!(b & MASK))
        return out;
    out -= MASK << 7;
    b = *in;
    ++in;
    out += b << 14;
    if (!(b & MASK))
        return out;
    out -= MASK << 14;
    b = *in;
    ++in;
    out += b << 21;
    if (!(b & MASK))
        return out;
    out -= MASK << 21;
    b = *in;
    ++in;
    out += b << 28;
    // last byte always has MSB == 0, so we don't need to check and subtract
    // 0x80

    return out;
}

template <typename T, typename Iterator>
inline void VByteCompressor::VSkip(Iterator &in) {
    constexpr T MASK = 0x80;
    T v = *in;
    ++in;
    if (!(v & MASK))
        return;
    v = *in;
    ++in;
    if (!(v & MASK))
        return;
    v = *in;
    ++in;
    if (!(v & MASK))
        return;
    v = *in;
    ++in;
    if (!(v & MASK))
        return;
    v = *in;
    ++in;
    if (!(v & MASK))
        return;
    v = *in;
    ++in;
    if (!(v & MASK))
        return;
    v = *in;
    ++in;
    if (!(v & MASK))
        return;
    v = *in;
    ++in;
    if (!(v & MASK))
        return;
    v = *in;
    ++in;
    if (!(v & MASK))
        return;
    ++in;
    return;
}
} // namespace infinity
