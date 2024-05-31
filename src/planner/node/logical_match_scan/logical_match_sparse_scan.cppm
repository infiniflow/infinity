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

export module logical_match_sparse_scan;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import logical_match_scan_base;
import base_expression;
import match_sparse_expression;
import base_table_ref;
import table_entry;
import internal_types;
import data_type;
import fast_rough_filter;
import common_query_filter;

namespace infinity {

export class LogicalMatchSparseScan final : public LogicalMatchScanBase {
public:
    explicit LogicalMatchSparseScan(u64 node_id, SharedPtr<BaseTableRef> base_table_ref, SharedPtr<MatchSparseExpression> match_sparse_expr)
        : LogicalMatchScanBase(node_id, LogicalNodeType::kMatchSparseScan, base_table_ref, match_sparse_expr) {}

    String name() override { return "LogicalMatchSparseScan"; }

public:
};
} // namespace infinity
