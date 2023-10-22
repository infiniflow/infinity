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

export module physical_merge_limit;

namespace infinity {

export class PhysicalMergeLimit final : public PhysicalOperator {
public:
    explicit PhysicalMergeLimit(SharedPtr<Vector<String>> output_names, SharedPtr<Vector<SharedPtr<DataType>>> output_types, u64 id)
        : PhysicalOperator(PhysicalOperatorType::kMergeLimit, nullptr, nullptr, id), output_names_(Move(output_names)),
          output_types_(Move(output_types)) {}

    ~PhysicalMergeLimit() override = default;

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
