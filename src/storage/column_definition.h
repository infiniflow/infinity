//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include <string>
#include <set>
#include "column_base.h"

namespace infinity {

enum class ConstrainType {
    kInvalid,
    kPrimaryKey,
    kUnique,
    kNull,
    kNotNull,
};


class ColumnDefinition : public ColumnBase {
public:
    ColumnDefinition(std::string column_name, uint64_t column_id, LogicalType logical_type, bool nullable, std::set<ConstrainType> constrains);

    [[nodiscard]] std::string ToString() const;

private:
    uint64_t column_id_;
    LogicalType logical_type_{LogicalTypeId::kInvalid, -1, -1, -1};
    bool nullable_{false};
    std::set<ConstrainType> constrains_;
};

struct TableColumnDefinition final {

};

}