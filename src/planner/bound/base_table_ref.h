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
    explicit BaseTableRef(SharedPtr<Table> table_ptr_,
                          std::string alias,
                          const Vector<std::string>& column_names,
                          const Vector<DataType>& column_types)
        : TableRef(TableRefType::kBaseTable, std::move(alias)),
        table_ptr_(std::move(table_ptr_)),
        column_names_(column_names),
        column_types_(column_types)
        {}

    SharedPtr<Table> table_ptr_;

    Vector<std::string> column_names_;
    Vector<DataType> column_types_;
};

}