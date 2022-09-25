//
// Created by JinHai on 2022/9/13.
//

#pragma once

#include <string>

namespace infinity {

enum class JoinType {
    kInvalid,
    kInner,
    kLeft,
    kRight,
    kFull,
    kCross,
    kNatural
};

std::string
ToString(JoinType type);

}
