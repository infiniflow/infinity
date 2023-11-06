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

export module physical_planner;
namespace infinity {

class QueryContext;

export class PhysicalPlanner {
public:
    explicit PhysicalPlanner(QueryContext *query_context_ptr) : query_context_ptr_(query_context_ptr) {}

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildPhysicalOperator(const SharedPtr<LogicalNode> &logical_operator) const;

private:
    QueryContext *query_context_ptr_;

    // Create operator
    [[nodiscard]] SharedPtr<PhysicalOperator> BuildCreateTable(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildCreateIndex(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildCreateCollection(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildCreateSchema(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildPreparedPlan(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildCreateView(const SharedPtr<LogicalNode> &logical_operator) const;

    // Drop operator
    [[nodiscard]] SharedPtr<PhysicalOperator> BuildDropTable(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildDropIndex(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildDropCollection(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildDropSchema(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildDropView(const SharedPtr<LogicalNode> &logical_operator) const;

    // Insert operator
    [[nodiscard]] SharedPtr<PhysicalOperator> BuildInsert(const SharedPtr<LogicalNode> &logical_operator) const;

    // Delete operator
    [[nodiscard]] SharedPtr<PhysicalOperator> BuildDelete(const SharedPtr<LogicalNode> &logical_operator) const;

    // Update operator
    [[nodiscard]] SharedPtr<PhysicalOperator> BuildUpdate(const SharedPtr<LogicalNode> &logical_operator) const;

    // Import operator
    [[nodiscard]] SharedPtr<PhysicalOperator> BuildImport(const SharedPtr<LogicalNode> &logical_operator) const;

    // Export operator
    [[nodiscard]] SharedPtr<PhysicalOperator> BuildExport(const SharedPtr<LogicalNode> &logical_operator) const;

    // Alter operator
    [[nodiscard]] SharedPtr<PhysicalOperator> BuildAlter(const SharedPtr<LogicalNode> &logical_operator) const;

    // Select operator
    [[nodiscard]] SharedPtr<PhysicalOperator> BuildAggregate(const SharedPtr<LogicalNode> &logical_operator) const;

    // Operator
    [[nodiscard]] SharedPtr<PhysicalOperator> BuildJoin(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildCrossProduct(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildSort(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildLimit(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildProjection(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildFilter(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildIntersect(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildUnion(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildExcept(const SharedPtr<LogicalNode> &logical_operator) const;

    // Scan
    [[nodiscard]] SharedPtr<PhysicalOperator> BuildShow(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildTableScan(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildViewScan(const SharedPtr<LogicalNode> &logical_operator) const;

    [[nodiscard]] SharedPtr<PhysicalOperator> BuildDummyScan(const SharedPtr<LogicalNode> &logical_operator) const;

    // Flush
    [[nodiscard]] SharedPtr<PhysicalOperator> BuildFlush(const SharedPtr<LogicalNode> &logical_operator) const;

    // Knn
    [[nodiscard]] SharedPtr<PhysicalOperator> BuildKnn(const SharedPtr<LogicalNode> &logical_operator) const;

    // Command
    [[nodiscard]] SharedPtr<PhysicalOperator> BuildCommand(const SharedPtr<LogicalNode> &logical_operator) const;

    // Explain
    [[nodiscard]] SharedPtr<PhysicalOperator> BuildExplain(const SharedPtr<LogicalNode> &logical_operator) const;
};

} // namespace infinity
