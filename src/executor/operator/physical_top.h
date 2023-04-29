//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalTop : public PhysicalOperator {
public:
    explicit
    PhysicalTop(uint64_t id): PhysicalOperator(PhysicalOperatorType::kTop, nullptr, nullptr, id) {}

    ~PhysicalTop() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return left_->GetOutputNames();
    }

    inline SharedPtr<Vector<DataType>>
    GetOutputTypes() const final {
        return left_->GetOutputTypes();
    }

};

}

