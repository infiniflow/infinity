//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalSortMergeJoin : public PhysicalOperator {
public:
    PhysicalSortMergeJoin(): PhysicalOperator(PhysicalOperatorType::kJoinSortMerge, nullptr, nullptr) {}
    ~PhysicalSortMergeJoin() = default;
};

}
