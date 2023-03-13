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
        case StatementType::kInvalidStmt: {
            PlannerError("Unexpected statement type")
        }
        case StatementType::kSelect: {
            auto* select_statement = (SelectStatement*)statement;
            result->emplace_back(MakeShared<String>("SELECT STATEMENT: "));
            break;
        }
        case StatementType::kCopy:
            break;
        case StatementType::kInsert:
            break;
        case StatementType::kUpdate:
            break;
        case StatementType::kDelete:
            break;
        case StatementType::kCreate: {
            auto* create_statement = (CreateStatement*)statement;
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
                    String create_schema = String(intent_size, ' ') + "CREATE TABLE: ";
                    result->emplace_back(MakeShared<String>(create_schema));
                    auto* table_info = (CreateTableInfo*)create_statement->create_info_.get();
                    intent_size += 2;
                    String schema_name = String(intent_size, ' ') + "name: " + table_info->table_name_;
                    result->emplace_back(MakeShared<String>(schema_name));
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
                    break;
                }
                case DDLType::kCollection: {
                    String create_collection = String(intent_size, ' ') + "CREATE COLLECTION: ";
                    result->emplace_back(MakeShared<String>(create_collection));
                    auto* collection_info = (CreateCollectionInfo*)create_statement->create_info_.get();
                    intent_size += 2;
                    String schema_name = String(intent_size, ' ') + "name: " + collection_info->collection_name_;
                    result->emplace_back(MakeShared<String>(schema_name));
                    String conflict = String(intent_size, ' ') + "conflict type: ";
                    switch(collection_info->conflict_type_) {
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
                case DDLType::kView: {
                    break;
                }
                case DDLType::kIndex: {
                    break;
                }
            }
        }
        case StatementType::kDrop:
            break;
        case StatementType::kPrepare:
            break;
        case StatementType::kExecute:
            break;
        case StatementType::kAlter:
            break;
        case StatementType::kShow:
            break;
        case StatementType::kExplain:
            break;
    }

    return ;
}

}