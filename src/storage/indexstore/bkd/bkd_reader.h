#pragma once


#include "storage/io/file_reader.h"

#include <memory>
#include <vector>

namespace infinity {
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

    int32_t GetTreeDepth() const;

    void ReadIndex(FileReader* index_in);

    int ReadMeta(FileReader* meta_in);

};
}
