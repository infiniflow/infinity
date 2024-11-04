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
import physical_operator;
import logical_node;
import query_context;
import global_resource_usage;

export module physical_planner;
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

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildPhysicalOperator(const SharedPtr<LogicalNode> &logical_operator) const;

private:
    QueryContext *query_context_ptr_;

    // Create operator
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildCreateTable(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildCreateIndex(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildCreateCollection(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildCreateDatabase(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildPreparedPlan(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildCreateView(const SharedPtr<LogicalNode> &logical_operator) const;

    // Drop operator
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildDropTable(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildDropIndex(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildDropCollection(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildDropSchema(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildDropView(const SharedPtr<LogicalNode> &logical_operator) const;

    // Insert operator
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildInsert(const SharedPtr<LogicalNode> &logical_operator) const;

    // Delete operator
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildDelete(const SharedPtr<LogicalNode> &logical_operator) const;

    // Update operator
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildUpdate(const SharedPtr<LogicalNode> &logical_operator) const;

    // Import operator
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildImport(const SharedPtr<LogicalNode> &logical_operator) const;

    // Export operator
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildExport(const SharedPtr<LogicalNode> &logical_operator) const;

    // Alter operator
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildAlter(const SharedPtr<LogicalNode> &logical_operator) const;

    // Select operator
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildAggregate(const SharedPtr<LogicalNode> &logical_operator) const;

    // Operator
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildJoin(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildCrossProduct(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildSort(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildLimit(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildTop(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildProjection(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildFilter(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildIntersect(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildUnion(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildExcept(const SharedPtr<LogicalNode> &logical_operator) const;

    // Scan
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildShow(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildTableScan(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildIndexScan(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildViewScan(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildDummyScan(const SharedPtr<LogicalNode> &logical_operator) const;

    // Flush
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildFlush(const SharedPtr<LogicalNode> &logical_operator) const;

    // Optimize
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildOptimize(const SharedPtr<LogicalNode> &logical_operator) const;

    // Knn
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildKnn(const SharedPtr<LogicalNode> &logical_operator) const;
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildMatchTensorScan(const SharedPtr<LogicalNode> &logical_operator) const;
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildMatchSparseScan(const SharedPtr<LogicalNode> &logical_operator) const;
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildMatch(const SharedPtr<LogicalNode> &logical_operator) const;
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildFusion(const SharedPtr<LogicalNode> &logical_operator) const;

    // Command
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildCommand(const SharedPtr<LogicalNode> &logical_operator) const;

    // Compact
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildCompact(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildCompactIndex(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] UniquePtr<PhysicalOperator> BuildCompactFinish(const SharedPtr<LogicalNode> &logical_operator) const;

    // Read cache
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildReadCache(const SharedPtr<LogicalNode> &logical_operator) const;

    // Explain
    [[nodiscard]] UniquePtr<PhysicalOperator> BuildExplain(const SharedPtr<LogicalNode> &logical_operator) const;
};

} // namespace infinity
