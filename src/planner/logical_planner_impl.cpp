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

#include <cassert>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <memory>

module infinity_core:logical_planner.impl;

import :logical_planner;

import :stl;
import :bind_context;

import :infinity_exception;
import :query_binder;
import :bound_delete_statement;
import :bound_update_statement;
import :bound_select_statement;
import :bound_compact_statement;
import :insert_binder;
import :logical_insert;
import :logical_node;
import :cast_expression;
import :cast_function;
import :bound_cast_func;
import :base_expression;
import :third_party;
import :table_def;
import :logical_alter;
import :logical_create_table;
import :logical_create_collection;
import :logical_create_schema;
import :logical_create_view;
import :logical_create_index;
import :logical_delete;
import :logical_drop_table;
import :logical_drop_collection;
import :logical_drop_schema;
import :logical_drop_index;
import :logical_drop_view;
import :logical_show;
import :logical_flush;
import :logical_optimize;
import :logical_export;
import :logical_import;
import :logical_explain;
import :logical_command;
import :logical_check;
import :explain_logical_plan;
import :explain_ast;

import :virtual_store;

import :status;
import :default_values;
import :index_base;
import :index_ivf;
import :index_hnsw;
import :index_diskann;
import :index_secondary;
import :index_emvb;
import :index_bmp;
import :index_full_text;
import :base_table_ref;
import :table_ref;
import logical_type;
import parsed_expr;
import extra_ddl_info;
import create_schema_info;
import create_table_info;
import create_index_info;
import create_collection_info;
import create_view_info;
import drop_collection_info;
import embedding_info;
import array_info;
import type_info;
import drop_index_info;
import drop_schema_info;
import drop_table_info;
import drop_view_info;
import column_def;
import :logger;
import statement_common;
import :block_index;
import column_expr;
import function_expr;
import insert_row_expr;
import :special_function;
import :utility;
import :wal_manager;
import :infinity_context;

import :new_txn;
import :meta_info;
import :db_meeta;
import :table_meeta;
import :new_catalog;
import :kv_store;
import base_statement;
import select_statement;
import insert_statement;
import update_statement;
import delete_statement;
import create_statement;
import drop_statement;
import show_statement;
import flush_statement;
import copy_statement;
import explain_statement;
import prepare_statement;
import execute_statement;
import alter_statement;
import command_statement;
import compact_statement;
import check_statement;
import optimize_statement;
import data_type;
import :query_context;

namespace infinity {

Status LogicalPlanner::Build(const BaseStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    if (bind_context_ptr.get() == nullptr) {
        bind_context_ptr = BindContext::Make(nullptr);
    }
    switch (statement->Type()) {
        case StatementType::kSelect: {
            return BuildSelect(static_cast<const SelectStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kInsert: {
            StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
            if (storage_mode == StorageMode::kUnInitialized) {
                UnrecoverableError("Uninitialized storage mode");
            }

            if (storage_mode != StorageMode::kWritable) {
                return Status::InvalidNodeRole("Attempt to write on non-writable node");
            }
            return BuildInsert(const_cast<InsertStatement *>(static_cast<const InsertStatement *>(statement)), bind_context_ptr);
        }
        case StatementType::kUpdate: {
            StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
            if (storage_mode == StorageMode::kUnInitialized) {
                UnrecoverableError("Uninitialized storage mode");
            }

            if (storage_mode != StorageMode::kWritable) {
                return Status::InvalidNodeRole("Attempt to write on non-writable node");
            }
            return BuildUpdate(static_cast<const UpdateStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kDelete: {
            StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
            if (storage_mode == StorageMode::kUnInitialized) {
                UnrecoverableError("Uninitialized storage mode");
            }

            if (storage_mode != StorageMode::kWritable) {
                return Status::InvalidNodeRole("Attempt to write on non-writable node");
            }
            return BuildDelete(static_cast<const DeleteStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kCreate: {
            StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
            if (storage_mode == StorageMode::kUnInitialized) {
                UnrecoverableError("Uninitialized storage mode");
            }

            if (storage_mode != StorageMode::kWritable) {
                return Status::InvalidNodeRole("Attempt to write on non-writable node");
            }
            return BuildCreate(const_cast<CreateStatement *>(static_cast<const CreateStatement *>(statement)), bind_context_ptr);
        }
        case StatementType::kDrop: {
            StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
            if (storage_mode == StorageMode::kUnInitialized) {
                UnrecoverableError("Uninitialized storage mode");
            }

            if (storage_mode != StorageMode::kWritable) {
                return Status::InvalidNodeRole("Attempt to write on non-writable node");
            }
            return BuildDrop(const_cast<DropStatement *>(static_cast<const DropStatement *>(statement)), bind_context_ptr);
        }
        case StatementType::kShow: {
            return BuildShow(const_cast<ShowStatement *>(static_cast<const ShowStatement *>(statement)), bind_context_ptr);
        }
        case StatementType::kFlush: {
            StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
            if (storage_mode == StorageMode::kUnInitialized) {
                UnrecoverableError("Uninitialized storage mode");
            }

            if (storage_mode != StorageMode::kWritable) {
                return Status::InvalidNodeRole("Attempt to write on non-writable node");
            }
            return BuildFlush(static_cast<const FlushStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kOptimize: {
            StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
            if (storage_mode == StorageMode::kUnInitialized) {
                UnrecoverableError("Uninitialized storage mode");
            }

            if (storage_mode != StorageMode::kWritable) {
                return Status::InvalidNodeRole("Attempt to write on non-writable node");
            }
            return BuildOptimize(const_cast<OptimizeStatement *>(static_cast<const OptimizeStatement *>(statement)), bind_context_ptr);
        }
        case StatementType::kCopy: {
            return BuildCopy(const_cast<CopyStatement *>(static_cast<const CopyStatement *>(statement)), bind_context_ptr);
        }
        case StatementType::kExplain: {
            return BuildExplain(static_cast<const ExplainStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kPrepare: {
            return BuildPrepare(static_cast<const PrepareStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kExecute: {
            return BuildExecute(static_cast<const ExecuteStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kAlter: {
            StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
            if (storage_mode == StorageMode::kUnInitialized) {
                UnrecoverableError("Uninitialized storage mode");
            }

            if (storage_mode != StorageMode::kWritable) {
                return Status::InvalidNodeRole("Attempt to write on non-writable node");
            }
            return BuildAlter(const_cast<AlterStatement *>(static_cast<const AlterStatement *>(statement)), bind_context_ptr);
        }
        case StatementType::kCommand: {
            return BuildCommand(static_cast<const CommandStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kCompact: {
            StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
            if (storage_mode == StorageMode::kUnInitialized) {
                UnrecoverableError("Uninitialized storage mode");
            }

            if (storage_mode != StorageMode::kWritable) {
                return Status::InvalidNodeRole("Attempt to write on non-writable node");
            }
            return BuildCompact(static_cast<const CompactStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kCheck: {
            return BuildCheck(static_cast<const CheckStatement *>(statement), bind_context_ptr);
        }
        default: {
            UnrecoverableError("Invalid statement type.");
        }
    }
    return Status::OK();
}

Status LogicalPlanner::BuildSelect(const SelectStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    UniquePtr<QueryBinder> query_binder_ptr = MakeUnique<QueryBinder>(this->query_context_ptr_, bind_context_ptr);
    UniquePtr<BoundSelectStatement> bound_statement_ptr = query_binder_ptr->BindSelect(*statement);
    this->logical_plan_ = bound_statement_ptr->BuildPlan(query_context_ptr_);
    return Status::OK();
}

Status LogicalPlanner::BuildInsert(InsertStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    BindSchemaName(statement->schema_name_);
    if (statement->select_ == nullptr) {
        return BuildInsertValue(statement, bind_context_ptr);
    } else {
        return BuildInsertSelect(statement, bind_context_ptr);
    }
}

Status LogicalPlanner::BuildInsertValue(const InsertStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    bind_context_ptr->expression_binder_ = MakeShared<InsertBinder>(query_context_ptr_);

    // Get schema name
    const String &schema_name = statement->schema_name_;
    const String &table_name = statement->table_name_;

    // Get table name
    if (table_name.empty()) {
        String error_message = "Insert statement missing table table_name.";
        UnrecoverableError(error_message);
    }
    // Check schema and table in the catalog

    SharedPtr<TableInfo> table_info;
    NewTxn *new_txn = query_context_ptr_->GetNewTxn();
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    String table_key;
    Status status = new_txn->GetTableMeta(schema_name, table_name, db_meta, table_meta, &table_key);
    if (!status.ok()) {
        RecoverableError(status);
    }
    table_info = MakeShared<TableInfo>();
    status = table_meta->GetTableInfo(*table_info);
    if (!status.ok()) {
        RecoverableError(status);
    }
    table_info->db_name_ = MakeShared<String>(schema_name);
    table_info->table_name_ = MakeShared<String>(table_name);
    table_info->table_key_ = table_key;

    // check
    const auto row_count = statement->insert_rows_.size();
    if (row_count == 0) {
        RecoverableError(Status::NotSupport("No insert batch row found!"));
    } else if (row_count > INSERT_BATCH_ROW_LIMIT) {
        RecoverableError(Status::NotSupport("Insert batch row limit shouldn't more than 8192."));
    }

    for (SizeT idx = 0; idx < row_count; ++idx) {
        const auto &insert_row = statement->insert_rows_[idx];
        if (insert_row->values_.empty()) {
            RecoverableError(Status::SyntaxError("INSERT: Input value list is empty."));
        }
        if ((!insert_row->columns_.empty()) && insert_row->columns_.size() != insert_row->values_.size()) {
            RecoverableError(Status::SyntaxError("INSERT: Input column name count mismatch with value count."));
        }
    }

    // Create value list
    Vector<Vector<SharedPtr<BaseExpression>>> value_list_array(row_count);
    // Rearrange the inserted value to match the table.
    // SELECT INTO TABLE (c, a) VALUES (1, 2); => SELECT INTO TABLE (a, b, c) VALUES( 2, NULL, 1);
    for (SizeT idx = 0; idx < row_count; ++idx) {
        const InsertRowExpr &insert_row = *(statement->insert_rows_[idx]);
        Vector<SharedPtr<BaseExpression>> src_value_list;
        src_value_list.reserve(insert_row.values_.size());
        for (const auto &val : insert_row.values_) {
            if (!val) {
                RecoverableError(Status::SyntaxError("INSERT: Invalid input value! Got nullptr expr"));
            }
            auto value_expr = bind_context_ptr->expression_binder_->BuildExpression(*val, bind_context_ptr.get(), 0, true);
            src_value_list.emplace_back(std::move(value_expr));
        }
        auto &dst_value_list = value_list_array[idx];
        if (!insert_row.columns_.empty()) {
            // use column names
            assert(insert_row.columns_.size() == src_value_list.size());
            const auto column_count = src_value_list.size();
            SizeT table_column_count = table_info->column_count_;
            // Create value list with table column size and null value
            Vector<SharedPtr<BaseExpression>> rewrite_value_list(table_column_count);
            Vector<bool> has_value(table_column_count, false);
            for (SizeT column_idx = 0; column_idx < column_count; ++column_idx) {
                const auto &column_name = insert_row.columns_[column_idx];
                const ColumnDef *column_def = table_info->GetColumnDefByName(column_name);
                if (column_def == nullptr) {
                    RecoverableError(Status::SyntaxError(fmt::format("INSERT: Column {} not found in table {}.", column_name, table_name)));
                }
                SizeT table_column_idx = table_info->GetColumnIdxByID(column_def->id());
                const SharedPtr<DataType> &table_column_type = column_def->column_type_;
                auto &src_value = src_value_list[column_idx];
                auto &dst_value = rewrite_value_list[table_column_idx];
                DataType value_type = src_value->Type();
                if ((value_type != *table_column_type) && LogicalInsert::NeedCastInInsert(value_type, *table_column_type)) {
                    // If the inserted value type mismatches with table
                    // column type, cast the inserted value type to correct
                    // one.
                    BoundCastFunc cast = CastFunction::GetBoundFunc(value_type, *table_column_type);
                    auto cast_expr = MakeShared<CastExpression>(cast, src_value, *table_column_type);
                    dst_value = std::move(cast_expr);
                } else {
                    // LogicalType are same and type info is also OK.
                    dst_value = std::move(src_value);
                }
                has_value[table_column_idx] = true;
            }
            for (SizeT column_idx = 0; column_idx < table_column_count; ++column_idx) {
                if (has_value[column_idx]) {
                    continue;
                }
                const ColumnDef *column_def = table_info->GetColumnDefByIdx(column_idx);
                if (!column_def->has_default_value()) {
                    RecoverableError(Status::SyntaxError(fmt::format("INSERT: No default value found for column {}.", column_def->ToString())));
                }
                auto &dst_value = rewrite_value_list[column_idx];
                auto default_value_expr =
                    bind_context_ptr->expression_binder_->BuildExpression(*column_def->default_expr_.get(), bind_context_ptr.get(), 0, true);
                const SharedPtr<DataType> &table_column_type = column_def->column_type_;
                DataType value_type = default_value_expr->Type();
                if ((*table_column_type != value_type) && LogicalInsert::NeedCastInInsert(value_type, *table_column_type)) {
                    BoundCastFunc cast = CastFunction::GetBoundFunc(value_type, *table_column_type);
                    auto cast_expr = MakeShared<CastExpression>(cast, default_value_expr, *table_column_type);
                    dst_value = std::move(cast_expr);
                } else {
                    // LogicalType are same and type info is also OK.
                    dst_value = std::move(default_value_expr);
                }
            }
            dst_value_list = std::move(rewrite_value_list);
        } else {
            // append default values
            SizeT table_column_count = table_info->column_count_;
            dst_value_list = std::move(src_value_list);
            dst_value_list.reserve(table_column_count);
            for (SizeT column_idx = dst_value_list.size(); column_idx < table_column_count; ++column_idx) {
                const ColumnDef *column_def = table_info->GetColumnDefByIdx(column_idx);
                if (column_def->has_default_value()) {
                    auto value_expr =
                        bind_context_ptr->expression_binder_->BuildExpression(*column_def->default_expr_.get(), bind_context_ptr.get(), 0, true);
                    dst_value_list.emplace_back(std::move(value_expr));
                } else {
                    RecoverableError(Status::SyntaxError(fmt::format("INSERT: No default value found for column {}.", column_def->ToString())));
                }
            }
            if (dst_value_list.size() != table_column_count) {
                RecoverableError(Status::SyntaxError(fmt::format("INSERT failed: The number of values ({}) does not match the number of columns "
                                                                 "({}).",
                                                                 dst_value_list.size(),
                                                                 table_column_count)));
            }
            for (SizeT column_idx = 0; column_idx < table_column_count; ++column_idx) {
                auto &dst_value = dst_value_list[column_idx];
                const SharedPtr<DataType> &table_column_type = table_info->GetColumnDefByIdx(column_idx)->column_type_;
                DataType value_type = dst_value->Type();
                if (*table_column_type != value_type && LogicalInsert::NeedCastInInsert(value_type, *table_column_type)) {
                    // need cast
                    BoundCastFunc cast = CastFunction::GetBoundFunc(value_type, *table_column_type);
                    auto cast_expr = MakeShared<CastExpression>(cast, dst_value, *table_column_type);
                    dst_value = std::move(cast_expr);
                }
            }
        }
    }

    // Create logical insert node.
    auto logical_insert = MakeShared<LogicalInsert>(bind_context_ptr->GetNewLogicalNodeId(),
                                                    table_info,
                                                    bind_context_ptr->GenerateTableIndex(),
                                                    std::move(value_list_array));

    // FIXME: check if we need to append operator
    //    this->AppendOperator(logical_insert, bind_context_ptr);

    this->logical_plan_ = std::move(logical_insert);
    return Status::OK();
}

Status LogicalPlanner::BuildInsertSelect(const InsertStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    String schema_name = statement->schema_name_;
    BindSchemaName(schema_name);

    // Get table info
    const String &table_name = statement->table_name_;

    SharedPtr<TableInfo> table_info;
    NewTxn *new_txn = query_context_ptr_->GetNewTxn();
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    String table_key;
    Status status = new_txn->GetTableMeta(schema_name, table_name, db_meta, table_meta, &table_key);
    if (!status.ok()) {
        RecoverableError(status);
    }
    table_info = MakeShared<TableInfo>();
    status = table_meta->GetTableInfo(*table_info);
    if (!status.ok()) {
        RecoverableError(status);
    }
    table_info->db_name_ = MakeShared<String>(schema_name);
    table_info->table_name_ = MakeShared<String>(table_name);
    table_info->table_key_ = table_key;

    // Build SELECT plan - use the exact same approach as BuildSelect
    UniquePtr<QueryBinder> query_binder_ptr = MakeUnique<QueryBinder>(this->query_context_ptr_, bind_context_ptr);
    UniquePtr<BoundSelectStatement> bound_statement_ptr = query_binder_ptr->BindSelect(*statement->select_);
    SharedPtr<LogicalNode> select_plan = bound_statement_ptr->BuildPlan(query_context_ptr_);

    // Create INSERT node with SELECT as child
    auto logical_insert = MakeShared<LogicalInsert>(bind_context_ptr->GetNewLogicalNodeId(),
                                                    table_info,
                                                    bind_context_ptr->GenerateTableIndex(),
                                                    Vector<Vector<SharedPtr<BaseExpression>>>{});
    logical_insert->set_left_node(select_plan);

    this->logical_plan_ = logical_insert;
    return Status::OK();
}

Status LogicalPlanner::BuildUpdate(const UpdateStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    UniquePtr<QueryBinder> query_binder_ptr = MakeUnique<QueryBinder>(this->query_context_ptr_, bind_context_ptr);
    UniquePtr<BoundUpdateStatement> bound_statement_ptr = query_binder_ptr->BindUpdate(*statement);
    this->logical_plan_ = bound_statement_ptr->BuildPlan(query_context_ptr_);
    return Status::OK();
}

Status LogicalPlanner::BuildDelete(const DeleteStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    // FIXME: After supporting Truncate, switch to the Truncate instruction when there is no where_expr_.
    UniquePtr<QueryBinder> query_binder_ptr = MakeUnique<QueryBinder>(this->query_context_ptr_, bind_context_ptr);
    UniquePtr<BoundDeleteStatement> bound_statement_ptr = query_binder_ptr->BindDelete(*statement);
    this->logical_plan_ = bound_statement_ptr->BuildPlan(query_context_ptr_);
    return Status::OK();
}

Status LogicalPlanner::BuildCreate(CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    BindSchemaName(statement->create_info_->schema_name_);
    switch (statement->ddl_type()) {
        case DDLType::kTable: {
            return BuildCreateTable(statement, bind_context_ptr);
        }
        case DDLType::kCollection: {
            return BuildCreateCollection(statement, bind_context_ptr);
        }
        case DDLType::kView: {
            return BuildCreateView(statement, bind_context_ptr);
        }
        case DDLType::kIndex: {
            return BuildCreateIndex(statement, bind_context_ptr);
        }
        case DDLType::kDatabase: {
            return BuildCreateDatabase(statement, bind_context_ptr);
        }
        default: {
            String error_message = "Not supported";
            UnrecoverableError(error_message);
        }
    }
    return Status::OK();
}

Pair<bool, String> VerifyColumnDataType(const DataType &data_type) {
    switch (data_type.type()) {
        case LogicalType::kBoolean:
        case LogicalType::kTinyInt:
        case LogicalType::kSmallInt:
        case LogicalType::kInteger:
        case LogicalType::kBigInt:
        case LogicalType::kFloat:
        case LogicalType::kDouble:
        case LogicalType::kFloat16:
        case LogicalType::kBFloat16:
        case LogicalType::kVarchar:
        case LogicalType::kDate:
        case LogicalType::kTime:
        case LogicalType::kTimestamp:
        case LogicalType::kDateTime: {
            break;
        }
        case LogicalType::kEmbedding: {
            if (auto *embedding_info = static_cast<const EmbeddingInfo *>(data_type.type_info().get());
                embedding_info->Dimension() > EMBEDDING_LIMIT) {
                return std::make_pair(
                    false,
                    fmt::format("Embedding dimension: {} is larger than EMBEDDING_LIMIT: {}", embedding_info->Dimension(), EMBEDDING_LIMIT));
            }
            break;
        }
        case LogicalType::kMultiVector:
        case LogicalType::kTensor:
        case LogicalType::kTensorArray:
        case LogicalType::kSparse: {
            break;
        }
        case LogicalType::kArray: {
            auto *array_info = static_cast<const ArrayInfo *>(data_type.type_info().get());
            if (const auto [element_success, err_msg] = VerifyColumnDataType(array_info->ElemType()); !element_success) {
                return std::make_pair(false, fmt::format("Not supported data type: {}, error in element type: {}", data_type.ToString(), err_msg));
            }
            break;
        }
        case LogicalType::kHugeInt:
        case LogicalType::kDecimal:
        case LogicalType::kInterval:
        case LogicalType::kTuple:
        case LogicalType::kPoint:
        case LogicalType::kLine:
        case LogicalType::kLineSeg:
        case LogicalType::kBox:
        case LogicalType::kCircle:
        case LogicalType::kUuid:
        case LogicalType::kRowID:
        case LogicalType::kMixed:
        case LogicalType::kNull:
        case LogicalType::kMissing:
        case LogicalType::kEmptyArray:
        case LogicalType::kInvalid: {
            return std::make_pair(false, fmt::format("Not supported data type: {}", data_type.ToString()));
            break;
        }
    }
    return {true, String{}};
}

Status LogicalPlanner::BuildCreateTable(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *create_table_info = (CreateTableInfo *)statement->create_info_.get();

    // Check if columns is given.
    Vector<SharedPtr<ColumnDef>> columns;
    SizeT column_count = create_table_info->column_defs_.size();
    if (column_count == 0) {
        return Status::NoColumnDefined(create_table_info->table_name_);
    }
    Set<String> dedup_set;
    columns.reserve(column_count);
    for (SizeT idx = 0; idx < column_count; ++idx) {
        const String &column_name = create_table_info->column_defs_[idx]->name();
        const String &column_comment = create_table_info->column_defs_[idx]->comment();
        switch (IdentifierValidation(column_name)) {
            case IdentifierValidationStatus::kOk:
                break;
            case IdentifierValidationStatus::kEmpty:
                return Status::EmptyColumnName();
            case IdentifierValidationStatus::kExceedLimit:
                return Status::ExceedColumnNameLength(column_name.length());
            case IdentifierValidationStatus::kInvalidName: {
                return Status::InvalidColumnName(column_name);
            }
        }

        if (dedup_set.contains(column_name)) {
            return Status::DuplicateColumnName(column_name);
        } else {
            dedup_set.insert(column_name);
        }

        const DataType *data_type = create_table_info->column_defs_[idx]->type().get();
        if (const auto [success, err_msg] = VerifyColumnDataType(*data_type); !success) {
            return Status::NotSupport(err_msg);
        }

        SharedPtr<ColumnDef> column_def = MakeShared<ColumnDef>(idx,
                                                                create_table_info->column_defs_[idx]->type(),
                                                                column_name,
                                                                create_table_info->column_defs_[idx]->constraints_,
                                                                column_comment,
                                                                std::move(create_table_info->column_defs_[idx]->default_expr_));
        columns.emplace_back(column_def);
    }

    SharedPtr<String> schema_name_ptr = MakeShared<String>(create_table_info->schema_name_);

    switch (IdentifierValidation(create_table_info->table_name_)) {
        case IdentifierValidationStatus::kOk:
            break;
        case IdentifierValidationStatus::kEmpty:
            return Status::EmptyTableName();
        case IdentifierValidationStatus::kExceedLimit:
            return Status::ExceedTableNameLength(create_table_info->table_name_.length());
        case IdentifierValidationStatus::kInvalidName: {
            return Status::InvalidTableName(create_table_info->table_name_);
        }
    }

    SharedPtr<TableDef> table_def_ptr = TableDef::Make(MakeShared<String>("default_db"),
                                                       MakeShared<String>(create_table_info->table_name_),
                                                       MakeShared<String>(create_table_info->comment_),
                                                       std::move(columns));

    for (HashSet<String> visited_param_names; auto *property_ptr : create_table_info->properties_) {
        auto &[param_name, param_value] = *property_ptr;
        if (auto [_, success] = visited_param_names.insert(param_name); !success) {
            return Status::SyntaxError(fmt::format("Duplicate table property param name: {}", param_name));
        }
        if (param_name == "bloom_filter_columns") {
            Vector<ColumnID> bloom_filter_columns;
            // spilt the param_value string by ',', find corresponding column id and add to bloom_filter_columns
            IStringStream column_name_stream(param_value);
            String column_name;
            while (std::getline(column_name_stream, column_name, ',')) {
                // remove leading and trailing spaces
                if (SizeT start = column_name.find_first_not_of(' '); start != String::npos) {
                    column_name = column_name.substr(start);
                }
                if (SizeT end = column_name.find_last_not_of(' '); end != String::npos) {
                    column_name = column_name.substr(0, end + 1);
                }
                // find column id by column name
                if (SizeT column_id = table_def_ptr->GetColIdByName(column_name); column_id == static_cast<SizeT>(-1)) {
                    return Status::SyntaxError(fmt::format("Column {} not found in table {}", column_name, *table_def_ptr->table_name()));
                } else {
                    bloom_filter_columns.push_back(column_id);
                }
            }
            // remove duplicate column id
            std::sort(bloom_filter_columns.begin(), bloom_filter_columns.end());
            bloom_filter_columns.erase(std::unique(bloom_filter_columns.begin(), bloom_filter_columns.end()), bloom_filter_columns.end());
            // check if bloom filter can be created for the column
            for (ColumnID column_id : bloom_filter_columns) {
                if (auto &def = table_def_ptr->columns()[column_id]; def->type()->SupportBloomFilter()) {
                    def->build_bloom_filter_ = true;
                } else {
                    return Status::SyntaxError(
                        fmt::format("Bloom filter can't be created for {} type column {}", def->type()->ToString(), def->name()));
                }
            }
        }
    }

    SharedPtr<LogicalNode> logical_create_table_operator = LogicalCreateTable::Make(bind_context_ptr->GetNewLogicalNodeId(),
                                                                                    schema_name_ptr,
                                                                                    table_def_ptr,
                                                                                    bind_context_ptr->GenerateTableIndex(),
                                                                                    create_table_info->conflict_type_);

    if (create_table_info->select_ != nullptr) {
        SharedPtr<BindContext> select_bind_context_ptr = BindContext::Make(nullptr);
        QueryBinder select_query_binder(this->query_context_ptr_, select_bind_context_ptr);
        UniquePtr<BoundSelectStatement> bound_statement_ptr = select_query_binder.BindSelect(*create_table_info->select_);
        logical_create_table_operator->set_left_node(bound_statement_ptr->BuildPlan(this->query_context_ptr_));
    }

    this->logical_plan_ = logical_create_table_operator;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status::OK();
}

Status LogicalPlanner::BuildCreateCollection(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *create_collection_info = (CreateCollectionInfo *)statement->create_info_.get();

    SharedPtr<String> schema_name_ptr = MakeShared<String>(create_collection_info->schema_name_);
    SharedPtr<String> collection_name_ptr = MakeShared<String>(create_collection_info->collection_name_);

    SharedPtr<LogicalNode> logical_create_collection_operator = LogicalCreateCollection::Make(bind_context_ptr->GetNewLogicalNodeId(),
                                                                                              schema_name_ptr,
                                                                                              collection_name_ptr,
                                                                                              bind_context_ptr->GenerateTableIndex(),
                                                                                              create_collection_info->conflict_type_);

    this->logical_plan_ = logical_create_collection_operator;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status::OK();
}

Status LogicalPlanner::BuildCreateDatabase(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *create_schema_info = (CreateSchemaInfo *)statement->create_info_.get();

    switch (IdentifierValidation(create_schema_info->schema_name_)) {
        case IdentifierValidationStatus::kOk:
            break;
        case IdentifierValidationStatus::kEmpty:
            return Status::EmptyDBName();
        case IdentifierValidationStatus::kExceedLimit:
            return Status::ExceedDBNameLength(create_schema_info->schema_name_.length());
        case IdentifierValidationStatus::kInvalidName: {
            return Status::InvalidIdentifierName(create_schema_info->schema_name_);
        }
    }

    SharedPtr<String> schema_name_ptr = MakeShared<String>(create_schema_info->schema_name_);
    SharedPtr<String> comment_ptr = MakeShared<String>(create_schema_info->comment_);

    SharedPtr<LogicalNode> logical_create_schema_operator =
        LogicalCreateSchema::Make(bind_context_ptr->GetNewLogicalNodeId(), schema_name_ptr, create_schema_info->conflict_type_, comment_ptr);

    this->logical_plan_ = logical_create_schema_operator;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status::OK();
}

Status LogicalPlanner::BuildCreateView(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    return Status::NotSupport("View isn't supported");
#if 0
    CreateViewInfo *create_view_info = (CreateViewInfo *)(statement->create_info_.get());

    // Check if columns is given.
    SharedPtr<Vector<String>> columns_ptr;
    SizeT column_count = create_view_info->view_columns_->size();

    // Build create view statement
    UniquePtr<QueryBinder> query_binder_ptr = MakeUnique<QueryBinder>(this->query_context_ptr_, bind_context_ptr);
    UniquePtr<BoundSelectStatement> bound_statement_ptr = query_binder_ptr->BindSelect(*create_view_info->select_);

    if (column_count == 0) {
        // Not specify the view column
        columns_ptr = bound_statement_ptr->names_ptr_;
    } else {
        // Specify the view column
        if (column_count != bound_statement_ptr->names_ptr_->size()) {
            String error_message = "Create view column count isn't matched.";
            UnrecoverableError(error_message);
        }
        columns_ptr = MakeShared<Vector<String>>(*(create_view_info->view_columns_));
    }

    SharedPtr<LogicalNode> logical_create_view_operator = LogicalCreateView::Make(bind_context_ptr->GetNewLogicalNodeId(),
                                                                                  columns_ptr,
                                                                                  bound_statement_ptr->types_ptr_,
                                                                                  static_pointer_cast<CreateViewInfo>(statement->create_info_));

    this->logical_plan_ = logical_create_view_operator;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status::OK();
#endif
}

Status LogicalPlanner::BuildCreateIndex(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *create_index_info = (CreateIndexInfo *)statement->create_info_.get();
    NewTxn *new_txn = query_context_ptr_->GetNewTxn();

    auto schema_name = MakeShared<String>(create_index_info->schema_name_);
    auto table_name = MakeShared<String>(create_index_info->table_name_);
    auto index_comment = MakeShared<String>(create_index_info->comment_);
    if (table_name->empty()) {
        Status status = Status::InvalidIndexName("No index name.");
        RecoverableError(status);
    }
    //    if (create_index_info->column_names_->size() != 1) {
    //        UnrecoverableError("Creating index only support single column now.");
    //    }

    SharedPtr<String> index_name = MakeShared<String>(std::move(create_index_info->index_name_));
    UniquePtr<QueryBinder> query_binder_ptr = MakeUnique<QueryBinder>(this->query_context_ptr_, bind_context_ptr);
    auto base_table_ref = query_binder_ptr->GetTableRef(*schema_name, *table_name);

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = new_txn->GetTableMeta(*base_table_ref->table_info_->db_name_, *base_table_ref->table_info_->table_name_, db_meta, table_meta);
    if (!status.ok()) {
        RecoverableError(status);
    }

    IndexInfo *index_info = create_index_info->index_info_;
    SharedPtr<IndexBase> base_index_ptr{nullptr};
    String index_filename = fmt::format("{}_{}", create_index_info->table_name_, *index_name);
    switch (index_info->index_type_) {
        case IndexType::kFullText: {
            assert(index_info->index_param_list_ != nullptr);
            IndexFullText::ValidateColumnDataType(base_table_ref, index_info->column_name_);
            base_index_ptr =
                IndexFullText::Make(index_name, index_comment, index_filename, {index_info->column_name_}, *(index_info->index_param_list_));
            break;
        }
        case IndexType::kHnsw: {
            assert(index_info->index_param_list_ != nullptr);
            // The following check might affect performance
            IndexHnsw::ValidateColumnDataType(base_table_ref, index_info->column_name_, *(index_info->index_param_list_)); // may throw exception
            base_index_ptr = IndexHnsw::Make(index_name, index_comment, index_filename, {index_info->column_name_}, *(index_info->index_param_list_));
            break;
        }
        case IndexType::kIVF: {
            assert(index_info->index_param_list_ != nullptr);
            auto ivf_ptr = IndexIVF::Make(index_name, index_comment, index_filename, {index_info->column_name_}, *(index_info->index_param_list_));
            ivf_ptr->ValidateColumnDataType(base_table_ref, index_info->column_name_); // may throw exception
            base_index_ptr = std::move(ivf_ptr);
            break;
        }
        case IndexType::kSecondary: {
            IndexSecondary::ValidateColumnDataType(base_table_ref, index_info->column_name_); // may throw exception
            base_index_ptr = IndexSecondary::Make(index_name, index_comment, index_filename, {index_info->column_name_});
            break;
        }
        case IndexType::kEMVB: {
            assert(index_info->index_param_list_ != nullptr);
            IndexEMVB::ValidateColumnDataType(base_table_ref, index_info->column_name_); // may throw exception
            base_index_ptr = IndexEMVB::Make(index_name, index_comment, index_filename, {index_info->column_name_}, *(index_info->index_param_list_));
            break;
        }
        case IndexType::kBMP: {
            assert(index_info->index_param_list_ != nullptr);
            IndexBMP::ValidateColumnDataType(base_table_ref, index_info->column_name_); // may throw exception
            base_index_ptr = IndexBMP::Make(index_name, index_comment, index_filename, {index_info->column_name_}, *(index_info->index_param_list_));
            break;
        }
        case IndexType::kDiskAnn: {
            assert(index_info->index_param_list_ != nullptr);
            IndexDiskAnn::ValidateColumnDataType(base_table_ref, index_info->column_name_); // may throw exception
            base_index_ptr =
                IndexDiskAnn::Make(index_name, index_comment, index_filename, {index_info->column_name_}, *(index_info->index_param_list_));
            break;
        }
        case IndexType::kInvalid: {
            Status status = Status::InvalidIndexType("Invalid index");
            RecoverableError(status);
            break;
        }
    }

    auto logical_create_index_operator =
        MakeShared<LogicalCreateIndex>(bind_context_ptr->GetNewLogicalNodeId(), base_table_ref, base_index_ptr, create_index_info->conflict_type_);

    this->logical_plan_ = logical_create_index_operator;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status::OK();
}

Status LogicalPlanner::BuildDrop(DropStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    BindSchemaName(statement->drop_info_->schema_name_);
    switch (statement->ddl_type()) {
        case DDLType::kTable: {
            return BuildDropTable(statement, bind_context_ptr);
        }
        case DDLType::kCollection: {
            return BuildDropCollection(statement, bind_context_ptr);
        }
        case DDLType::kDatabase: {
            return BuildDropSchema(statement, bind_context_ptr);
        }
        case DDLType::kIndex: {
            return BuildDropIndex(statement, bind_context_ptr);
        }
        case DDLType::kView: {
            return BuildDropView(statement, bind_context_ptr);
        }
        case DDLType::kInvalid: {
            String error_message = "Invalid drop statement type.";
            UnrecoverableError(error_message);
        }
    }
    return Status::OK();
}

Status LogicalPlanner::BuildDropTable(const DropStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *drop_table_info = (DropTableInfo *)statement->drop_info_.get();

    SharedPtr<String> schema_name_ptr{nullptr};
    if (drop_table_info->schema_name_.empty()) {
        schema_name_ptr = MakeShared<String>("default_db");
    } else {
        schema_name_ptr = MakeShared<String>(drop_table_info->schema_name_);
    }

    SharedPtr<String> table_name_ptr = MakeShared<String>(drop_table_info->table_name_);
    SharedPtr<LogicalNode> logical_drop_table =
        MakeShared<LogicalDropTable>(bind_context_ptr->GetNewLogicalNodeId(), schema_name_ptr, table_name_ptr, drop_table_info->conflict_type_);

    this->logical_plan_ = logical_drop_table;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status::OK();
}

Status LogicalPlanner::BuildDropCollection(const DropStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *drop_collection_info = (DropCollectionInfo *)statement->drop_info_.get();

    SharedPtr<String> schema_name_ptr{nullptr};
    if (drop_collection_info->schema_name_.empty()) {
        schema_name_ptr = MakeShared<String>("default_db");
    } else {
        schema_name_ptr = MakeShared<String>(drop_collection_info->schema_name_);
    }

    SharedPtr<LogicalNode> logical_drop_collection = MakeShared<LogicalDropCollection>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                                       schema_name_ptr,
                                                                                       MakeShared<String>(drop_collection_info->collection_name_),
                                                                                       drop_collection_info->conflict_type_);

    this->logical_plan_ = logical_drop_collection;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status::OK();
}

Status LogicalPlanner::BuildDropSchema(const DropStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *drop_schema_info = (DropSchemaInfo *)statement->drop_info_.get();
    switch (IdentifierValidation(drop_schema_info->schema_name_)) {
        case IdentifierValidationStatus::kOk:
            break;
        case IdentifierValidationStatus::kEmpty:
            return Status::EmptyDBName();
        case IdentifierValidationStatus::kExceedLimit:
            return Status::ExceedDBNameLength(drop_schema_info->schema_name_.length());
        case IdentifierValidationStatus::kInvalidName: {
            return Status::InvalidIdentifierName(drop_schema_info->schema_name_);
        }
    }

    if (IsEqual(drop_schema_info->schema_name_, query_context_ptr_->schema_name())) {
        Status status = Status::DroppingUsingDb(drop_schema_info->schema_name_);
        RecoverableError(status);
    }

    SharedPtr<String> schema_name_ptr = MakeShared<String>(drop_schema_info->schema_name_);

    SharedPtr<LogicalNode> logical_drop_schema =
        MakeShared<LogicalDropSchema>(bind_context_ptr->GetNewLogicalNodeId(), schema_name_ptr, drop_schema_info->conflict_type_);

    this->logical_plan_ = logical_drop_schema;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status::OK();
}

Status LogicalPlanner::BuildDropIndex(const DropStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *drop_index_info = (DropIndexInfo *)statement->drop_info_.get();

    SharedPtr<String> schema_name_ptr = MakeShared<String>(drop_index_info->schema_name_);
    SharedPtr<String> table_name_ptr = MakeShared<String>(drop_index_info->table_name_);
    SharedPtr<String> index_name_ptr = MakeShared<String>(drop_index_info->index_name_);

    SharedPtr<LogicalNode> logical_drop_index = MakeShared<LogicalDropIndex>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                             schema_name_ptr,
                                                                             table_name_ptr,
                                                                             index_name_ptr,
                                                                             drop_index_info->conflict_type_);

    this->logical_plan_ = logical_drop_index;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status::OK();
}

Status LogicalPlanner::BuildDropView(const DropStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *drop_view_info = (DropViewInfo *)statement->drop_info_.get();

    SharedPtr<String> schema_name_ptr = MakeShared<String>(drop_view_info->schema_name_);
    SharedPtr<String> view_name_ptr = MakeShared<String>(drop_view_info->view_name_);

    SharedPtr<LogicalNode> logical_drop_view =
        MakeShared<LogicalDropView>(bind_context_ptr->GetNewLogicalNodeId(), schema_name_ptr, view_name_ptr, drop_view_info->conflict_type_);

    this->logical_plan_ = logical_drop_view;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status::OK();
}

Status LogicalPlanner::BuildPrepare(const PrepareStatement *, SharedPtr<BindContext> &) {
    Status status = Status::NotSupport("Prepare statement isn't supported.");
    RecoverableError(status);
    return Status::OK();
}

Status LogicalPlanner::BuildExecute(const ExecuteStatement *, SharedPtr<BindContext> &) {
    Status status = Status::NotSupport("Execute statement isn't supported.");
    RecoverableError(status);
    return Status::OK();
}

Status LogicalPlanner::BuildCopy(CopyStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    BindSchemaName(statement->schema_name_);
    if (statement->copy_from_) {
        StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
        if (storage_mode == StorageMode::kUnInitialized) {
            UnrecoverableError("Uninitialized storage mode");
        }

        if (storage_mode != StorageMode::kWritable) {
            return Status::InvalidNodeRole("Attempt to write on non-writable node");
        }
        // Import
        return BuildImport(statement, bind_context_ptr);
    } else {
        // Export
        return BuildExport(statement, bind_context_ptr);
    }
}

Status LogicalPlanner::BuildExport(const CopyStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    // Currently export only support jsonl and CSV
    switch (statement->copy_file_type_) {
        case CopyFileType::kJSONL:
        case CopyFileType::kFVECS:
        case CopyFileType::kCSV:
        case CopyFileType::kPARQUET: {
            break;
        }
        default: {
            Status status =
                Status::NotSupport(fmt::format("Exporting file type: {} isn't supported.", *CopyFileTypeToStr(statement->copy_file_type_)));
            RecoverableError(status);
        }
    }

    // Check the file existence
    String to_write_path;
    if (VirtualStore::Exists(statement->file_path_)) {
        Status status = Status::DuplicatedFile(statement->file_path_);
        RecoverableError(status);
    }

    SharedPtr<TableInfo> table_info;
    Status status;
    NewTxn *new_txn = query_context_ptr_->GetNewTxn();
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> tmp_table_meta;
    status = new_txn->GetTableMeta(statement->schema_name_, statement->table_name_, db_meta, tmp_table_meta);
    if (!status.ok()) {
        RecoverableError(status);
    }
    auto table_meta = MakeUnique<TableMeeta>(tmp_table_meta->db_id_str(),
                                             tmp_table_meta->table_id_str(),
                                             tmp_table_meta->kv_instance(),
                                             tmp_table_meta->begin_ts(),
                                             tmp_table_meta->commit_ts());
    table_info = MakeShared<TableInfo>();
    status = table_meta->GetTableInfo(*table_info);
    if (!status.ok()) {
        RecoverableError(status);
    }
    table_info->db_name_ = MakeShared<String>(statement->schema_name_);
    table_info->table_name_ = MakeShared<String>(statement->table_name_);

    Vector<u64> column_idx_array;
    if (statement->expr_array_ != nullptr) {
        // Export columns
        Vector<ParsedExpr *> &expr_array = *statement->expr_array_;
        column_idx_array.reserve(expr_array.size());

        for (ParsedExpr *expr : expr_array) {
            switch (expr->type_) {
                case ParsedExprType::kColumn: {
                    ColumnExpr *column_expr = static_cast<ColumnExpr *>(expr);
                    if (column_expr->star_) {
                        Status status = Status::NotSupport("Not support to export STAR expression");
                        RecoverableError(status);
                    }
                    if (column_expr->generated_) {
                        String error_message = "Column expression shouldn't be generated here";
                        UnrecoverableError(error_message);
                    }

                    SizeT name_count = column_expr->names_.size();
                    switch (name_count) {
                        case 1: {
                            u64 column_idx = table_info->GetColumnIdByName(column_expr->names_[0]);
                            column_idx_array.emplace_back(column_idx);
                            break;
                        }
                        case 2: {
                            if (statement->table_name_ != column_expr->names_[0]) {
                                Status status = Status::InvalidTableName(column_expr->names_[0]);
                                RecoverableError(status);
                            }
                            u64 column_idx = table_info->GetColumnIdByName(column_expr->names_[1]);
                            column_idx_array.emplace_back(column_idx);
                            break;
                        }
                        case 3: {
                            if (statement->schema_name_ != column_expr->names_[0]) {
                                Status status = Status::InvalidTableName(column_expr->names_[0]);
                                RecoverableError(status);
                            }
                            if (statement->table_name_ != column_expr->names_[1]) {
                                Status status = Status::InvalidTableName(column_expr->names_[1]);
                                RecoverableError(status);
                            }
                            u64 column_idx = table_info->GetColumnIdByName(column_expr->names_[2]);
                            column_idx_array.emplace_back(column_idx);
                            break;
                        }
                        case 0: {
                            Status status = Status::UnexpectedError("No column name is given");
                            RecoverableError(status);
                        }
                        default: {
                            std::stringstream ss;
                            for (SizeT i = 0; i < name_count - 1; ++i) {
                                ss << column_expr->names_[i] << ".";
                            }
                            ss << column_expr->names_[name_count - 1];

                            Status status = Status::InvalidColumnName(ss.str());
                            RecoverableError(status);
                        }
                    }
                    break;
                }
                case ParsedExprType::kFunction: {
                    FunctionExpr *function_expr = static_cast<FunctionExpr *>(expr);
                    auto [special_function_ptr, status] =
                        NewCatalog::GetSpecialFunctionByNameNoExcept(query_context_ptr_->storage()->new_catalog(), function_expr->func_name_);
                    if (status.ok()) {
                        switch (special_function_ptr->special_type()) {
                            case SpecialType::kRowID: {
                                column_idx_array.emplace_back(COLUMN_IDENTIFIER_ROW_ID);
                                break;
                            }
                            case SpecialType::kCreateTs: {
                                column_idx_array.emplace_back(COLUMN_IDENTIFIER_CREATE);
                                break;
                            }
                            case SpecialType::kDeleteTs: {
                                column_idx_array.emplace_back(COLUMN_IDENTIFIER_DELETE);
                                break;
                            }
                            default: {
                                Status error_status = Status::NotSupport(fmt::format("Not support to export: {}", special_function_ptr->ToString()));
                                RecoverableError(error_status);
                            }
                        }
                    } else {
                        RecoverableError(status);
                    }
                    break;
                }
                default: {
                    Status status = Status::NotSupport("Only column or some special functions are supported to export");
                }
            }
        }

        if (statement->copy_file_type_ == CopyFileType::kFVECS) {
            if (column_idx_array.size() != 1) {
                Status status = Status::ColumnCountMismatch(fmt::format("Attempt to export {} columns as FVECS file", column_idx_array.size()));
                RecoverableError(status);
            }

            const ColumnDef *column_def = table_info->GetColumnDefByIdx(column_idx_array[0]);
            if (column_def->type()->type() != LogicalType::kEmbedding) {
                Status status = Status::NotSupport(
                    fmt::format("Attempt to export column: {} with type: {} as FVECS file", column_def->name(), column_def->type()->ToString()));
                RecoverableError(status);
            }
        }
    }

    SharedPtr<BlockIndex> block_index;
    block_index = MakeShared<BlockIndex>();
    block_index->NewInit(std::move(table_meta));

    SharedPtr<LogicalNode> logical_export = MakeShared<LogicalExport>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                      table_info,
                                                                      statement->schema_name_,
                                                                      statement->table_name_,
                                                                      statement->file_path_,
                                                                      statement->header_,
                                                                      statement->delimiter_,
                                                                      statement->copy_file_type_,
                                                                      statement->offset_,
                                                                      statement->limit_,
                                                                      statement->row_limit_,
                                                                      column_idx_array,
                                                                      block_index);

    this->logical_plan_ = logical_export;
    return Status::OK();
}

Status LogicalPlanner::BuildImport(const CopyStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    // Check the table existence
    SharedPtr<TableInfo> table_info;
    NewTxn *new_txn = query_context_ptr_->GetNewTxn();
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = new_txn->GetTableMeta(statement->schema_name_, statement->table_name_, db_meta, table_meta);
    if (!status.ok()) {
        RecoverableError(status);
    }
    table_info = MakeShared<TableInfo>();
    status = table_meta->GetTableInfo(*table_info);
    if (!status.ok()) {
        RecoverableError(status);
    }
    table_info->db_name_ = MakeShared<String>(statement->schema_name_);
    table_info->table_name_ = MakeShared<String>(statement->table_name_);

    // Check the file existence
    String to_write_path;
    if (!VirtualStore::Exists(statement->file_path_)) {
        RecoverableError(Status::FileNotFound(statement->file_path_));
    }

    SharedPtr<LogicalNode> logical_import = MakeShared<LogicalImport>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                      table_info,
                                                                      statement->file_path_,
                                                                      statement->header_,
                                                                      statement->delimiter_,
                                                                      statement->copy_file_type_);

    this->logical_plan_ = logical_import;
    return Status::OK();
}

Status LogicalPlanner::BuildAlter(AlterStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    if (statement->schema_name_.empty()) {
        statement->schema_name_ = query_context_ptr_->schema_name();
    }
    NewTxn *new_txn = query_context_ptr_->GetNewTxn();
    Status status;
    Optional<TableMeeta> table_meta;
    Optional<DBMeeta> db_meta;
    status = new_txn->GetTableMeta(statement->schema_name_, statement->table_name_, db_meta, table_meta);
    if (!status.ok()) {
        RecoverableError(status);
    }
    SharedPtr<TableInfo> table_info = MakeShared<TableInfo>();
    status = table_meta->GetTableInfo(*table_info);
    if (!status.ok()) {
        RecoverableError(status);
    }
    table_info->db_name_ = MakeShared<String>(statement->schema_name_);
    table_info->table_name_ = MakeShared<String>(statement->table_name_);

    switch (statement->type_) {
        case AlterStatementType::kRenameTable: {
            auto *rename_table_statement = static_cast<RenameTableStatement *>(statement);
            this->logical_plan_ = MakeShared<LogicalRenameTable>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                 table_info,
                                                                 std::move(rename_table_statement->new_table_name_));
            break;
        }
        case AlterStatementType::kAddColumns: {
            auto *add_columns_statement = static_cast<AddColumnsStatement *>(statement);
            std::vector<std::shared_ptr<ColumnDef>> column_defs = add_columns_statement->column_defs_;
            for (const auto &column_def : column_defs) {
                if (!column_def->has_default_value()) {
                    RecoverableError(Status::NotSupport("Add column without default value isn't supported."));
                }
                bool found = true;
                ColumnID column_id = 0;
                std::tie(column_id, status) = table_meta->GetColumnIDByColumnName(column_def->name());
                if (!status.ok()) {
                    if (status.code() != ErrorCode::kColumnNotExist) {
                        RecoverableError(status);
                    }
                    found = false;
                }
                if (found) {
                    RecoverableError(Status::DuplicateColumnName(column_def->name()));
                }
            }
            this->logical_plan_ = MakeShared<LogicalAddColumns>(bind_context_ptr->GetNewLogicalNodeId(), table_info, std::move(column_defs));
            break;
        }
        case AlterStatementType::kDropColumns: {
            auto *drop_columns_statement = static_cast<DropColumnsStatement *>(statement);
            Vector<String> column_names = drop_columns_statement->column_names_;
            for (const auto &column_name : column_names) {
                ColumnID column_id = 0;
                std::tie(column_id, status) = table_meta->GetColumnIDByColumnName(column_name);
                if (!status.ok()) {
                    RecoverableError(Status::ColumnNotExist(column_name));
                }
                bool has_index = false;
                status = NewCatalog::CheckColumnIfIndexed(*table_meta, column_id, has_index);
                if (!status.ok()) {
                    RecoverableError(status);
                }
                if (has_index) {
                    RecoverableError(Status::NotSupport(fmt::format("Drop column {} which is indexed.", column_name)));
                }
            }
            this->logical_plan_ = MakeShared<LogicalDropColumns>(bind_context_ptr->GetNewLogicalNodeId(), table_info, std::move(column_names));
            break;
        }
        default: {
            RecoverableError(Status::NotSupport("Alter statement isn't supported."));
        }
    }
    return Status::OK();
}

Status LogicalPlanner::BuildCommand(const CommandStatement *command_statement, SharedPtr<BindContext> &bind_context_ptr) {
    switch (command_statement->command_info_->type()) {
        case CommandType::kUse: {
            UseCmd *use_command_info = (UseCmd *)(command_statement->command_info_.get());

            NewTxn *new_txn = query_context_ptr_->GetNewTxn();
            auto [db_info, status] = new_txn->GetDatabaseInfo(use_command_info->db_name());
            if (!status.ok()) {
                return status;
            }
            SharedPtr<LogicalNode> logical_command =
                MakeShared<LogicalCommand>(bind_context_ptr->GetNewLogicalNodeId(), command_statement->command_info_);
            this->logical_plan_ = logical_command;
            break;
        }
        case CommandType::kSet: {
            SharedPtr<LogicalNode> logical_command =
                MakeShared<LogicalCommand>(bind_context_ptr->GetNewLogicalNodeId(), command_statement->command_info_);

            this->logical_plan_ = logical_command;
            break;
        }
        case CommandType::kExport: {
            SharedPtr<LogicalNode> logical_command =
                MakeShared<LogicalCommand>(bind_context_ptr->GetNewLogicalNodeId(), command_statement->command_info_);

            this->logical_plan_ = logical_command;
            break;
        }
        case CommandType::kCheckTable: {
            CheckTable *check_table = (CheckTable *)(command_statement->command_info_.get());
            auto *new_txn = query_context_ptr_->GetNewTxn();
            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            Status status = new_txn->GetTableMeta(query_context_ptr_->schema_name(), check_table->table_name(), db_meta, table_meta);
            if (!status.ok()) {
                return status;
            }
            SharedPtr<LogicalNode> logical_command =
                MakeShared<LogicalCommand>(bind_context_ptr->GetNewLogicalNodeId(), command_statement->command_info_);

            this->logical_plan_ = logical_command;
            break;
        }
        case CommandType::kCleanup: {
            StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
            if (storage_mode == StorageMode::kUnInitialized) {
                UnrecoverableError("Uninitialized storage mode");
            }

            if (storage_mode != StorageMode::kWritable) {
                return Status::InvalidNodeRole("Attempt to write on non-writable node");
            }
            auto logical_command = MakeShared<LogicalCommand>(bind_context_ptr->GetNewLogicalNodeId(), command_statement->command_info_);
            this->logical_plan_ = logical_command;
            break;
        }
        case CommandType::kTestCommand: {
            auto logical_command = MakeShared<LogicalCommand>(bind_context_ptr->GetNewLogicalNodeId(), command_statement->command_info_);
            this->logical_plan_ = logical_command;
            break;
        }
        case CommandType::kSnapshot: {
            StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
            if (storage_mode == StorageMode::kUnInitialized) {
                UnrecoverableError("Uninitialized storage mode");
            }

            auto logical_command = MakeShared<LogicalCommand>(bind_context_ptr->GetNewLogicalNodeId(), command_statement->command_info_);
            this->logical_plan_ = logical_command;
            break;
        }
        case CommandType::kDumpIndex: {
            StorageMode storage_mode = InfinityContext::instance().storage()->GetStorageMode();
            if (storage_mode == StorageMode::kUnInitialized) {
                UnrecoverableError("Uninitialized storage mode");
            }

            if (storage_mode != StorageMode::kWritable) {
                return Status::InvalidNodeRole("Attempt to write on non-writable node");
            }

            auto *dump_index = static_cast<DumpIndexCmd *>(command_statement->command_info_.get());
            if (dump_index->db_name().empty()) {
                dump_index->SetDBName(query_context_ptr_->schema_name());
            }

            auto logical_command = MakeShared<LogicalCommand>(bind_context_ptr->GetNewLogicalNodeId(), command_statement->command_info_);
            this->logical_plan_ = logical_command;
            break;
        }
        default: {
            String error_message = "Invalid command type.";
            UnrecoverableError(error_message);
        }
    }
    return Status::OK();
}

Status LogicalPlanner::BuildCompact(const CompactStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    UniquePtr<QueryBinder> query_binder_ptr = MakeUnique<QueryBinder>(this->query_context_ptr_, bind_context_ptr);
    UniquePtr<BoundCompactStatement> bound_statement_ptr = query_binder_ptr->BindCompact(*statement);
    this->logical_plans_ = bound_statement_ptr->BuildPlans(query_context_ptr_);
    return Status::OK();
}

Status LogicalPlanner::BuildShow(ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {

    BindSchemaName(statement->schema_name_);
    switch (statement->show_type_) {
        case ShowStmtType::kDatabase: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kDatabase,
                                                          statement->schema_name_,
                                                          None,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kTable: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kTable,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kIndex: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kIndex,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex(),
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          statement->index_name_);
            break;
        }
        case ShowStmtType::kIndexSegment: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kIndexSegment,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex(),
                                                          None,
                                                          statement->segment_id_,
                                                          None,
                                                          None,
                                                          None,
                                                          statement->index_name_);
            break;
        }
        case ShowStmtType::kIndexChunk: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kIndexChunk,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex(),
                                                          None,
                                                          statement->segment_id_,
                                                          None,
                                                          statement->chunk_id_,
                                                          None,
                                                          statement->index_name_);
            break;
        }
        case ShowStmtType::kDatabases: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kDatabases,
                                                          statement->schema_name_,
                                                          None,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kTables:
        case ShowStmtType::kCollections: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kTables,
                                                          statement->schema_name_,
                                                          None,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kTasks: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kTasks,
                                                          statement->schema_name_,
                                                          None,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kColumns: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kColumns,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex());

            break;
        }
        case ShowStmtType::kIndexes: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kIndexes,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kConfigs: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kConfigs,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kProfiles: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kProfiles,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kQueries: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kQueries,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kQuery: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kQuery,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex(),
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          statement->session_id_);
            break;
        }
        case ShowStmtType::kTransactions: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kTransactions,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kTransaction: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kTransaction,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex(),
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          statement->txn_id_);
            break;
        }
        case ShowStmtType::kTransactionHistory: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kTransactionHistory,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kSegments: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kSegments,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kSegment: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kSegment,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex(),
                                                          None,
                                                          statement->segment_id_);

            break;
        }
        case ShowStmtType::kBlocks: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kBlocks,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex(),
                                                          None,
                                                          statement->segment_id_);
            break;
        }
        case ShowStmtType::kBlock: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kBlock,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex(),
                                                          None,
                                                          statement->segment_id_,
                                                          statement->block_id_);
            break;
        }
        case ShowStmtType::kBlockColumn: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kBlockColumn,
                                                          statement->schema_name_,
                                                          statement->table_name_,
                                                          bind_context_ptr->GenerateTableIndex(),
                                                          None,
                                                          statement->segment_id_,
                                                          statement->block_id_,
                                                          statement->chunk_id_,
                                                          statement->column_id_);
            break;
        }
        case ShowStmtType::kSessionVariable: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kSessionVariable,
                                                          statement->schema_name_,
                                                          statement->var_name_,
                                                          bind_context_ptr->GenerateTableIndex());

            break;
        }
        case ShowStmtType::kSessionVariables: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kSessionVariables,
                                                          statement->schema_name_,
                                                          statement->var_name_,
                                                          bind_context_ptr->GenerateTableIndex());

            break;
        }
        case ShowStmtType::kGlobalVariable: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kGlobalVariable,
                                                          statement->schema_name_,
                                                          statement->var_name_,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kGlobalVariables: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kGlobalVariables,
                                                          statement->schema_name_,
                                                          statement->var_name_,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kConfig: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kConfig,
                                                          statement->schema_name_,
                                                          statement->var_name_,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kBuffer: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kBuffer,
                                                          statement->schema_name_,
                                                          statement->var_name_,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kMemIndex: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kMemIndex,
                                                          statement->schema_name_,
                                                          statement->var_name_,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kLogs: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kLogs,
                                                          statement->schema_name_,
                                                          statement->var_name_,
                                                          bind_context_ptr->GenerateTableIndex());
            break;
        }
        case ShowStmtType::kCatalog: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kCatalog,
                                                          statement->schema_name_,
                                                          statement->var_name_,
                                                          bind_context_ptr->GenerateTableIndex());

            break;
        }
        case ShowStmtType::kCatalogToFile: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kCatalogToFile,
                                                          statement->schema_name_,
                                                          statement->var_name_,
                                                          bind_context_ptr->GenerateTableIndex(),
                                                          statement->file_path_);

            break;
        }
        case ShowStmtType::kPersistenceFiles: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kPersistenceFiles,
                                                          "",
                                                          "",
                                                          bind_context_ptr->GenerateTableIndex());

            break;
        }
        case ShowStmtType::kPersistenceObjects: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kPersistenceObjects,
                                                          "",
                                                          "",
                                                          bind_context_ptr->GenerateTableIndex());

            break;
        }
        case ShowStmtType::kPersistenceObject: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kPersistenceObject,
                                                          "",
                                                          statement->file_name_.value(),
                                                          bind_context_ptr->GenerateTableIndex());

            break;
        }
        case ShowStmtType::kMemory: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kMemory,
                                                          "",
                                                          "",
                                                          bind_context_ptr->GenerateTableIndex());

            break;
        }
        case ShowStmtType::kMemoryObjects: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kMemoryObjects,
                                                          "",
                                                          "",
                                                          bind_context_ptr->GenerateTableIndex());

            break;
        }
        case ShowStmtType::kMemoryAllocation: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kMemoryAllocation,
                                                          "",
                                                          "",
                                                          bind_context_ptr->GenerateTableIndex());

            break;
        }
        case ShowStmtType::kFunction: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kFunction,
                                                          "",
                                                          "",
                                                          bind_context_ptr->GenerateTableIndex(),
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          statement->function_name_);
            break;
        }
        case ShowStmtType::kListSnapshots: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kListSnapshots,
                                                          "",
                                                          "",
                                                          bind_context_ptr->GenerateTableIndex(),
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          statement->function_name_);
            break;
        }
        case ShowStmtType::kShowSnapshot: {
            this->logical_plan_ = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                          ShowStmtType::kShowSnapshot,
                                                          "",
                                                          statement->snapshot_name_,
                                                          bind_context_ptr->GenerateTableIndex(),
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          None,
                                                          statement->function_name_);
            break;
        }
        default: {
            String error_message = "Unexpected show statement type.";
            UnrecoverableError(error_message);
        }
    }

    return Status::OK();
}

Status LogicalPlanner::BuildFlush(const FlushStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    switch (statement->type()) {
        case FlushType::kCatalog: {
            return BuildFlushCatalog(statement, bind_context_ptr);
        }
        case FlushType::kData: {
            return BuildFlushData(statement, bind_context_ptr);
        }
        case FlushType::kBuffer: {
            return BuildFlushBuffer(statement, bind_context_ptr);
        }
        case FlushType::kLog: {
            return BuildFlushLog(statement, bind_context_ptr);
        }
    }
    return Status::OK();
}

Status LogicalPlanner::BuildFlushCatalog(const FlushStatement *, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_flush = MakeShared<LogicalFlush>(bind_context_ptr->GetNewLogicalNodeId(), FlushType::kCatalog);
    this->logical_plan_ = logical_flush;
    return Status::OK();
}

Status LogicalPlanner::BuildFlushData(const FlushStatement *, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_flush = MakeShared<LogicalFlush>(bind_context_ptr->GetNewLogicalNodeId(), FlushType::kData);
    this->logical_plan_ = logical_flush;
    return Status::OK();
}

Status LogicalPlanner::BuildFlushLog(const FlushStatement *, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_flush = MakeShared<LogicalFlush>(bind_context_ptr->GetNewLogicalNodeId(), FlushType::kLog);
    this->logical_plan_ = logical_flush;
    return Status::OK();
}

Status LogicalPlanner::BuildFlushBuffer(const FlushStatement *, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_flush = MakeShared<LogicalFlush>(bind_context_ptr->GetNewLogicalNodeId(), FlushType::kBuffer);
    this->logical_plan_ = logical_flush;
    return Status::OK();
}

Status LogicalPlanner::BuildOptimize(OptimizeStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    BindSchemaName(statement->schema_name_);
    SharedPtr<LogicalNode> logical_optimize = MakeShared<LogicalOptimize>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                          statement->schema_name_,
                                                                          statement->table_name_,
                                                                          std::move(statement->index_name_),
                                                                          std::move(statement->opt_params_));
    this->logical_plan_ = logical_optimize;
    return Status::OK();
}

Status LogicalPlanner::BuildExplain(const ExplainStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    UniquePtr<QueryBinder> query_binder_ptr = MakeUnique<QueryBinder>(this->query_context_ptr_, bind_context_ptr);

    SharedPtr<LogicalExplain> explain_node = MakeShared<LogicalExplain>(bind_context_ptr->GetNewLogicalNodeId(), statement->type_);

    switch (statement->type_) {
        case ExplainType::kAst: {
            SharedPtr<Vector<SharedPtr<String>>> texts_ptr = MakeShared<Vector<SharedPtr<String>>>();
            Status status = ExplainAST::Explain(statement->statement_, texts_ptr);
            if (!status.ok()) {
                return status;
            }
            explain_node->SetText(texts_ptr);
            break;
        }
        case ExplainType::kUnOpt: {
            Build(statement->statement_, bind_context_ptr);
            SharedPtr<Vector<SharedPtr<String>>> texts_ptr = MakeShared<Vector<SharedPtr<String>>>();
            Status status = ExplainLogicalPlan::Explain(this->logical_plan_.get(), texts_ptr);
            if (!status.ok()) {
                return status;
            }
            explain_node->SetText(texts_ptr);
            break;
        }
        default: {
            Build(statement->statement_, bind_context_ptr);
            explain_node->set_left_node(this->logical_plan_);
        }
    }

    this->logical_plan_ = explain_node;
    return Status::OK();
}

Status LogicalPlanner::BuildCheck(const CheckStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {

    switch (statement->check_type_) {
        case CheckStmtType::kSystem: {
            this->logical_plan_ = MakeShared<LogicalCheck>(bind_context_ptr->GetNewLogicalNodeId(), CheckStmtType::kSystem, None, None);
            break;
        }
        case CheckStmtType::kTable: {

            Optional<String> table_name = statement->table_name_;
            if (!table_name.has_value()) {
                String error_message = "Check statement missing table table_name.";
                UnrecoverableError(error_message);
            }

            NewTxn *new_txn = query_context_ptr_->GetNewTxn();
            Optional<DBMeeta> db_meta;
            Optional<TableMeeta> table_meta;
            String table_key;

            if (!statement->schema_name_.has_value()) {
                auto check_statement = const_cast<CheckStatement *>(statement);
                check_statement->schema_name_ = query_context_ptr_->schema_name();
            }
            Optional<String> schema_name = statement->schema_name_;
            Status status = new_txn->GetTableMeta(schema_name.value(), table_name.value(), db_meta, table_meta, &table_key);
            if (!status.ok()) {
                RecoverableError(status);
            }
            this->logical_plan_ = MakeShared<LogicalCheck>(bind_context_ptr->GetNewLogicalNodeId(),
                                                           CheckStmtType::kTable,
                                                           statement->schema_name_,
                                                           statement->table_name_);
            break;
        }
        default: {
            String error_message = "Unexpected check statement type.";
            UnrecoverableError(error_message);
        }
    }

    return Status::OK();
}

void LogicalPlanner::BindSchemaName(String &schema_name) const {
    if (schema_name.empty()) {
        schema_name = query_context_ptr_->schema_name();
    }
}

} // namespace infinity
