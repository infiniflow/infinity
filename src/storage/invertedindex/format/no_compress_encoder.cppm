module;

import stl;
import byte_slice_reader;
import byte_slice_writer;

export module no_compress_encoder;

namespace infinity {

export template <typename T>
class NoCompressIntEncoder {
public:
    NoCompressIntEncoder() = default;
    ~NoCompressIntEncoder() = default;

    inline u32 Encode(ByteSliceWriter &slice_writer, const T *src, u32 src_len) const;

    inline u32 Decode(T *dest, u32 dest_len, ByteSliceReader &slice_reader) const;
};

template <typename T>
u32 NoCompressIntEncoder<T>::Encode(ByteSliceWriter &slice_writer, const T *src, u32 src_len) const {
    u32 encode_len = 0;
    slice_writer.WriteByte((u8)src_len);
    encode_len += sizeof(u8);
    slice_writer.Write((const void *)src, src_len * sizeof(T));
    return encode_len + src_len * sizeof(T);
}

template <typename T>
u32 NoCompressIntEncoder<T>::Decode(T *dest, u32 dest_len, ByteSliceReader &slice_reader) const {
    u32 read_count = slice_reader.ReadByte();
    slice_reader.Read((void *)dest, read_count * sizeof(T));
    return read_count;
}

} // namespace infinity
