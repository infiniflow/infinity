#pragma once

#include "index_tree.h"

#include <memory>
#include <vector>

namespace infinity {
class LegacyIndexTree : public IndexTree {
public:
    explicit LegacyIndexTree(std::shared_ptr<BKDReader>&& reader);

    std::shared_ptr<IndexTree> Clone() override;
    void PushLeft() override;
    void PushRight() override;
    void Pop() override;
    int64_t GetLeafBlockFP() override;
    std::shared_ptr<BytesRef> GetSplitDimValue() override;

private:
    void SetNodeData();

private:
    int64_t leaf_block_fp_ = 0;
    std::shared_ptr<std::vector<uint8_t>> split_dim_value_;
    std::shared_ptr<BytesRef> scratch_{};
};

}