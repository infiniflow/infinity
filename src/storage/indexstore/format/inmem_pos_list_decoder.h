#pragma once

#include "buffered_byte_slice_reader.h"
#include "common/memory/memory_pool.h"
#include "in_doc_pos_state.h"
#include "pair_value_skiplist_reader.h"
#include "pos_list_format_option.h"

namespace infinity {

class BufferedByteSlice;

class InMemPositionListDecoder {
public:
    InMemPositionListDecoder(const PositionListFormatOption& option, MemoryPool* session_pool);

    ~InMemPositionListDecoder();

    void
    Init(ttf_t total_tf, PairValueSkipListReader* skipListReader, BufferedByteSlice* pos_list_buffer);

    bool
    SkipTo(ttf_t current_ttf, InDocPositionState* state);

    bool
    LocateRecord(const NormalInDocState* state, uint32_t& tf);

    uint32_t
    DecodeRecord(pos_t* pos_buffer, uint32_t pos_buffer_len, pospayload_t* payload_buffer, uint32_t payload_buffer_len);

private:
    BufferedByteSlice* pos_list_buffer_;
    BufferedByteSliceReader pos_list_reader_;
};

}// namespace infinity
