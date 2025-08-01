// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

export module infinity_core:doc_list_format_option;

import :stl;
import :posting_field;
import :index_defines;
import :infinity_exception;

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
        has_block_max_ = (option_flag & of_block_max) ? 1 : 0;
        unused_ = 0;
        // when has_block_max_ is set, has_tf_list_ must also be set
        if (has_block_max_ and !has_tf_list_) {
            UnrecoverableError("Invalid option flag: block_max must be used with term frequency list.");
        }
    }

    bool HasTF() const { return has_tf_ == 1; }
    bool HasTfList() const { return has_tf_list_ == 1; }
    bool HasDocPayload() const { return has_doc_payload_ == 1; }
    bool HasBlockMax() const { return has_block_max_ == 1; }
    bool operator==(const DocListFormatOption &right) const {
        return has_tf_ == right.has_tf_ && has_tf_list_ == right.has_tf_list_ && has_doc_payload_ == right.has_doc_payload_ &&
               short_list_vbyte_compress_ == right.short_list_vbyte_compress_ && has_block_max_ == right.has_block_max_;
    }
    bool IsShortListVbyteCompress() const { return short_list_vbyte_compress_ == 1; }
    void SetShortListVbyteCompress(bool flag) { short_list_vbyte_compress_ = flag ? 1 : 0; }

private:
    u8 has_tf_ : 1;
    u8 has_tf_list_ : 1;
    u8 has_doc_payload_ : 1;
    u8 short_list_vbyte_compress_ : 1;
    u8 has_block_max_ : 1;
    u8 unused_ : 3;
};

export class DocSkipListFormat : public PostingFields {
public:
    explicit DocSkipListFormat(const DocListFormatOption &option) { Init(option); }

    ~DocSkipListFormat() = default;

    bool HasTfList() const { return has_tf_list_; }
    bool HasBlockMax() const { return has_block_max_; }

private:
    void Init(const DocListFormatOption &option) {
        u8 row_count = 0;
        u32 offset = 0;
        {
            // NoCompressPostingValue<u32> *doc_id_field = new NoCompressPostingValue<u32>;
            TypedPostingField<u32> *doc_id_field = new TypedPostingField<u32>;
            doc_id_field->location_ = row_count++;
            doc_id_field->offset_ = offset;
            doc_id_field->encoder_ = GetSkipListEncoder();
            values_.push_back(doc_id_field);
            offset += sizeof(u32);
        }
        if (option.HasTfList()) {
            has_tf_list_ = true;
            TypedPostingField<u32> *tf_field = new TypedPostingField<u32>;
            tf_field->location_ = row_count++;
            tf_field->offset_ = offset;
            tf_field->encoder_ = GetSkipListEncoder();
            values_.push_back(tf_field);
            offset += sizeof(u32);
        }
        if (option.HasBlockMax()) {
            has_block_max_ = true;
            // max_tf in a block
            TypedPostingField<u32> *max_tf_field = new TypedPostingField<u32>;
            max_tf_field->location_ = row_count++;
            max_tf_field->offset_ = offset;
            max_tf_field->encoder_ = GetSkipListEncoder();
            values_.push_back(max_tf_field);
            offset += sizeof(u32);
            // max_percentage in a block (max tf / doc_len, quantized to u16)
            TypedPostingField<u16> *max_percentage_field = new TypedPostingField<u16>;
            max_percentage_field->location_ = row_count++;
            max_percentage_field->offset_ = offset;
            max_percentage_field->encoder_ = GetTermPercentageEncoder();
            values_.push_back(max_percentage_field);
            offset += sizeof(u16);
        }
        {
            TypedPostingField<u32> *offset_field = new TypedPostingField<u32>;
            offset_field->location_ = row_count++;
            offset_field->offset_ = offset;
            offset_field->encoder_ = GetSkipListEncoder();
            values_.push_back(offset_field);
        }
    }

private:
    bool has_tf_list_ = false;
    bool has_block_max_ = false;
};

export class DocListFormat : public PostingFields {
public:
    explicit DocListFormat(const DocListFormatOption &option) : option_(option), skiplist_format_(nullptr) { Init(option_); }

    ~DocListFormat() {
        if (skiplist_format_) {
            delete skiplist_format_;
            skiplist_format_ = nullptr;
        }
    };

    const DocSkipListFormat *GetDocSkipListFormat() const { return skiplist_format_; }
    const DocListFormatOption GetOption() const { return option_; }

private:
    void Init(const DocListFormatOption &option) {
        u8 row_count = 0;
        u32 offset = 0;
        {
            TypedPostingField<u32> *doc_id_field = new TypedPostingField<u32>;
            doc_id_field->location_ = row_count++;
            doc_id_field->offset_ = offset;
            doc_id_field->encoder_ = GetDocIDEncoder();
            values_.push_back(doc_id_field);
            offset += sizeof(u32);
        }
        if (option.HasTfList()) {
            TypedPostingField<u32> *tf_field = new TypedPostingField<u32>;
            tf_field->location_ = row_count++;
            tf_field->offset_ = offset;
            tf_field->encoder_ = GetTFEncoder();
            values_.push_back(tf_field);
            offset += sizeof(u32);
        }
        if (option.HasDocPayload()) {
            PostingField *doc_payload_value = new TypedPostingField<u16>;
            doc_payload_value->location_ = row_count++;
            doc_payload_value->offset_ = offset;
            values_.push_back(doc_payload_value);
        }
        if (skiplist_format_) {
            delete skiplist_format_;
            skiplist_format_ = nullptr;
        }
        skiplist_format_ = new DocSkipListFormat(option);
    }

public:
    const DocListFormatOption option_{OPTION_FLAG_ALL};

private:
    DocSkipListFormat *skiplist_format_;
};

} // namespace infinity
