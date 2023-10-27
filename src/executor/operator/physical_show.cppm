//
// Created by JinHai on 2022/9/8.
//

module;

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import logical_show;

export module physical_show;

namespace infinity {

export class PhysicalShow : public PhysicalOperator {
public:
    explicit PhysicalShow(u64 id, ShowType type, String db_name, String object_name, u64 table_index)
        : PhysicalOperator(PhysicalOperatorType::kShow, nullptr, nullptr, id), scan_type_(type), db_name_(Move(db_name)),
          object_name_(Move(object_name)), table_index_(table_index) {}

    ~PhysicalShow() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline ShowType scan_type() const { return scan_type_; }

    inline const String &db_name() const { return db_name_; };

    inline const String &object_name() const { return object_name_; };

private:
    void ExecuteShowTable(QueryContext *query_context);

    void ExecuteShowViews(QueryContext *query_context);

    void ExecuteShowColumns(QueryContext *query_context);

    void ExecuteShowTableDetail(QueryContext *query_context, const Vector<SharedPtr<ColumnDef>> &table_columns);

    void ExecuteShowViewDetail(QueryContext *query_context,
                               const SharedPtr<Vector<SharedPtr<DataType>>> &column_types,
                               const SharedPtr<Vector<String>> &column_names);

    /// Execute push based show table
    void ExecuteShowTable(QueryContext *query_context, ShowInputState *input_state, ShowOutputState *output_state);

    /// Execute push based describe table
    void ExecuteShowColumns(QueryContext *query_context, ShowInputState *input_state, ShowOutputState *output_state);

    void ExecuteShowIndexes(QueryContext *query_context, ShowInputState *input_state, ShowOutputState *output_state);

private:
    ShowType scan_type_{ShowType::kInvalid};
    String db_name_{};
    String object_name_{};
    u64 table_index_{};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
