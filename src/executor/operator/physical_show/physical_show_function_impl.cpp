module infinity_core:physical_show_function.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;
import :utility;

import std;

import compilation_config;

using namespace infinity;

void PhysicalShow::ExecuteShowFunction(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);

    std::string function_name = *function_name_;
    ToLower(function_name);
    if (function_name == "server_version") {

        std::string version_info =
            fmt::format("{}.{}.{} {} {} {}", version_major(), version_minor(), version_patch(), system_build_time(), build_type(), git_commit_id());
        {
            // name
            Value value = Value::MakeVarchar(version_info);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        fmt::print("Release: {}.{}.{} build on {} with {} mode from branch: {}, commit-id: {}\n",
                   version_major(),
                   version_minor(),
                   version_patch(),
                   system_build_time(),
                   build_type(),
                   git_branch_name(),
                   git_commit_id());
    } else {
        Status status = Status::Unknown(fmt::format("function: {}", function_name));
        RecoverableError(status);
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
    return;
}