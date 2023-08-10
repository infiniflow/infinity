//
// Created by jinhai on 23-4-28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

enum class SinkType {
    kStream,
    kBatch,
};

class PhysicalSink final : public PhysicalOperator {
public:
    explicit PhysicalSink(u64 id,
                          SinkType sink_type,
                          SharedPtr<Vector<String>> names,
                          SharedPtr<Vector<SharedPtr<DataType>>> types)
            : PhysicalOperator(PhysicalOperatorType::kSink, nullptr, nullptr,id),
            sink_type_(sink_type),
            output_names_(std::move(names)),
            output_types_(std::move(types)) {

    }

    ~PhysicalSink() override = default;

    void
    Init() override;

    void
    Execute(SharedPtr<QueryContext>& query_context) override;

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
    SinkType sink_type_{SinkType::kBatch};
};

}
