module;

import stl;
import memory_pool;
import position_list_decoder;
import buffered_byte_slice_reader;
import in_doc_pos_state;
import position_list_skiplist_reader;
import position_list_format_option;
import index_defines;

export module inmem_position_list_decoder;

namespace infinity {

class BufferedByteSlice;

export class InMemPositionListDecoder : public PositionListDecoder {
public:
    InMemPositionListDecoder(const PositionListFormatOption &option, MemoryPool *session_pool);

    ~InMemPositionListDecoder();

    void Init(ttf_t total_tf, PositionListSkipListReader *skip_list_reader, BufferedByteSlice *pos_list_buffer);

    bool SkipTo(ttf_t current_ttf, InDocPositionState *state);

    bool LocateRecord(const InDocPositionState *state, u32 &tf);

    u32 DecodeRecord(pos_t *pos_buffer, u32 pos_buffer_len);

private:
    BufferedByteSlice *pos_list_buffer_;
    BufferedByteSliceReader pos_list_reader_;
};

} // namespace infinity
