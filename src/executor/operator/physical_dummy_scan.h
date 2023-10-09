//
// Created by JinHai on 2022/10/25.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalDummyScan final : public PhysicalOperator {
public:
    explicit PhysicalDummyScan(uint64_t id) : PhysicalOperator(PhysicalOperatorType::kDummyScan, nullptr, nullptr, id) {}

    ~PhysicalDummyScan() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
