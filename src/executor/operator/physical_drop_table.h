//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalDropTable : public PhysicalOperator {
public:
    explicit PhysicalDropTable(SharedPtr<String> schema_name,
                               SharedPtr<String> tbl_name,
                               u64 table_index,
                               uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kDropTable, nullptr, nullptr, id),
        schema_name_(std::move(schema_name)),
        table_name_(std::move(tbl_name)),
        table_index_(table_index)
        {}

    ~PhysicalDropTable() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

private:
    SharedPtr<String> schema_name_;
    SharedPtr<String> table_name_;
    u64 table_index_;
};

}

