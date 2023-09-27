#pragma once
#include <memory>

#include "fastpfor.h"
#include "storage/io/byte_slice_reader.h"
#include "storage/io/byte_slice_writer.h"
#include "common/utility/infinity_assert.h"
#include "new_pfordelta_compressor.h"

namespace infinity {

using NewPForDeltaCompressor = indexlib::NewPForDeltaCompressor;

template <typename T, typename Compressor>
class IntEncoder
{
public:
    const static size_t ENCODER_BUFFER_SIZE = 256;
    const static size_t ENCODER_BUFFER_BYTE_SIZE = ENCODER_BUFFER_SIZE * sizeof(uint32_t);
public:
    IntEncoder() {}
    virtual ~IntEncoder() {}

public:
    inline uint32_t Encode(ByteSliceWriter& slice_writer, const T* src, uint32_t src_len) const;

    inline uint32_t Decode(T* dest, uint32_t dest_len, ByteSliceReader& slice_reader) const;

private:
    Compressor compressor_;
};

template <>
inline uint32_t
IntEncoder<uint32_t, SIMDBitPacking>::Encode(ByteSliceWriter& slice_writer, const uint32_t* src, uint32_t src_len) const {
    uint8_t buffer[ENCODER_BUFFER_BYTE_SIZE];
    size_t encode_len;
    encode_len = compressor_.Compress(src, src_len, (uint32_t*)buffer, encode_len);
    slice_writer.Write((const uint8_t*)&encode_len, sizeof(uint8_t));
    slice_writer.Write((const uint8_t*)buffer, encode_len);
    return encode_len;
}

template <>
inline uint32_t
IntEncoder<uint32_t, SIMDBitPacking>::Decode(uint32_t* dest, uint32_t dest_len, ByteSliceReader& slice_reader) const {
    uint8_t buffer[ENCODER_BUFFER_BYTE_SIZE];
    uint32_t comp_len = slice_reader.ReadByte();
    void* buf_ptr = buffer;
    size_t len = slice_reader.ReadMayCopy(buf_ptr, comp_len);
    if (len != comp_len) {
        StorageError("Decode posting FAILED");
    }
    size_t destlen = dest_len;
    compressor_.Decompress((const uint32_t*)buf_ptr, comp_len, dest, destlen);
    return destlen;
}

template <>
inline uint32_t
IntEncoder<uint16_t, NewPForDeltaCompressor>::Encode(ByteSliceWriter& slice_writer, const uint16_t* src, uint32_t src_len) const {
    uint8_t buffer[ENCODER_BUFFER_BYTE_SIZE];
    uint32_t len;
    len = compressor_.Compress((uint32_t*)buffer, ENCODER_BUFFER_SIZE, src, src_len, true);
    uint32_t encode_len = len * sizeof(uint32_t);
    slice_writer.Write((const uint8_t*)buffer, encode_len);
    return encode_len;
}

template <>
inline uint32_t
IntEncoder<uint16_t, NewPForDeltaCompressor>::Decode(uint16_t* dest, uint32_t dest_len, ByteSliceReader& slice_reader) const {
    uint8_t buffer[ENCODER_BUFFER_BYTE_SIZE];
    uint32_t header = (uint32_t)slice_reader.PeekInt32();
    size_t comp_len = compressor_.GetCompressedLength(header) * sizeof(uint32_t);
    assert(comp_len <= ENCODER_BUFFER_BYTE_SIZE);
    void* buf_ptr = buffer;
    size_t len = slice_reader.ReadMayCopy(buf_ptr, comp_len);
    if (len != comp_len) {
         StorageError("Decode posting FAILED");
    }
    return (uint32_t)compressor_.Decompress(dest, dest_len, (const uint32_t*)buf_ptr, comp_len);
}


}
