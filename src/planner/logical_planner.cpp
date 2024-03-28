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
#include <tuple>
#include <vector>

module logical_planner;

import stl;
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
import txn;
import table_entry_type;
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
import logical_optimize;
import logical_export;
import logical_import;
import logical_explain;
import logical_command;
import explain_logical_plan;
import explain_ast;

import local_file_system;

import status;
import default_values;
import index_base;
import index_ivfflat;
import index_hnsw;
import index_secondary;
import index_full_text;
import base_table_ref;
import table_ref;
import logical_type;

import extra_ddl_info;
import create_schema_info;
import create_table_info;
import create_index_info;
import create_collection_info;
import create_view_info;
import drop_collection_info;
import drop_index_info;
import drop_schema_info;
import drop_table_info;
import drop_view_info;
import column_def;

namespace {

using namespace infinity;

enum class IdentifierValidationStatus {
    kOk,
    kEmpty,
    kExceedLimit,
    kInvalidName,
};

IdentifierValidationStatus IdentifierValidation(const String &identifier) {
    if (identifier.empty()) {
        return IdentifierValidationStatus::kEmpty;
    }

    u64 identifier_len = identifier.length();
    if (identifier_len >= MAX_IDENTIFIER_NAME_LENGTH) {
        return IdentifierValidationStatus::kExceedLimit;
    }

    if (!std::isalpha(identifier[0]) && identifier[0] != '_') {
        return IdentifierValidationStatus::kInvalidName;
    }
    for (SizeT i = 1; i < identifier_len; i++) {
        char ch = identifier[i];
        if (!std::isalnum(ch) && ch != '_') {
            return IdentifierValidationStatus::kInvalidName;
        }
    }

    return IdentifierValidationStatus::kOk;
}

} // namespace

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
            return BuildInsert(const_cast<InsertStatement *>(static_cast<const InsertStatement *>(statement)), bind_context_ptr);
        }
        case StatementType::kUpdate: {
            return BuildUpdate(static_cast<const UpdateStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kDelete: {
            return BuildDelete(static_cast<const DeleteStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kCreate: {
            return BuildCreate(const_cast<CreateStatement *>(static_cast<const CreateStatement *>(statement)), bind_context_ptr);
        }
        case StatementType::kDrop: {
            return BuildDrop(const_cast<DropStatement *>(static_cast<const DropStatement *>(statement)), bind_context_ptr);
        }
        case StatementType::kShow: {
            return BuildShow(const_cast<ShowStatement *>(static_cast<const ShowStatement *>(statement)), bind_context_ptr);
        }
        case StatementType::kFlush: {
            return BuildFlush(static_cast<const FlushStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kOptimize: {
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
            return BuildAlter(static_cast<const AlterStatement *>(statement), bind_context_ptr);
        }
        case StatementType::kCommand: {
            return BuildCommand(static_cast<const CommandStatement *>(statement), bind_context_ptr);
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
        UnrecoverableError("Insert statement missing table table_name.");
    }
    // Check schema and table in the catalog
    Txn *txn = query_context_ptr_->GetTxn();
    auto [table_entry, status] = txn->GetTableByName(schema_name, table_name);
    if (!status.ok()) {
        RecoverableError(status);
    }

    if (table_entry->EntryType() == TableEntryType::kCollectionEntry) {
        RecoverableError(Status::NotSupport("Currently, collection isn't supported."));
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
            if (statement_column_count != value_list.size()) {
                RecoverableError(Status::SyntaxError(fmt::format("INSERT: Target column count ({}) and "
                                                                 "input value count mismatch ({})",
                                                                 statement_column_count,
                                                                 value_list.size())));
            }

            //        Value null_value = Value::MakeNullData();
            //        SharedPtr<BaseExpression> null_value_expr = MakeShared<ValueExpression>(null_value);

            SizeT table_column_count = table_entry->ColumnCount();

            // Create value list with table column size and null value
            Vector<SharedPtr<BaseExpression>> rewrite_value_list(table_column_count, nullptr);

            SizeT column_count = statement->columns_->size();

            if (column_count != table_column_count) {
                RecoverableError(Status::SyntaxError(fmt::format("INSERT: Table column count ({}) and "
                                                                 "input value count mismatch ({})",
                                                                 table_column_count,
                                                                 column_count)));
            }

            for (SizeT column_idx = 0; column_idx < column_count; ++column_idx) {
                const auto &column_name = statement->columns_->at(column_idx);
                SizeT table_column_id = table_entry->GetColumnIdByName(column_name);
                const SharedPtr<DataType> &table_column_type = table_entry->GetColumnDefByID(table_column_id)->column_type_;
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
            SizeT table_column_count = table_entry->ColumnCount();
            if (value_list.size() != table_column_count) {
                RecoverableError(Status::SyntaxError(fmt::format("INSERT: Table column count ({}) and "
                                                                 "input value count mismatch ({})",
                                                                 table_column_count,
                                                                 value_list.size())));
            }

            // Create value list with table column size and null value
            Vector<SharedPtr<BaseExpression>> rewrite_value_list(table_column_count, nullptr);

            for (SizeT column_idx = 0; column_idx < table_column_count; ++column_idx) {
                const SharedPtr<DataType> &table_column_type = table_entry->GetColumnDefByID(column_idx)->column_type_;
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
    return Status::OK();
}

Status LogicalPlanner::BuildInsertSelect(const InsertStatement *, SharedPtr<BindContext> &) {
    RecoverableError(Status::NotSupport("Not supported"));
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
            UnrecoverableError("Not supported");
        }
    }
    return Status::OK();
}

Status LogicalPlanner::BuildCreateTable(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *create_table_info = (CreateTableInfo *)statement->create_info_.get();

    // Check if columns is given.
    Vector<SharedPtr<ColumnDef>> columns;
    SizeT column_count = create_table_info->column_defs_.size();
    if (column_count == 0) {
        return Status::NoColumnDefined(create_table_info->table_name_);
    }

    columns.reserve(column_count);
    for (SizeT idx = 0; idx < column_count; ++idx) {

        switch (IdentifierValidation(create_table_info->column_defs_[idx]->name())) {
            case IdentifierValidationStatus::kOk:
                break;
            case IdentifierValidationStatus::kEmpty:
                return Status::EmptyColumnName();
            case IdentifierValidationStatus::kExceedLimit:
                return Status::ExceedColumnNameLength(create_table_info->column_defs_[idx]->name().length());
            case IdentifierValidationStatus::kInvalidName: {
                return Status::InvalidColumnName(create_table_info->column_defs_[idx]->name());
            }
        }

        SharedPtr<ColumnDef> column_def = MakeShared<ColumnDef>(idx,
                                                                create_table_info->column_defs_[idx]->type(),
                                                                create_table_info->column_defs_[idx]->name(),
                                                                create_table_info->column_defs_[idx]->constraints_);
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

    SharedPtr<TableDef> table_def_ptr = TableDef::Make(MakeShared<String>("default"), MakeShared<String>(create_table_info->table_name_), columns);
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
            for (Vector<SharedPtr<ColumnDef>> &columns = table_def_ptr->columns(); ColumnID column_id : bloom_filter_columns) {
                if (auto &def = columns[column_id]; def->type()->SupportBloomFilter()) {
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
            return Status::InvalidDBName(create_schema_info->schema_name_);
        }
    }

    SharedPtr<String> schema_name_ptr = MakeShared<String>(create_schema_info->schema_name_);

    SharedPtr<LogicalNode> logical_create_schema_operator =
        LogicalCreateSchema::Make(bind_context_ptr->GetNewLogicalNodeId(), schema_name_ptr, create_schema_info->conflict_type_);

    this->logical_plan_ = logical_create_schema_operator;
    this->names_ptr_->emplace_back("OK");
    this->types_ptr_->emplace_back(LogicalType::kInteger);
    return Status::OK();
}

Status LogicalPlanner::BuildCreateView(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
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
            UnrecoverableError("Create view column count isn't matched.");
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
}

Status LogicalPlanner::BuildCreateIndex(const CreateStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    auto *create_index_info = (CreateIndexInfo *)statement->create_info_.get();

    auto schema_name = MakeShared<String>(create_index_info->schema_name_);
    auto table_name = MakeShared<String>(create_index_info->table_name_);
    if (table_name->empty()) {
        RecoverableError(Status::InvalidIndexName("No index name."));
    }
    //    if (create_index_info->column_names_->size() != 1) {
    //        UnrecoverableError("Creating index only support single column now.");
    //    }

    SharedPtr<String> index_name = MakeShared<String>(std::move(create_index_info->index_name_));
    UniquePtr<QueryBinder> query_binder_ptr = MakeUnique<QueryBinder>(this->query_context_ptr_, bind_context_ptr);
    auto base_table_ref = query_binder_ptr->GetTableRef(*schema_name, *table_name);

    auto &index_name_map = base_table_ref->table_entry_ptr_->index_meta_map();

    if (index_name_map.contains(*index_name)) {
        if (create_index_info->conflict_type_ == ConflictType::kError) {
            RecoverableError(Status::DuplicateIndex(fmt::format("Duplicated index name: {}", *index_name)));
        }
    }

    if (create_index_info->index_info_list_->size() != 1) {
        RecoverableError(Status::InvalidIndexDefinition(
            fmt::format("Index {} consists of {} IndexInfo however 1 is expected", *index_name, create_index_info->index_info_list_->size())));
    }
    IndexInfo *index_info = create_index_info->index_info_list_->at(0);
    SharedPtr<IndexBase> base_index_ptr{nullptr};
    switch (index_info->index_type_) {
        case IndexType::kFullText: {
            IndexFullText::ValidateColumnDataType(base_table_ref, index_info->column_name_);
            base_index_ptr = IndexFullText::Make(index_name,
                                                 fmt::format("{}_{}", create_index_info->table_name_, *index_name),
                                                 {index_info->column_name_},
                                                 *(index_info->index_param_list_));
            break;
        }
        case IndexType::kHnsw: {
            // The following check might affect performance
            IndexHnsw::ValidateColumnDataType(base_table_ref, index_info->column_name_); // may throw exception
            base_index_ptr = IndexHnsw::Make(index_name,
                                             fmt::format("{}_{}", create_index_info->table_name_, *index_name),
                                             {index_info->column_name_},
                                             *(index_info->index_param_list_));
            break;
        }
        case IndexType::kIVFFlat: {
            IndexIVFFlat::ValidateColumnDataType(base_table_ref, index_info->column_name_); // may throw exception
            base_index_ptr = IndexIVFFlat::Make(index_name,
                                                fmt::format("{}_{}", create_index_info->table_name_, *index_name),
                                                {index_info->column_name_},
                                                *(index_info->index_param_list_));
            break;
        }
        case IndexType::kSecondary: {
            IndexSecondary::ValidateColumnDataType(base_table_ref, index_info->column_name_); // may throw exception
            base_index_ptr =
                IndexSecondary::Make(index_name, fmt::format("{}_{}", create_index_info->table_name_, *index_name), {index_info->column_name_});
            break;
        }
        case IndexType::kInvalid: {
            UnrecoverableError("Invalid index type.");
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
            UnrecoverableError("Invalid drop statement type.");
        }
    }
    return Status::OK();
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
    return Status::OK();
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
            return Status::InvalidDBName(drop_schema_info->schema_name_);
        }
    }

    if (IsEqual(drop_schema_info->schema_name_, query_context_ptr_->schema_name())) {
        RecoverableError(Status::DroppingUsingDb(drop_schema_info->schema_name_));
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
    RecoverableError(Status::NotSupport("Prepare statement isn't supported."));
    return Status::OK();
}

Status LogicalPlanner::BuildExecute(const ExecuteStatement *, SharedPtr<BindContext> &) {
    RecoverableError(Status::NotSupport("Execute statement isn't supported."));
    return Status::OK();
}

Status LogicalPlanner::BuildCopy(CopyStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    BindSchemaName(statement->schema_name_);
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

    auto [table_entry, status] = txn->GetTableByName(statement->schema_name_, statement->table_name_);
    if (!status.ok()) {
        RecoverableError(status);
    }

    // Check the file existence
    LocalFileSystem fs;

    String to_write_path;
    if (!fs.Exists(statement->file_path_)) {
        RecoverableError(Status::FileNotFound(statement->file_path_));
    }

    SharedPtr<LogicalNode> logical_export = MakeShared<LogicalExport>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                      statement->schema_name_,
                                                                      statement->table_name_,
                                                                      statement->file_path_,
                                                                      statement->header_,
                                                                      statement->delimiter_,
                                                                      statement->copy_file_type_);

    this->logical_plan_ = logical_export;
    return Status::OK();
}

Status LogicalPlanner::BuildImport(const CopyStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    // Check the table existence
    Txn *txn = query_context_ptr_->GetTxn();
    auto [table_entry, status] = txn->GetTableByName(statement->schema_name_, statement->table_name_);
    if (!status.ok()) {
        RecoverableError(status);
    }

    // Check the file existence
    LocalFileSystem fs;

    String to_write_path;
    if (!fs.Exists(statement->file_path_)) {
        RecoverableError(Status::FileNotFound(statement->file_path_));
    }

    SharedPtr<LogicalNode> logical_import = MakeShared<LogicalImport>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                      table_entry,
                                                                      statement->file_path_,
                                                                      statement->header_,
                                                                      statement->delimiter_,
                                                                      statement->copy_file_type_);

    this->logical_plan_ = logical_import;
    return Status::OK();
}

Status LogicalPlanner::BuildAlter(const AlterStatement *, SharedPtr<BindContext> &) {
    RecoverableError(Status::NotSupport("Alter statement isn't supported."));
    return Status::OK();
}

Status LogicalPlanner::BuildCommand(const CommandStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    Txn *txn = query_context_ptr_->GetTxn();
    auto *command_statement = (CommandStatement *)statement;
    switch (command_statement->command_info_->type()) {
        case CommandType::kUse: {
            UseCmd *use_command_info = (UseCmd *)(command_statement->command_info_.get());
            auto [db_entry, status] = txn->GetDatabase(use_command_info->db_name());
            if (status.ok()) {
                SharedPtr<LogicalNode> logical_command =
                    MakeShared<LogicalCommand>(bind_context_ptr->GetNewLogicalNodeId(), std::move(command_statement->command_info_));

                this->logical_plan_ = logical_command;
            } else {
                return status;
            }
            break;
        }
        case CommandType::kSet: {
            SharedPtr<LogicalNode> logical_command =
                MakeShared<LogicalCommand>(bind_context_ptr->GetNewLogicalNodeId(), std::move(command_statement->command_info_));

            this->logical_plan_ = logical_command;
            break;
        }
        case CommandType::kExport: {
            SharedPtr<LogicalNode> logical_command =
                MakeShared<LogicalCommand>(bind_context_ptr->GetNewLogicalNodeId(), std::move(command_statement->command_info_));

            this->logical_plan_ = logical_command;
            break;
        }
        case CommandType::kCheckTable: {
            CheckTable *check_table = (CheckTable *)(command_statement->command_info_.get());
            auto [table_entry, status] = txn->GetTableByName(query_context_ptr_->schema_name(), check_table->table_name());
            if (status.ok()) {
                SharedPtr<LogicalNode> logical_command =
                    MakeShared<LogicalCommand>(bind_context_ptr->GetNewLogicalNodeId(), std::move(command_statement->command_info_));

                this->logical_plan_ = logical_command;
            } else {
                return status;
            }
            break;
        }
        case CommandType::kCompactTable: {
            auto *compact_table = static_cast<CompactTable *>(command_statement->command_info_.get());
            BindSchemaName(compact_table->schema_name_);

            Txn *txn = query_context_ptr_->GetTxn();
            auto [table_entry, status] = txn->GetTableByName(compact_table->schema_name_, compact_table->table_name_);
            if (!status.ok()) {
                RecoverableError(status);
            }
            auto logical_command = MakeShared<LogicalCommand>(bind_context_ptr->GetNewLogicalNodeId(), std::move(command_statement->command_info_));
            logical_command->table_entry_ = table_entry;

            this->logical_plan_ = logical_command;
            break;
        }
        default: {
            UnrecoverableError("Invalid command type.");
        }
    }
    return Status::OK();
}

Status LogicalPlanner::BuildShow(ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    BindSchemaName(statement->schema_name_);
    switch (statement->show_type_) {
        case ShowStmtType::kDatabase: {
            return BuildShowDatabase(statement, bind_context_ptr);
        }
        case ShowStmtType::kTable: {
            return BuildShowTable(statement, bind_context_ptr);
        }
        case ShowStmtType::kIndex: {
            return BuildShowIndex(statement, bind_context_ptr);
        }
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
        case ShowStmtType::kConfigs: {
            return BuildShowConfigs(statement, bind_context_ptr);
        }
        case ShowStmtType::kProfiles: {
            return BuildShowProfiles(statement, bind_context_ptr);
        }
        case ShowStmtType::kSegments: {
            return BuildShowSegments(statement, bind_context_ptr);
        }
        case ShowStmtType::kSegment: {
            return BuildShowSegment(statement, bind_context_ptr);
        }
        case ShowStmtType::kBlocks: {
            return BuildShowBlocks(statement, bind_context_ptr);
        }
        case ShowStmtType::kSessionStatus: {
            return BuildShowSessionStatus(statement, bind_context_ptr);
        }
        case ShowStmtType::kGlobalStatus: {
            return BuildShowGlobalStatus(statement, bind_context_ptr);
        }
        case ShowStmtType::kVar: {
            return BuildShowVar(statement, bind_context_ptr);
        }
        default: {
            UnrecoverableError("Unexpected show statement type.");
        }
    }
    return Status::OK();
}

Status LogicalPlanner::BuildShowConfigs(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowConfigs,
                                                                  statement->schema_name_,
                                                                  statement->table_name_,
                                                                  bind_context_ptr->GenerateTableIndex());
    this->logical_plan_ = logical_show;
    return Status::OK();
}

Status LogicalPlanner::BuildShowProfiles(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowProfiles,
                                                                  statement->schema_name_,
                                                                  statement->table_name_,
                                                                  bind_context_ptr->GenerateTableIndex());
    this->logical_plan_ = logical_show;
    return Status::OK();
}

Status LogicalPlanner::BuildShowIndexes(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowIndexes,
                                                                  statement->schema_name_,
                                                                  statement->table_name_,
                                                                  bind_context_ptr->GenerateTableIndex());
    this->logical_plan_ = logical_show;
    return Status::OK();
}

Status LogicalPlanner::BuildShowColumns(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowColumn,
                                                                  query_context_ptr_->schema_name(),
                                                                  statement->table_name_,
                                                                  bind_context_ptr->GenerateTableIndex());

    this->logical_plan_ = logical_show;
    return Status::OK();
}

Status LogicalPlanner::BuildShowSegments(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowSegments,
                                                                  query_context_ptr_->schema_name(),
                                                                  statement->table_name_,
                                                                  bind_context_ptr->GenerateTableIndex(),
                                                                  statement->segment_id_,
                                                                  statement->block_id_);

    this->logical_plan_ = logical_show;
    return Status::OK();
}

Status LogicalPlanner::BuildShowSegment(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowSegment,
                                                                  query_context_ptr_->schema_name(),
                                                                  statement->table_name_,
                                                                  bind_context_ptr->GenerateTableIndex(),
                                                                  statement->segment_id_);

    this->logical_plan_ = logical_show;
    return Status::OK();
}

Status LogicalPlanner::BuildShowBlocks(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowBlocks,
                                                                  query_context_ptr_->schema_name(),
                                                                  statement->table_name_,
                                                                  bind_context_ptr->GenerateTableIndex(),
                                                                  statement->segment_id_);

    this->logical_plan_ = logical_show;
    return Status::OK();
}

Status LogicalPlanner::BuildShowTables(const ShowStatement *, SharedPtr<BindContext> &bind_context_ptr) {
    String object_name;
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowTables,
                                                                  query_context_ptr_->schema_name(),
                                                                  object_name,
                                                                  bind_context_ptr->GenerateTableIndex());

    // FIXME: check if we need to append operator
    //    this->AppendOperator(logical_show, bind_context_ptr);
    this->logical_plan_ = logical_show;
    return Status::OK();
}

Status LogicalPlanner::BuildShowViews(const ShowStatement *, SharedPtr<BindContext> &bind_context_ptr) {
    String object_name;
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowViews,
                                                                  query_context_ptr_->schema_name(),
                                                                  object_name,
                                                                  bind_context_ptr->GenerateTableIndex());
    this->logical_plan_ = logical_show;
    return Status::OK();
}

Status LogicalPlanner::BuildShowDatabase(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    String object_name;
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowDatabase,
                                                                  statement->schema_name_,
                                                                  object_name,
                                                                  bind_context_ptr->GenerateTableIndex());
    this->logical_plan_ = logical_show;
    return Status::OK();
}

Status LogicalPlanner::BuildShowTable(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowTable,
                                                                  query_context_ptr_->schema_name(),
                                                                  statement->table_name_,
                                                                  bind_context_ptr->GenerateTableIndex());
    this->logical_plan_ = logical_show;
    return Status::OK();
}

Status LogicalPlanner::BuildShowIndex(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowIndex,
                                                                  query_context_ptr_->schema_name(),
                                                                  statement->table_name_,
                                                                  bind_context_ptr->GenerateTableIndex(),
                                                                  None,
                                                                  None,
                                                                  statement->index_name_);
    this->logical_plan_ = logical_show;
    return Status::OK();
}

Status LogicalPlanner::BuildShowDatabases(const ShowStatement *, SharedPtr<BindContext> &bind_context_ptr) {
    String object_name;
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowDatabases,
                                                                  query_context_ptr_->schema_name(),
                                                                  object_name,
                                                                  bind_context_ptr->GenerateTableIndex());
    this->logical_plan_ = logical_show;
    return Status::OK();
}

Status LogicalPlanner::BuildShowSessionStatus(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowSessionStatus,
                                                                  query_context_ptr_->schema_name(),
                                                                  statement->table_name_,
                                                                  bind_context_ptr->GenerateTableIndex(),
                                                                  statement->segment_id_,
                                                                  statement->block_id_);

    this->logical_plan_ = logical_show;
    return Status::OK();
}

Status LogicalPlanner::BuildShowGlobalStatus(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowGlobalStatus,
                                                                  query_context_ptr_->schema_name(),
                                                                  statement->table_name_,
                                                                  bind_context_ptr->GenerateTableIndex(),
                                                                  statement->segment_id_,
                                                                  statement->block_id_);

    this->logical_plan_ = logical_show;
    return Status::OK();
}

Status LogicalPlanner::BuildShowVar(const ShowStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    SharedPtr<LogicalNode> logical_show = MakeShared<LogicalShow>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                  ShowType::kShowVar,
                                                                  query_context_ptr_->schema_name(),
                                                                  statement->var_name_,
                                                                  bind_context_ptr->GenerateTableIndex(),
                                                                  statement->segment_id_,
                                                                  statement->block_id_);

    this->logical_plan_ = logical_show;
    return Status::OK();
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
    switch (statement->type()) {
        case OptimizeType::kIRS: {
            SharedPtr<LogicalNode> logical_optimize =
                MakeShared<LogicalOptimize>(bind_context_ptr->GetNewLogicalNodeId(), query_context_ptr_->schema_name(), statement->table_name_);
            this->logical_plan_ = logical_optimize;
            break;
        }
    }
    return Status::OK();
}

Status LogicalPlanner::BuildExplain(const ExplainStatement *statement, SharedPtr<BindContext> &bind_context_ptr) {
    UniquePtr<QueryBinder> query_binder_ptr = MakeUnique<QueryBinder>(this->query_context_ptr_, bind_context_ptr);

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
    return Status::OK();
}

void LogicalPlanner::BindSchemaName(String &schema_name) const {
    if (schema_name.empty()) {
        schema_name = query_context_ptr_->schema_name();
    }
}

} // namespace infinity
