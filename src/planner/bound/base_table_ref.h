//
// Created by JinHai on 2022/9/12.
//

#pragma once

#include "table_ref.h"
#include "planner/node/logical_table_scan.h"

#include <utility>

namespace infinity {

class BaseTableRef: public TableRef {
public:
    explicit BaseTableRef(std::shared_ptr<LogicalTableScan>  logical_table_scan)
        : TableRef(TableRefType::kBaseTable), logical_table_scan_(std::move(logical_table_scan)) {}

    std::shared_ptr<LogicalTableScan> logical_table_scan_;
};

}