//
// Created by jinhai on 23-9-15.
//

module;

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;

export module physical_except;

namespace infinity {

export class PhysicalExcept final : public PhysicalOperator {
public:
    explicit PhysicalExcept(SharedPtr<Vector<String>> output_names, SharedPtr<Vector<SharedPtr<DataType>>> output_types, u64 id)
        : PhysicalOperator(PhysicalOperatorType::kExcept, nullptr, nullptr, id), output_names_(Move(output_names)),
          output_types_(Move(output_types)) {}

    ~PhysicalExcept() override = default;

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
