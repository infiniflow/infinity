#pragma once

#include <memory>

#include "posting_value.h"
#include "storage/invertedindex/index_defines.h"

namespace infinity {

class PositionListFormatOption {
public:
    explicit PositionListFormatOption(optionflag_t option_flag = OPTION_FLAG_ALL) { Init(option_flag); }
    ~PositionListFormatOption() {}

    inline void Init(optionflag_t option_flag) {
        has_position_list_ = option_flag & of_position_list ? 1 : 0;
        if ((option_flag & of_term_frequency) && (option_flag & of_tf_bitmap)) {
            has_tf_bitmap_ = 1;
        } else {
            has_tf_bitmap_ = 0;
        }
        unused_ = 0;
    }

    bool HasPositionList() const { return has_position_list_ == 1; }

    bool HasTfBitmap() const { return has_tf_bitmap_ == 1; }

    bool operator==(const PositionListFormatOption &right) const {
        return has_position_list_ == right.has_position_list_ && has_tf_bitmap_ == right.has_tf_bitmap_;
    }

private:
    uint8_t has_position_list_ : 1;
    uint8_t has_tf_bitmap_ : 1;
    uint8_t unused_ : 6;
};

class PositionSkipListFormat : public PostingValues {
public:
    PositionSkipListFormat() = default;

    ~PositionSkipListFormat() = default;

    void Init(const PositionListFormatOption &option) {
        uint8_t row_count = 0;
        uint32_t offset = 0;
        if (!option.HasTfBitmap()) {
            TypedPostingValue<uint32_t> *total_pos_value = new TypedPostingValue<uint32_t>;
            total_pos_value->location_ = row_count++;
            total_pos_value->offset_ = offset;
            total_pos_value->encoder_ = GetPosListEncoder();
            values_.push_back(total_pos_value);
            offset += sizeof(uint32_t);
        }
        {
            TypedPostingValue<uint32_t> *offset_value = new TypedPostingValue<uint32_t>;
            offset_value->location_ = row_count++;
            offset_value->offset_ = offset;
            offset_value->encoder_ = GetPosListEncoder();
            values_.push_back(offset_value);
        }
    }

    bool HasTfList() const { return GetSize() > 1; }
};

class PositionListFormat : public PostingValues {
public:
    PositionListFormat(const PositionListFormatOption &option) : skiplist_format_(nullptr) { Init(option); }
    PositionListFormat() : skiplist_format_(nullptr) {}

    ~PositionListFormat() {
        if (skiplist_format_) {
            delete skiplist_format_;
            skiplist_format_ = nullptr;
        }
    };

    void Init(const PositionListFormatOption &option) {
        uint8_t row_count = 0;
        uint32_t offset = 0;
        {
            TypedPostingValue<uint32_t> *pos_value = new TypedPostingValue<uint32_t>;
            pos_value->location_ = row_count++;
            pos_value->offset_ = offset;
            pos_value->encoder_ = GetPosListEncoder();
            values_.push_back(pos_value);
            offset += sizeof(uint32_t);
        }
        skiplist_format_ = new PositionSkipListFormat;
        skiplist_format_->Init(option);
    }

    const PositionSkipListFormat *GetPositionSkipListFormat() const { return skiplist_format_; }

private:
    PositionSkipListFormat *skiplist_format_;
};

} // namespace infinity
