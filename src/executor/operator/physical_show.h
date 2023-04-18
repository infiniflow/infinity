//
// Created by JinHai on 2022/9/8.
//

#pragma once

#include <utility>

#include "planner/node/logical_show.h"
#include "executor/physical_operator.h"
#include "storage/collection.h"
#include "storage/view.h"

namespace infinity {

class PhysicalShow : public PhysicalOperator {
public:
    explicit PhysicalShow(u64 id,
                          ShowType type,
                          String schema_name,
                          String object_name,
                          u64 table_index)
            : PhysicalOperator(PhysicalOperatorType::kShow, nullptr, nullptr, id),
            scan_type_(type),
            schema_name_(std::move(schema_name)),
            object_name_(std::move(object_name)),
            table_index_(table_index)
            {}

    ~PhysicalShow() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return MakeShared<Vector<String>>();
    }

    inline ShowType
    scan_type() const {
        return scan_type_;
    }

    inline const String&
    schema_name() const {
        return schema_name_;
    };

    inline const String&
    object_name() const {
        return object_name_;
    };

private:
    void
    ExecuteShowTable(SharedPtr<QueryContext>& query_context);

    void
    ExecuteShowViews(SharedPtr<QueryContext>& query_context);

    void
    ExecuteShowColumns(SharedPtr<QueryContext>& query_context);

    void
    ExecuteShowTableDetail(SharedPtr<QueryContext>& query_context, const SharedPtr<Table>& table_ptr);

    void
    ExecuteShowCollectionDetail(SharedPtr<QueryContext>& query_context, const SharedPtr<Collection>& table_ptr);

    void
    ExecuteShowViewDetail(SharedPtr<QueryContext>& query_context, const SharedPtr<View>& table_ptr);

private:
    ShowType scan_type_{ShowType::kInvalid};
    String schema_name_{};
    String object_name_{};
    u64 table_index_{};
};

}
