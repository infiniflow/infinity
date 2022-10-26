//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "storage/table_definition.h"
#include "executor/physical_operator.h"

#include <memory>

namespace infinity {

class PhysicalCreateTable : public PhysicalOperator {
public:
    explicit PhysicalCreateTable(std::shared_ptr<std::string> schema_name,
                                 std::shared_ptr<TableDefinition> table_def_ptr,
                                 uint64_t id);
    explicit PhysicalCreateTable(std::shared_ptr<std::string> schema_name,
                                 const std::shared_ptr<PhysicalOperator>& input,
                                 uint64_t id);

    ~PhysicalCreateTable() override = default;

    void
    Init() override;

    void
    Execute(std::shared_ptr<QueryContext>& query_context) override;

private:
    std::shared_ptr<TableDefinition> table_def_ptr_;
    std::shared_ptr<std::string> schema_name_;

};

}
