#pragma once

#include "executor/physical_operator.h"
#include "parser/statement/extra/extra_ddl_info.h"
#include "storage/meta/definition/index_def/index_def.h"

namespace infinity {
class PhysicalCreateIndex : public PhysicalOperator {
public:
    void Init() override;

    void Execute(QueryContext *query_context) override;

    void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) override;

    SharedPtr<Vector<String>> GetOutputNames() const override { return output_names_; }

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override { return output_types_; }

public:
    static inline SharedPtr<PhysicalCreateIndex> Make(SharedPtr<String> schema_name,
                                                      SharedPtr<String> table_name,
                                                      SharedPtr<IndexDef> index_definition,
                                                      ConflictType conflict_type,
                                                      SharedPtr<Vector<String>> output_names,
                                                      SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                                      u64 id) {
        return MakeShared<PhysicalCreateIndex>(schema_name, table_name, index_definition, conflict_type, output_names, output_types, id);
    }

    PhysicalCreateIndex(SharedPtr<String> schema_name,
                        SharedPtr<String> table_name,
                        SharedPtr<IndexDef> index_definition,
                        ConflictType conflict_type,
                        SharedPtr<Vector<String>> output_names,
                        SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                        u64 id)
        : PhysicalOperator(PhysicalOperatorType::kCreateIndex, nullptr, nullptr, id), schema_name_(schema_name), table_name_(table_name),
          index_def_ptr_(index_definition), conflict_type_(conflict_type), output_names_(output_names), output_types_(output_types) {}

public:
    const SharedPtr<String> schema_name_{};
    const SharedPtr<String> table_name_{};
    const SharedPtr<IndexDef> index_def_ptr_{};
    const ConflictType conflict_type_{};

    const SharedPtr<Vector<String>> output_names_{};
    const SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};
} // namespace infinity