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
    Execute(QueryContext* query_context) final;

    virtual void
    Execute(QueryContext* query_context, InputState* input_state, OutputState* output_state) final;

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return output_names_;
    }

    inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final {
        return output_types_;
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
    ExecuteShowTable(QueryContext* query_context);

    void
    ExecuteShowViews(QueryContext* query_context);

    void
    ExecuteShowColumns(QueryContext* query_context);

    void
    ExecuteShowTableDetail(QueryContext* query_context,
                           const Vector<SharedPtr<ColumnDef>>& table_columns);

    void
    ExecuteShowViewDetail(QueryContext* query_context,
                          const SharedPtr<Vector<SharedPtr<DataType>>>& column_types,
                          const SharedPtr<Vector<String>>& column_names);

    /// Execute push based show table
    void
    ExecuteShowTable(QueryContext* query_context,
                     ShowInputState *input_state,
                     ShowOutputState *output_state);

    /// Execute push based describe table
    void
    ExecuteShowColumns(QueryContext* query_context,ShowInputState *input_state,ShowOutputState *output_state);


private:
    ShowType scan_type_{ShowType::kInvalid};
    String schema_name_{};
    String object_name_{};
    u64 table_index_{};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};

};

}
