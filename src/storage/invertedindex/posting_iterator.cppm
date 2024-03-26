module;

import stl;
import memory_pool;
import byte_slice_reader;
import posting_list_format;
import term_meta;
import in_doc_pos_iterator;
import in_doc_pos_state;
import multi_posting_decoder;
import segment_posting;
import index_defines;
import match_data;
import internal_types;

export module posting_iterator;

namespace infinity {
export class PostingIterator {
public:
    PostingIterator(optionflag_t flag, MemoryPool *session_pool);

    ~PostingIterator();

    bool Init(const SharedPtr<Vector<SegmentPosting>> &seg_postings, const u32 state_pool_size);

    void Reset();

    u32 GetDocFreq() const { return doc_freq_; }

    RowID SeekDoc(RowID docId);

    void SeekPosition(pos_t pos, pos_t &result);

    docpayload_t GetCurrentDocPayload() {
        if (posting_option_.HasDocPayload()) {
            DecodeTFBuffer();
            DecodeDocPayloadBuffer();
            return doc_payload_buffer_[GetDocOffsetInBuffer()];
        }
        return 0;
    }

    tf_t GetCurrentTF() {
        if (posting_option_.HasTfList()) {
            DecodeTFBuffer();
            return tf_buffer_[GetDocOffsetInBuffer()];
        }
        return 0;
    }

    ttf_t GetCurrentTTF() {
        if (posting_option_.HasTfList()) {
            DecodeTFBuffer();
            i32 offset = GetDocOffsetInBuffer();
            for (; tf_buffer_cursor_ < offset; ++tf_buffer_cursor_) {
                current_ttf_ += tf_buffer_[tf_buffer_cursor_];
            }
        }
        return current_ttf_;
    }

    bool HasPosition() const { return posting_option_.HasPositionList(); }

    void GetTermMatchData(TermColumnMatchData &match_data, bool fetch_position = false) {
        DecodeTFBuffer();
        DecodeDocPayloadBuffer();
        if (need_move_to_current_doc_) {
            MoveToCurrentDoc(fetch_position);
        }
        if (posting_option_.HasTfList()) {
            match_data.tf_ = tf_buffer_[GetDocOffsetInBuffer()];
        }
        if (posting_option_.HasDocPayload()) {
            match_data.doc_payload_ = doc_payload_buffer_[GetDocOffsetInBuffer()];
        }
    }

private:
    u32 GetCurrentSeekedDocCount() const { return posting_decoder_->InnerGetSeekedDocCount() + (GetDocOffsetInBuffer() + 1); }

    i32 GetDocOffsetInBuffer() const { return doc_buffer_cursor_ - doc_buffer_base_ - 1; }

    void DecodeTFBuffer() {
        if (posting_option_.HasTfList()) {
            if (posting_decoder_->DecodeCurrentTFBuffer(tf_buffer_)) {
                tf_buffer_cursor_ = 0;
            }
        }
    }

    void DecodeDocPayloadBuffer() {
        if (posting_option_.HasDocPayload()) {
            posting_decoder_->DecodeCurrentDocPayloadBuffer(doc_payload_buffer_);
        }
    }

    void MoveToCurrentDoc(bool fetch_position);

private:
    SharedPtr<Vector<SegmentPosting>> segment_postings_;
    PostingFormatOption posting_option_;
    MemoryPool *session_pool_;
    u32 doc_freq_ = 0;
    RowID last_doc_id_in_buffer_;
    RowID current_row_id_;
    docid_t *doc_buffer_cursor_;
    docid_t doc_buffer_[MAX_DOC_PER_RECORD];
    docid_t *doc_buffer_base_;
    ttf_t current_ttf_;
    i32 tf_buffer_cursor_;
    tf_t *tf_buffer_;
    docpayload_t *doc_payload_buffer_;
    MultiPostingDecoder *posting_decoder_;
    bool need_move_to_current_doc_;
    bool in_doc_pos_iter_inited_;
    InDocPositionState state_;
    InDocPositionIterator *in_doc_pos_iterator_;
};

} // namespace infinity