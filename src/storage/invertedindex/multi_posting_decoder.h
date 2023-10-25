#pragma once

#include "format/index_decoder.h"
#include "format/posting_decoder.h"
#include "in_doc_pos_iterator.h"
#include "in_doc_state_keeper.h"
#include "segment_posting.h"
#include "storage/io/byte_slice_reader.h"

namespace infinity {
class MultiPostingDecoder {
public:
    MultiPostingDecoder(InDocPositionState *state, MemoryPool *pool);

    ~MultiPostingDecoder();

    void Init(const std::shared_ptr<std::vector<SegmentPosting>> &seg_postings);

    inline void MoveToCurrentDocPosition(ttf_t current_ttf) { in_doc_state_keeper_.MoveToDoc(current_ttf); }

    bool DecodeDocBuffer(docid_t start_doc_id, docid_t *doc_buffer, docid_t &first_doc_id, docid_t &last_doc_id, ttf_t &current_ttf);

    bool DecodeCurrentTFBuffer(tf_t *tf_buffer);

    void DecodeCurrentDocPayloadBuffer(docpayload_t *doc_payload_buffer);

private:
    bool DecodeDocBufferInOneSegment(docid_t start_doc_id, docid_t *doc_buffer, docid_t &first_doc_id, docid_t &last_doc_id, ttf_t &current_ttf);

    IndexDecoder *CreateIndexDecoder(uint32_t doc_list_begin_pos);

    inline docid_t GetSegmentBaseDocId(uint32_t seg_cursor) {
        if (seg_cursor >= segment_count_) {
            return INVALID_DOCID;
        }

        SegmentPosting &posting = (*seg_postings_)[seg_cursor];
        return posting.GetBaseDocId();
    }

    inline uint32_t LocateSegment(uint32_t start_seg_cursor, docid_t start_doc_id) {
        docid_t curSegBaseDocId = GetSegmentBaseDocId(start_seg_cursor);
        if (curSegBaseDocId == INVALID_DOCID) {
            return start_seg_cursor;
        }

        uint32_t cur_seg_cursor = start_seg_cursor;
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
    uint32_t segment_cursor_;
    uint32_t segment_count_;

    std::shared_ptr<std::vector<SegmentPosting>> seg_postings_;
    ByteSliceReader doc_list_reader_;
    MemoryPool *session_pool_;
    InDocPositionIterator *in_doc_pos_iterator_;
    InDocStateKeeper in_doc_state_keeper_;
};
} // namespace infinity