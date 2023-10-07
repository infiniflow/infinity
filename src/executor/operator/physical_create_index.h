#pragma once

#include "executor/physical_operator.h"
#include "parser/statement/extra/create_index_info.h"
#include "parser/statement/extra/extra_ddl_info.h"
#include "storage/index_def.h"
namespace infinity {
class PhysicalCreateIndex : public PhysicalOperator {
public:
    void
    Init() override;

    void
    Execute(QueryContext* query_context) override;

    void
    Execute(QueryContext* query_context, InputState* input_state,
            OutputState* output_state) override;

    SharedPtr<Vector<String>>
    GetOutputNames() const override {
        return output_names_;
    }

    SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const override {
        return output_types_;
    }

public:
    PhysicalCreateIndex(SharedPtr<String> schema_name,
                        SharedPtr<IndexDef> index_definition,
                        ConflictType conflict_type,

                        SharedPtr<Vector<String>> output_names,
                        SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                        u64 id);

private:
    SharedPtr<String> schema_name_{};
    SharedPtr<IndexDef> index_definition_{};
    ConflictType conflict_type_{};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};
}// namespace infinity