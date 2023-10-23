//
// Created by jinhai on 23-10-1.
//

module;

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import table;

export module physical_merge_knn;

namespace infinity {

export class PhysicalMergeKnn final : public PhysicalOperator {
public:
    explicit PhysicalMergeKnn(u64 id,
                              SharedPtr<PhysicalOperator> left,
                              SharedPtr<Vector<String>> output_names,
                              SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                              Vector<SharedPtr<BaseExpression>> knn_exprs,
                              SharedPtr<BaseExpression> limit_expr,
                              OrderType order_by_type,
                              u64 knn_table_index)
        : PhysicalOperator(PhysicalOperatorType::kMergeKnn, Move(left), nullptr, id), output_names_(Move(output_names)),
          output_types_(Move(output_types)), knn_expressions_(Move(knn_exprs)), limit_expression_(Move(limit_expr)),
          order_by_type_(order_by_type), knn_table_index_(knn_table_index) {}

    ~PhysicalMergeKnn() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline u64 knn_table_index() const { return knn_table_index_; }

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
    u64 knn_table_index_{};

    Vector<SharedPtr<BaseExpression>> knn_expressions_{};
    SharedPtr<BaseExpression> limit_expression_{};
    OrderType order_by_type_{OrderType::kAsc};
};

} // namespace infinity
