export module infinity_core:logical_check;

import :logical_node_type;
import :column_binding;
import :logical_node;

import data_type;
import internal_types;
import check_statement;

namespace infinity {

export std::string ToString(CheckStmtType type);

export class LogicalCheck : public LogicalNode {
public:
    explicit LogicalCheck(u64 node_id,
                          CheckStmtType type,
                          std::optional<std::string> schema_name = std::nullopt,
                          std::optional<std::string> table_name = std::nullopt)
        : LogicalNode(node_id, LogicalNodeType::kCheck), check_type_(type), schema_name_(std::move(schema_name)), table_name_(std::move(table_name)) {
    }

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalCheck"; }

    [[nodiscard]] CheckStmtType check_type() const { return check_type_; }

    [[nodiscard]] std::optional<std::string> schema_name() const { return schema_name_; }

    [[nodiscard]] std::optional<std::string> table_name() const { return table_name_; }

private:
    CheckStmtType check_type_{CheckStmtType::kInvalid};

    std::optional<std::string> schema_name_;
    std::optional<std::string> table_name_;
};

} // namespace infinity
