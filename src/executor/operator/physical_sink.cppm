// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import load_meta;

export module physical_sink;

namespace infinity {

export enum class SinkType {
    kInvalid,
    kLocalQueue,
    kRemote,
    kResult,
};

export class PhysicalSink final : public PhysicalOperator {
public:
    explicit PhysicalSink(u64 id, SinkType sink_type, SharedPtr<Vector<String>> names, SharedPtr<Vector<SharedPtr<DataType>>> types, SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kSink, nullptr, nullptr, id, load_metas), type_(sink_type), output_names_(Move(names)),
          output_types_(Move(types)) {}

    ~PhysicalSink() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *output_state) final;

    bool Execute(QueryContext *query_context, SinkState *sink_state);

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline SinkType sink_type() const { return type_; }

private:
    void FillSinkStateFromLastOperatorState(MaterializeSinkState *materialize_sink_state, OperatorState *task_operator_state);

    void FillSinkStateFromLastOperatorState(ResultSinkState *result_sink_state, OperatorState *task_operator_state);

    void FillSinkStateFromLastOperatorState(MessageSinkState *message_sink_state, OperatorState *task_operator_state);

    void FillSinkStateFromLastOperatorState(SummarySinkState *message_sink_state, OperatorState *task_operator_state);

    void FillSinkStateFromLastOperatorState(QueueSinkState *queue_sink_state, OperatorState *task_operator_state);

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
    SinkType type_{SinkType::kInvalid};
};

} // namespace infinity
