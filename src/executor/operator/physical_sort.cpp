//
// Created by JinHai on 2022/7/28.
//

#include "physical_sort.h"

namespace infinity {

void
PhysicalSort::Init() {

}

void
PhysicalSort::Execute(SharedPtr<QueryContext>& query_context) {
    output_ = left_->output();
}

}

