//
// Created by jinhai on 23-4-25.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalExchange final : public PhysicalOperator {
public:
    explicit PhysicalExchange(u64 id)
            : PhysicalOperator(PhysicalOperatorType::kExchange, nullptr, nullptr,id)
    {}

    ~PhysicalExchange() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return output_names_;
    }

    inline SharedPtr<Vector<DataType>>
    GetOutputTypes() const final {
        return output_types_;
    }

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<DataType>> output_types_{};

};

}
