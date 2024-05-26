module;

import stl;
import byte_slice;

import byte_slice_reader;
import posting_decoder;
import index_defines;
import posting_field;
import doc_list_format_option;

export module index_decoder;

namespace infinity {

export class IndexDecoder {
public:
    IndexDecoder(const DocListFormatOption &doc_list_format_option) : doc_list_format_option_(doc_list_format_option) {}

    virtual ~IndexDecoder() = default;

    virtual bool DecodeSkipList(docid_t start_doc_id, docid_t &prev_last_doc_id, docid_t &last_doc_id, ttf_t &current_ttf) = 0;

    // u32: block max tf
    // u16: block max (ceil(tf / doc length) * numeric_limits<u16>::max())
    virtual Pair<u32, u16> GetBlockMaxInfo() const = 0;

    virtual bool DecodeCurrentDocIDBuffer(docid_t *doc_buffer) = 0;

    virtual bool DecodeCurrentTFBuffer(tf_t *tf_buffer) = 0;

    virtual void DecodeCurrentDocPayloadBuffer(docpayload_t *docpayload_buffer) = 0;

    virtual void InitSkipList(u32 start, u32 end, ByteSliceList *posting_list, df_t df) {}

    virtual void InitSkipList(u32 start, u32 end, ByteSlice *posting_list, df_t df) {}

    u32 InnerGetSeekedDocCount() const { return skiped_item_count_ << MAX_DOC_PER_RECORD_BIT_NUM; }

protected:
    u32 offset_ = 0;
    u32 record_len_ = 0;
    u32 last_doc_id_in_prev_record_ = 0;
    u32 last_doc_id_ = 0;
    u32 current_ttf_ = 0;
    u32 skiped_item_count_ = 0;
    DocListFormatOption doc_list_format_option_;
};

export template <typename SkipListType>
class SkipIndexDecoder : public IndexDecoder {
public:
    SkipIndexDecoder(ByteSliceReader *doc_list_reader,
                     u32 doc_list_begin,
                     const DocListFormatOption &doc_list_format_option)
        : IndexDecoder(doc_list_format_option), skiplist_reader_(nullptr), doc_list_reader_(doc_list_reader),
          doc_list_begin_pos_(doc_list_begin) {
        doc_id_encoder_ = GetDocIDEncoder();
        tf_list_encoder_ = GetTFEncoder();
        doc_payload_encoder_ = GetDocPayloadEncoder();
    }

    virtual ~SkipIndexDecoder() {
        if (skiplist_reader_) {
            delete skiplist_reader_;
        }
    }

    void InitSkipList(u32 start, u32 end, ByteSliceList *posting_list, df_t df) {
        skiplist_reader_ = new SkipListType(doc_list_format_option_);
        skiplist_reader_->Load(posting_list, start, end);
    }

    void InitSkipList(u32 start, u32 end, ByteSlice *posting_list, df_t df) {
        skiplist_reader_ = new SkipListType(doc_list_format_option_);
        skiplist_reader_->Load(posting_list, start, end);
    }

    bool DecodeSkipList(docid_t start_doc_id, docid_t &prev_last_doc_id, docid_t &last_doc_id, ttf_t &current_ttf) {
        auto ret = skiplist_reader_->SkipTo(start_doc_id, last_doc_id_, last_doc_id_in_prev_record_, offset_, record_len_);
        if (!ret) {
            return false;
        }
        skiped_item_count_ = skiplist_reader_->GetSkippedItemCount();
        prev_last_doc_id = last_doc_id_in_prev_record_;
        last_doc_id = last_doc_id_;
        current_ttf = current_ttf_ = skiplist_reader_->GetPrevTTF();
        return true;
    }

    // u32: block max tf
    // u16: block max (ceil(tf / doc length) * numeric_limits<u16>::max())
    Pair<u32, u16> GetBlockMaxInfo() const {
        return skiplist_reader_->GetBlockMaxInfo();
    }

    bool DecodeCurrentDocIDBuffer(docid_t *doc_buffer) {
        doc_list_reader_->Seek(offset_ + doc_list_begin_pos_);
        doc_id_encoder_->Decode((u32 *)doc_buffer, MAX_DOC_PER_RECORD, *doc_list_reader_);
        return true;
    }

    bool DecodeCurrentTFBuffer(tf_t *tf_buffer) {
        tf_list_encoder_->Decode((u32 *)tf_buffer, MAX_DOC_PER_RECORD, *doc_list_reader_);
        return true;
    }

    void DecodeCurrentDocPayloadBuffer(docpayload_t *doc_payload_buffer) {
        doc_payload_encoder_->Decode(doc_payload_buffer, MAX_DOC_PER_RECORD, *doc_list_reader_);
    }

private:
    SkipListType *skiplist_reader_;
    ByteSliceReader *doc_list_reader_;
    u32 doc_list_begin_pos_;
    const Int32Encoder *doc_id_encoder_;
    const Int32Encoder *tf_list_encoder_;
    const Int16Encoder *doc_payload_encoder_;
};

} // namespace infinity