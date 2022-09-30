//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "function/aggregate_function.h"

namespace infinity {

class Count : public AggregateFunction {
public:

    [[nodiscard]] std::string
    ToString() const override;
private:

};

}