#pragma once

#include "point_reader.h"
#include "bytesref.h"

#include <memory>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cassert>

namespace infinity {
//BKD Tree from Lucene 7.5
class PointWriter {
public:
    std::vector<int32_t> doc_IDs_;
    std::vector<int64_t> ords_long_;
    std::vector<int32_t> ords_;
    uint32_t next_write_;
    bool closed_;
    int32_t max_size_;

    uint32_t packed_bytes_length_;
    const int32_t values_per_block_;
    const bool single_value_per_doc_;
    ByteArrayList blocks_;
public:
    PointWriter(
        int32_t init_size,
        int32_t max_size,
        uint32_t packed_bytes_length,
        bool long_ords,
        bool single_value_per_doc);

    ~PointWriter();

    void ReadPackedValue(int32_t index, std::vector<uint8_t> &bytes);

    void GetPackedValueSlice(int32_t index, BytesRef& result);

    void WritePackedValue(int32_t index, std::shared_ptr<std::vector<uint8_t>> &bytes) {
        WritePackedValue(index, *bytes);
    };

    void WritePackedValue(int32_t index, const std::vector<uint8_t> &bytes);

    void WritePackedValue(int32_t index, const uint8_t* bytes, uint32_t length);

    std::vector<int> ComputeCardinality(int from, int to, int num_dims, int bytes_per_dim, std::vector<int32_t> &common_prefix_lengths);

    void Append(std::shared_ptr<std::vector<uint8_t>> &packedValue, int64_t ord, int32_t docid);

    void Append(const uint8_t* packedValue, uint32_t value_length, int64_t ord, int32_t docid);

    void Append(const std::vector<uint8_t> &packedValue, int64_t ord, int32_t docid);

    std::shared_ptr<PointReader> GetPointReader(int64_t start, int64_t length);

    std::shared_ptr<PointReader> GetPointReader(int64_t start, int64_t length, const std::vector<std::shared_ptr<PointReader>> &toCloseHeroically);

    void Close();

    void Destroy();

    std::string ToString();
};
}