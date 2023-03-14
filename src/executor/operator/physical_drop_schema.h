//
// Created by jinhai on 23-3-14.
//


#pragma once

#include "common/types/internal_types.h"
#include "main/query_context.h"
#include "executor/physical_operator.h"

namespace infinity {

class PhysicalDropSchema : public PhysicalOperator {
public:
    explicit
    PhysicalDropSchema(SharedPtr<String> schema_name,
                       ConflictType conflict_type,
                       u64 id)
                       : PhysicalOperator(PhysicalOperatorType::kDropSchema, nullptr, nullptr, id),
                        schema_name_(std::move(schema_name)),
                        conflict_type_(conflict_type)
                        {}

    ~PhysicalDropSchema() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

private:
    SharedPtr<String> schema_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

}


