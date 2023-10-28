module;

import stl;
import byte_slice;
import memory_pool;
import byte_slice_reader;
import posting_decoder;
import index_defines;
import posting_value;

export module index_decoder;

namespace infinity {

export class IndexDecoder {
public:
    IndexDecoder() {}

    virtual ~IndexDecoder() = default;

    virtual bool DecodeDocBuffer(docid_t start_doc_id, docid_t *doc_buffer, docid_t &first_doc_id, docid_t &last_doc_id, ttf_t &current_ttf) = 0;

    virtual bool DecodeCurrentTFBuffer(tf_t *tf_buffer) = 0;

    virtual void DecodeCurrentDocPayloadBuffer(docpayload_t *docpayload_buffer) = 0;

    virtual void InitSkipList(u32 start, u32 end, ByteSliceList *posting_list, df_t df) {}

    virtual void InitSkipList(u32 start, u32 end, ByteSlice *posting_list, df_t df) {}

    u32 InnerGetSeekedDocCount() const { return skiped_item_count_ << MAX_DOC_PER_RECORD_BIT_NUM; }

protected:
    u32 skiped_item_count_ = {0};
};

export template <typename SkipListType>
class SkipIndexDecoder : public IndexDecoder {
public:
    SkipIndexDecoder(MemoryPool *session_pool, ByteSliceReader *doc_list_reader, u32 doc_list_begin)
        : skiplist_reader_(nullptr), session_pool_(session_pool), doc_list_reader_(doc_list_reader), doc_list_begin_pos_(doc_list_begin) {
        doc_id_encoder_ = GetDocIDEncoder();
        tf_list_encoder_ = GetTFEncoder();
        doc_payload_encoder_ = GetDocPayloadEncoder();
    }

    virtual ~SkipIndexDecoder() {
        if (skiplist_reader_) {
            if (session_pool_) {
                skiplist_reader_->~SkipListType();
                session_pool_->Deallocate((void *)skiplist_reader_, sizeof(SkipListType));
            } else {
                delete skiplist_reader_;
            }
        }
    }

    void InitSkipList(u32 start, u32 end, ByteSliceList *posting_list, df_t df) {
        skiplist_reader_ = session_pool_ ? (new ((session_pool_)->Allocate(sizeof(SkipListType))) SkipListType()) : new SkipListType();
        skiplist_reader_->Load(posting_list, start, end, (df - 1) / MAX_DOC_PER_RECORD + 1);
    }

    void InitSkipList(u32 start, u32 end, ByteSlice *posting_list, df_t df) {
        skiplist_reader_ = session_pool_ ? (new ((session_pool_)->Allocate(sizeof(SkipListType))) SkipListType()) : new SkipListType();
        skiplist_reader_->Load(posting_list, start, end, (df - 1) / MAX_DOC_PER_RECORD + 1);
    }

    bool DecodeDocBuffer(docid_t start_doc_id, docid_t *doc_buffer, docid_t &first_doc_id, docid_t &last_doc_id, ttf_t &current_ttf) {
        u32 offset;
        u32 record_len;
        u32 last_doc_id_in_prev_record;

        auto ret = skiplist_reader_->SkipTo(start_doc_id, last_doc_id, last_doc_id_in_prev_record, offset, record_len);
        if (!ret) {
            return false;
        }
        current_ttf = skiplist_reader_->GetPrevTTF();
        skiped_item_count_ = skiplist_reader_->GetSkippedItemCount();

        doc_list_reader_->Seek(offset + this->doc_list_begin_pos_);
        doc_id_encoder_->Decode((u32 *)doc_buffer, MAX_DOC_PER_RECORD, *doc_list_reader_);

        first_doc_id = doc_buffer[0] + last_doc_id_in_prev_record;
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
    MemoryPool *session_pool_;
    ByteSliceReader *doc_list_reader_;
    u32 doc_list_begin_pos_;
    const Int32Encoder *doc_id_encoder_;
    const Int32Encoder *tf_list_encoder_;
    const Int16Encoder *doc_payload_encoder_;
};

} // namespace infinity