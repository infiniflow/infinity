//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"
#include "function/table/table_scan.h"

#include <utility>

namespace infinity {

class PhysicalTableScan : public PhysicalOperator {
public:
    explicit PhysicalTableScan(uint64_t id,
                               String table_alias,
                               u64 table_index,
                               Vector<String> column_names,
                               Vector<DataType> column_types,
                               SharedPtr<TableScanFunction> table_scan_function_ptr,
                               SharedPtr<TableScanFunctionData> table_scan_function_data_ptr)
        : PhysicalOperator(PhysicalOperatorType::kTableScan, nullptr, nullptr,id),
          table_alias_(std::move(table_alias)),
          table_index_(table_index),
          column_names_(std::move(column_names)),
          column_types_(std::move(column_types)),
          table_scan_func_ptr_(std::move(table_scan_function_ptr)),
          table_scan_function_data_ptr_(std::move(table_scan_function_data_ptr))
          {}

    ~PhysicalTableScan() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

    SharedPtr<Vector<String>>
    GetOutputNames() const final;

    inline String
    table_alias() const {
        return table_alias_;
    }

    inline const Vector<String>&
    column_names() const {
        return column_names_;
    }

    inline const Vector<DataType>&
    column_types() const {
        return column_types_;
    }

    inline const SharedPtr<TableScanFunctionData>&
    function_data() const {
        return table_scan_function_data_ptr_;
    }

    inline u64
    TableIndex() const {
        return table_index_;
    }

private:
    String table_alias_{};
    u64 table_index_{};
    Vector<String> column_names_{};
    Vector<DataType> column_types_{};
    SharedPtr<TableScanFunction> table_scan_func_ptr_{nullptr};
    SharedPtr<TableScanFunctionData> table_scan_function_data_ptr_{};
};


}
