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

export module infinity_core:physical_sink;

import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :load_meta;
import :infinity_exception;
import :logger;

import internal_types;
import data_type;

namespace infinity {

class FragmentContext;

export enum class SinkType {
    kInvalid,
    kLocalQueue,
    kRemote,
    kResult,
};

export std::string ToString(SinkType sink_type);

export class PhysicalSink final : public PhysicalOperator {
public:
    explicit PhysicalSink(u64 id,
                          SinkType sink_type,
                          std::shared_ptr<std::vector<std::string>> names,
                          std::shared_ptr<std::vector<std::shared_ptr<DataType>>> types,
                          std::shared_ptr<std::vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kSink, nullptr, nullptr, id, load_metas), output_names_(std::move(names)),
          output_types_(std::move(types)), type_(sink_type) {}

    ~PhysicalSink() override = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *output_state) final;

    bool Execute(QueryContext *query_context, FragmentContext *fragment_context, SinkState *sink_state);

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return output_names_; }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline SinkType sink_type() const { return type_; }

private:
    void FillSinkStateFromLastOperatorState(MaterializeSinkState *materialize_sink_state, OperatorState *task_operator_state);

    void FillSinkStateFromLastOperatorState(ResultSinkState *result_sink_state, OperatorState *task_operator_state);

    void FillSinkStateFromLastOperatorState(MessageSinkState *message_sink_state, OperatorState *task_operator_state);

    void FillSinkStateFromLastOperatorState(SummarySinkState *message_sink_state, OperatorState *task_operator_state);

    void FillSinkStateFromLastOperatorState(FragmentContext *fragment_context, QueueSinkState *queue_sink_state, OperatorState *task_operator_state);

private:
    std::shared_ptr<std::vector<std::string>> output_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};
    SinkType type_{SinkType::kInvalid};
};

} // namespace infinity
