//
// Created by JinHai on 2022/7/23.
//

#include "column_definition.h"
#include <sstream>
#include <utility>

namespace infinity {


ColumnDefinition::ColumnDefinition(
        std::string column_name,
        uint64_t column_id,
        infinity::LogicalType logical_type,
        bool nullable,
        std::set<ConstrainType> constrains)
    : name_(std::move(column_name)),
    column_id_(column_id),
    logical_type_(logical_type),
    nullable_(nullable),
    constrains_(std::move(constrains)) {}

std::string ColumnDefinition::ToString() const {
    std::stringstream ss;
    ss << name_ << " " << logical_type_.ToString() << " " << (nullable_ ? "null" : "not null") << std::endl;
    return ss.str();
}

}
