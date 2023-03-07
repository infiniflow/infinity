//
// Created by jinhai on 23-3-6.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class LogicalDropCollection final : public LogicalNode {
public:
    LogicalDropCollection(u64 node_id,
                     SharedPtr<String> schema_name,
                     SharedPtr<String> collection_name)
            : LogicalNode(node_id, LogicalNodeType::kDropTable),
              schema_name_(std::move(schema_name)),
              collection_name_(std::move(collection_name))
    {}

    [[nodiscard]] inline Vector<ColumnBinding>
    GetColumnBindings() const final {
        return {};
    }

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalDropCollection";
    }

    [[nodiscard]] SharedPtr<String>
    collection_name() const {
        return collection_name_;
    }

    [[nodiscard]] SharedPtr<String>
    schema_name() const {
        return schema_name_;
    }

private:
    SharedPtr<String> collection_name_{};
    SharedPtr<String> schema_name_{};
};

}
