//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include <utility>

#include "executor/physical_operator.h"
#include "parser/statement/extra/create_view_info.h"

namespace infinity {

class PhysicalCreateView final : public PhysicalOperator {
public:
    explicit
    PhysicalCreateView(u64 id,
                       SharedPtr<Vector<String>> names_ptr,
                       SharedPtr<Vector<SharedPtr<DataType>>> types_ptr,
                       SharedPtr<CreateViewInfo> create_view_info)
        : PhysicalOperator(PhysicalOperatorType::kCreateView, nullptr, nullptr, id),
        output_names_(std::move(names_ptr)),
        output_types_(std::move(types_ptr)),
        create_view_info_(std::move(create_view_info))
        {}

    ~PhysicalCreateView() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

    inline const SharedPtr<CreateViewInfo>&
    bound_select_statement() const {
        return create_view_info_;
    };

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return output_names_;
    }

    inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final {
        return output_types_;
    }

    inline const SharedPtr<CreateViewInfo>&
    create_view_info() const {
        return create_view_info_;
    }

private:
    SharedPtr<CreateViewInfo> create_view_info_{nullptr};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

}

