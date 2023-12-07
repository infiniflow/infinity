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
import table_collection_entry;
import base_expression;
import match_expression;
import base_table_ref;
import load_meta;

export module physical_match;

namespace infinity {

export class PhysicalMatch final : public PhysicalOperator {
public:
    explicit PhysicalMatch(u64 id, SharedPtr<BaseTableRef> base_table_ref, SharedPtr<MatchExpression> match_expr, SharedPtr<Vector<LoadMeta>> load_metas);

    ~PhysicalMatch() override;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    SharedPtr<Vector<String>> GetOutputNames() const final;

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    void FillingTableRefs(HashMap<SizeT, SharedPtr<BaseTableRef>> &table_refs) override {
        table_refs.insert({base_table_ref_->table_index_, base_table_ref_});
    }

    String ToString(i64 &space) const;

    SharedPtr<BaseTableRef> base_table_ref_{};
    SharedPtr<MatchExpression> match_expr_;
};

} // namespace infinity
