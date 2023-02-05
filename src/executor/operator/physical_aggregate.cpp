//
// Created by JinHai on 2022/7/28.
//

#include "physical_aggregate.h"

namespace infinity {

void
PhysicalAggregate::Init() {

}

void
PhysicalAggregate::Execute(SharedPtr<QueryContext>& query_context) {
    this->output_ = this->left_->output();
}

}
