//
// Created by JinHai on 2022/8/10.
//

module;

import parser;
import stl;

export module view;

// A view means a logical plan
namespace infinity {

class View {
public:
    explicit
    View(SharedPtr<CreateViewInfo> create_view_info, SharedPtr<Vector<String>> column_names, SharedPtr<Vector<SharedPtr<DataType>>> column_types)
        : create_view_info_(Move(create_view_info)), column_names_(Move(column_names)), column_types_(Move(column_types)) {}

    [[nodiscard]] inline const String &schema_name() const { return create_view_info_->schema_name_; }

    [[nodiscard]] inline const String &view_name() const { return create_view_info_->view_name_; }

    const SelectStatement *GetSQLStatement() { return create_view_info_->select_; }

    [[nodiscard]] inline const SharedPtr<Vector<String>> &column_names() const { return column_names_; };

    [[nodiscard]] inline const SharedPtr<Vector<SharedPtr<DataType>>> &column_types() const { return column_types_; };

private:
    SharedPtr<CreateViewInfo> create_view_info_{};
    SharedPtr<Vector<String>> column_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> column_types_{};
};
} // namespace infinity
