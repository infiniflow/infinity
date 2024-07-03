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

export module physical_source;

import stl;

import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import load_meta;
import infinity_exception;
import internal_types;
import data_type;
import logger;

namespace infinity {

export enum class SourceType {
    kInvalid,
    kTable,
    kLocalQueue,
    kEmpty,
    kRemote,
};

export class PhysicalSource final : public PhysicalOperator {
public:
    explicit PhysicalSource(u64 id,
                            SourceType source_type,
                            SharedPtr<Vector<String>> names,
                            SharedPtr<Vector<SharedPtr<DataType>>> types,
                            SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kSource, nullptr, nullptr, id, load_metas), output_names_(std::move(names)), output_types_(std::move(types)),
          type_(source_type) {}

    ~PhysicalSource() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    bool Execute(QueryContext *query_context, SourceState *source_state);

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    SizeT TaskletCount() override {
        String error_message = "Not implement: TaskletCount not Implement";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
        return 0;
    }

    inline SourceType source_type() const { return type_; }

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
    SourceType type_{SourceType::kInvalid};
};

} // namespace infinity
