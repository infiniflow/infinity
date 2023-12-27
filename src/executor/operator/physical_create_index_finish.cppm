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

export module physical_create_index_finish;

namespace infinity {
export class PhysicalCreateIndexFinish : public PhysicalOperator {
public:
    PhysicalCreateIndexFinish(u64 id,
                              UniquePtr<PhysicalOperator> left,
                              SharedPtr<String> db_name,
                              SharedPtr<String> table_name,
                              SharedPtr<IndexDef> index_def,
                              SharedPtr<Vector<String>> output_names,
                              SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                              SharedPtr<Vector<LoadMeta>> load_metas);

public:
    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    SizeT TaskletCount() override { return 1; }

    SharedPtr<Vector<String>> GetOutputNames() const override { return output_names_; }

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override { return output_types_; }

public:
    const SharedPtr<String> db_name_{};
    const SharedPtr<String> table_name_{};
    const SharedPtr<IndexDef> index_def_{};

    const SharedPtr<Vector<String>> output_names_{};
    const SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity