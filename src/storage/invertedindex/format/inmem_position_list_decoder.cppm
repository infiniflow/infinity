module;

export module inmem_position_list_decoder;

import stl;

import position_list_decoder;
import posting_byte_slice_reader;
import in_doc_pos_state;
import skiplist_reader;
import posting_list_format;
import position_list_format_option;
import index_defines;

namespace infinity {

class PostingByteSlice;

export class InMemPositionListDecoder : public PositionListDecoder {
public:
    InMemPositionListDecoder(const PostingFormatOption &option);

    ~InMemPositionListDecoder();

    void Init(ttf_t total_tf, SkipListReader *skip_list_reader, u32 skiplist_reader_real_size, PostingByteSlice *pos_list_buffer);

    bool SkipTo(ttf_t current_ttf, InDocPositionState *state);

    bool LocateRecord(const InDocPositionState *state, u32 &tf);

    u32 DecodeRecord(pos_t *pos_buffer, u32 pos_buffer_len);

private:
    PostingByteSlice *pos_list_buffer_;
    PostingByteSliceReader pos_list_reader_;
};

} // namespace infinity
