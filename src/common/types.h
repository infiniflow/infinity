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

enum class OrderByType {
    kAscending,
    kDescending
    // Default order by type also should be given.
};

std::string
ToString(OrderByType type);

}
