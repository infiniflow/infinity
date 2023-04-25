#pragma once
#include <stdint.h>
#include <vector>
#include <roaring/roaring.h>
#include "common.h"

namespace infinity {

enum BSIType {
    INT_INDEX,
    FLOAT_INDEX
};

struct Meta {
    BSIType bsi_type;
    double min;
    double max;
    size_t topk_limit;
};

class BitSliceIndexCreateOptions {
private:
    int64_t min_int_ = 0;
    int64_t max_int_ = 0;
    uint64_t min_ = 0;
    uint64_t max_ = 0;
    uint32_t limit_topk_ = 0;

public:
    BitSliceIndexCreateOptions();
    BitSliceIndexCreateOptions& SetMinMax(float min_d, float max_d);
    BitSliceIndexCreateOptions& SetMinMax(int64_t min_i, int64_t max_i);
    uint64_t Distance();
    int64_t GetActualIntVal(uint64_t v);
    float GetActualFloatVal(uint64_t v);
    uint64_t ToLocalVal(int64_t v);
    uint64_t ToLocalVal(float v);
    uint32_t LimitTopK() {
        return limit_topk_;
    }
    void SetLimitTopK(uint32_t v) {
        limit_topk_ = v;
    }
};

class BitSliceIndex {
protected:
    BitSliceIndex(BitSliceIndex&) = delete;
    BitSliceIndex& operator=(const BitSliceIndex&) = delete;
    BitSliceIndexCreateOptions options_;
    std::vector<RoaringBitmapPtr> bitmaps_;
    uint8_t bit_depth_ = 0;
    int DoGetMax(roaring_bitmap_t* filter, uint64_t& max, int64_t& count);
    int DoGetMin(roaring_bitmap_t* filter, uint64_t& min, int64_t& count);
    int DoRangeLT(uint64_t expect, bool allow_eq, roaring_bitmap_t* out);
    int DoRangeGT(uint64_t expect, bool allow_eq, roaring_bitmap_t* out);
    int DoRangeEQ(uint64_t expect, roaring_bitmap_t* out);
    int DoRangeNEQ(uint64_t expect, roaring_bitmap_t* out);
    int DoRangeBetween(uint64_t min, uint64_t max, roaring_bitmap_t* out);
    uint64_t DoRemoveMin();
    void DoRemove(uint32_t id, uint64_t val);
    bool DoPut(uint32_t id, uint64_t val, uint64_t& old_val);
    bool DoGet(uint32_t id, uint64_t& val);

    int DoInit(const Meta& meta);

public:
    BitSliceIndex() = default;
    int Init(const Meta& meta);
    int TopK(uint32_t k, roaring_bitmap_t* out);
    int Load(FILE* fp);
    int Save(FILE* fp, bool readonly);
    const roaring_bitmap_t* GetExistBitmap();
};

class BitSliceIntIndex : public BitSliceIndex {
public:
    int GetMin(roaring_bitmap_t* filter, int64_t& min, int64_t& count);
    int GetMax(roaring_bitmap_t* filter, int64_t& max, int64_t& count);
    int RangeBetween(int64_t min, int64_t max, roaring_bitmap_t* out);
    int RangeLT(int64_t expect, bool allow_eq, roaring_bitmap_t* out);
    int RangeGT(int64_t expect, bool allow_eq, roaring_bitmap_t* out);
    int RangeEQ(int64_t expect, roaring_bitmap_t* out);
    int RangeNEQ(int64_t expect, roaring_bitmap_t* out);
    void Remove(uint32_t id, int64_t val);
    void Put(uint32_t id, int64_t val);
    bool Get(uint32_t id, int64_t& val);
    int64_t RemoveMin();
};

typedef std::unique_ptr<BitSliceIntIndex> BitSliceIntIndexPtr;

class BitSliceFloatIndex : public BitSliceIndex {
private:
public:
    int GetMin(roaring_bitmap_t* filter, float& min, int64_t& count);
    int GetMax(roaring_bitmap_t* filter, float& max, int64_t& count);
    int RangeBetween(float min, float max, roaring_bitmap_t* out);
    int RangeLT(float expect, bool allow_eq, roaring_bitmap_t* out);
    int RangeGT(float expect, bool allow_eq, roaring_bitmap_t* out);
    int RangeEQ(float expect, roaring_bitmap_t* out);
    int RangeNEQ(float expect, roaring_bitmap_t* out);
    double RemoveMin();
    void Remove(uint32_t id, float val);
    bool Put(uint32_t id, float val, float& old_val);
    bool Get(uint32_t id, float& val);
};
typedef std::unique_ptr<BitSliceFloatIndex> BitSliceFloatIndexPtr;

}  // namespace infinity