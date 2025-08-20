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

export module infinity_core:physical_source;

import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :load_meta;
import :infinity_exception;
import :logger;

import internal_types;
import data_type;

namespace infinity {

export enum class SourceType {
    kInvalid,
    kTable,
    kLocalQueue,
    kEmpty,
    kRemote,
};

export std::string ToString(SourceType source_type);

export class PhysicalSource final : public PhysicalOperator {
public:
    explicit PhysicalSource(u64 id,
                            SourceType source_type,
                            std::shared_ptr<std::vector<std::string>> names,
                            std::shared_ptr<std::vector<std::shared_ptr<DataType>>> types,
                            std::shared_ptr<std::vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kSource, nullptr, nullptr, id, load_metas), output_names_(std::move(names)),
          output_types_(std::move(types)), type_(source_type) {}

    ~PhysicalSource() override = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    bool Execute(QueryContext *query_context, SourceState *source_state);

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return output_names_; }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline SourceType source_type() const { return type_; }

private:
    std::shared_ptr<std::vector<std::string>> output_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};
    SourceType type_{SourceType::kInvalid};
};

} // namespace infinity
