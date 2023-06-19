#include "point_writer.h"
#include "bkd_util.h"

#include <cstring>

namespace infinity {

PointWriter::PointWriter(
    int32_t init_size,
    int32_t max_size,
    uint32_t packed_bytes_length,
    bool long_ords,
    bool single_value_per_doc)
    :doc_IDs_(init_size, 0),
     next_write_(0),
     closed_(false),
     max_size_(max_size),
     packed_bytes_length_(packed_bytes_length),
     values_per_block_(std::max(1u, 4096 / packed_bytes_length)),
     single_value_per_doc_(single_value_per_doc) {
    if (!single_value_per_doc) {
        if (long_ords) {
            ords_long_ = std::vector<int64_t>(init_size, 0);
        } else {
            ords_ = std::vector<int32_t>(init_size, 0);
        }
    }
}

std::vector<int> PointWriter::ComputeCardinality(int from, int to, int num_dims, int bytes_per_dim, std::vector<int32_t> &common_prefix_lengths) {
    std::vector<int> leafCardinality(1,1);
    for (int i = from + 1; i < to; i++) {
        for (int dim = 0; dim < num_dims; dim++) {
            int start = dim * bytes_per_dim + common_prefix_lengths[dim];
            int end = dim * bytes_per_dim + bytes_per_dim;
            int32_t block1 = i / values_per_block_;
            int32_t blockIndex1 = i % values_per_block_;
            int32_t block2 = (i - 1) / values_per_block_;
            int32_t blockIndex2 = (i - 1) % values_per_block_;
            if (BKDUtil::Mismatch(blocks_[block1], blockIndex1 * packed_bytes_length_ + start, blockIndex1 * packed_bytes_length_ + end,
                                  blocks_[block2], blockIndex2 * packed_bytes_length_ + start, blockIndex2 * packed_bytes_length_ + end) != -1) {
                leafCardinality.push_back(1);
                break;
            } else {
                leafCardinality[leafCardinality.size() - 1]++;
            }
        }
    }
    return leafCardinality;
}

void PointWriter::ReadPackedValue(int32_t index, std::vector<uint8_t> &bytes) {
    assert(bytes.size() == packed_bytes_length_);
    int32_t block = index / values_per_block_;
    int32_t blockIndex = index % values_per_block_;
    auto start = blocks_.at(block).begin() + blockIndex * packed_bytes_length_;
    std::copy(start, start + packed_bytes_length_, bytes.begin());
}

void PointWriter::GetPackedValueSlice(int32_t index, BytesRef& result) {
    uint32_t block = index / values_per_block_;
    int blockIndex = index % values_per_block_;
    result.bytes_ = blocks_.at(block);
    result.offset_ = blockIndex * packed_bytes_length_;
    assert(result.length == static_cast<int32_t>(packed_bytes_length_));
}

void PointWriter::WritePackedValue(int32_t index, const uint8_t *bytes, uint32_t length) {
    assert(length == packed_bytes_length_);
    auto block = static_cast<uint32_t>(index / values_per_block_);
    int blockIndex = index % values_per_block_;
    while (blocks_.size() <= block) {
        int valuesInBlock = std::min(values_per_block_, (max_size_ - (static_cast<int32_t>(blocks_.size()) * values_per_block_)));
        blocks_.emplace_back(std::vector<uint8_t>(valuesInBlock * packed_bytes_length_));
    }
    memcpy(blocks_.at(block).data() + blockIndex * packed_bytes_length_, bytes, packed_bytes_length_);
}


void PointWriter::WritePackedValue(int32_t index, const std::vector<uint8_t> &bytes) {
    assert(bytes.size() == packed_bytes_length_);
    uint32_t block = static_cast<uint32_t>(index / values_per_block_);
    int blockIndex = index % values_per_block_;
    while (blocks_.size() <= block) {
        int valuesInBlock = std::min(values_per_block_, (max_size_ - (static_cast<int32_t>(blocks_.size()) * values_per_block_)));
        blocks_.emplace_back(std::vector<uint8_t>(valuesInBlock * packed_bytes_length_));
    }
    std::copy(bytes.begin(),
              bytes.begin() + packed_bytes_length_,
              blocks_.at(block).begin() + blockIndex * packed_bytes_length_);
}

void PointWriter::Append(const std::vector<uint8_t> &packedValue, int64_t ord, int32_t docid) {
    assert(closed_ == false);
    assert(packedValue.size() == packed_bytes_length_);
    while (doc_IDs_.size() < (next_write_ + 1)) {
        doc_IDs_.push_back(0);
        if (!single_value_per_doc_) {
            if (ords_long_.size() != 0) {
                ords_long_.push_back(0L);
            } else {
                ords_.push_back(0);
            }
        }
    }

    WritePackedValue(next_write_, packedValue);
    if (!single_value_per_doc_) {
        if (ords_long_.size() != 0) {
            ords_long_[next_write_] = ord;
        } else {
            assert(ord <= 0x7fffffff);
            ords_[next_write_] = (int) ord;
        }
    }
    doc_IDs_[next_write_] = docid;
    next_write_++;
}

void PointWriter::Append(const uint8_t *packedValue, uint32_t value_length, int64_t ord, int32_t docid) {
    assert(closed_ == false);
    assert(value_length == packed_bytes_length_);
    while (doc_IDs_.size() < (next_write_ + 1)) {
        doc_IDs_.push_back(0);
        if (!single_value_per_doc_) {
            if (ords_long_.size() != 0) {
                ords_long_.push_back(0l);
            } else {
                ords_.push_back(0);
            }
        }
    }

    WritePackedValue(next_write_, packedValue, value_length);
    if (!single_value_per_doc_) {
        if (ords_long_.size() != 0) {
            ords_long_[next_write_] = ord;
        } else {
            assert(ord <= 0x7fffffff);
            ords_[next_write_] = (int) ord;
        }
    }
    doc_IDs_[next_write_] = docid;
    next_write_++;
}

void PointWriter::Append(std::shared_ptr<std::vector<uint8_t>> &packedValue, int64_t ord, int32_t docid) {
    assert(closed_ == false);
    assert(packedValue->size() == packed_bytes_length_);
    while (doc_IDs_.size() < (next_write_ + 1)) {
        doc_IDs_.push_back(0);
        if (!single_value_per_doc_) {
            if (ords_long_.size() != 0) {
                ords_long_.push_back(0l);
            } else {
                ords_.push_back(0);
            }
        }
    }

    WritePackedValue(next_write_, packedValue);
    if (!single_value_per_doc_) {
        if (ords_long_.size() != 0) {
            ords_long_[next_write_] = ord;
        } else {
            assert(ord <= 0x7fffffff);
            ords_[next_write_] = (int) ord;
        }
    }
    doc_IDs_[next_write_] = docid;
    next_write_++;
}

std::shared_ptr<PointReader> PointWriter::GetPointReader(int64_t start, int64_t length) {
    assert((start + length) <= static_cast<int32_t>(doc_IDs_.size()));
    assert((start + length) <= next_write_);
    return std::make_shared<PointReader>(&blocks_, values_per_block_, packed_bytes_length_,
                                         ords_, ords_long_, &doc_IDs_, (int32_t) start,
                                         static_cast<int32_t>(start + length), single_value_per_doc_);
}

std::shared_ptr<PointReader> PointWriter::GetPointReader(int64_t start, int64_t length,
        const std::vector<std::shared_ptr<PointReader>> &toCloseHeroically) {
    return std::make_shared<PointReader>(&blocks_, values_per_block_, packed_bytes_length_, ords_, ords_long_, &doc_IDs_,
                                         (int32_t) start, next_write_, single_value_per_doc_);
}

void PointWriter::Close() {
    closed_ = true;
}

void PointWriter::Destroy() {
}

std::string PointWriter::ToString() {
    return "PointWriter(count=" + std::to_string(next_write_) + " alloc=" + std::to_string(doc_IDs_.size()) + ")";
}

PointWriter::~PointWriter() {
    Close();
}

}

