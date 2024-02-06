module;

export module multi_posting_decoder;

import stl;
import byte_slice_reader;
import memory_pool;
import index_decoder;
import in_doc_pos_iterator;
import in_doc_pos_state;
import in_doc_state_keeper;
import segment_posting;
import index_defines;
import posting_list_format;

namespace infinity {
export class MultiPostingDecoder {
public:
    MultiPostingDecoder(InDocPositionState *state, MemoryPool *pool);

    ~MultiPostingDecoder();

    void Init(const SharedPtr<Vector<SegmentPosting>> &seg_postings);

    inline void MoveToCurrentDocPosition(ttf_t current_ttf) { in_doc_state_keeper_.MoveToDoc(current_ttf); }

    bool DecodeDocBuffer(docid_t start_doc_id, docid_t *doc_buffer, docid_t &first_doc_id, docid_t &last_doc_id, ttf_t &current_ttf);

    bool DecodeCurrentTFBuffer(tf_t *tf_buffer);

    void DecodeCurrentDocPayloadBuffer(docpayload_t *doc_payload_buffer);

    InDocPositionIterator *GetInDocPositionIterator() { return in_doc_pos_iterator_; }

    const PostingFormatOption &GetPostingFormatOption() const { return cur_segment_format_option_; }

    u32 InnerGetSeekedDocCount() const { return index_decoder_->InnerGetSeekedDocCount(); }

private:
    bool DecodeDocBufferInOneSegment(docid_t start_doc_id, docid_t *doc_buffer, docid_t &first_doc_id, docid_t &last_doc_id, ttf_t &current_ttf);

    IndexDecoder *CreateIndexDecoder(u32 doc_list_begin_pos);

    inline docid_t GetSegmentBaseDocId(u32 seg_cursor) {
        if (seg_cursor >= segment_count_) {
            return INVALID_DOCID;
        }

        SegmentPosting &posting = (*seg_postings_)[seg_cursor];
        return posting.GetBaseDocId();
    }

    inline u32 LocateSegment(u32 start_seg_cursor, docid_t start_doc_id) {
        docid_t curSegBaseDocId = GetSegmentBaseDocId(start_seg_cursor);
        if (curSegBaseDocId == INVALID_DOCID) {
            return start_seg_cursor;
        }

        u32 cur_seg_cursor = start_seg_cursor;
        docid_t next_seg_base_doc_id = GetSegmentBaseDocId(cur_seg_cursor + 1);
        while (next_seg_base_doc_id != INVALID_DOCID && start_doc_id >= next_seg_base_doc_id) {
            ++cur_seg_cursor;
            next_seg_base_doc_id = GetSegmentBaseDocId(cur_seg_cursor + 1);
        }
        return cur_seg_cursor;
    }

    bool MoveToSegment(docid_t start_doc_id);

private:
    PostingFormatOption cur_segment_format_option_;
    docid_t base_doc_id_;
    bool need_decode_tf_;
    bool need_decode_doc_payload_;
    IndexDecoder *index_decoder_;
    u32 segment_cursor_;
    u32 segment_count_;

    SharedPtr<Vector<SegmentPosting>> seg_postings_;
    ByteSliceReader doc_list_reader_;
    MemoryPool *session_pool_;
    InDocPositionIterator *in_doc_pos_iterator_;
    InDocStateKeeper in_doc_state_keeper_;
};
} // namespace infinity