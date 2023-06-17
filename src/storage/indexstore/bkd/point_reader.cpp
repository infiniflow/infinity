#include "point_reader.h"

namespace infinity {

PointReader::PointReader(ByteArrayList* blocks,
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

}