//
// Created by JinHai on 2022/8/10.
//

module;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import parser;

export module logical_table_scan;

namespace infinity {

class BaseTableRef;
class TableCollectionEntry;

export class LogicalTableScan : public LogicalNode {
public:
    explicit LogicalTableScan(u64 node_id, SharedPtr<BaseTableRef> base_table_ref, bool add_row_id = false);

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    [[nodiscard]] TableCollectionEntry *table_collection_ptr() const;

    [[nodiscard]] String TableAlias() const;

    [[nodiscard]] u64 TableIndex() const;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalTableScan"; }

    SharedPtr<BaseTableRef> base_table_ref_{};

    bool add_row_id_;
};

} // namespace infinity
