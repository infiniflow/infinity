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

export module infinity_core:physical_planner;

import :physical_operator;
import :logical_node;

import global_resource_usage;

namespace infinity {

export class PhysicalPlanner {
public:
    explicit PhysicalPlanner(QueryContext *query_context_ptr) : query_context_ptr_(query_context_ptr) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("PhysicalPlanner");
#endif
    }

    ~PhysicalPlanner() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("PhysicalPlanner");
#endif
    }

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildPhysicalOperator(const std::shared_ptr<LogicalNode> &logical_operator) const;

private:
    QueryContext *query_context_ptr_;

    // Create operator
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildCreateTable(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildCreateIndex(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildCreateCollection(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildCreateDatabase(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildPreparedPlan(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildCreateView(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Drop operator
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildDropTable(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildDropIndex(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildDropCollection(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildDropSchema(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildDropView(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Insert operator
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildInsert(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Delete operator
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildDelete(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Update operator
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildUpdate(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Import operator
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildImport(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Export operator
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildExport(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Alter operator
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildAlter(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Select operator
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildAggregate(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Operator
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildJoin(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildCrossProduct(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildSort(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildLimit(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildTop(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildProjection(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildFilter(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildIntersect(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildUnion(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildExcept(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Scan
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildShow(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildTableScan(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildIndexScan(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildViewScan(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildDummyScan(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Flush
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildFlush(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Optimize
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildOptimize(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Knn
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildKnn(const std::shared_ptr<LogicalNode> &logical_operator) const;
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildMatchTensorScan(const std::shared_ptr<LogicalNode> &logical_operator) const;
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildMatchSparseScan(const std::shared_ptr<LogicalNode> &logical_operator) const;
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildMatch(const std::shared_ptr<LogicalNode> &logical_operator) const;
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildFusion(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Command
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildCommand(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Compact
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildCompact(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildCompactIndex(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildCompactFinish(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Read cache
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildReadCache(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Unnest
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildUnnest(const std::shared_ptr<LogicalNode> &logical_operator) const;

    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildUnnestAggregate(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Explain
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildExplain(const std::shared_ptr<LogicalNode> &logical_operator) const;

    // Check
    [[nodiscard]] std::unique_ptr<PhysicalOperator> BuildCheck(const std::shared_ptr<LogicalNode> &logical_operator) const;
};

} // namespace infinity
