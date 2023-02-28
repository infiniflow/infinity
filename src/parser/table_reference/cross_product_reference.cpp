//
// Created by jinhai on 23-2-28.
//

#include "cross_product_reference.h"

namespace infinity {

CrossProductReference::~CrossProductReference() {
    if(left_ != nullptr) {
        delete(left_);
        left_ = nullptr;
    }
    if(right_ != nullptr) {
        delete(right_);
        right_ = nullptr;
    }
}

String
CrossProductReference::ToString() {
    return "CrossProductReference";
}

};
