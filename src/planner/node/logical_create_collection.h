//
// Created by jinhai on 23-3-6.
//
#pragma once

#include <utility>

#include "planner/logical_node.h"

namespace infinity {

class LogicalCreateCollection : public LogicalNode {
public:
    static inline SharedPtr<LogicalCreateCollection>
    Make(u64 node_id,
         const SharedPtr<String>& schema_name,
         const SharedPtr<String>& collection_name,
         u64 table_index,
         ConflictType conflict_type) {
        return MakeShared<LogicalCreateCollection>(node_id, schema_name, collection_name, table_index, conflict_type);
    }

public:
    LogicalCreateCollection(u64 node_id,
                            SharedPtr<String> schema_name,
                            SharedPtr<String> collection_name_,
                            u64 table_index,
                            ConflictType conflict_type)
            : LogicalNode(node_id, LogicalNodeType::kCreateCollection),
              schema_name_(std::move(schema_name)),
              collection_name_(std::move(collection_name_)),
              table_index_(table_index),
              conflict_type_(conflict_type)
    {}

    [[nodiscard]] inline Vector<ColumnBinding>
    GetColumnBindings() const final {
        return {};
    }

    [[nodiscard]] inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return MakeShared<Vector<String>>();
    }

    [[nodiscard]] inline SharedPtr<Vector<DataType>>
    GetOutputTypes() const final {
        return MakeShared<Vector<DataType>>();
    }

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalCreateCollection";
    }

    [[nodiscard]] inline SharedPtr<String>
    collection_name() const {
        return collection_name_;
    }

    [[nodiscard]] inline SharedPtr<String>
    schema_name() const {
        return schema_name_;
    }

    [[nodiscard]] inline u64
    table_index() const {
        return table_index_;
    }

    [[nodiscard]] inline ConflictType
    conflict_type() const {
        return conflict_type_;
    }

private:
    SharedPtr<String> schema_name_{};
    SharedPtr<String> collection_name_{};
    u64 table_index_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

}

