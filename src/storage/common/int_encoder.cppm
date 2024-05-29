module;
#include <iostream>
import byte_slice_reader;
import byte_slice_writer;

import stl;
import infinity_exception;
import fastpfor;

#include "new_pfordelta_compressor.h"

export module int_encoder;

namespace infinity {

export using NewPForDeltaCompressor = indexlib::NewPForDeltaCompressor;

export template <typename T, typename Compressor>
class IntEncoder {
public:
    const static size_t ENCODER_BUFFER_SIZE = 1024;
    const static size_t ENCODER_BUFFER_BYTE_SIZE = ENCODER_BUFFER_SIZE * sizeof(u32);

public:
    IntEncoder() {}
    virtual ~IntEncoder() {}

public:
    // src_len: number of elements in src. Return number of bytes after compression
    inline u32 Encode(ByteSliceWriter &slice_writer, const T *src, u32 src_len) const;

    // return number of elements
    inline u32 Decode(T *dest, u32 dest_len, ByteSliceReader &slice_reader) const;

private:
    Compressor compressor_{};
};

template <typename T, typename Compressor>
inline u32 IntEncoder<T, Compressor>::Encode(ByteSliceWriter &slice_writer, const T *src, u32 src_len) const {
    u8 buffer[ENCODER_BUFFER_BYTE_SIZE];
    u32 len;
    len = compressor_.Compress((u32 *)buffer, ENCODER_BUFFER_SIZE, src, src_len, true);
    u32 encode_len = len * sizeof(u32);
    slice_writer.Write((const u8 *)buffer, encode_len);
    return encode_len;
}

template <typename T, typename Compressor>
inline u32 IntEncoder<T, Compressor>::Decode(T *dest, u32 dest_len, ByteSliceReader &slice_reader) const {
    u8 buffer[ENCODER_BUFFER_BYTE_SIZE];
    u32 header = (u32)slice_reader.PeekInt32();
    size_t comp_len = compressor_.GetCompressedLength(header) * sizeof(u32);
    assert(comp_len <= ENCODER_BUFFER_BYTE_SIZE);
    void *buf_ptr = buffer;
    size_t len = slice_reader.ReadMayCopy(buf_ptr, comp_len);
    if (len != comp_len) {
        UnrecoverableError("Decode posting FAILEDF");
    }
    return (u32)compressor_.Decompress(dest, dest_len, (const u32 *)buf_ptr, comp_len);
}

export template <FastPForCodec Codec>
class IntEncoder<u32, FastPForWrapper<Codec>> {
public:
    const static size_t ENCODER_BUFFER_SIZE = 1024;
    const static size_t ENCODER_BUFFER_BYTE_SIZE = ENCODER_BUFFER_SIZE * sizeof(u32);

public:
    IntEncoder() {}
    virtual ~IntEncoder() {}

public:
    // src_len: number of elements in src. Return number of bytes after compression
    inline u32 Encode(ByteSliceWriter &slice_writer, const u32 *src, u32 src_len) const;

    // return number of elements
    inline u32 Decode(u32 *dest, u32 dest_len, ByteSliceReader &slice_reader) const;

private:
    FastPForWrapper<Codec> compressor_{};
};

template <FastPForCodec Codec>
inline u32 IntEncoder<u32, FastPForWrapper<Codec>>::Encode(ByteSliceWriter &slice_writer, const u32 *src, u32 src_len) const {
    u8 buffer[ENCODER_BUFFER_BYTE_SIZE];
    SizeT encode_len = ENCODER_BUFFER_SIZE;
    compressor_.Compress(src, src_len, (u32 *)buffer, encode_len);
    slice_writer.Write((const u8 *)&encode_len, sizeof(u8));
    slice_writer.Write((const u8 *)buffer, encode_len * sizeof(u32));
    return encode_len * sizeof(u32) + sizeof(u8);
}

template <FastPForCodec Codec>
inline u32 IntEncoder<u32, FastPForWrapper<Codec>>::Decode(u32 *dest, u32 dest_len, ByteSliceReader &slice_reader) const {
    u8 buffer[ENCODER_BUFFER_BYTE_SIZE];
    u32 comp_len = slice_reader.ReadByte();
    void *buf_ptr = buffer;
    comp_len *= sizeof(u32);
    size_t len = slice_reader.ReadMayCopy(buf_ptr, comp_len);
    if (len != comp_len) {
        UnrecoverableError("Decode posting FAILEDF");
    }
    size_t destlen = dest_len;
    u32 count = comp_len / sizeof(u32);
    compressor_.Decompress((const u32 *)buf_ptr, count, dest, destlen);
    return destlen;
}

} // namespace infinity
