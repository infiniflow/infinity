//
// Created by jinhai on 23-3-16.
//

#pragma once

#include "executor/physical_operator.h"
#include "storage/table_def.h"

#include <memory>

namespace infinity {

class PhysicalCreateCollection final : public PhysicalOperator {
public:
    explicit PhysicalCreateCollection(SharedPtr<String> schema_name,
                                      SharedPtr<String> collection_name,
                                      ConflictType conflict_type,
                                      SharedPtr<Vector<String>> output_names,
                                      SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                      u64 table_index,
                                      u64 id);

    ~PhysicalCreateCollection() override = default;

    void Init() override;

    void Execute(QueryContext *query_context) final;

    virtual void Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline u64 table_index() const { return table_index_; }

    inline const SharedPtr<String> &schema_name() const { return schema_name_; }

    inline const SharedPtr<String> &collection_name() const { return collection_name_; }

    inline ConflictType conflict_type() const { return conflict_type_; }

private:
    SharedPtr<String> schema_name_{};
    SharedPtr<String> collection_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
    u64 table_index_{};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
