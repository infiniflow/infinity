//
// Created by JinHai on 2022/9/12.
//

#pragma once

#include <vector>

namespace infinity {

enum class BoundNodeType {
    kInvalid,
    kSelect
};

class BoundNode {
public:
    explicit
    BoundNode(BoundNodeType type) : type_(type) {}

    virtual
    ~BoundNode() = default;

    virtual std::shared_ptr<LogicalNode>
    BuildPlan() = 0;

    std::vector<std::string> names;
    std::vector<DataType> types;

    virtual int64_t
    GetTableIndex() = 0;

protected:
    BoundNodeType type_{BoundNodeType::kInvalid};
};
}
