#pragma once

#include "point_writer.h"
#include "bytesref.h"
#include "bit_set.h"
#include "storage/io/file_writer.h"
#include "storage/io/write_buffer.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <limits>

namespace infinity {

template<typename T>
using IntFunction = std::function<T(int)>;

class BKDWriter : public std::enable_shared_from_this<BKDWriter> {
public:
    static const int MAX_ARRAY_LENGTH = std::numeric_limits<std::int32_t>::max() - 1024;
    static const int MAX_DIMS = 8;
    static const std::wstring CODEC_NAME;
    static const int VERSION_START = 0;
    static const int VERSION_COMPRESSED_DOC_IDS = 1;
    static const int VERSION_COMPRESSED_VALUES = 2;
    static const int VERSION_IMPLICIT_SPLIT_DIM_1D = 3;
    static const int VERSION_PACKED_INDEX = 4;
    static const int VERSION_LEAF_STORES_BOUNDS = 5;
    static const int VERSION_SELECTIVE_INDEXING = 6;
    static const int VERSION_CURRENT = VERSION_SELECTIVE_INDEXING;

    // How many bytes each docs takes in the fixed-width offline format
    int32_t bytes_per_doc_;

    // How many dimensions we are storing at the leaf (data) nodes
    int32_t num_data_dims_;

    // How many dimensions we are indexing in the internal nodes
    int32_t num_index_dims_;

    // How many bytes each value in each dimension take
    int32_t bytes_per_dim_;

    // num_data_dims_ * bytes_per_dim_
    int32_t packed_bytes_length_;

    // num_index_dims_ * bytes_per_dim_
    int32_t packed_index_bytes_length_;

    // not used, copied from lucene, which is useful when OfflinePointWriter is necessary
    double max_mb_sort_in_heap_;

    int32_t max_depth_;

    int32_t max_points_in_leaf_node_;

    int32_t max_points_sort_in_heap_;

    // Minimum per-dim values, packed
    std::vector<uint8_t> min_packed_value_;

    // Maximum per-dim values, packed
    std::vector<uint8_t> max_packed_value_;

    int64_t point_count_ = 0;

    // true if we have so many values that we must write ords using long
    bool long_ords_;

    // An upper bound on how many points the caller will add (includes deletions)
    int64_t total_point_count_;

    // True if every document has at most one value.
    bool single_value_per_doc_;

    int32_t max_doc_;

    std::vector<uint8_t> scratch_diff_;

    std::vector<uint8_t> scratch1_;

    std::vector<uint8_t> scratch2_;

    BytesRef scratch_bytes_ref2 = BytesRef();

    std::vector<int32_t> common_prefix_lengths_;

    uint32_t docs_seen_;

    std::shared_ptr<PointWriter> point_writer_;

    // Sliced reference to points
    class PathSlice {
    public:
        PathSlice(
            std::shared_ptr<PointWriter> writer,
            int64_t start,
            int64_t count)
            :writer_(std::move(writer)),
             start_(start),
             count_(count) {}

    public:
        std::shared_ptr<PointWriter> writer_;
        int64_t start_;
        int64_t count_;
    };

public:
    BKDWriter(int32_t max_doc,
              int32_t num_data_dims,
              int32_t num_index_dims,
              int32_t bytes_per_dim,
              int32_t max_points_in_leaf_node,
              double max_points_sort_in_heap,
              int64_t total_point_count,
              bool single_value_per_doc,
              int32_t max_depth=8);

    BKDWriter(int32_t max_doc,
              int32_t num_data_dims,
              int32_t num_index_dims,
              int32_t bytes_per_dim,
              int32_t max_points_in_leaf_node,
              double max_points_sort_in_heap,
              int64_t total_point_count,
              bool single_value_per_doc,
              bool long_ords,
              int32_t max_depth=8);

    void Add(
        const uint8_t *packed_value,
        uint32_t value_len,
        int32_t doc_id);

    void Add(
        std::vector<uint8_t> &packed_value,
        int32_t docid);

    void MetaFinish(
        FileWriter *out,
        int64_t fPointer,
        int32_t type);

    int64_t Finish(
        FileWriter *out,
        FileWriter *index_out);
private:
    static void VerifyParams(
        int32_t num_data_dims,
        int32_t num_index_dims,
        int32_t max_points_in_leaf_node,
        double max_points_sort_in_heap,
        int64_t total_point_count);

    void WriteLeafBlockDocs(
        FileWriter *out,
        std::vector<int32_t> &docids,
        int32_t start,
        int32_t count);

    void WriteLeafBlockDocsBitmap(
        FileWriter *out,
        std::vector<int32_t> &docids,
        int32_t start,
        int32_t count);

    void WriteCommonPrefixes(
        FileWriter *out,
        std::vector<int32_t> &common_prefixes,
        std::vector<uint8_t> &packed_value) const;

    void WriteCommonPrefixes(
        FileWriter *out,
        std::vector<int32_t> &common_prefixes,
        std::vector<uint8_t> &packed_value,
        int offset) const;

    static int32_t AppendBlock(
        const std::shared_ptr<WriteBuffer>& write_buffer,
        ByteArrayList &blocks);

    static int64_t GetLeftMostLeafBlockFP(
        std::vector<int64_t> &leaf_block_fps,
        int32_t nodeID);

    int32_t RecursePackIndex(
        const std::shared_ptr<WriteBuffer> &write_buffer,
        std::vector<int64_t> &leaf_block_fps,
        std::vector<uint8_t> &split_packed_values,
        int64_t min_block_fp,
        ByteArrayList &blocks,
        int32_t nodeID,
        std::vector<uint8_t> &last_split_values,
        std::vector<bool> &negative_deltas,
        bool isLeft) const;

    std::shared_ptr<std::vector<uint8_t>>
    PackIndex(
      std::vector<int64_t> &leaf_block_fps,
      std::vector<uint8_t> &split_packed_values) const;

    void WriteIndex(
        FileWriter *out, 
        int32_t count_per_leaf, 
        std::vector<int64_t> &leaf_block_fps,
        std::vector<uint8_t> &split_packed_values);

    void WriteIndex(
        FileWriter *out,
        int32_t count_per_leaf,
        int32_t num_leaves,
        std::vector<uint8_t> &packed_index);

    static int32_t RunLen(
        const IntFunction<BytesRef *> &packed_values,
        int32_t start,
        int32_t end,
        int32_t byte_offset);

    void WriteLeafBlockPackedValuesRange(
        FileWriter *out,
        std::vector<int32_t> &common_prefix_lengths,
        int32_t start,
        int32_t end,
        const IntFunction<BytesRef *> &packed_values) const;

    void WriteLowCardinalityLeafBlockPackedValues(
        FileWriter *out,
        std::vector<int32_t> &common_prefix_lengths,
        int32_t count,
        const IntFunction<BytesRef *> &packed_values);

    void WriteHighCardinalityLeafBlockPackedValues(
        FileWriter *out,
        std::vector<int32_t> &common_prefix_lengths,
        int32_t count,
        int32_t sorted_dim,
        const IntFunction<BytesRef *> &packed_values,
        int32_t compressed_byte_offset) const;

    void WriteLeafBlockPackedValues(
        FileWriter *out,
        std::vector<int32_t> &common_prefix_lengths,
        int32_t count,
        int32_t sorted_dim,
        const IntFunction<BytesRef *> &packed_values,
        int32_t prefix_len_sum,
        bool low_cardinality);

    int32_t Split(
        std::vector<uint8_t> &min_packed_value,
        std::vector<uint8_t> &max_packed_value,
        std::vector<int32_t> &parent_splits);

    const uint8_t* MarkRightTree(
        int64_t right_count,
        int32_t split_dim,
        std::shared_ptr<PathSlice> &source,
        const std::shared_ptr<BitSet> &ord_bitset) const;

    std::shared_ptr<PointWriter>
    GetPointWriter(int64_t count, std::string &desc);

    bool LeafNodeIsLowCardinality(
        std::vector<int32_t> &common_prefix_lengths,
        int32_t count,
        int32_t sorted_dim,
        const IntFunction<BytesRef *> &packed_values,
        const std::vector<int> &leaf_cardinality,
        int32_t prefix_len_sum) const;

    void CreatePathSlice(
        std::vector<std::shared_ptr<PathSlice>> &slices,
        int32_t dim);

    std::shared_ptr<PointWriter> CreatePointWriterCopy(
        const std::shared_ptr<PointWriter> &writer,
        int64_t start,
        int64_t count);

    std::shared_ptr<PointWriter> Sort(
        int32_t dim,
        const std::shared_ptr<PointWriter> &writer,
        int64_t start,
        int64_t point_count);

    std::shared_ptr<PointWriter> Sort(int32_t dim);

    void SortPointWriter(
        std::shared_ptr<PointWriter> &writer,
        int32_t point_count,
        int32_t dim);

    std::shared_ptr<BKDWriter::PathSlice> SwitchToHeap(
        const std::shared_ptr<PathSlice> &source,
        const std::vector<std::shared_ptr<PointReader>> &to_close_heroically);

    void Build(
        int32_t nodeID,
        int32_t leaf_node_offset,
        std::vector<std::shared_ptr<PathSlice>> &slices,
        const std::shared_ptr<BitSet> &ord_bitset,
        FileWriter *out,
        std::vector<uint8_t> &min_packed_value,
        std::vector<uint8_t> &max_packed_value,
        std::vector<int32_t> &parent_splits,
        std::vector<uint8_t> &split_packed_values,
        std::vector<int64_t> &leaf_block_fps,
        const std::vector<std::shared_ptr<PointReader>> &to_close_heroically);
};

}