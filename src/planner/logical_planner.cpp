//
// Created by jinhai on 23-1-17.
//

#include "logical_planner.h"
#include "main/infinity.h"
#include "query_binder.h"
#include "planner/node/logical_create_table.h"
#include "planner/node/logical_drop_table.h"
#include "planner/binder/insert_binder.h"
#include "expression/cast_expression.h"
#include "planner/node/logical_insert.h"
#include "planner/node/logical_chunk_scan.h"
#include "common/types/info/varchar_info.h"

namespace infinity {

void
LogicalPlanner::Build(const hsql::SQLStatement& statement) {
    switch (statement.type()) {
        case hsql::kStmtError:
            break;
        case hsql::kStmtSelect: {
            BuildSelect(static_cast<const hsql::SelectStatement&>(statement));
            break;
        }
        case hsql::kStmtImport: {
            BuildImport(static_cast<const hsql::ImportStatement&>(statement));
            break;
        }
        case hsql::kStmtInsert: {
            BuildInsert(static_cast<const hsql::InsertStatement&>(statement));
            break;
        }
        case hsql::kStmtUpdate: {
            BuildUpdate(static_cast<const hsql::UpdateStatement&>(statement));
            break;
        }
        case hsql::kStmtDelete: {
            BuildDelete(static_cast<const hsql::DeleteStatement&>(statement));
            break;
        }
        case hsql::kStmtCreate: {
            BuildCreate(static_cast<const hsql::CreateStatement&>(statement));
            break;
        }
        case hsql::kStmtDrop: {
            BuildDrop(static_cast<const hsql::DropStatement&>(statement));
            break;
        }
        case hsql::kStmtPrepare: {
            BuildPrepare(static_cast<const hsql::PrepareStatement&>(statement));
            break;
        }
        case hsql::kStmtExecute: {
            BuildExecute(static_cast<const hsql::ExecuteStatement&>(statement));
            break;
        }
        case hsql::kStmtExport: {
            BuildExport(static_cast<const hsql::ExportStatement&>(statement));
            break;
        }
        case hsql::kStmtRename: {
            NotImplementError("Not implement rename statement.")
        }
        case hsql::kStmtAlter: {
            BuildAlter(static_cast<const hsql::AlterStatement&>(statement));
            break;
        }
        case hsql::kStmtShow: {
            BuildShow(static_cast<const hsql::ShowStatement&>(statement));
            break;
        }
        case hsql::kStmtTransaction:
            BuildTransaction(static_cast<const hsql::TransactionStatement&>(statement));
            break;
    }
}

void
LogicalPlanner::BuildSelect(const hsql::SelectStatement& statement) {
    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);
    SharedPtr<QueryBinder> query_binder_ptr = MakeShared<QueryBinder>(this->query_context_ptr_,
                                                                      bind_context_ptr);
    SharedPtr<BoundSelectStatement> bound_statement_ptr = query_binder_ptr->BindSelect(statement);
    this->logical_plan_ = bound_statement_ptr->BuildPlan();
}

void
LogicalPlanner::BuildInsert(const hsql::InsertStatement& statement) {
    switch(statement.type) {
        case hsql::kInsertValues:{
            return BuildInsertValue(statement);
        }
        case hsql::kInsertSelect:
            return BuildInsertSelect(statement);
        default:
            PlannerError("Inserting type isn't supported");
    }
}

void
LogicalPlanner::BuildInsertValue(const hsql::InsertStatement &statement) {
    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);
    bind_context_ptr->expression_binder_ = MakeShared<InsertBinder>(query_context_ptr_);

    // Get schema name
    String schema_name = statement.schema == nullptr? "Default" : statement.schema;

    // Get table name
    if(statement.tableName == nullptr) {
        PlannerError("Insert statement missing table name.");
    }
    String table_name = String{statement.tableName};
    // Check schema and table in the catalog
    SharedPtr<Table> table_ptr = Infinity::instance().catalog()->GetTableByName(schema_name, table_name);
    if(table_ptr == nullptr) { PlannerError(schema_name + "." + table_name + " not exists.")}

    // Create value list
    Vector<SharedPtr<BaseExpression>> value_list;
    value_list.reserve(statement.values->size());
    for (const auto* expr : *statement.values) {
        SharedPtr<BaseExpression> value_expr
                = bind_context_ptr->expression_binder_->BuildExpression(*expr, bind_context_ptr, 0, true);
        value_list.emplace_back(value_expr);
    }

    // Rearrange the inserted value to match the table.
    // SELECT INTO TABLE (c, a) VALUES (1, 2); => SELECT INTO TABLE (a, b, c) VALUES( 2, NULL, 1);
    if (statement.columns != nullptr) {
        SizeT statement_column_count = statement.columns->size();
        PlannerAssert(statement_column_count == value_list.size(),
                      "INSERT: Target column count and input column count mismatch");

//        Value null_value = Value::MakeNullData();
//        SharedPtr<BaseExpression> null_value_expr = MakeShared<ValueExpression>(null_value);

        SizeT table_column_count = table_ptr->ColumnCount();

        // Create value list with table column size and null value
        Vector<SharedPtr<BaseExpression>> rewrite_value_list(table_column_count, nullptr);

        SizeT column_idx = 0;
        for(const auto& column_name : *statement.columns) {
            // Get table index from the inserted value column name;
            SizeT table_column_id = table_ptr->GetColumnIdByName(column_name);
            DataType table_column_type = table_ptr->GetColumnTypeById(table_column_id);
            DataType value_type = value_list[column_idx]->Type();
            if(value_type == table_column_type) {
                rewrite_value_list[table_column_id] = value_list[column_idx];
            } else {
                // If the inserted value type mismatches with table column type, cast the inserted value type to correct one.
                SharedPtr<BaseExpression> cast_expr = MakeShared<CastExpression>(value_list[column_idx], table_column_type);
                rewrite_value_list[table_column_id] = cast_expr;
            }
            ++ column_idx;
        }

        value_list = rewrite_value_list;
    } else {
        SizeT table_column_count = table_ptr->ColumnCount();

        // Create value list with table column size and null value
        Vector<SharedPtr<BaseExpression>> rewrite_value_list(table_column_count, nullptr);

        for(SizeT column_idx = 0; column_idx < table_column_count; ++ column_idx) {
            DataType table_column_type = table_ptr->GetColumnTypeById(column_idx);
            DataType value_type = value_list[column_idx]->Type();
            if(table_column_type == value_type) {
                rewrite_value_list[column_idx] = value_list[column_idx];
            } else {
                SharedPtr<BaseExpression> cast_expr = MakeShared<CastExpression>(value_list[column_idx], table_column_type);
                rewrite_value_list[column_idx] = cast_expr;
            }
        }
        value_list = rewrite_value_list;
    }

    // Create logical insert node.
    SharedPtr<LogicalNode> logical_insert = MakeShared<LogicalInsert>(table_ptr,
                                                                      bind_context_ptr->GenerateTableIndex(),
                                                                      value_list);

    // FIXME: check if we need to append operator
//    this->AppendOperator(logical_insert, bind_context_ptr);

    this->logical_plan_ = logical_insert;
}

void
LogicalPlanner::BuildInsertSelect(const hsql::InsertStatement &statement) {
    PlannerError("Inserting select isn't supported.");
}

void
LogicalPlanner::BuildImport(const hsql::ImportStatement& statement) {
    switch(statement.type) {
        case hsql::kImportCSV:
            return BuildImportCsv(statement);
        case hsql::kImportTbl:
            return BuildImportTbl(statement);
        case hsql::kImportBinary:
            return BuildImportBinary(statement);
        case hsql::kImportAuto:
            return BuildImportAuto(statement);
    }
}

void
LogicalPlanner::BuildImportCsv(const hsql::ImportStatement &statement) {
    PlannerError("Import from CSV file isn't supported.");
}

void
LogicalPlanner::BuildImportTbl(const hsql::ImportStatement &statement) {
    PlannerError("Import from other table file isn't supported.");
}

void
LogicalPlanner::BuildImportBinary(const hsql::ImportStatement &statement) {
    PlannerError("Import from binary file isn't supported.");
}

void
LogicalPlanner::BuildImportAuto(const hsql::ImportStatement &statement) {
    PlannerError("Import from automatically identified format isn't supported.");
}

void
LogicalPlanner::BuildUpdate(const hsql::UpdateStatement& statement) {
    PlannerError("Update isn't supported.");
}

void
LogicalPlanner::BuildDelete(const hsql::DeleteStatement& statement) {
    PlannerError("Delete isn't supported.");
}

void
LogicalPlanner::BuildCreate(const hsql::CreateStatement& statement) {
    switch (statement.type) {
        case hsql::CreateType::kCreateTable: {
            BuildCreateTable(statement);
            break;
        }
        case hsql::CreateType::kCreateView: {
            BuildCreateView(statement);
            break;
        }
        case hsql::CreateType::kCreateTableFromTbl: {
            BuildCreateTableFromTable(statement);
            break;
        }
        case hsql::CreateType::kCreateIndex: {
            BuildCreateIndex(statement);
            break;
        }
        default: {
            PlannerError("Creating type isn't supported");
        }
    }
}

void
LogicalPlanner::BuildCreateTable(const hsql::CreateStatement &statement) {
    // Check if columns is given.
    Vector<SharedPtr<ColumnDef>> columns;
    for(SizeT idx = 0; idx < statement.columns->size(); ++ idx) {
        const hsql::ColumnDefinition* statement_column = statement.columns->at(idx);
        String column_name(statement_column->name);
        bool nullable = statement_column->nullable;
        DataType column_data_type = DataType::ConvertType(statement_column->type);
        std::set<ConstrainType> constraints;
        for(hsql::ConstraintType constraint_type: *(statement_column->column_constraints)) {
            switch (constraint_type) {
                case hsql::ConstraintType::None: {
                    TypeError("Unexpected constraint type.")
                }
                case hsql::ConstraintType::NotNull: {
                    constraints.insert(ConstrainType::kNotNull);
                    break;
                }
                case hsql::ConstraintType::Null: {
                    constraints.insert(ConstrainType::kNull);
                    break;
                }
                case hsql::ConstraintType::PrimaryKey: {
                    constraints.insert(ConstrainType::kPrimaryKey);
                    break;
                }
                case hsql::ConstraintType::Unique: {
                    constraints.insert(ConstrainType::kUnique);
                    break;
                }
            }
        }

        SharedPtr<ColumnDef> column_def_ptr = MakeShared<ColumnDef>(column_name, idx, column_data_type, constraints);
        columns.emplace_back(column_def_ptr);
    }

    SharedPtr<String> schema_name_ptr = MakeShared<String>("Default");
    if(statement.schema != nullptr) {
        schema_name_ptr = MakeShared<String>(statement.schema);
    }

    SharedPtr<TableDef> table_def_ptr
            = MakeShared<TableDef>(statement.tableName, columns, statement.ifNotExists);

    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);
    SharedPtr<LogicalNode> logical_create_table_operator
            = LogicalCreateTable::Make(schema_name_ptr,
                                       table_def_ptr,
                                       bind_context_ptr->GenerateTableIndex());

    // FIXME: check if we need to append operator
//    this->AppendOperator(logical_create_table_operator, bind_context_ptr);

    if(statement.select != nullptr) {
        SharedPtr<BindContext> select_bind_context_ptr = BindContext::Make(nullptr);
        QueryBinder select_query_binder(this->query_context_ptr_, select_bind_context_ptr);
        SharedPtr<BoundSelectStatement> bound_statement_ptr = select_query_binder.BindSelect(*statement.select);
        logical_create_table_operator->set_left_node(bound_statement_ptr->logical_plan_);
    }

    this->logical_plan_ = logical_create_table_operator;
    this->names_ptr_->emplace_back(String("OK"));
    this->types_ptr_->emplace_back(DataType(LogicalType::kInteger));
}

void
LogicalPlanner::BuildCreateView(const hsql::CreateStatement &statement) {
    PlannerError("Creating view isn't supported.");
}

void
LogicalPlanner::BuildCreateTableFromTable(const hsql::CreateStatement &statement) {
    PlannerError("Creating table from table isn't supported.");
}

void
LogicalPlanner::BuildCreateIndex(const hsql::CreateStatement &statement) {
    PlannerError("Creating index isn't supported.");
}

void
LogicalPlanner::BuildDrop(const hsql::DropStatement& statement) {
    switch(statement.type) {
        case hsql::kDropTable: {
            BuildDropTable(statement);
            break;
        }
        case hsql::kDropSchema: {
            BuildDropSchema(statement);
            break;
        }
        case hsql::kDropIndex: {
            BuildDropIndex(statement);
            break;
        }
        case hsql::kDropView: {
            BuildDropView(statement);
            break;
        }
        case hsql::kDropPreparedStatement: {
            BuildDropPreparedStatement(statement);
            break;
        }
        default: {
            PlannerError("Drop type isn't supported");
        }
    }
}

void
LogicalPlanner::BuildDropTable(const hsql::DropStatement &statement) {
    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);

    SharedPtr<String> schema_name_ptr = MakeShared<String>("Default");
    if(statement.schema != nullptr) {
        schema_name_ptr = MakeShared<String>(statement.schema);
    }

    SharedPtr<LogicalNode> logical_drop_table
            = MakeShared<LogicalDropTable>(schema_name_ptr,
                                           MakeShared<String>(statement.name),
                                           bind_context_ptr->GenerateTableIndex());
    // FIXME: check if we need to append operator
    //    this->AppendOperator(logical_drop_table, bind_context_ptr);

    this->logical_plan_ = logical_drop_table;
    this->names_ptr_->emplace_back(String("OK"));
    this->types_ptr_->emplace_back(DataType(LogicalType::kInteger));
}

void
LogicalPlanner::BuildDropSchema(const hsql::DropStatement &statement) {
    PlannerError("Dropping schema isn't supported.");
}

void
LogicalPlanner::BuildDropIndex(const hsql::DropStatement &statement) {
    PlannerError("Dropping index isn't supported.");
}

void
LogicalPlanner::BuildDropView(const hsql::DropStatement &statement) {
    PlannerError("Dropping view isn't supported.");
}

void
LogicalPlanner::BuildDropPreparedStatement(const hsql::DropStatement &statement) {
    PlannerError("Dropping prepared statement isn't supported.");
}

void
LogicalPlanner::BuildPrepare(const hsql::PrepareStatement& statement) {
    PlannerError("Prepare statement isn't supported.");
}

void
LogicalPlanner::BuildExecute(const hsql::ExecuteStatement& statement) {
    PlannerError("Execute statement isn't supported.");
}

void
LogicalPlanner::BuildExport(const hsql::ExportStatement& statement) {
    switch(statement.type) {
        case hsql::kImportCSV:
            return BuildExportCsv(statement);
        case hsql::kImportTbl:
            return BuildExportTbl(statement);
        case hsql::kImportBinary:
            return BuildExportBinary(statement);
        case hsql::kImportAuto:
            return BuildExportAuto(statement);
    }
}

void
LogicalPlanner::BuildExportCsv(const hsql::ExportStatement &statement) {
    PlannerError("Exporting to CSV file isn't supported.");
}

void
LogicalPlanner::BuildExportTbl(const hsql::ExportStatement &statement) {
    PlannerError("Exporting to table file isn't supported.");
}

void
LogicalPlanner::BuildExportBinary(const hsql::ExportStatement &statement) {
    PlannerError("Exporting to binary file isn't supported.");
}

void
LogicalPlanner::BuildExportAuto(const hsql::ExportStatement &statement) {
    PlannerError("Exporting in auto mode isn't supported.");
}

void
LogicalPlanner::BuildAlter(const hsql::AlterStatement& statement) {
    switch(statement.action->type) {
        case hsql::DropColumn:
            return BuildAlterDropColumn(statement);
    }
}

void
LogicalPlanner::BuildAlterDropColumn(const hsql::AlterStatement& statement) {
    PlannerError("Alter: DROP COLUMN isn't supported.");
}

void
LogicalPlanner::BuildShow(const hsql::ShowStatement& statement) {
    switch(statement.type) {
        case hsql::kShowTables : {
            return BuildShowTables(statement);
        }
        case hsql::kShowColumns : {
            return BuildShowColumns(statement);
        }
        default:
            PlannerError("Don't support show type.");
    }
}

void
LogicalPlanner::BuildShowColumns(const hsql::ShowStatement& statement) {
    PlannerError("Show columns isn't supported.");
}

void
LogicalPlanner::BuildShowTables(const hsql::ShowStatement& statement) {
    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);

    SharedPtr<LogicalNode> logical_chunk_scan =
            MakeShared<LogicalChunkScan>(ChunkScanType::kShowTables,
                                         bind_context_ptr->GenerateTableIndex());

    // FIXME: check if we need to append operator
//    this->AppendOperator(logical_chunk_scan, bind_context_ptr);
    this->logical_plan_ = logical_chunk_scan;

    this->names_ptr_->emplace_back(String("table_name"));
    this->names_ptr_->emplace_back(String("column_count"));
    this->names_ptr_->emplace_back(String("row_count"));
    this->names_ptr_->emplace_back(String("block_count"));
    this->names_ptr_->emplace_back(String("block_size"));

    auto table_name_type_info_ptr = VarcharInfo::Make(TABLE_NAME_LIMIT);
    this->types_ptr_->emplace_back(DataType(LogicalType::kVarchar, table_name_type_info_ptr));
    this->types_ptr_->emplace_back(DataType(LogicalType::kBigInt));
    this->types_ptr_->emplace_back(DataType(LogicalType::kBigInt));
    this->types_ptr_->emplace_back(DataType(LogicalType::kBigInt));
    this->types_ptr_->emplace_back(DataType(LogicalType::kBigInt));
}

void
LogicalPlanner::BuildTransaction(const hsql::TransactionStatement& statement) {
    switch(statement.command) {
        case hsql::kBeginTransaction:
            return BuildTransactionBegin(statement);
        case hsql::kCommitTransaction:
            return BuildTransactionCommit(statement);
        case hsql::kRollbackTransaction:
            return BuildTransactionRollback(statement);
    }
}

void
LogicalPlanner::BuildTransactionBegin(const hsql::TransactionStatement &statement) {
    PlannerError("Transaction command: BEGIN isn't supported.");
}

void
LogicalPlanner::BuildTransactionCommit(const hsql::TransactionStatement &statement) {
    PlannerError("Transaction command: COMMIT isn't supported.");
}

void
LogicalPlanner::BuildTransactionRollback(const hsql::TransactionStatement &statement) {
    PlannerError("Transaction command: ROLLBACK isn't supported.");
}

}
