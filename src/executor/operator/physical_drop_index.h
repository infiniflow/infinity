#pragma once

#include "executor/physical_operator.h"
#include "parser/statement/extra/extra_ddl_info.h"

namespace infinity {

class PhysicalDropIndex final : public PhysicalOperator {
public:
    explicit PhysicalDropIndex(SharedPtr<String> schema_name,
                               SharedPtr<String> table_name,
                               SharedPtr<String> index_name,
                               ConflictType conflict_type,
                               SharedPtr<Vector<String>> output_names,
                               SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                               u64 id)
        : PhysicalOperator(PhysicalOperatorType::kDropIndex, nullptr, nullptr, id), schema_name_(std::move(schema_name)),
          table_name_(std::move(table_name)), index_name_(std::move(index_name)), conflict_type_(conflict_type),
          output_names_(std::move(output_names)), output_types_(std::move(output_types)) {}

    ~PhysicalDropIndex() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) override;

    void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) override;

    inline SharedPtr<Vector<String>> GetOutputNames() const override { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override { return output_types_; }

public:
    const SharedPtr<String> schema_name_{};
    const SharedPtr<String> table_name_{};
    const SharedPtr<String> index_name_{};
    const ConflictType conflict_type_{ConflictType::kInvalid};

    const SharedPtr<Vector<String>> output_names_{};
    const SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};
} // namespace infinity