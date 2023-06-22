#pragma once

#include "bytesref.h"

#include <memory>
#include <vector>

namespace infinity{

class BKDReader;
class IndexTree {
protected:
    explicit IndexTree(std::shared_ptr<BKDReader>& reader);

public:
    virtual ~IndexTree() = default;
    virtual void PushLeft();
    virtual void PushRight();
    virtual void Pop();
    virtual bool IsLeafNode();
    virtual bool NodeExists();
    virtual int32_t GetNodeId();
    virtual std::vector<uint8_t>&GetSplitPackedValue();
    virtual int32_t GetSplitDim();
    virtual int32_t GetNumLeaves();
    virtual std::shared_ptr<IndexTree> Clone() = 0;
    virtual std::shared_ptr<BytesRef> GetSplitDimValue() = 0;
    virtual int64_t GetLeafBlockFP() = 0;
private:
    int32_t GetNumLeavesSlow(int32_t node);

protected:
    int32_t node_id_ = 0;
    int32_t level_ = 0;
    int32_t split_dim_ = 0;
    std::vector<std::vector<uint8_t>> split_packed_value_stack_;

protected:
    std::shared_ptr<BKDReader> reader_;
};

}