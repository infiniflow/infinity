//
// Created by jinhai on 23-3-14.
//

#pragma once

#include "executor/physical_operator.h"
#include "parser/statement/extra/extra_ddl_info.h"

namespace infinity {

class QueryContext;

class PhysicalCreateSchema final : public PhysicalOperator {
public:
    explicit PhysicalCreateSchema(SharedPtr<String> schema_name,
                                  ConflictType conflict_type,
                                  SharedPtr<Vector<String>> output_names,
                                  SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                  u64 id)
        : PhysicalOperator(PhysicalOperatorType::kCreateDatabase, nullptr, nullptr, id), schema_name_(std::move(schema_name)),
          conflict_type_(conflict_type), output_names_(std::move(output_names)), output_types_(std::move(output_types)) {}

    ~PhysicalCreateSchema() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline SharedPtr<String> schema_name() const { return schema_name_; }

    inline ConflictType conflict_type() const { return conflict_type_; }

private:
    SharedPtr<String> schema_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
