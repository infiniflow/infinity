//
// Created by jinhai on 23-3-6.
//
#include "planner/logical_node.h"

namespace infinity {

class LogicalDropIndex final : public LogicalNode {
public:
    LogicalDropIndex(u64 node_id,
                     SharedPtr<String> schema_name,
                     SharedPtr<String> index_name)
            : LogicalNode(node_id, LogicalNodeType::kDropSchema),
              schema_name_(std::move(schema_name)),
              index_name_(std::move(index_name))
    {}

    inline Vector<ColumnBinding>
    GetColumnBindings() const final {
        return {};
    }

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalDropIndex";
    }

    [[nodiscard]] SharedPtr<String>
    schema_name() const {
        return schema_name_;
    }

    [[nodiscard]] SharedPtr<String>
    index_name() const {
        return index_name_;
    }
private:
    SharedPtr<String> schema_name_{};
    SharedPtr<String> index_name_{};
};

}
