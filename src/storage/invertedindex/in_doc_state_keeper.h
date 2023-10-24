#pragma once

#include "common/memory/byte_slice.h"
#include "format/in_doc_pos_state.h"
#include "format/inmem_posting_decoder.h"
#include "format/pos_list_decoder.h"
#include "format/posting_decoder.h"

namespace infinity {
class InDocStateKeeper {
public:
    InDocStateKeeper(InDocPositionState *state, MemoryPool *session_pool);

    ~InDocStateKeeper();

public:
    void MoveToDoc(ttf_t current_ttf);

    void MoveToSegment(ByteSlice *pos_list, tf_t total_tf, uint32_t pos_list_begin, const PositionListFormatOption &option);

    void MoveToSegment(InMemPositionListDecoder *pos_decoder);

private:
    InDocPositionState *state_;
    std::vector<PositionListDecoder *> pos_decoders_;
    MemoryPool *session_pool_;
};
} // namespace infinity