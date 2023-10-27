//
// Created by JinHai on 2022/10/25.
//

module;

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;

export module physical_dummy_scan;

namespace infinity {

export class PhysicalDummyScan final : public PhysicalOperator {
public:
    explicit PhysicalDummyScan(u64 id) : PhysicalOperator(PhysicalOperatorType::kDummyScan, nullptr, nullptr, id) {}

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
