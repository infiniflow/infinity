//
// Created by jinhai on 23-4-28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalSink final : public PhysicalOperator {
public:
    explicit PhysicalSink(u64 id)
    : PhysicalOperator(PhysicalOperatorType::kSink, nullptr, nullptr,id)
    {}

    ~PhysicalSink() override = default;

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
