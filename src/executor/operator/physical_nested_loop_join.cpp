//
// Created by JinHai on 2022/7/28.
//

#include "physical_nested_loop_join.h"

namespace infinity {

void
PhysicalNestedLoopJoin::Init() {

}

void
PhysicalNestedLoopJoin::Execute(SharedPtr<QueryContext>& query_context) {

}

SharedPtr<Vector<String>>
PhysicalNestedLoopJoin::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    SharedPtr<Vector<String>> left_output_names = left_->GetOutputNames();
    SharedPtr<Vector<String>> right_output_names = right_->GetOutputNames();

    result->reserve(left_output_names->size() + right_output_names->size());
    for(auto& name_str: *left_output_names) {
        result->emplace_back(name_str);
    }

    for(auto& name_str: *right_output_names) {
        result->emplace_back(name_str);
    }

    return result;
}


SharedPtr<Vector<SharedPtr<DataType>>>
PhysicalNestedLoopJoin::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result = MakeShared<Vector<SharedPtr<DataType>>>();
    SharedPtr<Vector<SharedPtr<DataType>>> left_output_types = left_->GetOutputTypes();
    SharedPtr<Vector<SharedPtr<DataType>>> right_output_types = right_->GetOutputTypes();

    result->reserve(left_output_types->size() + right_output_types->size());
    for(auto& left_type: *left_output_types) {
        result->emplace_back(left_type);
    }

    for(auto& right_type: *right_output_types) {
        result->emplace_back(right_type);
    }

    return result;
}

}
