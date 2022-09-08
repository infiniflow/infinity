//
// Created by JinHai on 2022/7/23.
//

#pragma once

#include <string>
#include <set>
#include "data_type.h"

namespace infinity {

enum class ConstrainType {
    kInvalid,
    kPrimaryKey,
    kUnique,
    kNull,
    kNotNull,
};


class ColumnDefinition {
public:
    ColumnDefinition(std::string column_name, uint64_t column_id, LogicalType logical_type, bool nullable, std::set<ConstrainType> constrains);

    [[nodiscard]] std::string ToString() const;
    [[nodiscard]] std::string name() const { return name_; }
    [[nodiscard]] const LogicalType& logical_type() const { return logical_type_; }
private:
    std::string name_;
    uint64_t column_id_;
    LogicalType logical_type_{LogicalTypeId::kInvalid, -1, -1, -1};
    bool nullable_{false};
    std::set<ConstrainType> constrains_;
};

struct TableColumnDefinition final {

};

}