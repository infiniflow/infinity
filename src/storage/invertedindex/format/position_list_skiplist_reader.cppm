module;

export module position_list_skiplist_reader;

import stl;
import byte_slice;
import byte_slice_reader;
import skiplist_reader;
import index_defines;

namespace infinity {
export class PositionListSkipListReader : public SkipListReader {
public:
    PositionListSkipListReader();

    PositionListSkipListReader(const PositionListSkipListReader &other) noexcept;

    virtual ~PositionListSkipListReader();

public:
    void Load(const ByteSliceList *byte_slice_list, u32 start, u32 end, const u32 &item_count);

    void Load(ByteSlice *byte_slice, u32 start, u32 end, const u32 &item_count);

    bool SkipTo(u32 query_key, u32 &key, u32 &prev_key, u32 &value, u32 &delta) override;

    bool SkipTo(u32 query_key, u32 &key, u32 &value, u32 &delta) { return SkipTo(query_key, key, prev_key_, value, delta); }

    u32 GetPrevKey() const { return prev_key_; }

    u32 GetCurrentKey() const { return current_key_; }

    u32 GetLastValueInBuffer() const override;

    u32 GetLastKeyInBuffer() const override;

private:
    void Load_(u32 start, u32 end, const u32 &item_count);

protected:
    virtual Pair<int, bool> LoadBuffer();

protected:
    u32 current_key_;
    u32 current_value_;
    u32 prev_key_;
    u32 prev_value_;
    u32 key_buffer_[SKIP_LIST_BUFFER_SIZE];
    u32 value_buffer_[SKIP_LIST_BUFFER_SIZE];
    u32 current_cursor_;
    u32 num_in_buffer_;
    u32 *key_buffer_base_;
    u32 *value_buffer_base_;
};

} // namespace infinity