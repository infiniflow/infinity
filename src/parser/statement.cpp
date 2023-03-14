//
// Created by jinhai on 23-3-13.
//

#include "statement.h"

namespace infinity {

void
Statement::BuildString(const BaseStatement* statement,
                       SharedPtr<Vector<SharedPtr<String>>>& result,
                       i64 intent_size) {
    switch(statement->Type()) {
        case StatementType::kSelect: {
            BuildSelect((SelectStatement*)statement, result, intent_size);
            break;
        }
        case StatementType::kCopy: {
            BuildCopy((CopyStatement*)statement, result, intent_size);
            break;
        }
        case StatementType::kInsert: {
            BuildInsert((InsertStatement*)statement, result, intent_size);
            break;
        }
        case StatementType::kUpdate:
            break;
        case StatementType::kDelete:
            break;
        case StatementType::kCreate: {
            BuildCreate((CreateStatement*)statement, result, intent_size);
            break;
        }
        case StatementType::kDrop: {
            BuildDrop((DropStatement*)statement, result, intent_size);
            break;
        }
        case StatementType::kPrepare:
            break;
        case StatementType::kExecute:
            break;
        case StatementType::kAlter:
            break;
        case StatementType::kShow: {
            BuildShow((ShowStatement*)statement, result, intent_size);
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
            String conflict = String(intent_size, ' ') + "conflict type: ";
            switch(schema_info->conflict_type_) {
                case ConflictType::kInvalid: {
                    PlannerError("Unexpected create schema conflict type")
                    break;
                }
                case ConflictType::kIgnore:
                    conflict += "Ignore";
                    break;
                case ConflictType::kError:
                    conflict += "Issue error";
                    break;
                case ConflictType::kReplace:
                    conflict += "Replace";
                    break;
            }
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
            String conflict = String(intent_size, ' ') + "conflict type: ";
            switch(table_info->conflict_type_) {
                case ConflictType::kInvalid: {
                    PlannerError("Unexpected create schema conflict type")
                    break;
                }
                case ConflictType::kIgnore:
                    conflict += "Ignore";
                    break;
                case ConflictType::kError:
                    conflict += "Issue error";
                    break;
                case ConflictType::kReplace:
                    conflict += "Replace";
                    break;
            }
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
            String conflict = String(intent_size, ' ') + "conflict type: ";
            switch(collection_info->conflict_type_) {
                case ConflictType::kInvalid: {
                    PlannerError("Unexpected create schema conflict type")
                }
                case ConflictType::kIgnore:
                    conflict += "Ignore";
                    break;
                case ConflictType::kError:
                    conflict += "Issue error";
                    break;
                case ConflictType::kReplace:
                    conflict += "Replace";
                    break;
            }
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
Statement::BuildDrop(const DropStatement* statement,
                     SharedPtr<Vector<SharedPtr<String>>>& result,
                     i64 intent_size) {

}

void
Statement::BuildSelect(const SelectStatement* statement,
                       SharedPtr<Vector<SharedPtr<String>>>& result,
                       i64 intent_size) {
    result->emplace_back(MakeShared<String>("SELECT: "));
    return ;
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