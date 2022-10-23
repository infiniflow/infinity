//
// Created by JinHai on 2022/10/23.
//

#pragma once

#include <string>
#include <utility>
#include "data_type.h"

namespace infinity {

class ColumnBase {
public:
    ColumnBase(std::string column_name, LogicalType logical_type) : name_(std::move(column_name)), type_(logical_type) {}

    [[nodiscard]] std::string name() const { return name_; }
    [[nodiscard]] const LogicalType& logical_type() const { return type_; }

    std::string name_;
    LogicalType type_{LogicalTypeId::kInvalid, -1, -1, -1};
};

}

