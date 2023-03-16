//
// Created by jinhai on 23-3-16.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalDropCollection : public PhysicalOperator {
public:
    explicit PhysicalDropCollection(SharedPtr<String> schema_name,
                                    SharedPtr<String> collection_name,
                                    ConflictType conflict_type,
                                    u64 id)
            : PhysicalOperator(PhysicalOperatorType::kDropCollection, nullptr, nullptr, id),
              schema_name_(std::move(schema_name)),
              collection_name_(std::move(collection_name)),
              conflict_type_(conflict_type)
              {}

    ~PhysicalDropCollection() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

private:
    SharedPtr<String> schema_name_{};
    SharedPtr<String> collection_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};
};

}

