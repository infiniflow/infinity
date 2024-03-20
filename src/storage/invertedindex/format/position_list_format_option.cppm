module;

import stl;
import posting_field;
import index_defines;
export module position_list_format_option;

namespace infinity {

export class PositionListFormatOption {
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
    u8 has_position_list_ : 1;
    u8 has_tf_bitmap_ : 1;
    u8 unused_ : 6;
};

export class PositionSkipListFormat : public PostingFields {
public:
    PositionSkipListFormat() = default;

    ~PositionSkipListFormat() = default;

    void Init(const PositionListFormatOption &option) {
        u8 row_count = 0;
        u32 offset = 0;
        if (!option.HasTfBitmap()) {
            TypedPostingField<u32> *total_pos_value = new TypedPostingField<u32>;
            total_pos_value->location_ = row_count++;
            total_pos_value->offset_ = offset;
            total_pos_value->encoder_ = GetPosListEncoder();
            values_.push_back(total_pos_value);
            offset += sizeof(u32);
        }
        {
            TypedPostingField<u32> *offset_value = new TypedPostingField<u32>;
            offset_value->location_ = row_count++;
            offset_value->offset_ = offset;
            offset_value->encoder_ = GetPosListEncoder();
            values_.push_back(offset_value);
        }
    }

    bool HasTfList() const { return GetSize() > 1; }
};

export class PositionListFormat : public PostingFields {
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
        u8 row_count = 0;
        u32 offset = 0;
        {
            TypedPostingField<u32> *pos_value = new TypedPostingField<u32>;
            pos_value->location_ = row_count++;
            pos_value->offset_ = offset;
            pos_value->encoder_ = GetPosListEncoder();
            values_.push_back(pos_value);
            offset += sizeof(u32);
        }
        skiplist_format_ = new PositionSkipListFormat;
        skiplist_format_->Init(option);
    }

    const PositionSkipListFormat *GetPositionSkipListFormat() const { return skiplist_format_; }

private:
    PositionSkipListFormat *skiplist_format_;
};

} // namespace infinity
