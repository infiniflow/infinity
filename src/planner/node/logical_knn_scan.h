//
// Created by jinhai on 23-9-21.
//


#pragma once

#include "planner/logical_node.h"
#include "parser/parsed_expr/knn_expr.h"

namespace infinity {

class KnnExpression;

class BaseTableRef;

class LogicalKnnScan : public LogicalNode {
public:
    explicit
    LogicalKnnScan(u64 node_id,
                   SharedPtr<BaseTableRef> base_table_ref);

    [[nodiscard]] Vector<ColumnBinding>
    GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>>
    GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final;

    [[nodiscard]] TableCollectionEntry*
    table_collection_ptr() const;

    [[nodiscard]] String
    TableAlias() const;

    [[nodiscard]] u64
    TableIndex() const;

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalKnnScan";
    }

    SharedPtr<BaseTableRef> base_table_ref_{};

    Vector<SharedPtr<BaseExpression>> knn_expressions_{};

    SharedPtr<BaseExpression> limit_expression_{};
    SharedPtr<BaseExpression> filter_expression_{};
    OrderType order_by_type_{OrderType::kAsc};

    u64 knn_table_index_{};
};

}

