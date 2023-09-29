//
// Created by jinhai on 23-2-28.
//

#include "join_reference.h"
#include "cross_product_reference.h"

namespace infinity {

JoinReference::~JoinReference() {
    if(left_ != nullptr) {
        delete (left_);
        left_ = nullptr;
    }
    if(right_ != nullptr) {
        delete (right_);
        right_ = nullptr;
    }

    if(condition_ != nullptr) {
        delete (condition_);
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
        case JoinType::kInner:
            return "INNER JOIN";
        case JoinType::kLeft:
            return "LEFT JOIN";
        case JoinType::kRight:
            return "RIGHT JOIN";
        case JoinType::kFull:
            return "FULL JOIN";
        case JoinType::kCross:
            return "CROSS PRODUCT";
        case JoinType::kNatural:
            return "NATURAL JOIN";
        case JoinType::kSemi:
            return "SEMI JOIN";
        case JoinType::kMark:
            return "MARK JOIN";
        case JoinType::kAnti:
            return "ANTI JOIN";
    }
    PlannerError("Unreachable");
}

};
