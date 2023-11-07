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
import logical_node_type;
import column_binding;
import logical_node;
import parser;

export module logical_knn_scan;

namespace infinity {

class BaseExpression;
class KnnExpression;
class BaseTableRef;
class TableCollectionEntry;

export class LogicalKnnScan : public LogicalNode {
public:
    explicit LogicalKnnScan(u64 node_id, SharedPtr<BaseTableRef> base_table_ref);

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    [[nodiscard]] TableCollectionEntry *table_collection_ptr() const;

    [[nodiscard]] String TableAlias() const;

    [[nodiscard]] u64 TableIndex() const;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalKnnScan"; }

    SharedPtr<BaseTableRef> base_table_ref_{};

    Vector<SharedPtr<BaseExpression>> knn_expressions_{};

    SharedPtr<BaseExpression> limit_expression_{};
    SharedPtr<BaseExpression> filter_expression_{};
    OrderType order_by_type_{OrderType::kAsc};

    u64 knn_table_index_{};
};

} // namespace infinity
