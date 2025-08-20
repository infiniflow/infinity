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

export module infinity_core:physical_create_index_prepare;

import :physical_operator_type;
import :physical_operator;
import :operator_state;
import :load_meta;
import :base_table_ref;
import :index_base;

import internal_types;
import extra_ddl_info;
import data_type;

namespace infinity {
export class PhysicalCreateIndexPrepare : public PhysicalOperator {
public:
    PhysicalCreateIndexPrepare(u64 id,
                               std::shared_ptr<BaseTableRef> base_table_ref,
                               std::shared_ptr<IndexBase> index_definition,
                               ConflictType conflict_type,
                               std::shared_ptr<std::vector<std::string>> output_names,
                               std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types,
                               std::shared_ptr<std::vector<LoadMeta>> load_metas,
                               bool prepare);

public:
    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    std::shared_ptr<std::vector<std::string>> GetOutputNames() const override { return output_names_; }

    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const override { return output_types_; }

public:
    const std::shared_ptr<BaseTableRef> base_table_ref_{};

    const std::shared_ptr<IndexBase> index_def_ptr_{};
    const ConflictType conflict_type_{};

    const std::shared_ptr<std::vector<std::string>> output_names_{};
    const std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};

    // if prepare_ is true, then index is constructed with multiple threads
    const bool prepare_;
};

} // namespace infinity