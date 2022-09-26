//
// Created by JinHai on 2022/9/25.
//

#include "types.h"

namespace infinity {

std::string
ToString(JoinType type) {
    switch(type) {
        case JoinType::kInvalid: return "Invalid";
        case JoinType::kInner: return "Inner Join";
        case JoinType::kLeft: return "Left Join";
        case JoinType::kRight: return "Right Join";
        case JoinType::kFull: return "Full Join";
        case JoinType::kCross: return "Cross Product";
        case JoinType::kNatural: return "Natural Join";
    }
}

std::string
ToString(OrderByType type) {
    switch(type) {
        case OrderByType::kAscending: return "ASC";
        case OrderByType::kDescending: return "DESC";
    }
}

}