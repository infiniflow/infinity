//
// Created by jinhai on 23-3-13.
//

#include "statement.h"
#include "parser/table_reference/cross_product_reference.h"
#include "parser/table_reference/join_reference.h"
#include "parser/table_reference/table_reference.h"
#include "parser/table_reference/subquery_reference.h"

namespace infinity {

void
Statement::Explain(const BaseStatement* statement,
                   SharedPtr<Vector<SharedPtr<String>>>& stmt_string,
                   i64 intent_size) {
    switch(statement->Type()) {
        case StatementType::kSelect: {
            BuildSelect((SelectStatement*)statement, stmt_string, intent_size);
            break;
        }
        case StatementType::kCopy: {
            BuildCopy((CopyStatement*)statement, stmt_string, intent_size);
            break;
        }
        case StatementType::kInsert: {
            BuildInsert((InsertStatement*)statement, stmt_string, intent_size);
            break;
        }
        case StatementType::kUpdate:
            break;
        case StatementType::kDelete:
            break;
        case StatementType::kCreate: {
            BuildCreate((CreateStatement*)statement, stmt_string, intent_size);
            break;
        }
        case StatementType::kDrop: {
            BuildDrop((DropStatement*)statement, stmt_string, intent_size);
            break;
        }
        case StatementType::kPrepare:
            break;
        case StatementType::kExecute:
            break;
        case StatementType::kAlter:
            break;
        case StatementType::kShow: {
            BuildShow((ShowStatement*)statement, stmt_string, intent_size);
            break;
        }
        default: {
            PlannerError("Unexpected statement type")
        }
    }

    return ;
}

void
Statement::BuildCreate(const CreateStatement* create_statement,
                       SharedPtr<Vector<SharedPtr<String>>>& result,
                       i64 intent_size) {

    switch(create_statement->ddl_type()) {
        case DDLType::kInvalid: {
            PlannerError("Invalid DDL type.")
        }
        case DDLType::kSchema: {
            String create_schema = String(intent_size, ' ') + "CREATE SCHEMA: ";
            result->emplace_back(MakeShared<String>(create_schema));
            auto* schema_info = (CreateSchemaInfo*)create_statement->create_info_.get();
            intent_size += 2;
            String schema_name = String(intent_size, ' ') + "name: " + schema_info->schema_name_;
            result->emplace_back(MakeShared<String>(schema_name));
            String conflict = String(intent_size, ' ') + "conflict type: " + ConflictTypeToStr(schema_info->conflict_type_);
            result->emplace_back(MakeShared<String>(conflict));
            break;
        }
        case DDLType::kTable: {
            String create_table = String(intent_size, ' ') + "CREATE TABLE: ";
            result->emplace_back(MakeShared<String>(create_table));
            auto* table_info = (CreateTableInfo*)create_statement->create_info_.get();
            intent_size += 2;
            String schema_name = String(intent_size, ' ') + "schema: " + table_info->schema_name_;
            result->emplace_back(MakeShared<String>(schema_name));
            String table_name = String(intent_size, ' ') + "table: " + table_info->table_name_;
            result->emplace_back(MakeShared<String>(table_name));
            String conflict = String(intent_size, ' ') + "conflict type: " + ConflictTypeToStr(table_info->conflict_type_);
            result->emplace_back(MakeShared<String>(conflict));
            String column_names = String(intent_size, ' ') + "columns: (";

            SizeT column_count = table_info->column_defs_.size();
            if(column_count == 0) {
                PlannerError("Table definition without any columns");
            }

            for(SizeT idx = 0; idx < column_count - 1; ++ idx) {
                column_names += table_info->column_defs_[idx]->ToString() + ", ";
            }
            column_names += table_info->column_defs_.back()->ToString();
            column_names += ")";
            result->emplace_back(MakeShared<String>(column_names));

            SizeT constraint_count = table_info->constraints_.size();
            if(constraint_count > 0) {
                String constraints_str = "Constraints: (";
                for(SizeT idx = 0; idx < constraint_count - 1; ++ idx) {
                    constraints_str += table_info->constraints_[idx]->ToString() + ", ";
                }
                constraints_str += table_info->constraints_.back()->ToString() + ")";
                result->emplace_back(MakeShared<String>(constraints_str));
            }
            break;
        }
        case DDLType::kCollection: {
            String create_collection = String(intent_size, ' ') + "CREATE COLLECTION: ";
            result->emplace_back(MakeShared<String>(create_collection));
            auto* collection_info = (CreateCollectionInfo*)create_statement->create_info_.get();
            intent_size += 2;
            String schema_name = String(intent_size, ' ') + "schema: " + collection_info->schema_name_;
            result->emplace_back(MakeShared<String>(schema_name));
            String collection_name = String(intent_size, ' ') + "name: " + collection_info->collection_name_;
            result->emplace_back(MakeShared<String>(collection_name));
            String conflict = String(intent_size, ' ') + "conflict type: " + ConflictTypeToStr(collection_info->conflict_type_);
            result->emplace_back(MakeShared<String>(conflict));
            break;
        }
        case DDLType::kView: {
            break;
        }
        case DDLType::kIndex: {
            break;
        }
    }
}

void
Statement::BuildInsert(const InsertStatement* insert_statement,
            SharedPtr<Vector<SharedPtr<String>>>& result,
            i64 intent_size) {
    result->emplace_back(MakeShared<String>("INSERT: "));
    intent_size += 2;
    String schema_name = String(intent_size, ' ') + "schema: " + insert_statement->schema_name_;
    result->emplace_back(MakeShared<String>(schema_name));
    String table_name = String(intent_size, ' ') + "table: " + insert_statement->table_name_;
    result->emplace_back(MakeShared<String>(table_name));
    String values = String(intent_size, ' ') + "values: ";
    SizeT value_count = insert_statement->values_->size();
    if(value_count == 0) {
        PlannerError("Insert value list is empty");
    }
    for(SizeT idx = 0; idx < value_count - 1; ++ idx) {
        values += insert_statement->values_->at(idx)->ToString() + ", ";
    }
    values += insert_statement->values_->back()->ToString();
    result->emplace_back(MakeShared<String>(values));
}

void
Statement::BuildDrop(const DropStatement* drop_statement,
                     SharedPtr<Vector<SharedPtr<String>>>& result,
                     i64 intent_size) {
    switch(drop_statement->ddl_type()) {
        case DDLType::kInvalid: {
            PlannerError("Invalid DDL type.")
        }
        case DDLType::kSchema: {
            String drop_schema = String(intent_size, ' ') + "DROP SCHEMA: ";
            result->emplace_back(MakeShared<String>(drop_schema));
            auto* schema_info = (DropSchemaInfo*)drop_statement->drop_info_.get();
            intent_size += 2;
            String schema_name = String(intent_size, ' ') + "name: " + schema_info->schema_name_;
            result->emplace_back(MakeShared<String>(schema_name));
            String conflict = String(intent_size, ' ') + "conflict type: " + ConflictTypeToStr(schema_info->conflict_type_);
            result->emplace_back(MakeShared<String>(conflict));
            break;
        }
        case DDLType::kTable: {
            String drop_table = String(intent_size, ' ') + "CREATE TABLE: ";
            result->emplace_back(MakeShared<String>(drop_table));
            auto* table_info = (DropTableInfo*)drop_statement->drop_info_.get();
            intent_size += 2;
            String schema_name = String(intent_size, ' ') + "schema: " + table_info->schema_name_;
            result->emplace_back(MakeShared<String>(schema_name));
            String table_name = String(intent_size, ' ') + "table: " + table_info->table_name_;
            result->emplace_back(MakeShared<String>(table_name));
            String conflict = String(intent_size, ' ') + "conflict type: " + ConflictTypeToStr(table_info->conflict_type_);
            result->emplace_back(MakeShared<String>(conflict));
            break;
        }
        case DDLType::kCollection: {
            String drop_collection = String(intent_size, ' ') + "DROP COLLECTION: ";
            result->emplace_back(MakeShared<String>(drop_collection));
            auto* collection_info = (DropCollectionInfo*)drop_statement->drop_info_.get();
            intent_size += 2;
            String schema_name = String(intent_size, ' ') + "schema: " + collection_info->schema_name_;
            result->emplace_back(MakeShared<String>(schema_name));
            String collection_name = String(intent_size, ' ') + "name: " + collection_info->collection_name_;
            result->emplace_back(MakeShared<String>(collection_name));
            String conflict = String(intent_size, ' ') + "conflict type: " + ConflictTypeToStr(collection_info->conflict_type_);
            result->emplace_back(MakeShared<String>(conflict));
            break;
        }
        case DDLType::kView: {
            break;
        }
        case DDLType::kIndex: {
            break;
        }
    }
}

void
Statement::BuildSelect(const SelectStatement* select_statement,
                       SharedPtr<Vector<SharedPtr<String>>>& result,
                       i64 intent_size,
                       SharedPtr<String> alias_ptr) {
    if(alias_ptr != nullptr) {
        String select_str = String(intent_size, ' ') + "SELECT AS " + *alias_ptr;
        result->emplace_back(MakeShared<String>(select_str));
    } else {
        String select_str = String(intent_size, ' ') + "SELECT";
        result->emplace_back(MakeShared<String>(select_str));
    }

    intent_size += 2;
    {
        if(select_statement->with_exprs_ != nullptr) {
            String with_str = String(intent_size, ' ') + "WITH";
            result->emplace_back(MakeShared<String>(with_str));

            intent_size += 2;

            SizeT with_count = select_statement->with_exprs_->size();
            for(SizeT idx = 0; idx < with_count; ++ idx) {
                auto* with_expr = select_statement->with_exprs_->at(idx);
                SharedPtr<String> alias_str = MakeShared<String>(with_expr->alias_);
                BuildSelect((SelectStatement*)with_expr->select_, result, intent_size, alias_ptr);
            }
            intent_size -= 2;
        }
    }

    {
        String projection_str = String(intent_size, ' ') + "projection: ";
        SizeT select_count = select_statement->select_list_->size();
        if (select_count == 0) {
            PlannerError("No select list");
        }
        for (SizeT idx = 0; idx < select_count - 1; ++idx) {
            ParsedExpr *expr = select_statement->select_list_->at(idx);
            projection_str += expr->ToString() + ", ";
        }
        projection_str += select_statement->select_list_->back()->ToString();

        result->emplace_back(MakeShared<String>(projection_str));
    }

    BuildBaseTableRef(select_statement->table_ref_, result, intent_size);

    if(select_statement->where_expr_ != nullptr) {
        String filter_str = String(intent_size, ' ') + "filter: " + select_statement->where_expr_->ToString();
        result->emplace_back(MakeShared<String>(filter_str));
    }

    if(select_statement->group_by_list_ != nullptr) {
        String group_by = String(intent_size, ' ') + "groupby: ";
        SizeT group_count = select_statement->group_by_list_->size();
        for(SizeT idx = 0; idx < group_count - 1; ++ idx) {
            group_by += select_statement->group_by_list_->at(idx)->ToString() + ", ";
        }
        group_by += select_statement->group_by_list_->back()->ToString();
        result->emplace_back(MakeShared<String>(group_by));
    }

    if(select_statement->having_expr_ != nullptr) {
        String having_str = String(intent_size, ' ') + "having: " + select_statement->having_expr_->ToString();
        result->emplace_back(MakeShared<String>(having_str));
    }

    if(select_statement->order_by_list != nullptr) {
        String order_str = String(intent_size, ' ') + "groupby: ";
        SizeT order_count = select_statement->order_by_list->size();
        for(SizeT idx = 0; idx < order_count - 1; ++ idx) {
            OrderByExpr* order_expr = select_statement->order_by_list->at(idx);
            order_str += order_expr->expr_->ToString();
            if(order_expr->type_ == OrderType::kAsc) {
                order_str += " Ascending, ";
            } else {
                order_str += " Descending, ";
            }
        }
        OrderByExpr* order_expr = select_statement->order_by_list->back();
        order_str += order_expr->expr_->ToString();
        if(order_expr->type_ == OrderType::kAsc) {
            order_str += " Ascending";
        } else {
            order_str += " Descending";
        }
        result->emplace_back(MakeShared<String>(order_str));
    }

    if(select_statement->limit_expr_) {
        String limit_str = String(intent_size, ' ') + "limit: " + select_statement->limit_expr_->ToString();
        result->emplace_back(MakeShared<String>(limit_str));
    }

    if(select_statement->offset_expr_) {
        String offset_str = String(intent_size, ' ') + "offset: " + select_statement->offset_expr_->ToString();
        result->emplace_back(MakeShared<String>(offset_str));
    }

    intent_size -= 2;
    if(select_statement->nested_select_ != nullptr) {
        switch(select_statement->set_op_) {
            case SetOperatorType::kUnion: {
                String union_str = String(intent_size, ' ') + "UNION: ";
                result->emplace_back(MakeShared<String>(union_str));
                break;
            }
            case SetOperatorType::kUnionAll: {
                String union_all_str = String(intent_size, ' ') + "UNION ALL: ";
                result->emplace_back(MakeShared<String>(union_all_str));
                break;
            }
            case SetOperatorType::kIntersect: {
                String intersect_str = String(intent_size, ' ') + "INTERSECT: ";
                result->emplace_back(MakeShared<String>(intersect_str));
                break;
            }
            case SetOperatorType::kExcept: {
                String except_str = String(intent_size, ' ') + "EXCEPT: ";
                result->emplace_back(MakeShared<String>(except_str));
                break;
            }
        }
        BuildSelect(select_statement->nested_select_, result, intent_size);
    }
}

void
Statement::BuildBaseTableRef(const BaseTableReference* base_table_ref,
                             SharedPtr<Vector<SharedPtr<String>>>& result,
                             i64 intent_size) {
    String from_str;
    switch (base_table_ref->type_) {
        case TableRefType::kCrossProduct: {
            from_str = String(intent_size, ' ') + "cross product";
            auto *cross_product_ref = (CrossProductReference *) base_table_ref;
            if (cross_product_ref->alias_ != nullptr) {
                from_str += " AS " + String(cross_product_ref->alias_->alias_);
                PlannerAssert(cross_product_ref->alias_->column_alias_array_ == nullptr,
                              "Table reference has columns alias");
            } else {
                from_str += ": ";
            }
            result->emplace_back(MakeShared<String>(from_str));

            intent_size += 2;
            for (auto *sub_table_ref: cross_product_ref->tables_) {
                BuildBaseTableRef(sub_table_ref, result, intent_size);
            }
            break;
        }
        case TableRefType::kJoin: {

            auto *join_reference = (JoinReference *) base_table_ref;
            from_str = String(intent_size, ' ') + "table join on: " + join_reference->condition_->ToString();
            if (join_reference->alias_ != nullptr) {
                from_str += " AS " + String(join_reference->alias_->alias_);
                PlannerAssert(join_reference->alias_->column_alias_array_ == nullptr,
                              "Table reference has columns alias");
            }
            result->emplace_back(MakeShared<String>(from_str));

            intent_size += 2;
            BuildBaseTableRef(join_reference->left_, result, intent_size);
            BuildBaseTableRef(join_reference->right_, result, intent_size);
            break;
        }
        case TableRefType::kTable: {
            from_str = String(intent_size, ' ') + "table: ";
            auto *table_reference = (TableReference *) base_table_ref;
            if (!table_reference->schema_name_.empty()) {
                from_str += table_reference->schema_name_ + ".";
            }
            from_str += table_reference->table_name_;
            if (table_reference->alias_ != nullptr) {
                from_str += " AS " + String(table_reference->alias_->alias_);
                PlannerAssert(table_reference->alias_->column_alias_array_ == nullptr,
                              "Table reference has columns alias");
            }
            result->emplace_back(MakeShared<String>(from_str));
            break;
        }
        case TableRefType::kSubquery: {
            from_str = String(intent_size, ' ') + "subquery table: ";
            auto *subquery_reference = (SubqueryReference *) base_table_ref;
            if (subquery_reference->alias_ != nullptr) {
                from_str += " AS " + String(subquery_reference->alias_->alias_);
                PlannerAssert(subquery_reference->alias_->column_alias_array_ == nullptr,
                              "Table reference has columns alias");
            } else {
                from_str += ": ";
            }
            result->emplace_back(MakeShared<String>(from_str));

            intent_size += 2;
            BuildSelect(subquery_reference->select_statement_, result, intent_size);
            break;
        }
        case TableRefType::kDummy: {
            break;
        }
    }
}

void
Statement::BuildShow(const ShowStatement* show_statement,
                     SharedPtr<Vector<SharedPtr<String>>>& result,
                     i64 intent_size) {

    switch(show_statement->show_type_) {
        case ShowStmtType::kColumns: {
            result->emplace_back(MakeShared<String>("SHOW COLUMNS: "));
            intent_size += 2;
            String schema_name = String(intent_size, ' ') + "schema: " + show_statement->schema_name_;
            result->emplace_back(MakeShared<String>(schema_name));
            String table_name = String(intent_size, ' ') + "table: " + show_statement->table_name_;
            result->emplace_back(MakeShared<String>(table_name));
            break;
        }
        case ShowStmtType::kTables: {
            result->emplace_back(MakeShared<String>("SHOW TABLES: "));
            intent_size += 2;
            String schema_name = String(intent_size, ' ') + "schema: " + show_statement->schema_name_;
            result->emplace_back(MakeShared<String>(schema_name));
            break;
        }
    }
}

void
Statement::BuildCopy(const CopyStatement* statement,
                     SharedPtr<Vector<SharedPtr<String>>>& result,
                     i64 intent_size) {
    return ;
}

}