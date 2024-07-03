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
        unused_ = 0;
    }

    bool HasPositionList() const { return has_position_list_ == 1; }

    bool operator==(const PositionListFormatOption &right) const { return has_position_list_ == right.has_position_list_; }

private:
    u8 has_position_list_ : 1;
    u8 unused_ : 7;
};

export class PositionSkipListFormat : public PostingFields {
public:
    PositionSkipListFormat() = default;

    ~PositionSkipListFormat() = default;

    void Init(const PositionListFormatOption &option) {
        u8 row_count = 0;
        u32 offset = 0;
        {
            TypedPostingField<u32> *total_pos_field = new TypedPostingField<u32>;
            total_pos_field->location_ = row_count++;
            total_pos_field->offset_ = offset;
            total_pos_field->encoder_ = GetPosListEncoder();
            values_.push_back(total_pos_field);
            offset += sizeof(u32);
        }
        {
            TypedPostingField<u32> *offset_field = new TypedPostingField<u32>;
            offset_field->location_ = row_count++;
            offset_field->offset_ = offset;
            offset_field->encoder_ = GetPosListEncoder();
            values_.push_back(offset_field);
        }
    }

    bool HasTfList() const { return GetSize() > 1; }
};

export class PositionListFormat : public PostingFields {
public:
    explicit PositionListFormat(const PositionListFormatOption &option) : option_(option), skiplist_format_(nullptr) { Init(option); }

    ~PositionListFormat() {
        if (skiplist_format_) {
            delete skiplist_format_;
            skiplist_format_ = nullptr;
        }
    };

    const PositionSkipListFormat *GetPositionSkipListFormat() const { return skiplist_format_; }
    const PositionListFormatOption GetOption() const { return option_; }

private:
    void Init(const PositionListFormatOption &option) {
        u8 row_count = 0;
        u32 offset = 0;
        {
            TypedPostingField<u32> *pos_field = new TypedPostingField<u32>;
            pos_field->location_ = row_count++;
            pos_field->offset_ = offset;
            pos_field->encoder_ = GetPosListEncoder();
            values_.push_back(pos_field);
            offset += sizeof(u32);
        }
        skiplist_format_ = new PositionSkipListFormat;
        skiplist_format_->Init(option);
    }

public:
    const PositionListFormatOption option_;

private:
    PositionSkipListFormat *skiplist_format_;
};

} // namespace infinity
