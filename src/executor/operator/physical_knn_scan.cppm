//
// Created by jinhai on 23-9-22.
//

module;

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;
import global_block_id;
import base_table_ref;
import table_collection_entry;
import block_index;

export module physical_knn_scan;

namespace infinity {

export class PhysicalKnnScan final : public PhysicalOperator {
public:
    explicit PhysicalKnnScan(u64 id,
                             SharedPtr<BaseTableRef> base_table_ref,
                             Vector<SharedPtr<BaseExpression>> knn_expressions,
                             SharedPtr<BaseExpression> limit_expression,
                             SharedPtr<BaseExpression> filter_expression,
                             OrderType order_by_type,
                             SharedPtr<Vector<String>> output_names,
                             SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                             u64 knn_table_index)
        : PhysicalOperator(PhysicalOperatorType::kKnnScan, nullptr, nullptr, id), base_table_ref_(Move(base_table_ref)),
          knn_expressions_(Move(knn_expressions)), limit_expression_(Move(limit_expression)),
          filter_expression_(Move(filter_expression)), order_by_type_(order_by_type), output_names_(Move(output_names)),
          output_types_(Move(output_types)), knn_table_index_(knn_table_index) {}

    ~PhysicalKnnScan() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    [[nodiscard]] TableCollectionEntry *table_collection_ptr() const;

    [[nodiscard]] String TableAlias() const;

    BlockIndex *GetBlockIndex() const;

    Vector<SizeT> &ColumnIDs() const;

    Vector<SharedPtr<Vector<GlobalBlockID>>> PlanBlockEntries(i64 parallel_count) const;

    SizeT BlockEntryCount() const;

public:
    SharedPtr<BaseTableRef> base_table_ref_{};

    Vector<SharedPtr<BaseExpression>> knn_expressions_{};

    SharedPtr<BaseExpression> limit_expression_{};
    SharedPtr<BaseExpression> filter_expression_{};
    OrderType order_by_type_{OrderType::kAsc};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
    u64 knn_table_index_{};

private:
    template <typename T>
    void ExecuteInternal(QueryContext *query_context, KnnScanInputState *input_state, KnnScanOutputState *output_state);
};

} // namespace infinity
