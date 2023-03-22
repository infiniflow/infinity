//
// Created by JinHai on 2022/8/10.
//

#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "parser/statement/select_statement.h"
#include "common/types/data_type.h"
#include "planner/bound_select_statement.h"
#include "parser/statement/extra/create_view_info.h"

// A view means a logical plan
namespace infinity {

class LogicalNode;

class View {
public:
    explicit
    View(SharedPtr<CreateViewInfo> create_view_info,
         SharedPtr<Vector<String>> column_names,
         SharedPtr<Vector<DataType>> column_types)
         : create_view_info_(std::move(create_view_info)),
         column_names_(std::move(column_names)),
         column_types_(std::move(column_types))
         {}

    [[nodiscard]] inline const String&
    view_name() const {
        return create_view_info_->view_name_;
    }

    const SelectStatement*
    GetSQLStatement() {
        return create_view_info_->select_;
    }

    [[nodiscard]] inline const SharedPtr<Vector<String>>&
    column_names() const {
        return column_names_;
    };

    inline const SharedPtr<Vector<DataType>>&
    column_types() const {
        return column_types_;
    };

private:
    SharedPtr<CreateViewInfo> create_view_info_{};
    SharedPtr<Vector<String>> column_names_{};
    SharedPtr<Vector<DataType>> column_types_{};
};
}

