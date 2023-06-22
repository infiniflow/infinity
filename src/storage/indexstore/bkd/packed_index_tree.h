#pragma once

#include "index_tree.h"
#include "storage/io/file_reader.h"

#include <memory>
#include <vector>

namespace infinity {

class PackedIndexTree : public IndexTree {
public:
    explicit PackedIndexTree(std::shared_ptr<BKDReader>&& reader);

    std::shared_ptr<IndexTree> Clone() override;
    void PushLeft() override;
    void PushRight() override;
    void Pop() override;
    int64_t GetLeafBlockFP() override;
    std::shared_ptr<BytesRef> GetSplitDimValue() override;

private:
    void ReadNodeData(bool isLeft);

private:
    std::shared_ptr<FileReader> in_;
    std::vector<int64_t> leaf_block_fp_stack_;
    std::vector<int32_t> left_node_positions_;
    std::vector<int32_t> right_node_positions_;
    std::vector<int32_t> split_dims_;
    std::vector<bool> negative_deltas_;
    std::vector<std::shared_ptr<std::vector<uint8_t>>> split_values_stack_;
    std::shared_ptr<BytesRef> scratch_;
};

}