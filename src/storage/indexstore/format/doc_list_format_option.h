#pragma once

#include "storage/indexstore/index_defines.h"
#include "posting_value.h"

namespace infinity {

class DocListFormatOption {
public:
    explicit DocListFormatOption(optionflag_t option_flag = OPTION_FLAG_ALL) {
        Init(option_flag);
    }
    ~DocListFormatOption() = default;

    inline void Init(optionflag_t option_flag) {
        has_doc_payload_ = option_flag & of_doc_payload ? 1 : 0;
        if (option_flag & of_term_frequency) {
            has_tf_ = 1;
            if (option_flag & of_tf_bitmap) {
                has_tf_bitmap_ = 1;
                has_tf_list_ = 0;
            } else {
                has_tf_bitmap_ = 0;
                has_tf_list_ = 1;
            }
        } else {
            has_tf_ = 0;
            has_tf_list_ = 0;
            has_tf_bitmap_ = 0;
        }
        short_list_vbyte_compress_ = 0;
        unused_ = 0;
    }

    bool HasTF() const {
        return has_tf_ == 1;
    }
    bool HasTfList() const {
        return has_tf_list_ == 1;
    }
    bool HasTfBitmap() const {
        return has_tf_bitmap_ == 1;
    }
    bool HasDocPayload() const {
        return has_doc_payload_ == 1;
    }
    bool operator==(const DocListFormatOption& right) const {
        return has_tf_ == right.has_tf_ && has_tf_list_ == right.has_tf_list_ && has_tf_bitmap_ == right.has_tf_bitmap_ &&
               has_doc_payload_ == right.has_doc_payload_ && short_list_vbyte_compress_ == right.short_list_vbyte_compress_;
    }
    bool IsShortListVbyteCompress() const {
        return short_list_vbyte_compress_ == 1;
    }
    void SetShortListVbyteCompress(bool flag) {
        short_list_vbyte_compress_ = flag ? 1 : 0;
    }

private:
    uint8_t has_tf_                     : 1;
    uint8_t has_tf_list_                : 1;
    uint8_t has_tf_bitmap_              : 1;
    uint8_t has_doc_payload_            : 1;
    uint8_t short_list_vbyte_compress_  : 1;
    uint8_t unused_                     : 3;
};

class DocSkipListFormat : public PostingValues {
public:
    DocSkipListFormat() = default;
    ~DocSkipListFormat() = default;

    void Init(const DocListFormatOption& option) {
        uint8_t row_count = 0;
        uint32_t offset = 0;
        {
            PostingValue* doc_id_value = new TypedPostingValue<uint32_t>;
            doc_id_value->location_ = row_count++;
            doc_id_value->offset_ = offset;
            values_.push_back(doc_id_value);
            offset += sizeof(uint32_t);
        }
        if(option.HasTfList()) {
            PostingValue* tf_value = new TypedPostingValue<uint32_t>;
            tf_value->location_ = row_count++;
            tf_value->offset_ = offset;
            values_.push_back(tf_value);
            offset += sizeof(uint32_t);
        }
        {
            PostingValue* offset_value = new TypedPostingValue<uint32_t>;
            offset_value->location_ = row_count++;
            offset_value->offset_ = offset;
            values_.push_back(offset_value);
        }
    }
};

class DocListFormat : public PostingValues {
public:
    DocListFormat() = default;
    ~DocListFormat() = default;

    void Init(const DocListFormatOption& option) {
        skiplist_format_.Init(option);
        uint8_t row_count = 0;
        uint32_t offset = 0;
        {
            TypedPostingValue<uint32_t>* doc_id_value = new TypedPostingValue<uint32_t>;
            doc_id_value->location_ = row_count++;
            doc_id_value->offset_ = offset;
            doc_id_value->encoder_ = GetDocIDEncoder();
            values_.push_back(doc_id_value);
            offset += sizeof(uint32_t);
        }
        if(option.HasTfList()) {
            TypedPostingValue<uint32_t>* tf_value = new TypedPostingValue<uint32_t>;
            tf_value->location_ = row_count++;
            tf_value->offset_ = offset;
            tf_value->encoder_ = GetTFEncoder();
            values_.push_back(tf_value);
            offset += sizeof(uint32_t);
        }
        if(option.HasDocPayload()) {
            PostingValue* doc_payload_value = new TypedPostingValue<uint16_t>;
            doc_payload_value->location_ = row_count++;
            doc_payload_value->offset_ = offset;
            values_.push_back(doc_payload_value);
        }
    }
private:
    DocSkipListFormat skiplist_format_;
};

}
