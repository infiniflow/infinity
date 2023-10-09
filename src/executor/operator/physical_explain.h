//
// Created by jinhai on 23-3-13.
//

#pragma once

#include <utility>

#include "executor/physical_operator.h"
#include "parser/statement/explain_statement.h"

namespace infinity {

class PhysicalExplain final : public PhysicalOperator {
public:
    explicit PhysicalExplain(u64 id, ExplainType type, SharedPtr<Vector<SharedPtr<String>>> text_array, SharedPtr<PhysicalOperator> left)
        : PhysicalOperator(PhysicalOperatorType::kExplain, std::move(left), nullptr, id), explain_type_(type), texts_(std::move(text_array)) {}

    ~PhysicalExplain() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    void SetExplainText(SharedPtr<Vector<SharedPtr<String>>> text) { texts_ = std::move(text); }

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline ExplainType explain_type() const { return explain_type_; }

private:
    ExplainType explain_type_{ExplainType::kPhysical};
    SharedPtr<Vector<SharedPtr<String>>> texts_{nullptr};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
