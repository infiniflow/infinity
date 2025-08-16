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

export module infinity_core:logical_planner;

import :bind_context;
import :load_meta;
import :logical_node;
import :status;

import internal_types;
import base_statement;
import select_statement;
import copy_statement;
import insert_statement;
import create_statement;
import drop_statement;
import show_statement;
import check_statement;
import flush_statement;
import optimize_statement;
import update_statement;
import delete_statement;
import prepare_statement;
import execute_statement;
import alter_statement;
import explain_statement;
import command_statement;
import compact_statement;
import data_type;
import extra_ddl_info;
import global_resource_usage;

namespace infinity {

export class LogicalPlanner {
public:
    explicit LogicalPlanner(QueryContext *query_context_ptr) : query_context_ptr_(query_context_ptr) {
        names_ptr_ = std::make_shared<std::vector<std::string>>();
        types_ptr_ = std::make_shared<std::vector<DataType>>();
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("LogicalPlanner");
#endif
    }

    ~LogicalPlanner() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("LogicalPlanner");
#endif
    }

    Status Build(const BaseStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildSelect(const SelectStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildInsert(InsertStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildInsertValue(const InsertStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildInsertSelect(const InsertStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Update operator
    Status BuildUpdate(const UpdateStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Delete operator
    Status BuildDelete(const DeleteStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Create operator
    Status BuildCreate(CreateStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildCreateDatabase(const CreateStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildCreateTable(const CreateStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildCreateCollection(const CreateStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildCreateView(const CreateStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildCreateIndex(const CreateStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Drop operator
    Status BuildDrop(DropStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildDropTable(const DropStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildDropCollection(const DropStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildDropSchema(const DropStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildDropIndex(const DropStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildDropView(const DropStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Prepare operator
    Status BuildPrepare(const PrepareStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Execute operator
    Status BuildExecute(const ExecuteStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildCopy(CopyStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Export operator
    Status BuildExport(const CopyStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Import operator
    Status BuildImport(const CopyStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Alter operator
    Status BuildAlter(AlterStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Show operator
    Status BuildShow(ShowStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Flush
    Status BuildFlush(const FlushStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildFlushCatalog(const FlushStatement *, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildFlushData(const FlushStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildFlushLog(const FlushStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildFlushBuffer(const FlushStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Optimize
    Status BuildOptimize(OptimizeStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildCommand(const CommandStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    Status BuildCompact(const CompactStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Explain
    Status BuildExplain(const ExplainStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    // Check
    Status BuildCheck(const CheckStatement *statement, std::shared_ptr<BindContext> &bind_context_ptr);

    [[nodiscard]] std::vector<std::shared_ptr<LogicalNode>> LogicalPlans() const {
        if (logical_plans_.empty()) {
            return {logical_plan_};
        }
        return logical_plans_;
    }

private:
    void BindSchemaName(std::string &schema_name) const;

private:
    static Status ValidIdentifier(const std::string &identifier);

    QueryContext *query_context_ptr_{};

    std::shared_ptr<std::vector<std::string>> names_ptr_{};
    std::shared_ptr<std::vector<DataType>> types_ptr_{};
    std::shared_ptr<LogicalNode> logical_plan_{};
    std::vector<std::shared_ptr<LogicalNode>> logical_plans_{};
};

} // namespace infinity
