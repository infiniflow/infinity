module;

import stl;
import memory_pool;
import byte_slice;
import in_doc_pos_state;
import position_list_decoder;
import posting_decoder;
import inmem_posting_decoder;
import inmem_position_list_decoder;
import posting_list_format;
import index_defines;
export module in_doc_state_keeper;

namespace infinity {
export class InDocStateKeeper {
public:
    InDocStateKeeper(InDocPositionState *state, MemoryPool *session_pool);

    ~InDocStateKeeper();

public:
    void MoveToDoc(ttf_t current_ttf);

    void MoveToSegment(ByteSlice *pos_list, tf_t total_tf, u32 pos_list_begin, const PostingFormatOption &option);

    void MoveToSegment(ByteSliceList *pos_list, tf_t total_tf, u32 pos_list_begin, const PostingFormatOption &option);

    void MoveToSegment(InMemPositionListDecoder *pos_decoder);

private:
    InDocPositionState *state_ = nullptr;
    Vector<PositionListDecoder *> pos_decoders_;
    MemoryPool *session_pool_;
};
} // namespace infinity