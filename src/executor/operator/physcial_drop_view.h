//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {
class PhysicalDropView final : public PhysicalOperator {
public:
    explicit
    PhysicalDropView(SharedPtr<String> schema_name,
                     SharedPtr<String> view_name,
                     ConflictType conflict_type,
                     SharedPtr<Vector<String>> output_names,
                     SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                     u64 id)
            : PhysicalOperator(PhysicalOperatorType::kDropView, nullptr, nullptr, id),
              schema_name_(std::move(schema_name)),
              view_name_(std::move(view_name)),
              output_names_(std::move(output_names)),
              output_types_(std::move(output_types)),
              conflict_type_(conflict_type) {
    }

    ~PhysicalDropView() override = default;

    void
    Init() override;

    void
    Execute(QueryContext* query_context) override;

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return output_names_;
    }

    inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final {
        return output_types_;
    }

    inline SharedPtr<String>
    schema_name() const {
        return schema_name_;
    }

    inline SharedPtr<String>
    view_name() const {
        return view_name_;
    }

    inline ConflictType
    conflict_type() const {
        return conflict_type_;
    }

private:
    SharedPtr<String> schema_name_{};
    SharedPtr<String> view_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

}
