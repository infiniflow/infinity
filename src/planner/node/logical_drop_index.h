//
// Created by jinhai on 23-3-6.
//
#include "parser/statement/extra/extra_ddl_info.h"
#include "planner/logical_node.h"

namespace infinity {

class LogicalDropIndex final : public LogicalNode {
public:
    LogicalDropIndex(u64 node_id,
                     SharedPtr<String> schema_name,
                     SharedPtr<String> table_name,
                     SharedPtr<String> index_name,
                     ConflictType conflict_type)
        : LogicalNode(node_id, LogicalNodeType::kDropIndex), schema_name_(std::move(schema_name)), table_name_(std::move(table_name)),
          index_name_(std::move(index_name)), conflict_type_(conflict_type) {}

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalDropIndex"; }

    [[nodiscard]] SharedPtr<String> schema_name() const { return schema_name_; }

    [[nodiscard]] SharedPtr<String> table_name() const { return table_name_; }

    [[nodiscard]] SharedPtr<String> index_name() const { return index_name_; }

    [[nodiscard]] inline ConflictType conflict_type() const { return conflict_type_; }

private:
    SharedPtr<String> schema_name_{};
    SharedPtr<String> table_name_{};
    SharedPtr<String> index_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

} // namespace infinity
