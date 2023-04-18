//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalUnionAll : public PhysicalOperator {
public:
    explicit PhysicalUnionAll(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kUnionAll, nullptr, nullptr, id) {}
    ~PhysicalUnionAll() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return MakeShared<Vector<String>>();
    }

};

}
