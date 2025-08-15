module;

export module infinity_core:physical_check;

import :stl;
import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :base_expression;
import :logical_show;
import :load_meta;
import :infinity_exception;
import :variables;
import :data_block;
import :logger;

import internal_types;
import column_def;
import data_type;
import check_statement;

namespace infinity {

export class PhysicalCheck : public PhysicalOperator {
public:
    explicit PhysicalCheck(u64 id,
                           CheckStmtType type,
                           std::optional<std::string> schema_name,
                           std::optional<std::string> table_name,
                           std::shared_ptr<std::vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kCheck, nullptr, nullptr, id, load_metas), check_type_(type), schema_name_(std::move(schema_name)),
          table_name_(std::move(table_name)) {}

    ~PhysicalCheck() override = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *output_state) final;

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return output_names_; }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline CheckStmtType check_type() const { return check_type_; }

private:
    void ExecuteCheckSystem(QueryContext *query_context, CheckOperatorState *operator_state);

    void ExecuteCheckTable(QueryContext *query_context, CheckOperatorState *operator_state);

private:
    CheckStmtType check_type_{CheckStmtType::kInvalid};
    std::optional<std::string> schema_name_;
    std::optional<std::string> table_name_;

    std::shared_ptr<std::vector<std::string>> output_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};
};

} // namespace infinity
