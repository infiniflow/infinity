module;

import stl;
import byte_slice_reader;
import byte_slice_writer;

export module vbyte_compress_encoder;

namespace infinity {

export template <typename T>
class VByteIntEncoder {
public:
    VByteIntEncoder() = default;
    ~VByteIntEncoder() = default;

    inline u32 Encode(ByteSliceWriter &slice_writer, const T *src, u32 src_len) const;

    inline u32 Decode(T *dest, u32 dest_len, ByteSliceReader &slice_reader) const;
};

template <typename T>
u32 VByteIntEncoder<T>::Encode(ByteSliceWriter &slice_writer, const T *src, u32 src_len) const {
    u32 len = 1;
    slice_writer.WriteByte((u8)src_len);
    for (u32 i = 0; i < src_len; ++i) {
        len += slice_writer.WriteVInt(src[i]);
    }
    return len;
}

template <typename T>
u32 VByteIntEncoder<T>::Decode(T *dest, u32 dest_len, ByteSliceReader &slice_reader) const {
    u32 len = slice_reader.ReadByte();
    for (u32 i = 0; i < len; ++i) {
        dest[i] = slice_reader.ReadVUInt32();
    }
    return len;
}

} // namespace infinity
