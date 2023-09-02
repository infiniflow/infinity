//
// Created by JinHai on 2022/7/28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalUpdate : public PhysicalOperator {
public:
    explicit PhysicalUpdate(uint64_t id)
        : PhysicalOperator(PhysicalOperatorType::kUpdate, nullptr, nullptr, id) {}
    ~PhysicalUpdate() override = default;

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

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};

};

}
