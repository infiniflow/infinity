// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "join_reference.h"
#include "cross_product_reference.h"
#include "parser_assert.h"

namespace infinity {

JoinReference::~JoinReference() {
    if (left_ != nullptr) {
        delete (left_);
        left_ = nullptr;
    }
    if (right_ != nullptr) {
        delete (right_);
        right_ = nullptr;
    }

    if (condition_ != nullptr) {
        delete (condition_);
        condition_ = nullptr;
    }

    if (using_columns_ != nullptr) {
        for (auto column_ptr : *using_columns_) {
            free(column_ptr);
        }
        delete using_columns_;
    }
}

std::string JoinReference::ToString() { return "Join"; }

std::string JoinReference::ToString(JoinType type) {
    switch (type) {
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
    ParserError("Unreachable");
}

}; // namespace infinity
