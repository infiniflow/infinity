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
    explicit BaseTableRef(std::shared_ptr<Table> table_ptr_)
        : TableRef(TableRefType::kBaseTable), table_ptr_(std::move(table_ptr_)) {}

    std::shared_ptr<Table> table_ptr_;
};

}