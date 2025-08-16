export module infinity_core:vbyte_compressor;

import :infinity_type;

namespace infinity {

export class VByteCompressor {
public:
    static u32 EncodeVInt32(u8 *output_byte, i32 value);

    static i32 DecodeVInt32(u8 *&input_byte, u32 &input_byte_length);

    static u32 EncodeVInt64(u8 *output_byte, i64 value);

    static i64 DecodeVInt64(u8 *&input_byte, u32 &input_byte_length);

    static u32 GetVInt32Length(i32 value);

    static u32 GetVInt64Length(i64 value);

    static void WriteVUInt32(u32 value, char *&cursor);

    static u32 ReadVUInt32(char *&cursor);

    template <typename T, typename Iterator>
    static T ReadVInt(Iterator &in);

    template <typename T, typename Iterator>
    static void VSkip(Iterator &in);

private:
    static u8 ReadByte(const u8 *input_byte, u32 &pos);
    static void WriteByte(u8 *output_byte, u32 &pos, u8 value);
};

inline u8 VByteCompressor::ReadByte(const u8 *input_byte, u32 &pos) { return input_byte[pos++]; }

inline void VByteCompressor::WriteByte(u8 *output_byte, u32 &pos, u8 value) { output_byte[pos++] = value; }

inline u32 VByteCompressor::GetVInt32Length(i32 value) {
    u8 l = 1;
    u32 ui = value;
    while ((ui & ~0x7F) != 0) {
        l++;
        ui >>= 7;
    }
    return l;
}

inline u32 VByteCompressor::GetVInt64Length(i64 value) {
    u8 l = 1;
    u64 ui = value;
    while ((ui & ~0x7F) != 0) {
        l++;
        ui >>= 7;
    }
    return l;
}

inline void VByteCompressor::WriteVUInt32(u32 value, char *&cursor) {
    while (value > 0x7F) {
        *cursor++ = 0x80 | (value & 0x7F);
        value >>= 7;
    }
    *cursor++ = value & 0x7F;
}

inline u32 VByteCompressor::ReadVUInt32(char *&cursor) {
    u8 byte = *(u8 *)cursor++;
    u32 value = byte & 0x7F;
    int shift = 7;

    while (byte & 0x80) {
        byte = *(u8 *)cursor++;
        value |= ((byte & 0x7F) << shift);
        shift += 7;
    }
    return value;
}

inline u32 VByteCompressor::EncodeVInt32(u8 *output_byte, i32 value) {
    u32 l = 0;
    u32 ui = value;
    while ((ui & ~0x7F) != 0) {
        WriteByte(output_byte, l, (u8)((ui & 0x7f) | 0x80));
        ui >>= 7;
    }
    WriteByte(output_byte, l, (u8)ui);
    return l;
}

inline u32 VByteCompressor::EncodeVInt64(u8 *output_byte, i64 value) {
    u32 l = 0;
    u64 ui = value;
    while ((ui & ~0x7F) != 0) {
        WriteByte(output_byte, l, (u8)((ui & 0x7f) | 0x80));
        ui >>= 7;
    }
    WriteByte(output_byte, l, (u8)ui);
    return l;
}

inline i32 VByteCompressor::DecodeVInt32(u8 *&input_byte, u32 &input_byte_length) {
    u32 l = 0;
    auto b = ReadByte(input_byte, l);
    u32 i = b & 0x7F;
    for (i32 shift = 7; (b & 0x80) != 0; shift += 7) {
        b = ReadByte(input_byte, l);
        i |= (b & 0x7FL) << shift;
    }
    input_byte += l;
    input_byte_length -= l;
    return i;
}

inline i64 VByteCompressor::DecodeVInt64(u8 *&input_byte, u32 &input_byte_length) {
    u32 l = 0;
    auto b = ReadByte(input_byte, l);
    u64 i = (u64)b & 0x7FL;
    for (i32 shift = 7; (b & 0x80) != 0; shift += 7) {
        b = ReadByte(input_byte, l);
        i |= ((u64)b & 0x7FL) << shift;
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
