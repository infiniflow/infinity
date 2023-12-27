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
import physical_operator_type;
import physical_operator;
import query_context;
import operator_state;
import load_meta;

import index_def;
import base_table_ref;

export module physical_create_index_do;

namespace infinity {

export class PhysicalCreateIndexDo : public PhysicalOperator {
public:
    PhysicalCreateIndexDo(u64 id,
                          UniquePtr<PhysicalOperator> left,
                          SharedPtr<BaseTableRef> base_table_ref,
                          SharedPtr<String> index_name,
                          SharedPtr<Vector<String>> output_names,
                          SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                          SharedPtr<Vector<LoadMeta>> load_metas);

public:
    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    SizeT TaskletCount() override { return 0; }

    SharedPtr<Vector<String>> GetOutputNames() const override { return output_names_; }

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override { return output_types_; }

public:
    // for create fragemnt context
    const SharedPtr<BaseTableRef> base_table_ref_{};
    const SharedPtr<String> index_name_{};

    const SharedPtr<Vector<String>> output_names_{};
    const SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity