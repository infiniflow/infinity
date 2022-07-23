//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include <string>
#include "data_type.h"

namespace infinity {


class ColumnDefinition {
public:
    ColumnDefinition(std::string column_name, LogicalType logical_type, bool nullable);

    std::string to_string() const;
private:
    std::string name_;
    LogicalType logical_type_{LogicalTypeId::kInvalid};
    bool nullable_{false};
};

struct TableColumnDefinition final {

};

}