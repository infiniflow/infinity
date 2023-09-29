//
// Created by jinhai on 23-3-6.
//

#pragma once

#include <utility>

#include "planner/logical_node.h"

namespace infinity {

class LogicalCreateSchema : public LogicalNode {
public:
    static inline SharedPtr<LogicalCreateSchema>
    Make(u64 node_id,
         const SharedPtr<String>& schema_name,
         ConflictType conflict_type) {
        return MakeShared<LogicalCreateSchema>(node_id, schema_name, conflict_type);
    }

public:
    LogicalCreateSchema(u64 node_id,
                        SharedPtr<String> schema_name,
                        ConflictType conflict_type)
            : LogicalNode(node_id, LogicalNodeType::kCreateSchema),
              schema_name_(std::move(schema_name)),
              conflict_type_(conflict_type) {}

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
        return "LogicalCreateSchema";
    }

    [[nodiscard]] inline SharedPtr<String>
    schema_name() const {
        return schema_name_;
    }

    [[nodiscard]] inline ConflictType
    conflict_type() const {
        return conflict_type_;
    }

private:
    SharedPtr<String> schema_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

}

