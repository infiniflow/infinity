//
// Created by JinHai on 2022/9/15.
//

#pragma once

#include <string>

namespace infinity {
// This class is used to indicate the expression execution state
class OperationState {
public:

    std::string operation_name_;
private:
};

class FunctionState : public OperationState {
public:

};

}

