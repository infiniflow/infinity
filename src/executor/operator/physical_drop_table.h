//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalDropTable : public PhysicalOperator {
public:
    explicit PhysicalDropTable(std::shared_ptr<std::string> schema_name,
                               std::shared_ptr<std::string> tbl_name,
                               uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kDropTable, nullptr, nullptr, id),
        schema_name_(std::move(schema_name)),
        table_name_(std::move(tbl_name)) {}

    ~PhysicalDropTable() = default;
    void Execute() override;

private:
    std::shared_ptr<std::string> schema_name_;
    std::shared_ptr<std::string> table_name_;
};

}

