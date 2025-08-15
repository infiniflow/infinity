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

export module infinity_core:logical_knn_scan;

import :logical_node;
import :logical_node_type;
import :base_expression;
import :knn_expression;
import :base_table_ref;
import :logical_match_scan_base;
import :infinity_type;

import std;

namespace infinity {

export class LogicalKnnScan : public LogicalMatchScanBase {
public:
    explicit LogicalKnnScan(const u64 node_id,
                            std::shared_ptr<BaseTableRef> base_table_ref,
                            std::shared_ptr<BaseExpression> knn_expression,
                            const u64 table_index)
        : LogicalMatchScanBase(node_id, LogicalNodeType::kKnnScan, std::move(base_table_ref), std::move(knn_expression)),
          knn_table_index_(table_index) {}

    std::shared_ptr<KnnExpression> knn_expression() const { return std::static_pointer_cast<KnnExpression>(query_expression_); }

    inline std::string name() final { return "LogicalKnnScan"; }

public:
    u64 knn_table_index_{};
};

} // namespace infinity
