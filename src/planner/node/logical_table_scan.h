//
// Created by JinHai on 2022/8/10.
//

#pragma once

#include "planner/logical_node.h"

#include "function/table/seq_scan.h"

#include "storage/table.h"

namespace infinity {

class LogicalTableScan : public LogicalNode {
public:
    explicit
    LogicalTableScan(u64 node_id,
                     TableCollectionEntry* table_ptr,
                     SharedPtr<SeqScanFunction> seq_scan_func,
                     String table_alias,
                     u64 table_index,
                     Vector<SizeT> column_ids,
                     SharedPtr<Vector<String>> column_names,
                     SharedPtr<Vector<SharedPtr<DataType>>> column_types,
                     SharedPtr<Vector<SegmentEntry*>> segment_entries);

    [[nodiscard]] Vector<ColumnBinding>
    GetColumnBindings() const final;

    [[nodiscard]] inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return column_names_;
    }

    [[nodiscard]] inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final {
        return column_types_;
    }

    [[nodiscard]] TableCollectionEntry*
    table_collection_ptr() const {
        return table_collection_ptr_;
    }

    String
    ToString(i64& space) final;

    inline String
    name() final {
        return "LogicalTableScan";
    }

    String table_alias_{};
    u64 table_index_{};
    Vector<SizeT> column_ids_{};

    SharedPtr<Vector<String>> column_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> column_types_{};
    SharedPtr<Vector<SegmentEntry*>> segment_entries_{};

    SharedPtr<SeqScanFunction> table_scan_func_ptr_{nullptr};
private:
    TableCollectionEntry* table_collection_ptr_{};

};

}

