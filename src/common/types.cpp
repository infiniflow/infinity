//
// Created by JinHai on 2022/9/25.
//

#include "types.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

String
ToString(JoinType type) {
    switch(type) {
        case JoinType::kInner: return "Inner Join";
        case JoinType::kLeft: return "Left Join";
        case JoinType::kRight: return "Right Join";
        case JoinType::kFull: return "Full Join";
        case JoinType::kCross: return "Cross Product";
        case JoinType::kNatural: return "Natural Join";
        case JoinType::kSemi: return "Semi Join";
        case JoinType::kMark: return "Mark Join";
        case JoinType::kAnti: return "Anti Join";
    }
    PlannerError("Unreachable");
}

String
ToString(OrderType type) {
    switch(type) {
        case OrderType::kAsc: return "ASC";
        case OrderType::kDesc: return "DESC";
    }
    TypeError("Unexpected order type");
}

}