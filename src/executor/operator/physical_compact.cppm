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

export module physical_compact;

import stl;
import physical_operator;
import catalog;
import query_context;
import operator_state;
import parser;
import physical_operator_type;
import infinity_exception;
import load_meta;
import base_table_ref;

namespace infinity {

export class PhysicalCompact : public PhysicalOperator {
public:
    PhysicalCompact(u64 id, SharedPtr<BaseTableRef> table_ref, SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kCompact, nullptr, nullptr, id, load_metas), table_ref_(table_ref) {}

    void Init() final;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    SizeT TaskletCount() final {
        UnrecoverableError("Not implement: TaskletCount not Implement");
        return 0;
    }

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};

    const SharedPtr<BaseTableRef> table_ref_;
};

} // namespace infinity
