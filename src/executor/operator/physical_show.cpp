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

#include <string>

module physical_show;

import stl;
import txn;
import query_context;

import profiler;
import operator_state;
import data_block;

import infinity_exception;
import table_entry_type;
import value_expression;
import logical_show;
import table_detail;

import value;
import table_def;
import data_table;
import third_party;
import index_def;
import index_ivfflat;
import index_base;
import index_hnsw;
import index_full_text;
import database_detail;
import default_values;
import defer_op;
import config;
import session;
import options;
import status;
import local_file_system;
import utility;
import buffer_manager;
import session_manager;
import compilation_config;
import logical_type;
import create_index_info;
import column_index_entry;
import segment_iter;

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

            output_names_->reserve(7);
            output_types_->reserve(7);

            output_names_->emplace_back("index_name");
            output_names_->emplace_back("index_type");
            output_names_->emplace_back("column_id");
            output_names_->emplace_back("column_name");
            output_names_->emplace_back("path");
            output_names_->emplace_back("index segments");
            output_names_->emplace_back("other_parameters");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(bigint_type);
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
            output_names_->emplace_back("command parsing");
            output_names_->emplace_back("logical plann building");
            output_names_->emplace_back("plan optimizing");
            output_names_->emplace_back("physical plann building");
            output_names_->emplace_back("pipeline building");
            output_names_->emplace_back("task building");
            output_names_->emplace_back("execution");
            output_names_->emplace_back("commit");
            output_names_->emplace_back("rollback");
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
            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowSegments: {
            output_names_->reserve(2);
            output_types_->reserve(2);

            output_names_->emplace_back("path");
            output_names_->emplace_back("size");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowSessionStatus: {
            output_names_->reserve(2);
            output_types_->reserve(2);

            output_names_->emplace_back("name");
            output_names_->emplace_back("value");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowGlobalStatus: {
            output_names_->reserve(2);
            output_types_->reserve(2);

            output_names_->emplace_back("name");
            output_names_->emplace_back("value");

            output_types_->emplace_back(varchar_type);
            output_types_->emplace_back(varchar_type);
            break;
        }
        case ShowType::kShowVar: {
            output_names_->reserve(1);
            output_types_->reserve(1);
            output_names_->emplace_back("value");
            output_types_->emplace_back(varchar_type);
            break;
        }
        default: {
            RecoverableError(Status::NotSupport("Not implemented show type"));
        }
    }
}

bool PhysicalShow::Execute(QueryContext *query_context, OperatorState *operator_state) {
    ShowOperatorState *show_operator_state = (ShowOperatorState *)(operator_state);
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
        case ShowType::kShowSegments: {
            ExecuteShowSegments(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowViews: {
            ExecuteShowViews(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowSessionStatus: {
            ExecuteShowSessionStatus(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowGlobalStatus: {
            ExecuteShowGlobalStatus(query_context, show_operator_state);
            break;
        }
        case ShowType::kShowVar: {
            ExecuteShowVar(query_context, show_operator_state);
            break;
        }
        default: {
            UnrecoverableError("Invalid chunk scan type");
        }
    }
    return true;
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
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
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
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
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

    Vector<TableDetail> table_collections_detail;
    Status status = txn->GetTables(db_name_, table_collections_detail);

    if (!status.ok()) {
        show_operator_state->status_ = status;
        RecoverableError(status);
        return;
    }

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>>
        column_types{varchar_type, varchar_type, varchar_type, bigint_type, bigint_type, bigint_type, bigint_type, bigint_type};

    output_block_ptr->Init(column_types);

    for (auto &table_detail : table_collections_detail) {

        SizeT column_id = 0;
        {
            // Append schema name to the 0 column
            const String *db_name = table_detail.db_name_.get();
            Value value = Value::MakeVarchar(*db_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append table name to the 1 column
            const String *table_name = table_detail.table_name_.get();
            Value value = Value::MakeVarchar(*table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        TableEntryType table_type = table_detail.table_entry_type_;
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
                case TableEntryType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_detail.column_count_));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case TableEntryType::kCollectionEntry: {
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
                case TableEntryType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_detail.row_count_));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case TableEntryType::kCollectionEntry: {
                    // TODO: row count need to be given for collection.
                    Value value = Value::MakeBigInt(static_cast<i64>(0));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                default: {
                    UnrecoverableError("Invalid table type");
                }
            }
        }

        ++column_id;
        {
            // Append segment count the 5 column
            switch (table_type) {
                case TableEntryType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_detail.segment_count_));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case TableEntryType::kCollectionEntry: {
                    // TODO: segment count need to be given for collection.
                    Value value = Value::MakeBigInt(static_cast<i64>(0));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                default: {
                    UnrecoverableError("Invalid table type");
                }
            }
        }

        ++column_id;
        {
            // Append segment count the 5 column
            switch (table_type) {
                case TableEntryType::kTableEntry: {
                    Value value = Value::MakeBigInt(static_cast<i64>(table_detail.block_count_));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                case TableEntryType::kCollectionEntry: {
                    // TODO: segment count need to be given for collection.
                    Value value = Value::MakeBigInt(static_cast<i64>(0));
                    ValueExpression value_expr(value);
                    value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
                    break;
                }
                default: {
                    UnrecoverableError("Invalid table type");
                }
            }
        }

        ++column_id;
        {
            // Append block limit the 6 column
            SizeT default_row_size = table_detail.segment_capacity_;
            Value value = Value::MakeBigInt(default_row_size);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowViews(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Define output table schema
    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);

    // Get tables from catalog
    Txn *txn = query_context->GetTxn();

    Vector<ViewDetail> views_detail;
    Status status = txn->GetViews(db_name_, views_detail);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
    }

    // Prepare the output data block
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>>
        column_types{varchar_type, varchar_type, varchar_type, bigint_type, bigint_type, bigint_type, bigint_type, bigint_type};

    output_block_ptr->Init(column_types);

    for (auto &view_detail : views_detail) {

        SizeT column_id = 0;
        {
            // Append schema name to the 0 column
            const String *db_name = view_detail.db_name_.get();
            Value value = Value::MakeVarchar(*db_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append table name to the 1 column
            const String *table_name = view_detail.view_name_.get();
            Value value = Value::MakeVarchar(*table_name);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            // Append base table type to the 2 column
            Value value = Value::MakeBigInt(static_cast<i64>(view_detail.column_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
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
        MakeShared<ColumnDef>(8, varchar_type, "commit", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(9, varchar_type, "rollback", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(10, varchar_type, "total_cost", HashSet<ConstraintType>()),
    };

    auto catalog = txn->GetCatalog();
    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("profiles"), column_defs);

    // create data block for output state
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type,
                                             varchar_type};
    output_block_ptr->Init(column_types);

    auto records = catalog->GetProfilerRecords();
    for (SizeT i = 0; i < records.size(); ++i) {

        // Output record no
        ValueExpression record_no_expr(Value::MakeVarchar(fmt::format("{}", i)));
        record_no_expr.AppendToChunk(output_block_ptr->column_vectors[0]);

        // Output each query phase
        i64 total_cost{};
        SizeT column_count = column_defs.size();
        for (SizeT j = 0; j < column_count - 2; ++j) {
            i64 this_time = records[i]->ElapsedAt(j);
            total_cost += this_time;

            NanoSeconds duration(this_time);
            ValueExpression phase_cost_expr(Value::MakeVarchar(BaseProfiler::ElapsedToString(duration)));
            phase_cost_expr.AppendToChunk(output_block_ptr->column_vectors[j + 1]);
        }

        // Output total query duration
        NanoSeconds total_duration(total_cost);
        ValueExpression phase_cost_expr(Value::MakeVarchar(BaseProfiler::ElapsedToString(total_duration)));
        phase_cost_expr.AppendToChunk(output_block_ptr->column_vectors.back());
    }
    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

/**
 * @brief Execute Show table details statement (i.e. describe t1)
 * @param query_context
 * @param input_state
 * @param output_state
 */
void PhysicalShow::ExecuteShowColumns(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto txn = query_context->GetTxn();

    auto [table_entry, status] = txn->GetTableByName(db_name_, object_name_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "column_name", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "column_type", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(2, varchar_type, "constraint", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("Views"), column_defs);

    // create data block for output state
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(column_types);

    SizeT column_count = table_entry->ColumnCount();
    for (SizeT input_column_id = 0; input_column_id < column_count; ++input_column_id) {
        const ColumnDef *column = table_entry->GetColumnDefByID(input_column_id);

        SizeT output_column_idx = 0;
        {
            // Append column name to the first column
            Value value = Value::MakeVarchar(column->name());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[output_column_idx]);
        }

        ++output_column_idx;
        {
            // Append column type to the second column, if the column type is embedded type, append the embedded type
            String column_type;
            if (column->type()->type() == kEmbedding) {
                auto type = column->type();
                auto embedding_type = type->type_info()->ToString();
                column_type = fmt::format("{}({})", type->ToString(), embedding_type);

            } else {
                column_type = column->type()->ToString();
            }
            Value value = Value::MakeVarchar(column_type);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[output_column_idx]);
        }

        ++output_column_idx;
        {
            // Append column constraint to the third column
            String column_constraint;
            for (auto &constraint : column->constraints_) {
                column_constraint += " " + ConstrainTypeToString(constraint);
            }

            Value value = Value::MakeVarchar(column_constraint);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[output_column_idx]);
        }
    }
    output_block_ptr->Finalize();

    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowSegments(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto txn = query_context->GetTxn();
    TxnTimeStamp begin_ts = txn->BeginTS();

    auto [table_entry, status] = txn->GetTableByName(db_name_, object_name_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);

    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "path", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "size", HashSet<ConstraintType>()),
    };

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    auto chuck_filling = [&](const std::function<u64(const String &)> &file_size_func, const String &path) {
        SizeT column_id = 0;
        {
            Value value = Value::MakeVarchar(path);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(Utility::FormatByteSize(file_size_func(path)));

            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
        }
    };
    Vector<SharedPtr<DataType>> column_types{
        varchar_type,
        varchar_type,
    };
    output_block_ptr->Init(column_types);

    if (segment_id_.has_value() && block_id_.has_value()) {
        if (auto *segment_entry = table_entry->GetSegmentByID(*segment_id_, begin_ts); segment_entry) {
            auto *block_entry = segment_entry->GetBlockEntryByID(*block_id_);
            if (block_entry != nullptr) {
                auto version_path = block_entry->VersionFilePath();

                chuck_filling(LocalFileSystem::GetFileSizeByPath, version_path);
                SizeT column_count = table_entry->ColumnCount();
                for (SizeT column_id = 0; column_id < column_count; ++column_id) {
                    auto block_column_entry = block_entry->GetColumnBlockEntry(column_id);
                    auto col_file_path = block_column_entry->FilePath();

                    chuck_filling(LocalFileSystem::GetFileSizeByPath, col_file_path);
                    for (auto &outline : block_column_entry->OutlinePaths()) {
                        chuck_filling(LocalFileSystem::GetFileSizeByPath, outline);
                    }
                }
            }
        }
    } else if (segment_id_.has_value()) {
        if (auto *segment_entry = table_entry->GetSegmentByID(*segment_id_, begin_ts); segment_entry) {
            auto block_entry_iter = BlockEntryIter(segment_entry);
            for (auto *block_entry = block_entry_iter.Next(); block_entry != nullptr; block_entry = block_entry_iter.Next()) {
                auto dir_path = block_entry->DirPath();

                chuck_filling(LocalFileSystem::GetFolderSizeByPath, dir_path);
            }
        }
    } else {
        for (auto &[_, segment] : table_entry->segment_map()) { // FIXME: use table_ref here.
            const auto &dir_path = *segment->segment_dir();

            chuck_filling(LocalFileSystem::GetFolderSizeByPath, dir_path);
        }
    }
    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

// Execute describe system table
void PhysicalShow::ExecuteShowConfigs(QueryContext *query_context, ShowOperatorState *show_operator_state) {
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
    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
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
            Value value = Value::MakeVarchar(fmt::format("{}-{}", global_config->time_zone(), global_config->time_zone_bias()));
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
            Value value = Value::MakeVarchar("worker CPU limit");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar(std::to_string(global_config->worker_cpu_limit()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
        {
            // option name type
            Value value = Value::MakeVarchar("Total worker count used for computation");
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
            Value value = Value::MakeVarchar("enable_profile");
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
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowIndexes(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    auto txn = query_context->GetTxn();

    auto [table_entry, table_status] = txn->GetTableByName(db_name_, object_name_);
    if (!table_status.ok()) {
        show_operator_state->status_ = table_status;
        //        Error<UnrecoverableException>(table_status.message());
        return;
    }

    auto varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    auto bigint_type = MakeShared<DataType>(LogicalType::kBigInt);
    Vector<SharedPtr<ColumnDef>> column_defs = {MakeShared<ColumnDef>(0, varchar_type, "index_name", HashSet<ConstraintType>()),
                                                MakeShared<ColumnDef>(1, varchar_type, "method_type", HashSet<ConstraintType>()),
                                                MakeShared<ColumnDef>(2, bigint_type, "column_id", HashSet<ConstraintType>()),
                                                MakeShared<ColumnDef>(3, varchar_type, "column_name", HashSet<ConstraintType>()),
                                                MakeShared<ColumnDef>(4, varchar_type, "path", HashSet<ConstraintType>()),
                                                MakeShared<ColumnDef>(5, varchar_type, "index_segment", HashSet<ConstraintType>()),
                                                MakeShared<ColumnDef>(6, varchar_type, "other_parameters", HashSet<ConstraintType>())};

    auto table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("Views"), column_defs);

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> column_types{varchar_type, varchar_type, bigint_type, varchar_type, varchar_type, varchar_type, varchar_type};
    output_block_ptr->Init(column_types);

    for (const auto &[index_name, index_def_meta] : table_entry->index_meta_map()) {
        auto [table_index_entry, status] = index_def_meta->GetEntry(txn->TxnID(), txn->BeginTS());
        if (!status.ok()) {
            // Index isn't found.
            continue;
        }

        for (const auto &column_index_entry_pair : table_index_entry->column_index_map()) {
            u64 index_column_id = column_index_entry_pair.first;
            ColumnIndexEntry *column_index_entry = column_index_entry_pair.second.get();
            const IndexBase *index_base = column_index_entry->index_base_ptr();
            SizeT column_id = 0;
            {
                // Append index name to the first column
                Value value = Value::MakeVarchar(index_name);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append index method type to the second column
                Value value = Value::MakeVarchar(IndexInfo::IndexTypeToString(index_base->index_type_));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append index column id
                Value value = Value::MakeBigInt(index_column_id);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append index column names to the third column
                String column_names;
                SizeT idx = 0;
                for (auto &column_name : index_base->column_names_) {
                    column_names += column_name;
                    if (idx < index_base->column_names_.size() - 1) {
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
                // Append index path
                Value value = Value::MakeVarchar(*column_index_entry->col_index_dir());
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append Index segment
                SizeT segment_count = table_entry->segment_map().size();
                SizeT index_segment_count = column_index_entry->index_by_segment().size();
                String result_value = fmt::format("{}/{}", index_segment_count, segment_count);
                Value value = Value::MakeVarchar(result_value);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
            ++column_id;
            {
                // Append index other parameters to the fourth column
                String other_parameters;
                switch (index_base->index_type_) {
                    case IndexType::kIVFFlat: {
                        const IndexIVFFlat *index_ivfflat = static_cast<const IndexIVFFlat *>(index_base);
                        other_parameters = fmt::format("metric = {}, centroids_count = {}",
                                                       MetricTypeToString(index_ivfflat->metric_type_),
                                                       index_ivfflat->centroids_count_);
                        break;
                    }
                    case IndexType::kHnsw: {
                        const IndexHnsw *index_hnsw = static_cast<const IndexHnsw *>(index_base);
                        other_parameters = fmt::format("metric = {}, encode_type = {}, M = {}, ef_construction = {}, ef = {}",
                                                       MetricTypeToString(index_hnsw->metric_type_),
                                                       HnswEncodeTypeToString(index_hnsw->encode_type_),
                                                       index_hnsw->M_,
                                                       index_hnsw->ef_construction_,
                                                       index_hnsw->ef_);
                        break;
                    }
                    case IndexType::kFullText: {
                        const IndexFullText *index_full_text = static_cast<const IndexFullText *>(index_base);
                        other_parameters = fmt::format("analyzer = {}", index_full_text->analyzer_);
                        break;
                    }
                    case IndexType::kSecondary: {
                        // there is no other_parameters
                        break;
                    }
                    case IndexType::kInvalid: {
                        UnrecoverableError("Invalid index method type");
                    }
                    default: {
                        RecoverableError(Status::NotSupport("Not implemented"));
                        break;
                    }
                }
                Value value = Value::MakeVarchar(other_parameters);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
            }
        }
    }
    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
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
                column_constraint += " " + ConstrainTypeToString(constraint);
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

void PhysicalShow::ExecuteShowSessionStatus(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    Vector<SharedPtr<ColumnDef>> output_column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "name", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "value", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("session status"), output_column_defs);
    output_ = MakeShared<DataTable>(table_def, TableType::kResult);

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> output_column_types{
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(output_column_types);

    {
        {
            // option name
            Value value = Value::MakeVarchar("query count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option value
            SizeT query_count = query_context->current_session()->query_count();
            Value value = Value::MakeVarchar(std::to_string(query_count));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowGlobalStatus(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    Vector<SharedPtr<ColumnDef>> output_column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "name", HashSet<ConstraintType>()),
        MakeShared<ColumnDef>(1, varchar_type, "value", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("global status"), output_column_defs);
    output_ = MakeShared<DataTable>(table_def, TableType::kResult);

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> output_column_types{
        varchar_type,
        varchar_type,
    };

    output_block_ptr->Init(output_column_types);

    {
        {
            // option name
            Value value = Value::MakeVarchar("buffer pool usage");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option value
            BufferManager *buffer_manager = query_context->storage()->buffer_manager();
            u64 memory_limit = buffer_manager->memory_limit();
            u64 memory_usage = buffer_manager->memory_usage();
            Value value = Value::MakeVarchar(fmt::format("{}/{}", Utility::FormatByteSize(memory_usage), Utility::FormatByteSize(memory_limit)));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
    }

    {
        {
            // option name
            Value value = Value::MakeVarchar("session count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
        }
        {
            // option value
            SessionManager *session_manager = query_context->session_manager();
            u64 session_count = session_manager->GetSessionCount();
            Value value = Value::MakeVarchar(std::to_string(session_count));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[1]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

void PhysicalShow::ExecuteShowVar(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    SharedPtr<DataType> varchar_type = MakeShared<DataType>(LogicalType::kVarchar);
    Vector<SharedPtr<ColumnDef>> output_column_defs = {
        MakeShared<ColumnDef>(0, varchar_type, "value", HashSet<ConstraintType>()),
    };

    SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default"), MakeShared<String>("variables"), output_column_defs);
    output_ = MakeShared<DataTable>(table_def, TableType::kResult);

    UniquePtr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    Vector<SharedPtr<DataType>> output_column_types{
        varchar_type,
    };

    output_block_ptr->Init(output_column_types);

    SysVar system_var = SystemVariables::GetSysVarEnumByName(object_name_);
    switch (system_var) {
        case SysVar::kQueryCount: {
            SizeT query_count = query_context->current_session()->query_count();
            Value value = Value::MakeVarchar(std::to_string(query_count));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SysVar::kSessionCount: {
            SessionManager *session_manager = query_context->session_manager();
            u64 session_count = session_manager->GetSessionCount();
            Value value = Value::MakeVarchar(std::to_string(session_count));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SysVar::kBufferPoolUsage: {
            BufferManager *buffer_manager = query_context->storage()->buffer_manager();
            u64 memory_limit = buffer_manager->memory_limit();
            u64 memory_usage = buffer_manager->memory_usage();
            Value value = Value::MakeVarchar(fmt::format("{}/{}", Utility::FormatByteSize(memory_usage), Utility::FormatByteSize(memory_limit)));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SysVar::kVersion: {
            Value value = Value::MakeVarchar(fmt::format("{}-{}", query_context->global_config()->version(), git_commit_id()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SysVar::kQueryMemoryLimit: {
            u64 query_memory_limit = query_context->global_config()->query_memory_limit();
            Value value = Value::MakeVarchar(std::to_string(query_memory_limit));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SysVar::kQueryCpuLimit: {
            u64 query_cpu_limit = query_context->global_config()->query_cpu_limit();
            Value value = Value::MakeVarchar(std::to_string(query_cpu_limit));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SysVar::kLogLevel: {
            String log_level_str = LogLevel2Str(query_context->global_config()->log_level());
            Value value = Value::MakeVarchar(log_level_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SysVar::kSchedulePolicy: {
            String scheduler_policy = "Round Robin";
            Value value = Value::MakeVarchar(scheduler_policy);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SysVar::kListenAddress: {
            Value value = Value::MakeVarchar(query_context->global_config()->listen_address());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SysVar::kSQLPort: {
            Value value = Value::MakeVarchar(std::to_string(query_context->global_config()->pg_port()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SysVar::kSDKPort: {
            Value value = Value::MakeVarchar(std::to_string(query_context->global_config()->sdk_port()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SysVar::kHttpAPIPort: {
            Value value = Value::MakeVarchar(std::to_string(query_context->global_config()->http_port()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SysVar::kDataURL: {
            Value value = Value::MakeVarchar(query_context->global_config()->data_dir()->c_str());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        case SysVar::kTimezone: {
            String time_zone = fmt::format("{}-{}", query_context->global_config()->time_zone(), query_context->global_config()->time_zone_bias());
            Value value = Value::MakeVarchar(time_zone);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors[0]);
            break;
        }
        default: {
            RecoverableError(Status::NoSysVar(object_name_));
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}

} // namespace infinity
