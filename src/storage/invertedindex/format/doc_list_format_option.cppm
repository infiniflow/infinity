module;

import stl;
import posting_field;
import index_defines;

export module doc_list_format_option;

namespace infinity {

export class DocListFormatOption {
public:
    explicit DocListFormatOption(optionflag_t option_flag = OPTION_FLAG_ALL) { Init(option_flag); }

    ~DocListFormatOption() = default;

    inline void Init(optionflag_t option_flag) {
        has_doc_payload_ = option_flag & of_doc_payload ? 1 : 0;
        if (option_flag & of_term_frequency) {
            has_tf_ = 1;
            has_tf_list_ = 1;
        } else {
            has_tf_ = 0;
            has_tf_list_ = 0;
        }
        short_list_vbyte_compress_ = 0;
        unused_ = 0;
    }

    bool HasTF() const { return has_tf_ == 1; }
    bool HasTfList() const { return has_tf_list_ == 1; }
    bool HasDocPayload() const { return has_doc_payload_ == 1; }
    bool operator==(const DocListFormatOption &right) const {
        return has_tf_ == right.has_tf_ && has_tf_list_ == right.has_tf_list_ && has_doc_payload_ == right.has_doc_payload_ &&
               short_list_vbyte_compress_ == right.short_list_vbyte_compress_;
    }
    bool IsShortListVbyteCompress() const { return short_list_vbyte_compress_ == 1; }
    void SetShortListVbyteCompress(bool flag) { short_list_vbyte_compress_ = flag ? 1 : 0; }

private:
    u8 has_tf_ : 1;
    u8 has_tf_list_ : 1;
    u8 has_doc_payload_ : 1;
    u8 short_list_vbyte_compress_ : 1;
    u8 unused_ : 4;
};

export class DocSkipListFormat : public PostingFields {
public:
    DocSkipListFormat() = default;

    ~DocSkipListFormat() = default;

    void Init(const DocListFormatOption &option) {
        u8 row_count = 0;
        u32 offset = 0;
        {
            // NoCompressPostingValue<u32> *doc_id_value = new NoCompressPostingValue<u32>;
            TypedPostingField<u32> *doc_id_value = new TypedPostingField<u32>;
            doc_id_value->location_ = row_count++;
            doc_id_value->offset_ = offset;
            values_.push_back(doc_id_value);
            offset += sizeof(u32);
        }
        if (option.HasTfList()) {
            TypedPostingField<u32> *tf_value = new TypedPostingField<u32>;
            tf_value->location_ = row_count++;
            tf_value->offset_ = offset;
            values_.push_back(tf_value);
            offset += sizeof(u32);
        }
        {
            TypedPostingField<u32> *offset_value = new TypedPostingField<u32>;
            offset_value->location_ = row_count++;
            offset_value->offset_ = offset;
            values_.push_back(offset_value);
        }
    }

    bool HasTfList() const { return GetSize() > 2; }
};

export class DocListFormat : public PostingFields {
public:
    DocListFormat(const DocListFormatOption &option) : skiplist_format_(nullptr) { Init(option); }
    DocListFormat() : skiplist_format_(nullptr) {}

    ~DocListFormat() {
        if (skiplist_format_) {
            delete skiplist_format_;
            skiplist_format_ = nullptr;
        }
    };

    void Init(const DocListFormatOption &option) {
        u8 row_count = 0;
        u32 offset = 0;
        {
            TypedPostingField<u32> *doc_id_value = new TypedPostingField<u32>;
            doc_id_value->location_ = row_count++;
            doc_id_value->offset_ = offset;
            doc_id_value->encoder_ = GetDocIDEncoder();
            values_.push_back(doc_id_value);
            offset += sizeof(u32);
        }
        if (option.HasTfList()) {
            TypedPostingField<u32> *tf_value = new TypedPostingField<u32>;
            tf_value->location_ = row_count++;
            tf_value->offset_ = offset;
            tf_value->encoder_ = GetTFEncoder();
            values_.push_back(tf_value);
            offset += sizeof(u32);
        }
        if (option.HasDocPayload()) {
            PostingField *doc_payload_value = new TypedPostingField<u16>;
            doc_payload_value->location_ = row_count++;
            doc_payload_value->offset_ = offset;
            values_.push_back(doc_payload_value);
        }
        skiplist_format_ = new DocSkipListFormat;
        skiplist_format_->Init(option);
    }

    const DocSkipListFormat *GetDocSkipListFormat() const { return skiplist_format_; }

private:
    DocSkipListFormat *skiplist_format_;
};

} // namespace infinity
