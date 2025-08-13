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
                           Optional<String> schema_name,
                           Optional<String> table_name,
                           SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kCheck, nullptr, nullptr, id, load_metas), check_type_(type), schema_name_(std::move(schema_name)),
          table_name_(std::move(table_name)) {}

    ~PhysicalCheck() override = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline CheckStmtType check_type() const { return check_type_; }

private:
    void ExecuteCheckSystem(QueryContext *query_context, CheckOperatorState *operator_state);

    void ExecuteCheckTable(QueryContext *query_context, CheckOperatorState *operator_state);

private:
    CheckStmtType check_type_{CheckStmtType::kInvalid};
    Optional<String> schema_name_;
    Optional<String> table_name_;

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
