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

export module logical_match_scan_base;

import stl;
import logical_node_type;
import column_binding;
import logical_node;
import base_expression;
import base_table_ref;
import table_entry;
import internal_types;
import data_type;
import fast_rough_filter;
import common_query_filter;

namespace infinity {

export class LogicalMatchScanBase : public LogicalNode {
public:
    LogicalMatchScanBase(u64 node_id, LogicalNodeType node_type, SharedPtr<BaseTableRef> base_table_ref, SharedPtr<BaseExpression> query_expression);

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const override;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const override;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override;

    [[nodiscard]] TableEntry *table_collection_ptr() const;

    [[nodiscard]] String TableAlias() const;

    [[nodiscard]] u64 TableIndex() const;

    String ToString(i64 &space) const override;

public:
    SharedPtr<BaseTableRef> base_table_ref_{};

    SharedPtr<BaseExpression> query_expression_{};

    SharedPtr<BaseExpression> filter_expression_{};

    SharedPtr<CommonQueryFilter> common_query_filter_{};
};

} // namespace infinity
