//
// Created by jinhai on 23-4-28.
//

#pragma once

#include "executor/physical_operator.h"

namespace infinity {

enum class SinkType {
    kInvalid,
    kLocalQueue,
    kRemote,
    kResult,
};

class PhysicalSink final : public PhysicalOperator {
public:
    explicit
    PhysicalSink(u64 id,
                 SinkType sink_type,
                 SharedPtr<Vector<String>> names,
                 SharedPtr<Vector<SharedPtr<DataType>>> types)
            : PhysicalOperator(PhysicalOperatorType::kSink, nullptr, nullptr, id),
              type_(sink_type),
              output_names_(std::move(names)),
              output_types_(std::move(types)) {

    }

    ~PhysicalSink() override = default;

    void
    Init() override;

    void
    Execute(QueryContext* query_context) final;

    virtual void
    Execute(QueryContext* query_context, InputState* input_state, OutputState* output_state) final;

    void
    Execute(QueryContext* query_context, SinkState* sink_state);

    inline SharedPtr<Vector<String>>
    GetOutputNames() const final {
        return output_names_;
    }

    inline SharedPtr<Vector<SharedPtr<DataType>>>
    GetOutputTypes() const final {
        return output_types_;
    }

    inline SinkType
    sink_type() const {
        return type_;
    }

private:
    void
    FillSinkStateFromLastOutputState(MaterializeSinkState* materialize_sink_state, OutputState* task_output_state);

    void
    FillSinkStateFromLastOutputState(ResultSinkState* result_sink_state, OutputState* task_output_state);

    void
    FillSinkStateFromLastOutputState(MessageSinkState* message_sink_state, OutputState* task_output_state);

    void
    FillSinkStateFromLastOutputState(QueueSinkState* queue_sink_state, OutputState* task_output_state);

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
    SinkType type_{SinkType::kInvalid};
};

}
