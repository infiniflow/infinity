//
// Created by jinhai on 23-4-25.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

class PhysicalExchange final : public PhysicalOperator {
public:
    explicit PhysicalExchange(u64 id,
                              SharedPtr<Vector<String>> names,
                              SharedPtr<Vector<SharedPtr<DataType>>> types)
            : PhysicalOperator(PhysicalOperatorType::kExchange, nullptr, nullptr,id),
              output_names_(std::move(names)),
              output_types_(std::move(types))
    {}

    ~PhysicalExchange() override = default;

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
