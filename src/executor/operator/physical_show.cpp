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

#include <iostream>
#include <string>
import stl;
import txn;
import query_context;
import parser;
import profiler;
import operator_state;
import data_block;

import infinity_exception;
import table_collection_type;
import value_expression;
import logical_show;
import table_collection_detail;
import view_entry;
import base_entry;
import value;
import table_collection_entry;
import table_def;
import data_table;
import third_party;
import index_def_meta;
import index_def_entry;
import index_def;
import index_entry;
import ivfflat_index_def;
import database_detail;
import default_values;
import defer_op;
import config;
import session;
import options;
import status;

module physical_show;

namespace infinity {

void PhysicalShow::Init() {
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    output_names_ = MakeShared<Vector<String>>();
    output_types_ = MakeShared<Vector<SharedPtr<DataType>>>();

    switch (scan_type_) {
        case ShowType::kShowDatabases: {
            output_names_->reserve(1);
            output_types_->reserve(1);
            output_names_->emplace_back("database");
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowTables: {

            output_names_->reserve(8);
            output_types_->reserve(8);

            output_names_->emplace_back("database");
            output_names_->emplace_back("table");
            output_names_->emplace_back("type");
            output_names_->emplace_back("column_count");
            output_names_->emplace_back("row_count");
            output_names_->emplace_back("segment_count");
            output_names_->emplace_back("block_count");
            output_names_->emplace_back("segment_capacity");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            output_types_->emplace_back(bigint_type);
            break;
        }
        case ShowType::kShowColumn: {

            output_names_->reserve(3);
            output_types_->reserve(3);

            output_names_->emplace_back("column_name");
            output_names_->emplace_back("column_type");
            output_names_->emplace_back("constraint");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowIndexes: {

            output_names_->reserve(4);
            output_types_->reserve(4);

            output_names_->emplace_back("index_name");
            output_names_->emplace_back("method_type");
            output_names_->emplace_back("column_names");
            output_names_->emplace_back("other_parameters");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowConfigs: {

            output_names_->reserve(3);
            output_types_->reserve(3);

            output_names_->emplace_back("config_name");
            output_names_->emplace_back("value");
            output_names_->emplace_back("description");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowProfiles: {

            output_names_->reserve(9);
            output_types_->reserve(9);

            output_names_->emplace_back("record_no");
            output_names_->emplace_back("parser");
            output_names_->emplace_back("logical planner");
            output_names_->emplace_back("optimizer");
            output_names_->emplace_back("physical planner");
            output_names_->emplace_back("pipeline builder");
            output_names_->emplace_back("task builder");
            output_names_->emplace_back("executor");
            output_names_->emplace_back("total_cost");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        default: {
            Error<NotImplementException>("Not implemented show type");
        }
    }
}

void PhysicalShow::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto show_operator_state = (ShowOperatorState *)(operator_state);
    DeferFn defer_fn([&]() { show_operator_state->SetComplete(); });

    switch (scan_type_) {
        case ShowType::kShowDatabases: {
            ExecuteShowDatabases(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowTables: {
            ExecuteShowTable(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowColumn: {
            ExecuteShowColumns(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowIndexes: {
            ExecuteShowIndexes(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowConfigs: {
            ExecuteShowConfigs(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowProfiles: {
            ExecuteShowProfiles(query_context, show_operator_state);
            break;
        }
        default: {
            Error<ExecutorException>("Invalid chunk scan type");
        }
    }
}

/**
 * @brief Execute show table
 * @param query_context
 * @param input_state
 * @param output_state
 */
void PhysicalShow::ExecuteShowDatabases(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table schema
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    // Get tables from catalog
    Txn *txn = query_context->GetTxn();

    Vector<DatabaseDetail> databases_detail = txn->ListDatabases();

    // Prepare the output data block
    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> column_types{varchar_type};

    output_block_ptr->Init(column_types);

    for (auto &database_detail : databases_detail) {

        SizeT column_id = 0;
        {
            // Append schema name to the 0 column
            const String *db_name = database_detail.db_name_.get();
            Value value = Value::MakeVarchar(*db_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(output_block_ptr);
}

/**
 * @brief Execute show table
 * @param query_context
 * @param input_state
 * @param output_state
 */
void PhysicalShow::ExecuteShowTable(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table schema
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // Get tables from catalog
    Txn *txn = query_context->GetTxn();

    Vector<TableCollectionDetail> table_collections_detail;
    Status status = txn->GetTableCollections(db_name_, table_collections_detail);
    if (!status.ok()) {
        Error<ExecutorException>(status.message());
    }

    // Prepare the output data block
    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>>
        column_types{varchar_type, varchar_type, varchar_type, bigint_type, bigint_type, bigint_type, bigint_type, bigint_type};

    output_block_ptr->Init(column_types);

    for (auto &table_collection_detail : table_collections_detail) {

        SizeT column_id = 0;
        {
            // Append schema name to the 0 column
            const String *db_name = table_collection_detail.db_name_.get();
            Value value = Value::MakeVarchar(*db_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append table name to the 1 column
            const String *table_name = table_collection_detail.table_collection_name_.get();
            Value value = Value::MakeVarchar(*table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        TableCollectionType table_type = table_collection_detail.table_collection_type_;
        {
            // Append base table type to the 2 column
            const String &base_table_type_str = ToString(table_type);
            Value value = Value::MakeVarchar(base_table_type_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append column count the 3 column
            switch (table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_detail.column_count_));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case TableCollectionType::kCollectionEntry: {
                    // TODO: column count need to be given for table.
                    Value value = Value::MakeBigInt(static_cast<i64>(0));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
            }
        }

        ++column_id;
        {
            // Append row count the 4 column
            switch (table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_detail.row_count_));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case TableCollectionType::kCollectionEntry: {
                    // TODO: row count need to be given for collection.
                    Value value = Value::MakeBigInt(static_cast<i64>(0));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                default: {
                    Error<ExecutorException>("Invalid table type");
                }
            }
        }

        ++column_id;
        {
            // Append segment count the 5 column
            switch (table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_detail.segment_count_));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case TableCollectionType::kCollectionEntry: {
                    // TODO: segment count need to be given for collection.
                    Value value = Value::MakeBigInt(static_cast<i64>(0));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                default: {
                    Error<ExecutorException>("Invalid table type");
                }
            }
        }

        ++column_id;
        {
            // Append segment count the 5 column
            switch (table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_detail.block_count_));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case TableCollectionType::kCollectionEntry: {
                    // TODO: segment count need to be given for collection.
                    Value value = Value::MakeBigInt(static_cast<i64>(0));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                default: {
                    Error<ExecutorException>("Invalid table type");
                }
            }
        }

        ++column_id;
        {
            // Append block limit the 6 column
            SizeT default_row_size = table_collection_detail.segment_capacity_;
            Value value = Value::MakeBigInt(default_row_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(output_block_ptr);
}

void PhysicalShow::ExecuteShowProfiles(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto txn = query_context->GetTxn();
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "profile_no", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "parser", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(2, varchar_type, "logical_plan", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(3, varchar_type, "optimizer", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(4, varchar_type, "physical_plan", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(5, varchar_type, "pipeline_build", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(6, varchar_type, "task_build", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(7, varchar_type, "execution", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(8, varchar_type, "total_cost", HashSet<ConstraintType>()),
    };

    auto catalog = txn->GetCatalog();
    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("profiles"), column_defs);

    // create data block for output state
    auto output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>>
        column_types{varchar_type, varchar_type, varchar_type, varchar_type, varchar_type, varchar_type, varchar_type, varchar_type, varchar_type};
    output_block_ptr->Init(column_types);

    auto records = catalog->GetProfilerRecords();
    for (SizeT i = 0; i < records.size(); ++i) {

        // Output record no
        ValueExpression record_no_expr(Value::MakeVarchar(Format("{}", i)));
        record_no_expr.AppendToChunk(output_block_ptr->column_vectors[0]);

        // Output each query phase
        i64 total_cost{};
        for (SizeT j = 0; j < 7; ++j) {
            i64 this_time = records[i]->ElapsedAt(j);
            total_cost += this_time;

            NanoSeconds duration(this_time);
            ValueExpression phase_cost_expr(Value::MakeVarchar(BaseProfiler::ElapsedToString(duration)));
            phase_cost_expr.AppendToChunk(output_block_ptr->column_vectors[j + 1]);
        }

        // Output total query duration
        NanoSeconds total_duration(total_cost);
        ValueExpression phase_cost_expr(Value::MakeVarchar(BaseProfiler::ElapsedToString(total_duration)));
        phase_cost_expr.AppendToChunk(output_block_ptr->column_vectors[8]);

        output_block_ptr->Finalize();
    }
    show_operator_state->output_.emplace_back(Move(output_block_ptr));
}

/**
 * @brief Execute Show table details statement (i.e. describe t1)
 * @param query_context
 * @param input_state
 * @param output_state
 */
void PhysicalShow::ExecuteShowColumns(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto txn = query_context->GetTxn();

    BaseEntry *base_table_entry{nullptr};
    Status status = txn->GetTableByName(db_name_, object_name_, base_table_entry);
    if (!status.ok()) {
        show_operator_state->error_message_ = Move(status.msg_);
        Error<ExecutorException>(Format("{} isn't found", object_name_));
        return;
    }

    auto table_collection_entry = dynamic_cast<TableCollectionEntry *>(base_table_entry);

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "column_name", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "column_type", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(2, varchar_type, "constraint", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("Views"), column_defs);

    // create data block for output state
    auto output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(column_types);

    for (auto &column : table_collection_entry->columns_) {
        SizeT column_id = 0;
        {
            // Append column name to the first column
            Value value = Value::MakeVarchar(column->name());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append column type to the second column, if the column type is embedded type, append the embedded type
            String column_type;
            if (column->type()->type() == kEmbedding) {
                auto type = column->type();
                auto embedding_type = type->type_info()->ToString();
                column_type = Format("{}({})", type->ToString(), embedding_type);

            } else {
                column_type = column->type()->ToString();
            }
            Value value = Value::MakeVarchar(column_type);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append column constraint to the third column
            String column_constraint;
            for (auto &constraint : column->constraints_) {
                column_constraint += " " + ConstrainType2String(constraint);
            }

            Value value = Value::MakeVarchar(column_constraint);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
        output_block_ptr->Finalize();
    }

    show_operator_state->output_.emplace_back(Move(output_block_ptr));
}

// Execute describe system table
void PhysicalShow::ExecuteShowConfigs(QueryContext *query_context, ShowOperatorState *show_operator_state) {

    auto txn = query_context->GetTxn();
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "config_name", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "value", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(2, varchar_type, "description", HashSet<ConstraintType>()),
    };

    const Config *global_config = query_context->global_config();
    const SessionOptions *session_options = query_context->current_session()->options();

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("configs"), column_defs);

    // create data block for output state
    auto output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(column_types);

    // Config
    {
        {
            // option name
            Value value = Value::MakeVarchar("version");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option value

            Value value = Value::MakeVarchar(global_config->version());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option description
            Value value = Value::MakeVarchar("Infinity version.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("time_zone");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(Format("{}-{}", global_config->time_zone(), global_config->time_zone_bias()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Timezone");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("listen_ip_address");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(global_config->listen_address());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Infinity server listen ip address");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("postgres port");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->pg_port()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Postgres port");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("http port");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->http_port()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("HTTP port (Not available now)");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("SDK port");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->sdk_port()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("HTTP port (Not available now)");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("log_filename");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(*global_config->log_filename());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Log file name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("log_dir");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(*global_config->log_dir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Log directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("log_to_stdout");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = global_config->log_to_stdout() ? Value::MakeVarchar("True") : Value::MakeVarchar("False");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("If log is also output to standard output");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("log_max_size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->log_max_size()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Max log file size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("log_file_rotate_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->log_file_rotate_count()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Log files rotation limitation");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("log_level");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(LogLevel2Str(global_config->log_level()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Log level");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("data_dir");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(*global_config->data_dir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Data directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("wal_dir");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(*global_config->wal_dir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Write-Ahead Log directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("default_row_size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->default_row_size()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Default row size (not used)");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("buffer_pool_size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->buffer_pool_size()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Buffer pool size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("temp_dir");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(*global_config->temp_dir());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Temporary data directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("full_checkpoint_interval_sec");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->full_checkpoint_interval_sec()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Full check point interval(seconds)");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("delta_checkpoint_interval_sec");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->delta_checkpoint_interval_sec()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Incremental check point interval(seconds)");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("delta_checkpoint_interval_wal_bytes");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->delta_checkpoint_interval_wal_bytes()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Incremental check point size limits(bytes)");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("wal_file_size_threshold");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->wal_size_threshold()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("WAL File size limit");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("enable profiling");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = session_options->enable_profiling() ? Value::MakeVarchar("True") : Value::MakeVarchar("False");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Whether profiling is enable.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("history profiling records capacity");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(session_options->profile_history_capacity()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("History profiling record limit.");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[2]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(Move(output_block_ptr));
}

void PhysicalShow::ExecuteShowIndexes(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto txn = query_context->GetTxn();

    BaseEntry *base_table_entry{nullptr};
    Status table_status = txn->GetTableByName(db_name_, object_name_, base_table_entry);
    if (!table_status.ok()) {
        show_operator_state->error_message_ = Move(table_status.msg_);
        Error<ExecutorException>(table_status.message());
        return;
    }

    auto table_collection_entry = static_cast<TableCollectionEntry *>(base_table_entry);

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    Vector<SharedPtr<ColumnDef>> column_defs = {MakeShared<ColumnDef>(0, varchar_type, "index_name", HashSet<ConstraintType>()),
                                                MakeShared<ColumnDef>(1, varchar_type, "method_type", HashSet<ConstraintType>()),
                                                MakeShared<ColumnDef>(2, varchar_type, "column_names", HashSet<ConstraintType>()),
                                                MakeShared<ColumnDef>(3, varchar_type, "other_parameters", HashSet<ConstraintType>())};

    auto table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("Views"), column_defs);

    auto output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> column_types(4, varchar_type);
    output_block_ptr->Init(column_types);

    for (const auto &[index_name, index_def_meta] : table_collection_entry->indexes_) {
        BaseEntry* base_entry{nullptr};
        Status status = IndexDefMeta::GetEntry(index_def_meta.get(), txn->TxnID(), txn->BeginTS(), base_entry);
        if (!status.ok()) {
            // Index isn't found.
            continue;
        }
        auto index_def_entry = static_cast<IndexDefEntry *>(base_entry);
        auto index_def = index_def_entry->index_def_.get();
        SizeT column_id = 0;
        {
            // Append index name to the first column
            Value value = Value::MakeVarchar(*index_def->index_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
        ++column_id;
        {
            // Append index method type to the second column
            Value value = Value::MakeVarchar(IndexMethodToString(index_def->method_type_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
        ++column_id;
        {
            // Append index column names to the third column
            String column_names;
            SizeT idx = 0;
            for (auto &column_name : index_def->column_names_) {
                column_names += column_name;
                if (idx < index_def->column_names_.size() - 1) {
                    column_names += ",";
                }
                idx++;
            }
            Value value = Value::MakeVarchar(column_names);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
        ++column_id;
        {
            // Append index other parameters to the fourth column
            String other_parameters;
            switch (index_def->method_type_) {
                case IndexMethod::kIVFFlat: {
                    auto ivfflat_index_def = static_cast<IVFFlatIndexDef *>(index_def);
                    other_parameters = Format("metric = {}, centroids_count = {}",
                                              MetricTypeToString(ivfflat_index_def->metric_type_),
                                              ivfflat_index_def->centroids_count_);
                    break;
                }
                case IndexMethod::kInvalid: {
                    Error<ExecutorException>("Invalid index method type");
                }
                default: {
                    Error<NotImplementException>("Not implemented");
                    break;
                }
            }
            Value value = Value::MakeVarchar(other_parameters);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }
    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(Move(output_block_ptr));
}

void PhysicalShow::ExecuteShowTable(QueryContext *query_context) {
    // Define output table schema
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    SharedPtr<DataType> bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "schema", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "table", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(2, varchar_type, "type", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(3, bigint_type, "column_count", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(4, bigint_type, "row_count", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(5, bigint_type, "block_count", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(6, bigint_type, "block_size", HashSet<ConstraintType>()),
    };

    auto table_def = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<DataTable>(table_def, TableType::kResult);

    // Get tables from catalog
    // TODO: Use context to carry runtime information, such as current schema
    Txn *txn = query_context->GetTxn();

    Vector<TableCollectionDetail> table_collections_detail;
    Status status = txn->GetTableCollections(db_name_, table_collections_detail);
    if (!status.ok()) {
        Error<ExecutorException>(status.message());
    }

    // Prepare the output data block
    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type, varchar_type, bigint_type, bigint_type, bigint_type, bigint_type};

    output_block_ptr->Init(column_types);

    for (auto &table_collection_detail : table_collections_detail) {

        SizeT column_id = 0;
        {
            // Append schema name to the 0 column
            const String *db_name = table_collection_detail.db_name_.get();
            Value value = Value::MakeVarchar(*db_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append table name to the 1 column
            const String *table_name = table_collection_detail.table_collection_name_.get();
            Value value = Value::MakeVarchar(*table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        TableCollectionType table_type = table_collection_detail.table_collection_type_;
        {
            // Append base table type to the 2 column
            const String &base_table_type_str = ToString(table_type);
            Value value = Value::MakeVarchar(base_table_type_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append column count the 3 column
            switch (table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_detail.column_count_));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case TableCollectionType::kCollectionEntry: {
                    // TODO: column count need to be given for table.
                    Value value = Value::MakeBigInt(static_cast<i64>(0));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
            }
        }

        ++column_id;
        {
            // Append row count the 4 column
            switch (table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_detail.row_count_));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case TableCollectionType::kCollectionEntry: {
                    // TODO: row count need to be given for collection.
                    Value value = Value::MakeBigInt(static_cast<i64>(0));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                default: {
                    Error<ExecutorException>("Invalid table type");
                }
            }
        }

        ++column_id;
        {
            // Append segment count the 5 column
            switch (table_type) {
                case TableCollectionType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_collection_detail.segment_count_));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case TableCollectionType::kCollectionEntry: {
                    // TODO: segment count need to be given for collection.
                    Value value = Value::MakeBigInt(static_cast<i64>(0));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                default: {
                    Error<ExecutorException>("Invalid table type");
                }
            }
        }

        ++column_id;
        {
            // Append block limit the 6 column
            SizeT default_row_size = table_collection_detail.segment_capacity_;
            Value value = Value::MakeBigInt(default_row_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    output_->Append(output_block_ptr);
}

void PhysicalShow::ExecuteShowViews(QueryContext *query_context) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    SharedPtr<DataType> bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // Define output table schema
    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "schema", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "view", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(3, bigint_type, "column_count", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Views"), column_defs);
    output_ = MakeShared<DataTable>(table_def, TableType::kResult);

    // Get tables from catalog
    // TODO: Use context to carry runtime information, such as current schema
    Txn *txn = query_context->GetTxn();
    Vector<BaseEntry *> views = txn->GetViews(db_name_);

    // Prepare the output data block
    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        bigint_type,
    };

    output_block_ptr->Init(column_types);

    for (auto &base_entry : views) {
        ViewEntry *view_entry = static_cast<ViewEntry *>(base_entry);

        SizeT column_id = 0;
        {
            // Append schema name to the first column
            String &db_name = db_name_;
            Value value = Value::MakeVarchar(db_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append table name to the second column
            const String &table_name = *view_entry->view_name();
            Value value = Value::MakeVarchar(table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append column count to the third column
            Value value = Value::MakeBigInt(view_entry->column_names()->size());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    output_->Append(output_block_ptr);
}

void PhysicalShow::ExecuteShowColumns(QueryContext *query_context) {

    Txn *txn = query_context->GetTxn();

    BaseEntry *base_table_entry{nullptr};
    Status status = txn->GetTableByName(db_name_, object_name_, base_table_entry);
    if (!status.ok()) {
        Error<ExecutorException>(status.message());
    }

    TableCollectionEntry *table_collection_entry = dynamic_cast<TableCollectionEntry *>(base_table_entry);
    ExecuteShowTableDetail(query_context, table_collection_entry->columns_);

    BaseEntry* base_entry{nullptr};
    status = txn->GetViewByName(db_name_, object_name_, base_entry);
    if (!status.ok()) {
        Error<PlannerException>(status.message());
    }

    ViewEntry *view_entry = static_cast<ViewEntry *>(base_entry);

    ExecuteShowViewDetail(query_context, view_entry->column_types(), view_entry->column_names());

    Error<ExecutorException>(Format("No table, collection, or view name is {}.{}", this->db_name_, this->object_name_));
}

void PhysicalShow::ExecuteShowTableDetail(QueryContext *query_context, const Vector<SharedPtr<ColumnDef>> &table_collecton_columns) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "column_name", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "column_type", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(3, varchar_type, "constraint", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("Views"), column_defs);
    output_ = MakeShared<DataTable>(table_def, TableType::kResult);

    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(column_types);

    for (auto &column : table_collecton_columns) {

        SizeT column_id = 0;
        {
            // Append column name to the first column
            Value value = Value::MakeVarchar(column->name());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append column type to the second column
            String column_type = column->type()->ToString();
            Value value = Value::MakeVarchar(column_type);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append column constraint to the third column
            String column_constraint;
            for (auto &constraint : column->constraints_) {
                column_constraint += " " + ConstrainType2String(constraint);
            }

            Value value = Value::MakeVarchar(column_constraint);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    output_->Append(output_block_ptr);
}

void PhysicalShow::ExecuteShowViewDetail(QueryContext *query_context,
                                         const SharedPtr<Vector<SharedPtr<DataType>>> &view_column_types,
                                         const SharedPtr<Vector<String>> &view_column_names) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    Vector<SharedPtr<ColumnDef>> output_column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "column_name", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "column_type", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("Views"), output_column_defs);
    output_ = MakeShared<DataTable>(table_def, TableType::kResult);

    SharedPtr<DataBlock> output_block_ptr = DataBlock::Make();
    Vector<SharedPtr<DataType>> output_column_types{
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(output_column_types);

    SizeT column_count = view_column_types->size();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        SizeT column_id = 0;
        {
            // Append column name to the first column
            Value value = Value::MakeVarchar(view_column_names->at(idx));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append column type to the second column
            String column_type = view_column_types->at(idx)->ToString();
            Value value = Value::MakeVarchar(column_type);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    output_->Append(output_block_ptr);
}

} // namespace infinity
