//
// Created by JinHai on 2022/9/15.
//

#pragma once

#include "common/types/internal_types.h"

namespace infinity {

enum class OperatorStateType {
    kInvalid,
    kDDL,
};

struct InputState {
    inline explicit
    InputState(OperatorStateType state_type) : state_type_(state_type) {}

    OperatorStateType state_type_{OperatorStateType::kInvalid};
};

struct OutputState {
    inline explicit
    OutputState(OperatorStateType state_type) : state_type_(state_type) {}

    OperatorStateType state_type_{OperatorStateType::kInvalid};
};

struct DDLInputState : public InputState {
    inline explicit
    DDLInputState(): InputState(OperatorStateType::kDDL) {}
};

struct DDLOutputState : public OutputState {
    inline explicit
    DDLOutputState(): OutputState(OperatorStateType::kDDL) {}

    UniquePtr<String> error_message_;
};

}

