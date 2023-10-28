module;

import stl;
import byte_slice;
import byte_slice_reader;
export module skiplist_reader;

namespace infinity {

export class SkipListReader {
public:
    SkipListReader();
    SkipListReader(const SkipListReader &other);
    virtual ~SkipListReader();

public:
    virtual void Load(const ByteSliceList *byte_slice_list, u32 start, u32 end);

    virtual void Load(ByteSlice *byteSlice, u32 start, u32 end);

    virtual bool SkipTo(u32 query_key, u32 &key, u32 &prev_key, u32 &prev_value, u32 &value_delta) { return false; }

    u32 GetStart() const { return start_; }

    u32 GetEnd() const { return end_; }

    const ByteSliceList *GetByteSliceList() const { return byte_slice_reader_.GetByteSliceList(); }

    i32 GetSkippedItemCount() const { return skipped_item_count_; }

    virtual u32 GetPrevTTF() const { return 0; }
    virtual u32 GetCurrentTTF() const { return 0; }

    virtual u32 GetLastValueInBuffer() const { return 0; }
    virtual u32 GetLastKeyInBuffer() const { return 0; }

protected:
    u32 start_;
    u32 end_;
    ByteSliceReader byte_slice_reader_;
    i32 skipped_item_count_;
};

} // namespace infinity
