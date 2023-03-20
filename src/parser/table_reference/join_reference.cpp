//
// Created by jinhai on 23-2-28.
//

#include "join_reference.h"
#include "cross_product_reference.h"

namespace infinity {

JoinReference::~JoinReference() {
    if(left_ != nullptr) {
        delete(left_);
        left_ = nullptr;
    }
    if(right_ != nullptr) {
        delete(right_);
        right_ = nullptr;
    }

    if(condition_ != nullptr) {
        delete(condition_);
        condition_ = nullptr;
    }

    if(using_columns_ != nullptr) {
        for(auto column_ptr: *using_columns_) {
            free(column_ptr);
        }
        delete using_columns_;
    }

}

String
JoinReference::ToString() {
    return "Join";
}

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

};
