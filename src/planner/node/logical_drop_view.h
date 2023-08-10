//
// Created by jinhai on 23-3-6.
//


#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalDropView final : public LogicalNode {
public:
    LogicalDropView(u64 node_id,
                    SharedPtr<String> schema_name,
                    SharedPtr<String> view_name,
                    ConflictType conflict_type)
            : LogicalNode(node_id, LogicalNodeType::kDropView),
              schema_name_(std::move(schema_name)),
              view_name_(std::move(view_name)),
              conflict_type_(conflict_type)
    {}

    [[nodiscard]] inline Vector<ColumnBinding>
    GetColumnBindings() const final {
        return {};
    }

    [[nodiscard]] inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
        result->emplace_back("OK");
        return result;
    }

    [[nodiscard]] inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final {
        SharedPtr<Vector<SharedPtr<DataType>>> result_type = MakeShared<Vector<SharedPtr<DataType>>>();
        result_type->emplace_back(MakeShared<DataType>(LogicalType::kInteger));
        return result_type;
    }

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalDropView";
    }

    [[nodiscard]] SharedPtr<String>
    view_name() const {
        return view_name_;
    }

    [[nodiscard]] SharedPtr<String>
    schema_name() const {
        return schema_name_;
    }

    [[nodiscard]] inline ConflictType
    conflict_type() const {
        return conflict_type_;
    }

private:
    SharedPtr<String> view_name_{};
    SharedPtr<String> schema_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

}
