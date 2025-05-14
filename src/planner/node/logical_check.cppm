module;

export module logical_check;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import data_type;
import internal_types;
import check_statement;

namespace infinity {

export String ToString(CheckStmtType type);

export class LogicalCheck : public LogicalNode {
public:
    explicit LogicalCheck(u64 node_id, CheckStmtType type, Optional<String> schema_name = None, Optional<String> table_name = None)
        : LogicalNode(node_id, LogicalNodeType::kCheck), check_type_(type), schema_name_(std::move(schema_name)), table_name_(std::move(table_name)) {
    }

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalCheck"; }

    [[nodiscard]] CheckStmtType check_type() const { return check_type_; }

    [[nodiscard]] Optional<String> schema_name() const { return schema_name_; }

    [[nodiscard]] Optional<String> table_name() const { return table_name_; }

private:
    CheckStmtType check_type_{CheckStmtType::kInvalid};

    Optional<String> schema_name_;
    Optional<String> table_name_;
};

} // namespace infinity
