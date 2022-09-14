//
// Created by JinHai on 2022/9/14.
//

#pragma once
#include "function.h"
#include "storage/data_type.h"

namespace infinity {
class AggregateFunction : public Function {
public:
    explicit AggregateFunction(std::string name,
                               const std::vector<LogicalType>& argument_types,
                               LogicalType& return_type);

private:
    std::vector<LogicalType> argument_types_;
    LogicalType return_type_;

};
}

