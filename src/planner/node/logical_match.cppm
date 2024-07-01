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

export module logical_match;

import stl;
import logical_node_type;
import column_binding;
import logical_node;

import base_expression;
import match_expression;
import base_table_ref;
import table_entry;
import internal_types;
import data_type;
import common_query_filter;
import column_index_reader;
import doc_iterator;
import query_node;

namespace infinity {

export class LogicalMatch : public LogicalNode {
public:
    explicit LogicalMatch(u64 node_id, SharedPtr<BaseTableRef> base_table_ref, SharedPtr<MatchExpression> match_expr);

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    [[nodiscard]] TableEntry *table_collection_ptr() const;

    [[nodiscard]] String TableAlias() const;

    [[nodiscard]] u64 TableIndex() const;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalMatch"; }

    SharedPtr<BaseTableRef> base_table_ref_{};
    SharedPtr<MatchExpression> match_expr_{};
    SharedPtr<BaseExpression> filter_expression_{};
    IndexReader index_reader_;
    UniquePtr<QueryNode> query_tree_;
    float begin_threshold_;
    EarlyTermAlgo early_term_algo_{EarlyTermAlgo::kBMW};
    u32 top_n_{1};

    SharedPtr<CommonQueryFilter> common_query_filter_{};
};

} // namespace infinity
