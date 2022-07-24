//
// Created by JinHai on 2022/7/23.
//

#include "column_definition.h"
#include <sstream>
#include <utility>

namespace infinity {


ColumnDefinition::ColumnDefinition(std::string column_name, infinity::LogicalType logical_type, bool nullable, std::vector<ConstrainType> constrains)
    : name_(std::move(column_name)), logical_type_(logical_type), nullable_(nullable), constrains_(std::move(constrains)) {}

std::string ColumnDefinition::to_string() const {
    std::stringstream ss;
    ss << name_ << " " << logical_type_.to_string() << " " << (nullable_ ? "null" : "not null") << std::endl;
    return ss.str();
}

}
