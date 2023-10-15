//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"
#include "parser/definition/table_def.h"
#include "parser/statement/extra/extra_ddl_info.h"

namespace infinity {

class PhysicalCreateTable final : public PhysicalOperator {
public:
    explicit PhysicalCreateTable(SharedPtr<String> schema_name,
                                 SharedPtr<TableDef> table_def_ptr,
                                 SharedPtr<Vector<String>> output_names,
                                 SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                 ConflictType conflict_type,
                                 u64 table_index,
                                 u64 id);

    explicit PhysicalCreateTable(SharedPtr<String> schema_name,
                                 const SharedPtr<PhysicalOperator> &input,
                                 SharedPtr<Vector<String>> output_names,
                                 SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                 ConflictType conflict_type,
                                 u64 table_index,
                                 u64 id);

    ~PhysicalCreateTable() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline u64 table_index() const { return table_index_; }

    inline const SharedPtr<TableDef> &table_definition() const { return table_def_ptr_; }

    inline ConflictType conflict_type() const { return conflict_type_; }

    inline const SharedPtr<String> &schema_name() const { return schema_name_; }

private:
    SharedPtr<TableDef> table_def_ptr_{};
    SharedPtr<String> schema_name_{};
    u64 table_index_{};
    ConflictType conflict_type_{ConflictType::kInvalid};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
