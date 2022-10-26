//
// Created by JinHai on 2022/7/28.
//

#include "physical_project.h"

namespace infinity {

void
PhysicalProject::Init() {
    executor.Init(expressions_);
}

void
PhysicalProject::Execute(std::shared_ptr<QueryContext>& query_context) {
    // Get input from left child
    auto input_table = left_->output();

    // Execute the expression on the input table

    // Generate the output
    output_ = input_table;
}

}
