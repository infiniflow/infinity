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

module infinity_core:explain_ast.impl;

import :explain_ast;
import :status;
import :infinity_exception;

import std;
import third_party;

import drop_schema_info;
import drop_table_info;
import base_table_reference;
import cross_product_reference;
import join_reference;
import subquery_reference;
import table_reference;
import statement_common;
import base_statement;
import select_statement;
import copy_statement;
import insert_statement;
import create_statement;
import drop_statement;
import show_statement;
import flush_statement;
import optimize_statement;
import check_statement;
import parsed_expr;
import extra_ddl_info;
import create_schema_info;
import create_table_info;
import create_index_info;
import create_collection_info;
import drop_collection_info;

namespace infinity {

Status ExplainAST::Explain(const BaseStatement *statement, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &stmt_string, i64 intent_size) {
    switch (statement->Type()) {
        case StatementType::kSelect: {
            return BuildSelect((SelectStatement *)statement, stmt_string, intent_size);
        }
        case StatementType::kCopy: {
            return BuildCopy((CopyStatement *)statement, stmt_string, intent_size);
        }
        case StatementType::kInsert: {
            return BuildInsert((InsertStatement *)statement, stmt_string, intent_size);
        }
        case StatementType::kUpdate:
            break;
        case StatementType::kDelete:
            break;
        case StatementType::kCreate: {
            return BuildCreate((CreateStatement *)statement, stmt_string, intent_size);
        }
        case StatementType::kDrop: {
            return BuildDrop((DropStatement *)statement, stmt_string, intent_size);
        }
        case StatementType::kPrepare:
            return Status::NotSupport("Explain PREPARE statement isn't supported.");
        case StatementType::kExecute:
            return Status::NotSupport("Explain EXECUTE statement isn't supported.");
        case StatementType::kAlter:
            break;
        case StatementType::kShow: {
            return BuildShow((ShowStatement *)statement, stmt_string, intent_size);
            break;
        }
        case StatementType::kFlush: {
            return BuildFlush((FlushStatement *)statement, stmt_string, intent_size);
            break;
        }
        default: {
            UnrecoverableError("Unexpected statement type");
        }
    }
    return Status::OK();
}

Status ExplainAST::BuildCreate(const CreateStatement *create_statement,
                               std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                               i64 intent_size) {

    switch (create_statement->ddl_type()) {
        case DDLType::kInvalid: {
            UnrecoverableError("Invalid DDL type.");
        }
        case DDLType::kDatabase: {
            std::string create_schema = std::string(intent_size, ' ') + "CREATE SCHEMA: ";
            result->emplace_back(std::make_shared<std::string>(create_schema));
            auto *schema_info = (CreateSchemaInfo *)create_statement->create_info_.get();
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "table_name: " + schema_info->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string conflict = std::string(intent_size, ' ') + "conflict type: " + ConflictTypeToStr(schema_info->conflict_type_);
            result->emplace_back(std::make_shared<std::string>(conflict));
            break;
        }
        case DDLType::kTable: {
            std::string create_table = std::string(intent_size, ' ') + "CREATE TABLE: ";
            result->emplace_back(std::make_shared<std::string>(create_table));
            auto *table_info = (CreateTableInfo *)create_statement->create_info_.get();
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + table_info->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string table_name = std::string(intent_size, ' ') + "table: " + table_info->table_name_;
            result->emplace_back(std::make_shared<std::string>(table_name));
            std::string conflict = std::string(intent_size, ' ') + "conflict type: " + ConflictTypeToStr(table_info->conflict_type_);
            result->emplace_back(std::make_shared<std::string>(conflict));
            std::string column_names = std::string(intent_size, ' ') + "columns: (";

            size_t column_count = table_info->column_defs_.size();
            if (column_count == 0) {
                UnrecoverableError("Table definition without any columns");
            }

            for (size_t idx = 0; idx < column_count - 1; ++idx) {
                column_names += table_info->column_defs_[idx]->ToString() + ", ";
            }
            column_names += table_info->column_defs_.back()->ToString();
            column_names += ")";
            result->emplace_back(std::make_shared<std::string>(column_names));

            size_t constraint_count = table_info->constraints_.size();
            if (constraint_count > 0) {
                std::string constraints_str = "Constraints: (";
                for (size_t idx = 0; idx < constraint_count - 1; ++idx) {
                    constraints_str += table_info->constraints_[idx]->ToString() + ", ";
                }
                constraints_str += table_info->constraints_.back()->ToString() + ")";
                result->emplace_back(std::make_shared<std::string>(constraints_str));
            }
            break;
        }
        case DDLType::kCollection: {
            std::string create_collection = std::string(intent_size, ' ') + "CREATE COLLECTION: ";
            result->emplace_back(std::make_shared<std::string>(create_collection));
            auto *collection_info = (CreateCollectionInfo *)create_statement->create_info_.get();
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + collection_info->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string collection_name = std::string(intent_size, ' ') + "table_name: " + collection_info->collection_name_;
            result->emplace_back(std::make_shared<std::string>(collection_name));
            std::string conflict = std::string(intent_size, ' ') + "conflict type: " + ConflictTypeToStr(collection_info->conflict_type_);
            result->emplace_back(std::make_shared<std::string>(conflict));
            break;
        }
        case DDLType::kView: {
            return Status::NotSupport("Explain create view isn't supported");
        }
        case DDLType::kIndex: {
            break;
        }
    }
    return Status::OK();
}

Status ExplainAST::BuildInsert(const InsertStatement *insert_statement,
                               std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                               i64 intent_size) {
    result->emplace_back(std::make_shared<std::string>("INSERT: "));
    intent_size += 2;
    std::string schema_name = std::string(intent_size, ' ') + "database: " + insert_statement->schema_name_;
    result->emplace_back(std::make_shared<std::string>(schema_name));
    std::string table_name = std::string(intent_size, ' ') + "table: " + insert_statement->table_name_;
    result->emplace_back(std::make_shared<std::string>(table_name));
    std::string values = std::string(intent_size, ' ') + "insert rows: (";
    const size_t row_count = insert_statement->insert_rows_.size();
    if (row_count == 0) {
        UnrecoverableError("Insert rows list is empty");
    }
    for (size_t idx = 0; idx < row_count; ++idx) {
        if (idx != 0)
            values += ", ";
        auto *insert_row_expr = insert_statement->insert_rows_.at(idx).get();
        size_t column_count = insert_row_expr->columns_.size();
        if (column_count && column_count != insert_row_expr->values_.size()) {
            RecoverableError(Status::SyntaxError("Column count and value count mismatch"));
        }
        if (column_count) {
            values += "columns: (";
            for (size_t idx2 = 0; idx2 < column_count; ++idx2) {
                if (idx2) {
                    values += ", ";
                }
                values += insert_row_expr->columns_[idx2];
            }
            values += "), values: (";
        } else {
            values += "values: (";
            column_count = insert_row_expr->values_.size();
        }
        for (size_t idx2 = 0; idx2 < column_count; ++idx2) {
            if (idx2) {
                values += ", ";
            }
            values += insert_row_expr->values_[idx2]->ToString();
        }
        values += ")";
    }
    values += ")";
    result->emplace_back(std::make_shared<std::string>(values));
    return Status::OK();
}

Status
ExplainAST::BuildDrop(const DropStatement *drop_statement, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    switch (drop_statement->ddl_type()) {
        case DDLType::kInvalid: {
            UnrecoverableError("Invalid DDL type.");
        }
        case DDLType::kDatabase: {
            std::string drop_schema = std::string(intent_size, ' ') + "DROP SCHEMA: ";
            result->emplace_back(std::make_shared<std::string>(drop_schema));
            auto *schema_info = (DropSchemaInfo *)drop_statement->drop_info_.get();
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "table_name: " + schema_info->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string conflict = std::string(intent_size, ' ') + "conflict type: " + ConflictTypeToStr(schema_info->conflict_type_);
            result->emplace_back(std::make_shared<std::string>(conflict));
            break;
        }
        case DDLType::kTable: {
            std::string drop_table = std::string(intent_size, ' ') + "CREATE TABLE: ";
            result->emplace_back(std::make_shared<std::string>(drop_table));
            auto *table_info = (DropTableInfo *)drop_statement->drop_info_.get();
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + table_info->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string table_name = std::string(intent_size, ' ') + "table: " + table_info->table_name_;
            result->emplace_back(std::make_shared<std::string>(table_name));
            std::string conflict = std::string(intent_size, ' ') + "conflict type: " + ConflictTypeToStr(table_info->conflict_type_);
            result->emplace_back(std::make_shared<std::string>(conflict));
            break;
        }
        case DDLType::kCollection: {
            std::string drop_collection = std::string(intent_size, ' ') + "DROP COLLECTION: ";
            result->emplace_back(std::make_shared<std::string>(drop_collection));
            auto *collection_info = (DropCollectionInfo *)drop_statement->drop_info_.get();
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + collection_info->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string collection_name = std::string(intent_size, ' ') + "table_name: " + collection_info->collection_name_;
            result->emplace_back(std::make_shared<std::string>(collection_name));
            std::string conflict = std::string(intent_size, ' ') + "conflict type: " + ConflictTypeToStr(collection_info->conflict_type_);
            result->emplace_back(std::make_shared<std::string>(conflict));
            break;
        }
        case DDLType::kView: {
            return Status::NotSupport("DROP VIEW: Not supported");
            break;
        }
        case DDLType::kIndex: {
            break;
        }
    }
    return Status::OK();
}

Status ExplainAST::BuildSelect(const SelectStatement *select_statement,
                               std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                               i64 intent_size,
                               std::shared_ptr<std::string> alias_ptr) {
    Status status = Status::OK();
    if (alias_ptr.get() != nullptr) {
        std::string select_str = std::string(intent_size, ' ') + "SELECT AS " + *alias_ptr;
        result->emplace_back(std::make_shared<std::string>(select_str));
    } else {
        std::string select_str = std::string(intent_size, ' ') + "SELECT";
        result->emplace_back(std::make_shared<std::string>(select_str));
    }

    intent_size += 2;
    {
        if (select_statement->with_exprs_ != nullptr) {
            std::string with_str = std::string(intent_size, ' ') + "WITH";
            result->emplace_back(std::make_shared<std::string>(with_str));

            intent_size += 2;

            size_t with_count = select_statement->with_exprs_->size();
            for (size_t idx = 0; idx < with_count; ++idx) {
                auto *with_expr = select_statement->with_exprs_->at(idx);
                std::shared_ptr<std::string> alias_str = std::make_shared<std::string>(with_expr->alias_);
                status = BuildSelect((SelectStatement *)with_expr->select_, result, intent_size, alias_ptr);
                if (!status.ok()) {
                    return status;
                }
            }
            intent_size -= 2;
        }
    }

    {
        std::string projection_str = std::string(intent_size, ' ') + "projection: ";
        size_t select_count = select_statement->select_list_->size();
        if (select_count == 0) {
            UnrecoverableError("No select list");
        }
        for (size_t idx = 0; idx < select_count - 1; ++idx) {
            ParsedExpr *expr = select_statement->select_list_->at(idx);
            projection_str += expr->ToString() + ", ";
        }
        projection_str += select_statement->select_list_->back()->ToString();

        result->emplace_back(std::make_shared<std::string>(projection_str));
    }

    if (select_statement->highlight_list_ != nullptr) {
        std::string highlight_str = std::string(intent_size, ' ') + "highlight: ";
        size_t highlight_count = select_statement->highlight_list_->size();
        if (highlight_count == 0) {
            UnrecoverableError("No highlight expression");
        }
        for (size_t idx = 0; idx < highlight_count - 1; ++idx) {
            ParsedExpr *expr = select_statement->highlight_list_->at(idx);
            highlight_str += expr->ToString() + ", ";
        }
        highlight_str += select_statement->highlight_list_->back()->ToString();

        result->emplace_back(std::make_shared<std::string>(highlight_str));
    }

    status = BuildBaseTableRef(select_statement->table_ref_, result, intent_size);
    if (!status.ok()) {
        return status;
    }

    if (select_statement->where_expr_ != nullptr) {
        std::string filter_str = std::string(intent_size, ' ') + "filter: " + select_statement->where_expr_->ToString();
        result->emplace_back(std::make_shared<std::string>(filter_str));
    }

    if (select_statement->group_by_list_ != nullptr) {
        std::string group_by = std::string(intent_size, ' ') + "groupby: ";
        size_t group_count = select_statement->group_by_list_->size();
        for (size_t idx = 0; idx < group_count - 1; ++idx) {
            group_by += select_statement->group_by_list_->at(idx)->ToString() + ", ";
        }
        group_by += select_statement->group_by_list_->back()->ToString();
        result->emplace_back(std::make_shared<std::string>(group_by));
    }

    if (select_statement->having_expr_ != nullptr) {
        std::string having_str = std::string(intent_size, ' ') + "having: " + select_statement->having_expr_->ToString();
        result->emplace_back(std::make_shared<std::string>(having_str));
    }

    if (select_statement->order_by_list_ != nullptr) {
        std::string order_str = std::string(intent_size, ' ') + "groupby: ";
        size_t order_count = select_statement->order_by_list_->size();
        for (size_t idx = 0; idx < order_count - 1; ++idx) {
            OrderByExpr *order_expr = select_statement->order_by_list_->at(idx);
            order_str += order_expr->expr_->ToString();
            if (order_expr->type_ == OrderType::kAsc) {
                order_str += " Ascending, ";
            } else {
                order_str += " Descending, ";
            }
        }
        OrderByExpr *order_expr = select_statement->order_by_list_->back();
        order_str += order_expr->expr_->ToString();
        if (order_expr->type_ == OrderType::kAsc) {
            order_str += " Ascending";
        } else {
            order_str += " Descending";
        }
        result->emplace_back(std::make_shared<std::string>(order_str));
    }

    if (select_statement->limit_expr_) {
        std::string limit_str = std::string(intent_size, ' ') + "limit: " + select_statement->limit_expr_->ToString();
        result->emplace_back(std::make_shared<std::string>(limit_str));
    }

    if (select_statement->offset_expr_) {
        std::string offset_str = std::string(intent_size, ' ') + "offset: " + select_statement->offset_expr_->ToString();
        result->emplace_back(std::make_shared<std::string>(offset_str));
    }

    intent_size -= 2;
    if (select_statement->nested_select_ != nullptr) {
        switch (select_statement->set_op_) {
            case SetOperatorType::kUnion: {
                std::string union_str = std::string(intent_size, ' ') + "UNION: ";
                result->emplace_back(std::make_shared<std::string>(union_str));
                break;
            }
            case SetOperatorType::kUnionAll: {
                std::string union_all_str = std::string(intent_size, ' ') + "UNION ALL: ";
                result->emplace_back(std::make_shared<std::string>(union_all_str));
                break;
            }
            case SetOperatorType::kIntersect: {
                std::string intersect_str = std::string(intent_size, ' ') + "INTERSECT: ";
                result->emplace_back(std::make_shared<std::string>(intersect_str));
                break;
            }
            case SetOperatorType::kExcept: {
                std::string except_str = std::string(intent_size, ' ') + "EXCEPT: ";
                result->emplace_back(std::make_shared<std::string>(except_str));
                break;
            }
        }
        status = BuildSelect(select_statement->nested_select_, result, intent_size);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status ExplainAST::BuildBaseTableRef(const BaseTableReference *base_table_ref,
                                     std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                     i64 intent_size) {
    std::string from_str;
    switch (base_table_ref->type_) {
        case TableRefType::kCrossProduct: {
            from_str = std::string(intent_size, ' ') + "cross product";
            auto *cross_product_ref = (CrossProductReference *)base_table_ref;
            if (cross_product_ref->alias_ != nullptr) {
                from_str += " AS " + std::string(cross_product_ref->alias_->alias_);
                if (cross_product_ref->alias_->column_alias_array_ != nullptr) {
                    Status status = Status::SyntaxError("Table reference has columns alias");
                    RecoverableError(status);
                }
            } else {
                from_str += ": ";
            }
            result->emplace_back(std::make_shared<std::string>(from_str));

            intent_size += 2;
            for (auto *sub_table_ref : cross_product_ref->tables_) {
                BuildBaseTableRef(sub_table_ref, result, intent_size);
            }
            break;
        }
        case TableRefType::kJoin: {

            auto *join_reference = (JoinReference *)base_table_ref;
            from_str = std::string(intent_size, ' ') + "table join on: " + join_reference->condition_->ToString();
            if (join_reference->alias_ != nullptr) {
                from_str += " AS " + std::string(join_reference->alias_->alias_);
                if (join_reference->alias_->column_alias_array_ != nullptr) {
                    Status status = Status::SyntaxError("Table reference has columns alias");
                    RecoverableError(status);
                }
            }
            result->emplace_back(std::make_shared<std::string>(from_str));

            intent_size += 2;
            BuildBaseTableRef(join_reference->left_, result, intent_size);
            BuildBaseTableRef(join_reference->right_, result, intent_size);
            break;
        }
        case TableRefType::kTable: {
            from_str = std::string(intent_size, ' ') + "table: ";
            auto *table_reference = (TableReference *)base_table_ref;
            if (!table_reference->db_name_.empty()) {
                from_str += table_reference->db_name_ + ".";
            }
            from_str += table_reference->table_name_;
            if (table_reference->alias_ != nullptr) {
                from_str += " AS " + std::string(table_reference->alias_->alias_);
                if (table_reference->alias_->column_alias_array_ != nullptr) {
                    Status status = Status::SyntaxError("Table reference has columns alias");
                    RecoverableError(status);
                }
            }
            result->emplace_back(std::make_shared<std::string>(from_str));
            break;
        }
        case TableRefType::kSubquery: {
            from_str = std::string(intent_size, ' ') + "subquery table: ";
            auto *subquery_reference = (SubqueryReference *)base_table_ref;
            if (subquery_reference->alias_ != nullptr) {
                from_str += " AS " + std::string(subquery_reference->alias_->alias_);
                if (subquery_reference->alias_->column_alias_array_ != nullptr) {
                    Status status = Status::SyntaxError("Table reference has columns alias");
                    RecoverableError(status);
                }
            } else {
                from_str += ": ";
            }
            result->emplace_back(std::make_shared<std::string>(from_str));

            intent_size += 2;
            BuildSelect(subquery_reference->select_statement_, result, intent_size);
            break;
        }
        case TableRefType::kDummy: {
            break;
        }
    }
    return Status::OK();
}

Status
ExplainAST::BuildShow(const ShowStatement *show_statement, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {

    switch (show_statement->show_type_) {
        case ShowStmtType::kDatabase: {
            result->emplace_back(std::make_shared<std::string>("SHOW DATABASE: "));
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + show_statement->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            break;
        }
        case ShowStmtType::kTable: {
            result->emplace_back(std::make_shared<std::string>("SHOW TABLE: "));
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + show_statement->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string table_name = std::string(intent_size, ' ') + "table: " + show_statement->table_name_;
            result->emplace_back(std::make_shared<std::string>(table_name));
            break;
        }
        case ShowStmtType::kIndex: {
            result->emplace_back(std::make_shared<std::string>("SHOW INDEX: "));
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + show_statement->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string table_name = std::string(intent_size, ' ') + "table: " + show_statement->table_name_;
            result->emplace_back(std::make_shared<std::string>(table_name));
            std::string index_name = std::string(intent_size, ' ') + "index: " + show_statement->index_name_.value();
            result->emplace_back(std::make_shared<std::string>(index_name));
            break;
        }
        case ShowStmtType::kIndexSegment: {
            result->emplace_back(std::make_shared<std::string>("SHOW INDEX SEGMENT: "));
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + show_statement->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string table_name = std::string(intent_size, ' ') + "table: " + show_statement->table_name_;
            result->emplace_back(std::make_shared<std::string>(table_name));
            std::string index_name = std::string(intent_size, ' ') + "index: " + show_statement->index_name_.value();
            result->emplace_back(std::make_shared<std::string>(index_name));
            std::string index_segment = std::string(intent_size, ' ') + "segment: " + std::to_string(show_statement->segment_id_.value());
            result->emplace_back(std::make_shared<std::string>(index_segment));
            break;
        }
        case ShowStmtType::kIndexChunk: {
            result->emplace_back(std::make_shared<std::string>("SHOW INDEX CHUNK: "));
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + show_statement->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string table_name = std::string(intent_size, ' ') + "table: " + show_statement->table_name_;
            result->emplace_back(std::make_shared<std::string>(table_name));
            std::string index_name = std::string(intent_size, ' ') + "index: " + show_statement->index_name_.value();
            result->emplace_back(std::make_shared<std::string>(index_name));
            std::string index_segment = std::string(intent_size, ' ') + "segment: " + std::to_string(show_statement->segment_id_.value());
            result->emplace_back(std::make_shared<std::string>(index_segment));
            std::string index_chunk = std::string(intent_size, ' ') + "chunk: " + std::to_string(show_statement->chunk_id_.value());
            result->emplace_back(std::make_shared<std::string>(index_chunk));
            break;
        }

        case ShowStmtType::kColumns: {
            result->emplace_back(std::make_shared<std::string>("SHOW COLUMNS: "));
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + show_statement->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string table_name = std::string(intent_size, ' ') + "table: " + show_statement->table_name_;
            result->emplace_back(std::make_shared<std::string>(table_name));
            break;
        }
        case ShowStmtType::kCollections: {
            Status::NotSupport("Show collections");
            break;
        }
        case ShowStmtType::kTasks: {
            Status::NotSupport("Show views");
            break;
        }
        case ShowStmtType::kBuffer: {
            result->emplace_back(std::make_shared<std::string>("SHOW BUFFER"));
            break;
        }
        case ShowStmtType::kMemIndex: {
            result->emplace_back(std::make_shared<std::string>("SHOW MEMINDEX"));
            break;
        }
        case ShowStmtType::kTables: {
            result->emplace_back(std::make_shared<std::string>("SHOW TABLES: "));
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + show_statement->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            break;
        }
        case ShowStmtType::kDatabases: {
            result->emplace_back(std::make_shared<std::string>("SHOW DATABASES"));
            break;
        }
        case ShowStmtType::kSegments: {
            result->emplace_back(std::make_shared<std::string>("SHOW SEGMENTS: "));
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + show_statement->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string table_name = std::string(intent_size, ' ') + "table: " + show_statement->table_name_;
            result->emplace_back(std::make_shared<std::string>(table_name));
            break;
        }
        case ShowStmtType::kSegment: {
            result->emplace_back(std::make_shared<std::string>("SHOW SEGMENT: "));
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + show_statement->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string table_name = std::string(intent_size, ' ') + "table: " + show_statement->table_name_;
            result->emplace_back(std::make_shared<std::string>(table_name));
            std::string segment_info = std::string(intent_size, ' ') + "segment: " + std::to_string(show_statement->segment_id_.value());
            result->emplace_back(std::make_shared<std::string>(segment_info));
            break;
        }
        case ShowStmtType::kBlocks: {
            result->emplace_back(std::make_shared<std::string>("SHOW BLOCKS: "));
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + show_statement->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string table_name = std::string(intent_size, ' ') + "table: " + show_statement->table_name_;
            result->emplace_back(std::make_shared<std::string>(table_name));
            std::string segment_info = std::string(intent_size, ' ') + "segment: " + std::to_string(show_statement->segment_id_.value());
            result->emplace_back(std::make_shared<std::string>(segment_info));
            break;
        }
        case ShowStmtType::kBlock: {
            result->emplace_back(std::make_shared<std::string>("SHOW BLOCK: "));
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + show_statement->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string table_name = std::string(intent_size, ' ') + "table: " + show_statement->table_name_;
            result->emplace_back(std::make_shared<std::string>(table_name));
            std::string segment_info = std::string(intent_size, ' ') + "segment: " + std::to_string(show_statement->segment_id_.value());
            result->emplace_back(std::make_shared<std::string>(segment_info));
            std::string block_info = std::string(intent_size, ' ') + "block: " + std::to_string(show_statement->block_id_.value());
            result->emplace_back(std::make_shared<std::string>(block_info));
            break;
        }
        case ShowStmtType::kBlockColumn: {
            result->emplace_back(std::make_shared<std::string>("SHOW BLOCK COLUMN: "));
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + show_statement->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string table_name = std::string(intent_size, ' ') + "table: " + show_statement->table_name_;
            result->emplace_back(std::make_shared<std::string>(table_name));
            std::string segment_info = std::string(intent_size, ' ') + "segment: " + std::to_string(show_statement->segment_id_.value());
            result->emplace_back(std::make_shared<std::string>(segment_info));
            std::string block_info = std::string(intent_size, ' ') + "block: " + std::to_string(show_statement->block_id_.value());
            result->emplace_back(std::make_shared<std::string>(block_info));
            std::string column_info = std::string(intent_size, ' ') + "column: " + std::to_string(show_statement->column_id_.value());
            result->emplace_back(std::make_shared<std::string>(column_info));
            break;
        }
        case ShowStmtType::kIndexes: {
            result->emplace_back(std::make_shared<std::string>("SHOW INDEXES: "));
            intent_size += 2;
            std::string schema_name = std::string(intent_size, ' ') + "database: " + show_statement->schema_name_;
            result->emplace_back(std::make_shared<std::string>(schema_name));
            std::string table_name = std::string(intent_size, ' ') + "table: " + show_statement->table_name_;
            result->emplace_back(std::make_shared<std::string>(table_name));
            break;
        }
        case ShowStmtType::kConfigs: {
            result->emplace_back(std::make_shared<std::string>("SHOW CONFIGS"));
            break;
        }
        case ShowStmtType::kQueries: {
            result->emplace_back(std::make_shared<std::string>("SHOW QUERIES"));
            break;
        }
        case ShowStmtType::kQuery: {
            result->emplace_back(std::make_shared<std::string>("SHOW QUERY"));
            break;
        }
        case ShowStmtType::kTransactions: {
            result->emplace_back(std::make_shared<std::string>("SHOW TRANSACTIONS"));
            break;
        }
        case ShowStmtType::kTransaction: {
            result->emplace_back(std::make_shared<std::string>("SHOW TRANSACTION"));
            break;
        }
        case ShowStmtType::kTransactionHistory: {
            result->emplace_back(std::make_shared<std::string>("SHOW TRANSACTION HISTORY"));
            break;
        }
        case ShowStmtType::kProfiles: {
            result->emplace_back(std::make_shared<std::string>("SHOW QUERIES"));
            break;
        }
        case ShowStmtType::kSessionVariable: {
            result->emplace_back(std::make_shared<std::string>("SHOW SESSION VARIABLE"));
            break;
        }
        case ShowStmtType::kSessionVariables: {
            result->emplace_back(std::make_shared<std::string>("SHOW SESSION VARIABLES"));
            break;
        }
        case ShowStmtType::kGlobalVariable: {
            result->emplace_back(std::make_shared<std::string>("SHOW GLOBAL VARIABLE"));
            break;
        }
        case ShowStmtType::kGlobalVariables: {
            result->emplace_back(std::make_shared<std::string>("SHOW GLOBAL VARIABLES"));
            break;
        }
        case ShowStmtType::kConfig: {
            result->emplace_back(std::make_shared<std::string>("SHOW CONFIG"));
            break;
        }
        case ShowStmtType::kLogs: {
            result->emplace_back(std::make_shared<std::string>("SHOW LOGS"));
            break;
        }
        case ShowStmtType::kCatalog: {
            result->emplace_back(std::make_shared<std::string>("SHOW CATALOG"));
            break;
        }
        case ShowStmtType::kCatalogToFile: {
            result->emplace_back(std::make_shared<std::string>("SHOW CATALOG TO FILE"));
            break;
        }
        case ShowStmtType::kPersistenceFiles: {
            result->emplace_back(std::make_shared<std::string>("SHOW PERSISTENCE FILES"));
            break;
        }
        case ShowStmtType::kPersistenceObjects: {
            result->emplace_back(std::make_shared<std::string>("SHOW PERSISTENCE OBJECTS"));
            break;
        }
        case ShowStmtType::kPersistenceObject: {
            result->emplace_back(std::make_shared<std::string>("SHOW PERSISTENCE OBJECT"));
            break;
        }
        case ShowStmtType::kMemory: {
            result->emplace_back(std::make_shared<std::string>("SHOW MEMORY"));
            break;
        }
        case ShowStmtType::kMemoryObjects: {
            result->emplace_back(std::make_shared<std::string>("SHOW MEMORY OBJECTS"));
            break;
        }
        case ShowStmtType::kMemoryAllocation: {
            result->emplace_back(std::make_shared<std::string>("SHOW MEMORY ALLOCATION"));
            break;
        }
        case ShowStmtType::kFunction: {
            result->emplace_back(std::make_shared<std::string>("SHOW FUNCTION"));
            break;
        }
        case ShowStmtType::kListSnapshots: {
            result->emplace_back(std::make_shared<std::string>("LIST SNAPSHOTS"));
            break;
        }
        case ShowStmtType::kShowSnapshot: {
            result->emplace_back(std::make_shared<std::string>("SHOW SNAPSHOT"));
            break;
        }
        case ShowStmtType::kListCaches: {
            result->emplace_back(std::make_shared<std::string>("LIST CACHES"));
            break;
        }
        case ShowStmtType::kShowCache: {
            result->emplace_back(std::make_shared<std::string>("SHOW CACHE"));
            break;
        }
        case ShowStmtType::kInvalid: {
            UnrecoverableError("Invalid show statement type");
        }
    }
    return Status::OK();
}

Status ExplainAST::BuildFlush(const FlushStatement *flush_statement, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64) {
    switch (flush_statement->type_) {
        case FlushType::kCatalog:
            result->emplace_back(std::make_shared<std::string>("FLUSH CATALOG"));
            break;
        case FlushType::kData:
            result->emplace_back(std::make_shared<std::string>("FLUSH DATA"));
            break;
        case FlushType::kLog:
            result->emplace_back(std::make_shared<std::string>("FLUSH LOG"));
            break;
        case FlushType::kBuffer:
            result->emplace_back(std::make_shared<std::string>("FLUSH BUFFER"));
            break;
    }
    return Status::OK();
}

Status
ExplainAST::BuildOptimize(const OptimizeStatement *optimize_statement, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64) {
    result->emplace_back(std::make_shared<std::string>("OPTIMIZE TABLE"));
    return Status::OK();
}

Status
ExplainAST::BuildCopy(const CopyStatement *copy_statement, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    if (copy_statement->copy_from_) {
        // IMPORT
        result->emplace_back(std::make_shared<std::string>("IMPORT DATA:"));
    } else {
        // EXPORT
        result->emplace_back(std::make_shared<std::string>("EXPORT DATA:"));
    }

    std::shared_ptr<std::string> schema_name =
        std::make_shared<std::string>(std::string(intent_size, ' ') + "database: " + copy_statement->schema_name_);
    result->emplace_back(schema_name);

    std::shared_ptr<std::string> table_name = std::make_shared<std::string>(std::string(intent_size, ' ') + "table: " + copy_statement->table_name_);
    result->emplace_back(table_name);

    std::shared_ptr<std::string> path = std::make_shared<std::string>(std::string(intent_size, ' ') + "file: " + copy_statement->file_path_);
    result->emplace_back(path);

    switch (copy_statement->copy_file_type_) {
        case CopyFileType::kCSV: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + "file type: CSV");
            result->emplace_back(file_type);

            std::shared_ptr<std::string> header =
                std::make_shared<std::string>(std::string(intent_size, ' ') + "header: " + (copy_statement->header_ ? "Yes" : "No"));
            result->emplace_back(header);

            std::shared_ptr<std::string> delimiter =
                std::make_shared<std::string>(std::string(intent_size, ' ') + "delimiter: " + copy_statement->delimiter_);
            result->emplace_back(delimiter);
            if (!copy_statement->copy_from_) {
                // export
                std::shared_ptr<std::string> offset =
                    std::make_shared<std::string>(std::string(intent_size, ' ') + fmt::format("offset: {}", copy_statement->offset_));
                result->emplace_back(offset);
                std::shared_ptr<std::string> limit =
                    std::make_shared<std::string>(std::string(intent_size, ' ') + fmt::format("limit: {}", copy_statement->limit_));
                result->emplace_back(limit);
                std::shared_ptr<std::string> row_limit =
                    std::make_shared<std::string>(std::string(intent_size, ' ') + fmt::format("row_limit: {}", copy_statement->row_limit_));
                result->emplace_back(row_limit);
            }
            break;
        }
        case CopyFileType::kJSON: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + "file type: JSON");
            result->emplace_back(file_type);
            if (!copy_statement->copy_from_) {
                // export
                std::shared_ptr<std::string> offset =
                    std::make_shared<std::string>(std::string(intent_size, ' ') + fmt::format("offset: {}", copy_statement->offset_));
                result->emplace_back(offset);
                std::shared_ptr<std::string> limit =
                    std::make_shared<std::string>(std::string(intent_size, ' ') + fmt::format("limit: {}", copy_statement->limit_));
                result->emplace_back(limit);
                std::shared_ptr<std::string> row_limit =
                    std::make_shared<std::string>(std::string(intent_size, ' ') + fmt::format("row_limit: {}", copy_statement->row_limit_));
                result->emplace_back(row_limit);
            }
            break;
        }
        case CopyFileType::kFVECS: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + "file type: FVECS");
            result->emplace_back(file_type);
            if (!copy_statement->copy_from_) {
                // export
                std::shared_ptr<std::string> offset =
                    std::make_shared<std::string>(std::string(intent_size, ' ') + fmt::format("offset: {}", copy_statement->offset_));
                result->emplace_back(offset);
                std::shared_ptr<std::string> limit =
                    std::make_shared<std::string>(std::string(intent_size, ' ') + fmt::format("limit: {}", copy_statement->limit_));
                result->emplace_back(limit);
                std::shared_ptr<std::string> row_limit =
                    std::make_shared<std::string>(std::string(intent_size, ' ') + fmt::format("row_limit: {}", copy_statement->row_limit_));
                result->emplace_back(row_limit);
            }
            break;
        }
        case CopyFileType::kJSONL: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + "file type: JSONL");
            result->emplace_back(file_type);
            if (!copy_statement->copy_from_) {
                // export
                std::shared_ptr<std::string> offset =
                    std::make_shared<std::string>(std::string(intent_size, ' ') + fmt::format("offset: {}", copy_statement->offset_));
                result->emplace_back(offset);
                std::shared_ptr<std::string> limit =
                    std::make_shared<std::string>(std::string(intent_size, ' ') + fmt::format("limit: {}", copy_statement->limit_));
                result->emplace_back(limit);
                std::shared_ptr<std::string> row_limit =
                    std::make_shared<std::string>(std::string(intent_size, ' ') + fmt::format("row_limit: {}", copy_statement->row_limit_));
                result->emplace_back(row_limit);
            }
            break;
        }
        case CopyFileType::kCSR: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + "file type: CSR");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kBVECS: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + "file type: BVECS");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kPARQUET: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(std::string(intent_size, ' ') + "file type: PARQUET");
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kInvalid: {
            UnrecoverableError("Invalid file type");
        }
    }
    return Status::OK();
}

Status
ExplainAST::BuildCheck(const CheckStatement *check_statement, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    switch (check_statement->check_type_) {
        case CheckStmtType::kSystem: {
            result->emplace_back(std::make_shared<std::string>("CHECK SYSTEM"));
            break;
        }
        case CheckStmtType::kTable: {
            result->emplace_back(std::make_shared<std::string>("CHECK TABLE"));
            break;
        }
        case CheckStmtType::kInvalid: {
            UnrecoverableError("Invalid file type");
        }
    }
    return Status::OK();
}

} // namespace infinity
