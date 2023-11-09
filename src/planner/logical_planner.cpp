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

#include <memory>

import stl;
import parser;
import bind_context;

import infinity_exception;
import query_binder;
import bound_delete_statement;
import bound_update_statement;
import bound_select_statement;
import insert_binder;
import logical_insert;
import logical_node;
import cast_expression;
import cast_function;
import bound_cast_func;
import base_expression;
import base_entry;
import txn;
import table_collection_entry;
import table_collection_type;
import third_party;
import table_def;
import logical_create_table;
import logical_create_collection;
import logical_create_schema;
import logical_create_view;
import logical_create_index;
import logical_delete;
import logical_drop_table;
import logical_drop_collection;
import logical_drop_schema;
import logical_drop_index;
import logical_drop_view;
import logical_show;
import logical_flush;
import logical_export;
import logical_import;
import logical_explain;
import logical_command;
import explain_logical_plan;
import explain_ast;

import local_file_system;
import parser;
import index_def;
import ivfflat_index_def;
import status;
import hnsw_index_def;

module logical_planner;

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
            return BuildInsert(static_cast<const InsertStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kUpdate: {
            return BuildUpdate(static_cast<const UpdateStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kDelete: {
            return BuildDelete(static_cast<const DeleteStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kCreate: {
            return BuildCreate(static_cast<const CreateStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kDrop: {
            return BuildDrop(static_cast<const DropStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kShow: {
            return BuildShow(static_cast<const ShowStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kFlush: {
            return BuildFlush(static_cast<const FlushStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kCopy: {
            return BuildCopy(static_cast<const CopyStatement *>(statement), bind_context_ptr);
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
            return BuildAlter(static_cast<const AlterStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kCommand: {
            return BuildCommand(static_cast<const CommandStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kInvalidStmt: {
            Error<PlannerException>("Invalid statement type.");
            break;
        }
    }
    return Status();
}

Status LogicalPlanner::BuildSelect(const SelectStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<QueryBinder> query_binder_ptr = MakeShared<QueryBinder>(this->query_context_ptr_, bind_context_ptr);
    SharedPtr<BoundSelectStatement> bound_statement_ptr = query_binder_ptr->BindSelect(*statement);
    this->logical_plan_ = bound_statement_ptr->BuildPlan(query_context_ptr_);
    return Status();
}

Status LogicalPlanner::BuildInsert(const InsertStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
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
        Error<PlannerException>("Insert statement missing table table_name.");
    }
    // Check schema and table in the catalog
    Txn *txn = query_context_ptr_->GetTxn();
    EntryResult result = txn->GetTableByName(schema_name, table_name);
    if (result.err_.get() != nullptr) {
        Error<PlannerException>(*result.err_);
    }

    TableCollectionEntry *table_entry = static_cast<TableCollectionEntry *>(result.entry_);

    if (table_entry->table_collection_type_ == TableCollectionType::kCollectionEntry) {
        Error<PlannerException>("Currently, collection isn't supported.");
    }

    // Create value list
    Vector<Vector<SharedPtr<BaseExpression>>> value_list_array;
    SizeT value_count = statement->values_->size();
    for (SizeT idx = 0; idx < value_count; ++idx) {
        const auto *parsed_expr_list = statement->values_->at(idx);

        SizeT expr_count = parsed_expr_list->size();
        Vector<SharedPtr<BaseExpression>> value_list;
        value_list.reserve(expr_count);
        for (SizeT expr_idx = 0; expr_idx < expr_count; ++expr_idx) {
            const auto *parsed_expr = parsed_expr_list->at(expr_idx);
            SharedPtr<BaseExpression> value_expr =
                bind_context_ptr->expression_binder_->BuildExpression(*parsed_expr, bind_context_ptr.get(), 0, true);
            value_list.emplace_back(value_expr);
        }
        value_list_array.push_back(value_list);
    }

    // Rearrange the inserted value to match the table.
    // SELECT INTO TABLE (c, a) VALUES (1, 2); => SELECT INTO TABLE (a, b, c) VALUES( 2, NULL, 1);
    SizeT value_list_count = value_list_array.size();
    for (SizeT idx = 0; idx < value_list_count; ++idx) {
        auto &value_list = value_list_array[idx];
        if (statement->columns_ != nullptr) {
            SizeT statement_column_count = statement->columns_->size();
            Assert<PlannerException>(statement_column_count == value_list.size(), "INSERT: Target column count and input column count mismatch");

            //        Value null_value = Value::MakeNullData();
            //        SharedPtr<BaseExpression> null_value_expr = MakeShared<ValueExpression>(null_value);

            SizeT table_column_count = table_entry->columns_.size();

            // Create value list with table column size and null value
            Vector<SharedPtr<BaseExpression>> rewrite_value_list(table_column_count, nullptr);

            SizeT column_count = statement->columns_->size();
            for (SizeT column_idx = 0; column_idx < column_count; ++column_idx) {
                const auto &column_name = statement->columns_->at(column_idx);
                SizeT table_column_id = table_entry->GetColumnIdByName(column_name);
                const SharedPtr<DataType> &table_column_type = table_entry->columns_[table_column_id]->column_type_;
                DataType value_type = value_list[column_idx]->Type();
                if (value_type == *table_column_type) {
                    rewrite_value_list[table_column_id] = value_list[column_idx];
                } else {
                    if (LogicalInsert::NeedCastInInsert(value_type, *table_column_type)) {
                        // If the inserted value type mismatches with table
                        // column type, cast the inserted value type to correct
                        // one.
                        BoundCastFunc cast = CastFunction::GetBoundFunc(value_type, *table_column_type);
                        SharedPtr<BaseExpression> cast_expr = MakeShared<CastExpression>(cast, value_list[column_idx], *table_column_type);
                        rewrite_value_list[table_column_id] = cast_expr;
                    } else {
                        // LogicalType are same and type info is also OK.
                        rewrite_value_list[column_idx] = value_list[column_idx];
                    }
                }
            }

            value_list = rewrite_value_list;
        } else {
            SizeT table_column_count = table_entry->columns_.size();
            if (value_list.size() != table_column_count) {
                Error<PlannerException>(Format("INSERT: Table column count ({}) and "
                                               "input value count mismatch ({})",
                                               table_column_count,
                                               value_list.size()));
            }

            // Create value list with table column size and null value
            Vector<SharedPtr<BaseExpression>> rewrite_value_list(table_column_count, nullptr);

            for (SizeT column_idx = 0; column_idx < table_column_count; ++column_idx) {
                const SharedPtr<DataType> &table_column_type = table_entry->columns_[column_idx]->column_type_;
                DataType value_type = value_list[column_idx]->Type();
                if (*table_column_type == value_type) {
                    rewrite_value_list[column_idx] = value_list[column_idx];
                } else {
                    if (LogicalInsert::NeedCastInInsert(value_type, *table_column_type)) {
                        BoundCastFunc cast = CastFunction::GetBoundFunc(value_type, *table_column_type);
                        SharedPtr<BaseExpression> cast_expr = MakeShared<CastExpression>(cast, value_list[column_idx], *table_column_type);
                        rewrite_value_list[column_idx] = cast_expr;
                    } else {
                        // LogicalType are same and type info is also OK.
                        rewrite_value_list[column_idx] = value_list[column_idx];
                    }
                }
            }
            value_list = rewrite_value_list;
        }
    }

    // Create logical insert node.
    SharedPtr<LogicalNode> logical_insert =
        MakeShared<LogicalInsert>(bind_context_ptr->GetNewLogicalNodeId(), table_entry, bind_context_ptr->GenerateTableIndex(), value_list_array);

    // FIXME: check if we need to append operator
    //    this->AppendOperator(logical_insert, bind_context_ptr);

    this->logical_plan_ = logical_insert;
    return Status();
}

Status LogicalPlanner::BuildInsertSelect(const InsertStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    Error<PlannerException>("Not supported");
    return Status();
}

Status LogicalPlanner::BuildUpdate(const UpdateStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<QueryBinder> query_binder_ptr = MakeShared<QueryBinder>(this->query_context_ptr_, bind_context_ptr);
    SharedPtr<BoundUpdateStatement> bound_statement_ptr = query_binder_ptr->BindUpdate(*statement);
    this->logical_plan_ = bound_statement_ptr->BuildPlan(query_context_ptr_);
    return Status();
}

Status LogicalPlanner::BuildDelete(const DeleteStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    if (statement->where_expr_ == nullptr) {
        // Rewrite to a DropStatement
        DropStatement statement2;
        auto drop_table_info = MakeShared<DropTableInfo>();
        drop_table_info->schema_name_ = statement->schema_name_;
        drop_table_info->table_name_ = statement->table_name_;
        drop_table_info->conflict_type_ = ConflictType::kError;
        statement2.drop_info_ = drop_table_info;
        LogicalPlanner::BuildDrop(&statement2, bind_context_ptr);
    } else {
        SharedPtr<QueryBinder> query_binder_ptr = MakeShared<QueryBinder>(this->query_context_ptr_, bind_context_ptr);
        SharedPtr<BoundDeleteStatement> bound_statement_ptr = query_binder_ptr->BindDelete(*statement);
        this->logical_plan_ = bound_statement_ptr->BuildPlan(query_context_ptr_);
    }
    return Status();
}

Status LogicalPlanner::BuildCreate(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
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
        case DDLType::kSchema: {
            return BuildCreateSchema(statement, bind_context_ptr);
        }
        default: {
            Error<PlannerException>("Not supported");
        }
    }
    return Status();
}

Status LogicalPlanner::BuildCreateTable(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *create_table_info = (CreateTableInfo *)statement->create_info_.get();

    // Check if columns is given.
    Vector<SharedPtr<ColumnDef>> columns;
    SizeT column_count = create_table_info->column_defs_.size();
    columns.reserve(column_count);
    for (SizeT idx = 0; idx < column_count; ++idx) {
        SharedPtr<ColumnDef> column_def = MakeShared<ColumnDef>(idx,
                                                                create_table_info->column_defs_[idx]->type(),
                                                                create_table_info->column_defs_[idx]->name(),
                                                                create_table_info->column_defs_[idx]->constraints_);
        columns.emplace_back(column_def);
    }

    SharedPtr<String> schema_name_ptr = MakeShared<String>(create_table_info->schema_name_);

    SharedPtr<TableDef> table_def_ptr = TableDef::Make(MakeShared<String>("default"), MakeShared<String>(create_table_info->table_name_), columns);

    SharedPtr<LogicalNode> logical_create_table_operator = LogicalCreateTable::Make(bind_context_ptr->GetNewLogicalNodeId(),
                                                                                    schema_name_ptr,
                                                                                    table_def_ptr,
                                                                                    bind_context_ptr->GenerateTableIndex(),
                                                                                    create_table_info->conflict_type_);

    if (create_table_info->select_ != nullptr) {
        SharedPtr<BindContext> select_bind_context_ptr = BindContext::Make(nullptr);
        QueryBinder select_query_binder(this->query_context_ptr_, select_bind_context_ptr);
        SharedPtr<BoundSelectStatement> bound_statement_ptr = select_query_binder.BindSelect(*create_table_info->select_);
        logical_create_table_operator->set_left_node(bound_statement_ptr->BuildPlan(this->query_context_ptr_));
    }

    this->logical_plan_ = logical_create_table_operator;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status();
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
    return Status();
}

Status LogicalPlanner::BuildCreateSchema(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *create_schema_info = (CreateSchemaInfo *)statement->create_info_.get();

    SharedPtr<String> schema_name_ptr = MakeShared<String>(create_schema_info->schema_name_);

    SharedPtr<LogicalNode> logical_create_schema_operator =
        LogicalCreateSchema::Make(bind_context_ptr->GetNewLogicalNodeId(), schema_name_ptr, create_schema_info->conflict_type_);

    this->logical_plan_ = logical_create_schema_operator;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status();
}

Status LogicalPlanner::BuildCreateView(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    CreateViewInfo *create_view_info = (CreateViewInfo *)(statement->create_info_.get());

    // Check if columns is given.
    SharedPtr<Vector<String>> columns_ptr;
    SizeT column_count = create_view_info->view_columns_->size();

    // Build create view statement
    SharedPtr<QueryBinder> query_binder_ptr = MakeShared<QueryBinder>(this->query_context_ptr_, bind_context_ptr);
    SharedPtr<BoundSelectStatement> bound_statement_ptr = query_binder_ptr->BindSelect(*create_view_info->select_);

    if (column_count == 0) {
        // Not specify the view column
        columns_ptr = bound_statement_ptr->names_ptr_;
    } else {
        // Specify the view column
        Assert<PlannerException>(column_count == bound_statement_ptr->names_ptr_->size(), "Create view column count isn't matched.");
        columns_ptr = MakeShared<Vector<String>>(*(create_view_info->view_columns_));
    }

    SharedPtr<LogicalNode> logical_create_view_operator = LogicalCreateView::Make(bind_context_ptr->GetNewLogicalNodeId(),
                                                                                  columns_ptr,
                                                                                  bound_statement_ptr->types_ptr_,
                                                                                  static_pointer_cast<CreateViewInfo>(statement->create_info_));

    this->logical_plan_ = logical_create_view_operator;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status();
}

Status LogicalPlanner::BuildCreateIndex(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *create_index_info = (CreateIndexInfo *)statement->create_info_.get();

    auto schema_name = MakeShared<String>(create_index_info->schema_name_);
    auto table_name = MakeShared<String>(create_index_info->table_name_);
    if (create_index_info->column_names_->size() != 1) {
        Error<NotImplementException>("Creating index only support single column now.");
    }

    Vector<String> column_names(*create_index_info->column_names_);
    String index_name = Move(create_index_info->index_name_);
    IndexMethod method_type = IndexMethod::kInvalid;
    if (IsEqual(create_index_info->method_type_, "IVFFlat")) {
        method_type = IndexMethod::kIVFFlat;
    } else if (IsEqual(create_index_info->method_type_, "IVFSQ8")) {
        method_type = IndexMethod::kIVFSQ8;
    } else if (IsEqual(create_index_info->method_type_, "HNSW")) {
        method_type = IndexMethod::kHnsw;
    } else {
        PlannerException("Invalid index method type.");
    }
    // auto index_def_common = IndexDefCommon(Move(index_name), Move(method_type), Move(column_names));

    SharedPtr<IndexDef> index_def_ptr = nullptr;
    switch (method_type) {
        case IndexMethod::kIVFFlat: {
            index_def_ptr = IVFFlatIndexDef::Make(MakeShared<String>(index_name), Move(column_names), *create_index_info->index_para_list_);
            break;
        }
        case IndexMethod::kHnsw: {
            index_def_ptr = HnswIndexDef::Make(MakeShared<String>(index_name), Move(column_names), *create_index_info->index_para_list_);
            break;
        }
        case IndexMethod::kInvalid: {
            Error<PlannerException>("Invalid index method type.");
            break;
        }
        default: {
            Error<NotImplementException>("Not implemented");
        }
    }

    auto logical_create_index_operator =
        LogicalCreateIndex::Make(bind_context_ptr->GetNewLogicalNodeId(), schema_name, table_name, index_def_ptr, create_index_info->conflict_type_);

    this->logical_plan_ = logical_create_index_operator;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status();
}

Status LogicalPlanner::BuildDrop(const DropStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    switch (statement->ddl_type()) {
        case DDLType::kTable: {
            return BuildDropTable(statement, bind_context_ptr);
        }
        case DDLType::kCollection: {
            return BuildDropCollection(statement, bind_context_ptr);
        }
        case DDLType::kSchema: {
            return BuildDropSchema(statement, bind_context_ptr);
        }
        case DDLType::kIndex: {
            return BuildDropIndex(statement, bind_context_ptr);
        }
        case DDLType::kView: {
            return BuildDropView(statement, bind_context_ptr);
        }
        case DDLType::kInvalid: {
            Error<PlannerException>("Invalid drop statement type.");
        }
    }
    return Status();
}

Status LogicalPlanner::BuildDropTable(const DropStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *drop_table_info = (DropTableInfo *)statement->drop_info_.get();

    SharedPtr<String> schema_name_ptr{nullptr};
    if (drop_table_info->schema_name_.empty()) {
        schema_name_ptr = MakeShared<String>("default");
    } else {
        schema_name_ptr = MakeShared<String>(drop_table_info->schema_name_);
    }

    SharedPtr<String> table_name_ptr = MakeShared<String>(drop_table_info->table_name_);
    SharedPtr<LogicalNode> logical_drop_table =
        MakeShared<LogicalDropTable>(bind_context_ptr->GetNewLogicalNodeId(), schema_name_ptr, table_name_ptr, drop_table_info->conflict_type_);

    this->logical_plan_ = logical_drop_table;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status();
}

Status LogicalPlanner::BuildDropCollection(const DropStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *drop_collection_info = (DropCollectionInfo *)statement->drop_info_.get();

    SharedPtr<String> schema_name_ptr{nullptr};
    if (drop_collection_info->schema_name_.empty()) {
        schema_name_ptr = MakeShared<String>("default");
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
    return Status();
}

Status LogicalPlanner::BuildDropSchema(const DropStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *drop_schema_info = (DropSchemaInfo *)statement->drop_info_.get();
    if (drop_schema_info->schema_name_ == query_context_ptr_->schema_name()) {
        Error<PlannerException>(Format("Can't drop using database: {}", drop_schema_info->schema_name_));
    }

    SharedPtr<String> schema_name_ptr = MakeShared<String>(drop_schema_info->schema_name_);

    SharedPtr<LogicalNode> logical_drop_schema =
        MakeShared<LogicalDropSchema>(bind_context_ptr->GetNewLogicalNodeId(), schema_name_ptr, drop_schema_info->conflict_type_);

    this->logical_plan_ = logical_drop_schema;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status();
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
    return Status();
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
    return Status();
}

Status LogicalPlanner::BuildPrepare(const PrepareStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    Error<PlannerException>("Prepare statement isn't supported.");
}

Status LogicalPlanner::BuildExecute(const ExecuteStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    Error<PlannerException>("Execute statement isn't supported.");
}

Status LogicalPlanner::BuildCopy(const CopyStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    if (statement->copy_from_) {
        // Import
        return BuildImport(statement, bind_context_ptr);
    } else {
        // Export
        return BuildExport(statement, bind_context_ptr);
    }
}

Status LogicalPlanner::BuildExport(const CopyStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    // Check the table existence
    Txn *txn = query_context_ptr_->GetTxn();
    EntryResult result = txn->GetTableByName(statement->schema_name_, statement->table_name_);
    if (result.err_.get() != nullptr) {
        Error<PlannerException>(*result.err_);
    }

    // Check the file existence
    LocalFileSystem fs;

    String to_write_path;
    if (!fs.Exists(statement->file_path_)) {
        Error<PlannerException>(Format("File: {} doesn't exist.", statement->file_path_));
    }

    SharedPtr<LogicalNode> logical_export = MakeShared<LogicalExport>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                      statement->schema_name_,
                                                                      statement->table_name_,
                                                                      statement->file_path_,
                                                                      statement->header_,
                                                                      statement->delimiter_,
                                                                      statement->copy_file_type_);

    this->logical_plan_ = logical_export;
    return Status();
}

Status LogicalPlanner::BuildImport(const CopyStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    // Check the table existence
    Txn *txn = query_context_ptr_->GetTxn();
    EntryResult result = txn->GetTableByName(statement->schema_name_, statement->table_name_);
    if (result.entry_ == nullptr) {
        Error<PlannerException>(*result.err_);
    }
    auto table_collection_entry = dynamic_cast<TableCollectionEntry *>(result.entry_);

    // Check the file existence
    LocalFileSystem fs;

    String to_write_path;
    if (!fs.Exists(statement->file_path_)) {
        Error<PlannerException>(Format("File: {} doesn't exist.", fs.GetAbsolutePath(statement->file_path_)));
    }

    SharedPtr<LogicalNode> logical_import = MakeShared<LogicalImport>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                      table_collection_entry,
                                                                      statement->file_path_,
                                                                      statement->header_,
                                                                      statement->delimiter_,
                                                                      statement->copy_file_type_);

    this->logical_plan_ = logical_import;
    return Status();
}

Status LogicalPlanner::BuildAlter(const AlterStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    Error<PlannerException>("Alter statement isn't supported.");
}

Status LogicalPlanner::BuildCommand(const CommandStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    Txn *txn = query_context_ptr_->GetTxn();
    auto *command_statement = (CommandStatement *)statement;
    switch (command_statement->command_info_->type()) {
        case CommandType::kUse: {
            UseCmd *use_command_info = (UseCmd *)(command_statement->command_info_.get());
            EntryResult result = txn->GetDatabase(use_command_info->db_name());
            if (result.Success()) {
                SharedPtr<LogicalNode> logical_command =
                    MakeShared<LogicalCommand>(bind_context_ptr->GetNewLogicalNodeId(), command_statement->command_info_);

                this->logical_plan_ = logical_command;
            } else {
                Error<PlannerException>("Invalid command type.");
            }
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
            EntryResult result = txn->GetTableByName(query_context_ptr_->schema_name(), check_table->table_name());
            if (result.Success()) {
                SharedPtr<LogicalNode> logical_command =
                    MakeShared<LogicalCommand>(bind_context_ptr->GetNewLogicalNodeId(), command_statement->command_info_);

                this->logical_plan_ = logical_command;
            } else {
                Error<PlannerException>("Invalid command type.");
            }
            break;
        }
        default: {
            Error<PlannerException>("Invalid command type.");
        }
    }
    return Status();
}

Status LogicalPlanner::BuildShow(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    switch (statement->show_type_) {
        case ShowStmtType::kDatabases: {
            return BuildShowDatabases(statement, bind_context_ptr);
        }
        case ShowStmtType::kTables:
        case ShowStmtType::kCollections: {
            return BuildShowTables(statement, bind_context_ptr);
        }
        case ShowStmtType::kViews: {
            return BuildShowViews(statement, bind_context_ptr);
        }
        case ShowStmtType::kColumns: {
            return BuildShowColumns(statement, bind_context_ptr);
        }
        case ShowStmtType::kIndexes: {
            return BuildShowIndexes(statement, bind_context_ptr);
        }
        default: {
            Error<PlannerException>("Unexpected show statement type.");
        }
    }
    return Status();
}

Status LogicalPlanner::BuildShowIndexes(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowIndexes,
                                                                  statement->schema_name_,
                                                                  statement->table_name_,
                                                                  bind_context_ptr->GenerateTableIndex());
    this->logical_plan_ = logical_show;
    return Status();
}

Status LogicalPlanner::BuildShowColumns(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowColumn,
                                                                  statement->schema_name_,
                                                                  statement->table_name_,
                                                                  bind_context_ptr->GenerateTableIndex());

    this->logical_plan_ = logical_show;
    return Status();
}

Status LogicalPlanner::BuildShowTables(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    String object_name;
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowTables,
                                                                  query_context_ptr_->schema_name(),
                                                                  object_name,
                                                                  bind_context_ptr->GenerateTableIndex());

    // FIXME: check if we need to append operator
    //    this->AppendOperator(logical_show, bind_context_ptr);
    this->logical_plan_ = logical_show;
    return Status();
}

Status LogicalPlanner::BuildShowViews(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    String object_name;
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowViews,
                                                                  query_context_ptr_->schema_name(),
                                                                  object_name,
                                                                  bind_context_ptr->GenerateTableIndex());
    this->logical_plan_ = logical_show;
    return Status();
}

Status LogicalPlanner::BuildShowDatabases(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    String object_name;
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowDatabases,
                                                                  query_context_ptr_->schema_name(),
                                                                  object_name,
                                                                  bind_context_ptr->GenerateTableIndex());
    this->logical_plan_ = logical_show;
    return Status();
}

Status LogicalPlanner::BuildFlush(const FlushStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    switch (statement->type()) {
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
    return Status();
}

Status LogicalPlanner::BuildFlushData(const FlushStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_flush = MakeShared<LogicalFlush>(bind_context_ptr->GetNewLogicalNodeId(), FlushType::kData);
    this->logical_plan_ = logical_flush;
    return Status();
}

Status LogicalPlanner::BuildFlushLog(const FlushStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_flush = MakeShared<LogicalFlush>(bind_context_ptr->GetNewLogicalNodeId(), FlushType::kLog);
    this->logical_plan_ = logical_flush;
    return Status();
}

Status LogicalPlanner::BuildFlushBuffer(const FlushStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_flush = MakeShared<LogicalFlush>(bind_context_ptr->GetNewLogicalNodeId(), FlushType::kBuffer);
    this->logical_plan_ = logical_flush;
    return Status();
}

Status LogicalPlanner::BuildExplain(const ExplainStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<QueryBinder> query_binder_ptr = MakeShared<QueryBinder>(this->query_context_ptr_, bind_context_ptr);

    SharedPtr<LogicalExplain> explain_node = MakeShared<LogicalExplain>(bind_context_ptr->GetNewLogicalNodeId(), statement->type_);

    switch (statement->type_) {
        case ExplainType::kAst: {
            SharedPtr<Vector<SharedPtr<String>>> texts_ptr = MakeShared<Vector<SharedPtr<String>>>();
            ExplainAST::Explain(statement->statement_, texts_ptr);
            explain_node->SetText(texts_ptr);
            break;
        }
        case ExplainType::kUnOpt: {
            Build(statement->statement_, bind_context_ptr);
            SharedPtr<Vector<SharedPtr<String>>> texts_ptr = MakeShared<Vector<SharedPtr<String>>>();
            ExplainLogicalPlan::Explain(this->logical_plan_.get(), texts_ptr);
            explain_node->SetText(texts_ptr);
            break;
        }
        default: {
            Build(statement->statement_, bind_context_ptr);
            explain_node->set_left_node(this->logical_plan_);
        }
    }

    this->logical_plan_ = explain_node;
    return Status();
}

} // namespace infinity
