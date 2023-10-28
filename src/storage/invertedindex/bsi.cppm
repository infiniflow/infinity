module;

export module bsi;

import stl;

namespace roaring {
class Roaring;
}

namespace infinity {

using Roaring = roaring::Roaring;

class BitSlicedIndex {
public:
    BitSlicedIndex(u64 schema_id, u64 table_id, u64 column_id);

    ~BitSlicedIndex();

    bool DoPut(u32 id, u64 val, u64 &old_val);

    void DoRemove(u32 id, u64 val);

    bool DoGet(u32 id, u64 &val);

    int DoRangeLT(u64 expect, bool allow_eq, UniquePtr<Roaring> &filter);

    int DoRangeGT(u64 expect, bool allow_eq, UniquePtr<Roaring> &filter);

    int DoRangeEQ(u64 expect, UniquePtr<Roaring> &filter);

    int DoRangeNEQ(u64 expect, UniquePtr<Roaring> &filter);

    int DoRangeBetween(u64 min, u64 max, UniquePtr<Roaring> &filter);

private:
    u64 schema_id_;
    u64 table_id_;
    u64 column_id_;
    Vector<SharedPtr<Roaring>> bitmaps_;
    u8 bit_depth_ = 0;
};
} // namespace infinity