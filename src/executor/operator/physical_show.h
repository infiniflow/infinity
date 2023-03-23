//
// Created by JinHai on 2022/9/8.
//

#pragma once

#include <utility>

#include "planner/node/logical_show.h"
#include "executor/physical_operator.h"

namespace infinity {

class PhysicalShow : public PhysicalOperator {
public:
    explicit PhysicalShow(u64 id,
                          ShowType type,
                          String schema_name,
                          u64 table_index)
            : PhysicalOperator(PhysicalOperatorType::kShow, nullptr, nullptr, id),
            scan_type_(type),
            schema_name_(std::move(schema_name)),
            table_index_(table_index)
            {}

    ~PhysicalShow() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

    inline ShowType
    scan_type() const {
        return scan_type_;
    }
private:
    void
    ExecuteShowTable(SharedPtr<QueryContext>& query_context);

    void
    ExecuteShowViews(SharedPtr<QueryContext>& query_context);

    void
    ExecuteShowColumns(SharedPtr<QueryContext>& query_context);

private:
    ShowType scan_type_{ShowType::kInvalid};
    String schema_name_{};
    u64 table_index_{};
};

}
