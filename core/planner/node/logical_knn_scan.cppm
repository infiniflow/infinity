//
// Created by jinhai on 23-9-21.
//

module;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import parser;

export module logical_knn_scan;

namespace infinity {

class BaseExpression;
class KnnExpression;
class BaseTableRef;
class TableCollectionEntry;

export class LogicalKnnScan : public LogicalNode {
public:
    explicit LogicalKnnScan(u64 node_id, SharedPtr<BaseTableRef> base_table_ref);

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    [[nodiscard]] TableCollectionEntry *table_collection_ptr() const;

    [[nodiscard]] String TableAlias() const;

    [[nodiscard]] u64 TableIndex() const;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalKnnScan"; }

    SharedPtr<BaseTableRef> base_table_ref_{};

    Vector<SharedPtr<BaseExpression>> knn_expressions_{};

    SharedPtr<BaseExpression> limit_expression_{};
    SharedPtr<BaseExpression> filter_expression_{};
    OrderType order_by_type_{OrderType::kAsc};

    u64 knn_table_index_{};
};

} // namespace infinity
