module;

import stl;

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
import third_party;

export module posting_iterator;

namespace infinity {
export class PostingIterator {
public:
    PostingIterator(optionflag_t flag);

    ~PostingIterator();

    bool Init(SharedPtr<Vector<SegmentPosting>> seg_postings, const u32 state_pool_size);

    void Reset();

    u32 GetDocFreq() const { return doc_freq_; }

    bool SkipTo(RowID doc_id);

    RowID PrevBlockLastDocID() const { return last_doc_id_in_prev_block_; }

    RowID BlockLowestPossibleDocID() const { return lowest_possible_doc_id_in_current_block_; }

    RowID BlockLastDocID() const { return last_doc_id_in_current_block_; }

    // u32: block max tf
    // u16: block max (ceil(tf / doc length) * numeric_limits<u16>::max())
    Pair<u32, u16> GetBlockMaxInfo() const;

    RowID SeekDoc(RowID docId);

    inline RowID DocID() const { return current_row_id_; }

    Pair<bool, RowID> PeekInBlockRange(RowID doc_id, RowID doc_id_no_beyond);

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
    u32 doc_freq_ = 0;

    // info for skiplist, block max
    RowID last_doc_id_in_prev_block_ = INVALID_ROWID;
    RowID lowest_possible_doc_id_in_current_block_ = INVALID_ROWID;
    RowID last_doc_id_in_current_block_ = INVALID_ROWID;
    ttf_t current_ttf_ = 0;

    // info for decode buffer
    RowID current_row_id_ = INVALID_ROWID;
    docid_t *doc_buffer_cursor_ = nullptr;
    docid_t doc_buffer_[MAX_DOC_PER_RECORD] = {};
    docid_t *doc_buffer_base_ = nullptr;
    i32 tf_buffer_cursor_ = 0;
    tf_t *tf_buffer_ = nullptr;
    docpayload_t *doc_payload_buffer_ = nullptr;
    MultiPostingDecoder *posting_decoder_ = nullptr;
    bool finish_decode_docid_ = false;
    bool need_move_to_current_doc_ = false;
    bool in_doc_pos_iter_inited_ = false;
    InDocPositionState state_ = posting_option_.GetPosListFormatOption();
    InDocPositionIterator *in_doc_pos_iterator_ = nullptr;
};

} // namespace infinity