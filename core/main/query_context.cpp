//
// Created by jinhai on 23-10-16.
//


module;

#include <sstream>

import stl;
import session;
import config;
import fragment_scheduler;
import storage;
import resource_manager;
import txn;
import parser;
import infinity_assert;
import infinity_exception;
import logical_planner;
import logical_node_type;
import data_block;
import optimizer;
import physical_planner;
import fragment_builder;
import bind_context;
import logical_node;
import physical_operator;

module query_context;


namespace infinity {

String QueryResult::ToString() const {
    std::stringstream ss;

    switch (root_operator_type_) {
        case LogicalNodeType::kInsert: {
            return "INSERT 0 1";
        }
        case LogicalNodeType::kUpdate: {
            return "UPDATE 0 1";
        }
        case LogicalNodeType::kDelete: {
            return "DELETE 0 1";
        }
        default: {
            ss << std::endl;
        }
    }

    SizeT column_count = result_->ColumnCount();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        String end;
        if (idx != column_count - 1) {
            end = " ";
        }
        ss << result_->GetColumnNameById(idx) << end;
    }
    ss << std::endl;

    // Get Block count
    SizeT block_count = result_->DataBlockCount();

    // Iterate all blocks
    for (SizeT idx = 0; idx < block_count; ++idx) {
        // Get current block
        SharedPtr<DataBlock> current_block = result_->GetDataBlockById(idx);

        ss << current_block->ToString();
    }

    return ss.str();
}

void QueryContext::Init(Session *session_ptr,
                        const Config *global_config_ptr,
                        FragmentScheduler *scheduler_ptr,
                        Storage *storage_ptr,
                        ResourceManager *resource_manager_ptr) {
    session_ptr_ = session_ptr;
    global_config_ = global_config_ptr;
    scheduler_ = scheduler_ptr;
    storage_ = storage_ptr;
    resource_manager_ = resource_manager_ptr;
    initialized_ = true;
    cpu_number_limit_ = resource_manager_ptr->GetCpuResource();
    memory_size_limit_ = resource_manager_ptr->GetMemoryResource();
}

QueryResult QueryContext::Query(const String &query) {

    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> parsed_result = MakeShared<ParserResult>();

    parser->Parse(query, parsed_result);

    if (parsed_result->IsError()) {
        Error<PlannerException>(parsed_result->error_message_, __FILE_NAME__, __LINE__);
    }
    LogicalPlanner logical_planner(this);
    Optimizer optimizer(this);
    PhysicalPlanner physical_planner(this);
    FragmentBuilder fragment_builder(this);

    Assert<PlannerException>(parsed_result->statements_ptr_->size() == 1, "Only support single statement.", __FILE_NAME__, __LINE__);
    for (BaseStatement *statement : *parsed_result->statements_ptr_) {
        QueryResult query_result;
        try {
            this->CreateTxn();
            this->BeginTxn();

            // Build unoptimized logical plan for each SQL statement.
            SharedPtr<BindContext> bind_context;
            logical_planner.Build(statement, bind_context);
            current_max_node_id_ = bind_context->GetNewLogicalNodeId();

            parsed_result->Reset();

            SharedPtr<LogicalNode> unoptimized_plan = logical_planner.LogicalPlan();

            // Apply optimized rule to the logical plan
            SharedPtr<LogicalNode> optimized_plan = optimizer.optimize(unoptimized_plan);

            // Build physical plan
            SharedPtr<PhysicalOperator> physical_plan = physical_planner.BuildPhysicalOperator(optimized_plan);

            // Fragment Builder, only for test now.
            // SharedPtr<PlanFragment> plan_fragment = fragment_builder.Build(physical_plan);
            auto plan_fragment = fragment_builder.BuildFragment(physical_plan.get());

            scheduler_->Schedule(this, plan_fragment.get());
            query_result.result_ = plan_fragment->GetResult();
            query_result.root_operator_type_ = unoptimized_plan->operator_type();

            this->CommitTxn();
        } catch (const Exception &e) {
            this->RollbackTxn();
            throw Exception(e.what());
        } catch (std::exception &e) {
            throw e;
        }
        return query_result;
    }

    Error<NetworkException>("Not reachable", __FILE_NAME__, __LINE__);
}

void QueryContext::CreateTxn() {
    if (session_ptr_->txn_ == nullptr) {
        session_ptr_->txn_ = storage_->txn_manager()->CreateTxn();
    }
}

void QueryContext::BeginTxn() { session_ptr_->txn_->BeginTxn(); }

void QueryContext::CommitTxn() {
    session_ptr_->txn_->CommitTxn();
    session_ptr_->txn_ = nullptr;
}

void QueryContext::RollbackTxn() {
    session_ptr_->txn_->RollbackTxn();
    session_ptr_->txn_ = nullptr;
}

Txn *QueryContext::GetTxn() const { return session_ptr_->txn_; }

} // namespace infinity
