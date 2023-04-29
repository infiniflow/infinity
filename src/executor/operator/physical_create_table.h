//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "storage/table_def.h"
#include "executor/physical_operator.h"

#include <memory>

namespace infinity {

class PhysicalCreateTable final : public PhysicalOperator {
public:
    explicit
    PhysicalCreateTable(SharedPtr<String> schema_name,
                        SharedPtr<TableDef> table_def_ptr,
                        ConflictType conflict_type,
                        u64 table_index,
                        uint64_t id);

    explicit
    PhysicalCreateTable(SharedPtr<String> schema_name,
                        const SharedPtr<PhysicalOperator>& input,
                        ConflictType conflict_type,
                        u64 table_index,
                        uint64_t id);

    ~PhysicalCreateTable() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return output_names_;
    }

    inline SharedPtr<Vector<DataType>>
    GetOutputTypes() const final {
        return output_types_;
    }

    inline u64
    table_index() const {
        return table_index_;
    }

    inline const SharedPtr<TableDef>&
    table_definition() const {
        return table_def_ptr_;
    }

    inline ConflictType
    conflict_type() const {
        return conflict_type_;
    }

    inline const SharedPtr<String>&
    schema_name() const {
        return schema_name_;
    }

private:
    SharedPtr<TableDef> table_def_ptr_{};
    SharedPtr<String> schema_name_{};
    u64 table_index_{};
    ConflictType conflict_type_{ConflictType::kInvalid};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<DataType>> output_types_{};
};

}
