//
// Created by JinHai on 2022/10/25.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalDummyScan final : public PhysicalOperator {
public:
    explicit PhysicalDummyScan(uint64_t id)
            : PhysicalOperator(PhysicalOperatorType::kDummyScan, nullptr, nullptr,id)
    {}

    ~PhysicalDummyScan() override = default;

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
