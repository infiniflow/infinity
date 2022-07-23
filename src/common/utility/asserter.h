//
// Created by JinHai on 2022/7/20.
//

#pragma once

#include "str.h"

namespace infinity {

#define Assert(is_true, message)                                              \
if(!(is_true)) {                                                            \
    std::string errmsg = infinity::TrimPath(__FILE__) + ":" + std::to_string(__LINE__) + ":" + (message); \
    throw std::logic_error(errmsg);                                       \
}

#define ResponseError(message) Assert(false, message)
}
