#pragma once

#include "buffered_byte_slice_reader.h"
#include "pos_list_decoder.h"

namespace infinity {

class BufferedByteSlice;

class InMemPositionListDecoder : public PositionListDecoder {
public:
    InMemPositionListDecoder(const PositionListFormatOption &option, MemoryPool *session_pool);

    ~InMemPositionListDecoder();

    void Init(ttf_t total_tf, PairValueSkipListReader *skipListReader, BufferedByteSlice *pos_list_buffer);

    bool SkipTo(ttf_t current_ttf, InDocPositionState *state);

    bool LocateRecord(const InDocPositionState *state, uint32_t &tf);

    uint32_t DecodeRecord(pos_t *pos_buffer, uint32_t pos_buffer_len);

private:
    BufferedByteSlice *pos_list_buffer_;
    BufferedByteSliceReader pos_list_reader_;
};

} // namespace infinity
