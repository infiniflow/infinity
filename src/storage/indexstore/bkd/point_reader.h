#pragma once

#include "bit_set.h"

#include <memory>
#include <vector>

namespace infinity {
class PointWriter;
typedef std::vector<std::vector<uint8_t>> ByteArrayList;
// BKD Tree from Lucene 7.x
// We suppose within each segment, the points to be indexed could be
// contained within heap memory, as a result, we only use single class
// of PointWriter/PointReader, instead of HeapPointWriter/HeapPointReader
// and OfflinePointWriter/OfflinePointReader in the Lucene implementation
class PointReader {
public:
    PointReader(ByteArrayList *blocks,
                int32_t values_per_block,
                int32_t packed_bytes_length,
                const std::vector<int32_t> &ords,
                const std::vector<int64_t> &ords_long,
                std::vector<int32_t> *docids,
                int32_t start,
                int32_t end,
                bool single_value_per_doc);

    void ReadPackedValue(int32_t index, std::vector<uint8_t> &bytes);

    bool Next() {
        cur_read_++;
        return cur_read_ < end_;
    }

    const std::vector<uint8_t> &PackedValue();

    uint8_t* PackedValueRaw();

    int32_t Docid() {
        return (*doc_ids_)[cur_read_];
    }
    // Point ordinal
    int64_t Ord() {
        if (single_value_per_doc_) {
            return (*doc_ids_)[cur_read_];
        } else if (ords_long_.size() > 0) {
            return ords_long_[cur_read_];
        } else {
            return ords_[cur_read_];
        }
    }

    void MarkOrds(int64_t count, const std::shared_ptr<BitSet> &ord_bitset);

    // Splits this reader into left and right partitions
    int64_t Split(
        int64_t count,
        const std::shared_ptr<BitSet> &right_tree,
        const std::shared_ptr<PointWriter> &left,
        const std::shared_ptr<PointWriter> &right,
        bool do_clear_bits);

public:
    bool single_value_per_doc_;
    int32_t values_per_block_;
    int32_t packed_bytes_length_;
    int32_t end_;
    ByteArrayList* blocks_{};
    std::vector<int64_t> ords_long_;
    std::vector<int32_t> ords_;
    std::vector<int32_t> *doc_ids_;
    std::vector<uint8_t> scratch_;

private:
    int32_t cur_read_ = 0;
};

}