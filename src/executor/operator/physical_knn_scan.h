//
// Created by jinhai on 23-9-22.
//

#pragma once

#include "executor/physical_operator.h"

#include <memory>

namespace infinity {

class BaseTableRef;
class BaseExpression;
class PhysicalKnnScan final : public PhysicalOperator {
public:
    explicit
    PhysicalKnnScan(u64 id,
                    SharedPtr<BaseTableRef> base_table_ref,
                    Vector<SharedPtr<BaseExpression>> knn_expressions,
                    SharedPtr<BaseExpression> limit_expression,
                    SharedPtr<BaseExpression> filter_expression,
                    OrderType order_by_type,
                    SharedPtr<Vector<String>> output_names,
                    SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                    u64 knn_table_index)
            : PhysicalOperator(PhysicalOperatorType::kKnnScan, nullptr, nullptr, id),
            base_table_ref_(std::move(base_table_ref)),
            knn_expressions_(std::move(knn_expressions)),
            limit_expression_(std::move(limit_expression)),
            filter_expression_(std::move(filter_expression)),
            order_by_type_(order_by_type),
            output_names_(std::move(output_names)),
            output_types_(std::move(output_types)),
            knn_table_index_(knn_table_index)
            {}

    ~PhysicalKnnScan() override = default;

    void
    Init() override;

    void
    Execute(QueryContext* query_context) final;

    virtual void
    Execute(QueryContext* query_context, InputState* input_state, OutputState* output_state) final;

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return output_names_;
    }

    inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final {
        return output_types_;
    }

    [[nodiscard]] TableCollectionEntry*
    table_collection_ptr() const;

    [[nodiscard]] String
    TableAlias() const;

    Vector<SegmentEntry*>*
    SegmentEntriesPtr() const;

    Vector<SizeT>&
    ColumnIDs() const;

    Vector<SharedPtr<Vector<u64>>>
    PlanSegmentEntries(i64 parallel_count) const;

    SizeT
    SegmentEntryCount() const;

public:
    SharedPtr<BaseTableRef> base_table_ref_{};

    Vector<SharedPtr<BaseExpression>> knn_expressions_{};

    SharedPtr<BaseExpression> limit_expression_{};
    SharedPtr<BaseExpression> filter_expression_{};
    OrderType order_by_type_{OrderType::kAsc};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
    u64 knn_table_index_{};
};

}
