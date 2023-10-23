//
// Created by jinhai on 23-8-26.
//

module;

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import base_expression;

export module physical_flush;

namespace infinity {

export class PhysicalFlush final : public PhysicalOperator {
public:
    explicit PhysicalFlush(FlushType flush_type, u64 id)
        : PhysicalOperator(PhysicalOperatorType::kFlush, nullptr, nullptr, id), flush_type_(flush_type) {}

    ~PhysicalFlush() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline FlushType flush_type() const { return flush_type_; }

private:
    void FlushData(QueryContext *query_context);

    void FlushLog(QueryContext *query_context);

    void FlushBuffer(QueryContext *query_context);

private:
    FlushType flush_type_;

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
