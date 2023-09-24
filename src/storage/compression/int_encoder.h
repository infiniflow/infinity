#pragma once
#include <memory>

#include "fastpfor.h"
#include "storage/io/byte_slice_reader.h"
#include "storage/io/byte_slice_writer.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

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
    uint32_t Encode(ByteSliceWriter& slice_writer, const T* src, uint32_t src_len);

    uint32_t Decode(T* dest, uint32_t dest_len, ByteSliceReader& slice_reader);

private:
    Compressor compressor_;
};

template <typename T, typename Compressor>
uint32_t
IntEncoder<T, Compressor>::Encode(ByteSliceWriter& slice_writer, const T* src, uint32_t src_len) const {
    uint8_t buffer[ENCODER_BUFFER_BYTE_SIZE];
    uint32_t encode_len;
    encode_len = compressor_.Compress(src, src_len, (uint32_t*)dest, ENCODER_BUFFER_SIZE, encode_len);
    slice_writer.Write((const uint8_t*)buffer, encode_len);
    return encode_len;
}

template <typename T, typename Compressor>
uint32_t
IntEncoder<T, Compressor>::Decode(T* dest, uint32_t dest_len, ByteSliceReader& slice_reader) const {
    uint8_t buffer[ENCODER_BUFFER_BYTE_SIZE];
    void* buf_ptr = buffer;
    size_t len = slice_reader.ReadMayCopy(buf_ptr, comp_len);
    if (len != comp_len) {
        StorageError("Decode posting FAILED");
    }
    return compressor_.Decompress((const uint32_t*)buf_ptr, ENCODER_BUFFER_SIZE, dest, dest_len);
}

}
