#include "point_reader.h"
#include "point_writer.h"
#include "common/utility/infinity_assert.h"

#include <cassert>

namespace infinity {

PointReader::PointReader(
    ByteArrayList* blocks,
    int32_t values_per_block,
    int32_t packed_bytes_length,
    const std::vector<int32_t> &ords,
    const std::vector<int64_t> &ords_long,
    std::vector<int32_t> *docids,
    int32_t start,
    int32_t end,
    bool single_value_per_doc) {
    blocks_ = blocks;
    values_per_block_ = values_per_block;
    single_value_per_doc_ = single_value_per_doc;
    ords_ = ords;
    ords_long_ = ords_long;
    doc_ids_ = docids;
    cur_read_ = start - 1;
    end_ = end;
    packed_bytes_length_ = packed_bytes_length;
    scratch_ = std::vector<uint8_t>(packed_bytes_length);
}

void PointReader::ReadPackedValue(int32_t index, std::vector<uint8_t> &bytes) {
    int32_t block = index / values_per_block_;
    int32_t blockIndex = index % values_per_block_;
    std::copy((*blocks_)[block].begin() + blockIndex * packed_bytes_length_,
              (*blocks_)[block].begin() + blockIndex * packed_bytes_length_ + packed_bytes_length_,
              bytes.begin());
}

const std::vector<uint8_t> &PointReader::PackedValue() {
    ReadPackedValue(cur_read_, scratch_);
    return scratch_;
}

uint8_t* PointReader::PackedValueRaw() {
    int32_t block = cur_read_ / values_per_block_;
    int32_t blockIndex = cur_read_ % values_per_block_;
    return (*blocks_)[block].data() + blockIndex * packed_bytes_length_;
}

void PointReader::MarkOrds(int64_t count, const std::shared_ptr<BitSet> &ord_bitset) {
    for (int32_t i = 0; i < count; i++) {
        bool result = Next();
        StorageAssert(result, "did not see enough points from reader");
        assert(ord_bitset->Get(Ord()) == false);
        ord_bitset->Set(Ord());
    }
}

int64_t PointReader::Split(int64_t count,
                           const std::shared_ptr<BitSet> &right_tree,
                           const std::shared_ptr<PointWriter> &left,
                           const std::shared_ptr<PointWriter> &right,
                           bool do_clear_bits) {
    int64_t right_count = 0;
    for (int64_t i = 0; i < count; i++) {
        bool result = Next();
        assert(result);
        const std::vector<uint8_t> &packed_value = PackedValue();
        int64_t ordinal = Ord();
        int32_t doc_id = Docid();
        if (right_tree->Get(ordinal)) {
            right->Append(packed_value, ordinal, doc_id);
            right_count++;
            if (do_clear_bits) {
                right_tree->Clear(ordinal);
            }
        } else {
            left->Append(packed_value, ordinal, doc_id);
        }
    }

    return right_count;
}

}