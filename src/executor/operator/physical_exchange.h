//
// Created by jinhai on 23-4-25.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalExchange final : public PhysicalOperator {
public:
    explicit PhysicalExchange(uint64_t id)
            : PhysicalOperator(PhysicalOperatorType::kDummyScan, nullptr, nullptr,id)
    {}

    ~PhysicalExchange() override = default;

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
