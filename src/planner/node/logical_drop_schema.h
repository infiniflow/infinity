//
// Created by jinhai on 23-3-6.
//

#include "planner/logical_node.h"

namespace infinity {

class LogicalDropSchema final : public LogicalNode {
public:
    LogicalDropSchema(u64 node_id,
                      SharedPtr<String> schema_name)
            : LogicalNode(node_id, LogicalNodeType::kDropSchema),
              schema_name_(std::move(schema_name))
    {}

    [[nodiscard]] inline Vector<ColumnBinding>
    GetColumnBindings() const final {
        return {};
    }

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalDropSchema";
    }

    [[nodiscard]] SharedPtr<String>
    schema_name() const {
        return schema_name_;
    }

private:
    SharedPtr<String> schema_name_{};
};

}
