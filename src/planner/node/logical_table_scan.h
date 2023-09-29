//
// Created by JinHai on 2022/8/10.
//

#pragma once

#include "planner/logical_node.h"

namespace infinity {

class BaseTableRef;

class LogicalTableScan : public LogicalNode {
public:
    explicit
    LogicalTableScan(u64 node_id,
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
        return "LogicalTableScan";
    }

    SharedPtr<BaseTableRef> base_table_ref_{};

};

}

