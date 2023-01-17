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

    virtual SharedPtr<LogicalNode>
    BuildPlan() = 0;

    std::vector<String> names;
    std::vector<DataType> types;

    virtual i64
    GetTableIndex() = 0;

protected:
    BoundNodeType type_{BoundNodeType::kInvalid};
};
}
