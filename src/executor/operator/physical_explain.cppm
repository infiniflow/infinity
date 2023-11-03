//
// Created by jinhai on 23-3-13.
//

module;

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;

export module physical_explain;

namespace infinity {

export class PhysicalExplain final : public PhysicalOperator {
public:
    explicit PhysicalExplain(u64 id, ExplainType type, SharedPtr<Vector<SharedPtr<String>>> text_array, SharedPtr<PhysicalOperator> left)
        : PhysicalOperator(PhysicalOperatorType::kExplain, Move(left), nullptr, id), explain_type_(type), texts_(Move(text_array)) {}

    ~PhysicalExplain() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    void SetExplainText(SharedPtr<Vector<SharedPtr<String>>> text) { texts_ = Move(text); }

    void SetExplainTaskText(SharedPtr<Vector<SharedPtr<String>>> text) { task_texts = Move(text); }

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline ExplainType explain_type() const { return explain_type_; }

    static void AlignParagraphs(Vector<SharedPtr<String>>& array1, Vector<SharedPtr<String>>& array2);

private:
    ExplainType explain_type_{ExplainType::kPhysical};
    SharedPtr<Vector<SharedPtr<String>>> texts_{nullptr};
    SharedPtr<Vector<SharedPtr<String>>> task_texts{nullptr};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
