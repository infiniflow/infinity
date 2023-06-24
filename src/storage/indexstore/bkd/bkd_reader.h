#pragma once


#include "storage/io/file_reader.h"
#include "index_tree.h"

#include <roaring/roaring.hh>
#include <memory>
#include <vector>
#include <string>

namespace infinity {

class BKDReader;

enum class BKDQueryType {
    EQUAL,
    LESS_THAN,
    LESS_EQUAL,
    GREATER_THAN,
    GREATER_EQUAL
};

enum class Relation {
    // Return this if the cell is fully contained by the query
    CELL_INSIDE_QUERY,
    // Return this if the cell and query do not overlap
    CELL_OUTSIDE_QUERY,
    // Return this if the cell partially overlaps the query
    CELL_CROSSES_QUERY
};

class BKDVisitor {
public:
    roaring::Roaring* hits_;
    uint32_t num_hits_;
    BKDReader* reader_;
    BKDQueryType query_type_;
    std::string query_min_;
    std::string query_max_;
public:
    BKDVisitor(roaring::Roaring* hits, BKDQueryType query_type);
    ~BKDVisitor() = default;
    void SetReader(BKDReader* reader);

    Relation Compare(
        std::vector<uint8_t>& min_packed,
        std::vector<uint8_t>& max_packed);

    bool Matches(uint8_t* packed_value);

    void Visit(int docid);
    void Visit(int docid, std::vector<uint8_t> &packed_value);
    void Visit(roaring::Roaring &docid);
    void Visit(roaring::Roaring *docid, std::vector<uint8_t> &packed_value);
};

struct IntersectState {
    IntersectState(
        FileReader *in,
        int32_t num_dims,
        int32_t packed_bytes_length,
        int32_t packed_index_bytes_length,
        int32_t max_points_in_leaf_node,
        BKDVisitor *visitor,
        const std::shared_ptr<IndexTree> &index_visitor);

    std::shared_ptr<FileReader> in_;
    std::unique_ptr<roaring::Roaring> doc_ids_;
    std::vector<int32_t> scratch_doc_ids_;
    std::vector<uint8_t> scratch_data_packed_value_;
    std::vector<uint8_t> scratch_min_index_packed_value_;
    std::vector<uint8_t> scratch_max_index_packed_value_;
    std::vector<int32_t> common_prefix_lengths_;

    BKDVisitor *visitor_;
    std::shared_ptr<IndexTree> index_;
};

class BKDReader : public std::enable_shared_from_this<BKDReader> {
public:
    int32_t leaf_node_offset_{};

    int32_t num_data_dims_{};

    int32_t num_index_dims_{};

    int32_t bytes_per_dim_{};

    int32_t num_leaves_{};

    std::unique_ptr<FileReader> in_;

    int32_t max_points_in_leaf_node_{};

    std::vector<uint8_t> min_packed_value_;

    std::vector<uint8_t> max_packed_value_;

    int64_t point_count_{};

    int32_t doc_count_{};

    int32_t version_{};

    std::shared_ptr<std::vector<uint8_t>> packed_index_;

    std::shared_ptr<FileReader> clone_index_input_;

    int32_t bytes_per_index_entry_{};

    std::vector<int64_t> leaf_block_fps_;

    int32_t packed_bytes_length_{};

    int32_t packed_index_bytes_length_{};

    std::vector<uint8_t> split_packed_values_;

    int32_t type_{};

    int64_t meta_offset_{};

    int64_t index_fp_{};

public:
    BKDReader() = default;

    explicit BKDReader(FileReader *in);

    std::shared_ptr<IntersectState> 
    GetIntersectState(BKDVisitor *visitor);

    int32_t GetTreeDepth() const;

    void ReadIndex(FileReader* index_in);

    int ReadMeta(FileReader* meta_in);

    void Intersect(BKDVisitor* visitor);

    void Intersect(
        const std::shared_ptr<IntersectState> &s, 
        std::vector<uint8_t> &cell_min_packed, 
        std::vector<uint8_t> &cell_max_packed);

    void ReadCommonPrefixes(
        std::vector<int32_t> &common_prefix_lengths, 
        std::vector<uint8_t> &scratch_packed_value, 
        FileReader *in) const;

    void AddAll(
        const std::shared_ptr<IntersectState> &state);

    void VisitDocValues(
        std::vector<int32_t> &common_prefix_lengths,
        std::vector<uint8_t> &scratch_data_packed_value,
        const std::vector<uint8_t> &scratch_min_index_packed_value,
        const std::vector<uint8_t> &scratch_max_index_packed_value,
        FileReader *in,
        roaring::Roaring *doc_ids,
        int32_t count,
        BKDVisitor *visitor);
};
}
