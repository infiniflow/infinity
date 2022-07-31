//
// Created by JinHai on 2022/7/23.
//

#include "planner.h"

#include "main/infinity.h"

#include "planner/operator/logical_create_table.h"
#include "planner/operator/logical_drop_table.h"
#include "planner/operator/logical_chunk_scan.h"

#include "storage/table_definition.h"
#include "storage/column_definition.h"
#include "storage/table_with_fix_line.h"

#include "common/utility/asserter.h"


namespace infinity {


LogicalType
Planner::TypeConversion(hsql::ColumnType type) {
    switch (type.data_type) {
        case hsql::DataType::BOOLEAN: return LogicalType(LogicalTypeId::kBoolean, type.length, type.precision, type.scale);
        case hsql::DataType::SMALLINT: return LogicalType(LogicalTypeId::kSmallInt, type.length, type.precision, type.scale);
        case hsql::DataType::INT: return LogicalType(LogicalTypeId::kInteger, type.length, type.precision, type.scale);
        case hsql::DataType::BIGINT: return LogicalType(LogicalTypeId::kBigInt, type.length, type.precision, type.scale);
        case hsql::DataType::LONG: return LogicalType(LogicalTypeId::kBigInt, type.length, type.precision, type.scale);

        case hsql::DataType::DECIMAL: return LogicalType(LogicalTypeId::kDecimal, type.length, type.precision, type.scale);

        case hsql::DataType::REAL: return LogicalType(LogicalTypeId::kFloat, type.length, type.precision, type.scale);
        case hsql::DataType::FLOAT: return LogicalType(LogicalTypeId::kFloat, type.length, type.precision, type.scale);
        case hsql::DataType::DOUBLE: return LogicalType(LogicalTypeId::kDouble, type.length, type.precision, type.scale);

        case hsql::DataType::CHAR: return LogicalType(LogicalTypeId::kVarchar, type.length, type.precision, type.scale);
        case hsql::DataType::VARCHAR: return LogicalType(LogicalTypeId::kVarchar, type.length, type.precision, type.scale);
        case hsql::DataType::TEXT: return LogicalType(LogicalTypeId::kVarchar, type.length, type.precision, type.scale);

        case hsql::DataType::DATE: return LogicalType(LogicalTypeId::kDate, type.length, type.precision, type.scale);
        case hsql::DataType::TIME: return LogicalType(LogicalTypeId::kTime, type.length, type.precision, type.scale);
        case hsql::DataType::DATETIME: return LogicalType(LogicalTypeId::kDateTime, type.length, type.precision, type.scale);
        case hsql::DataType::UNKNOWN: return LogicalType(LogicalTypeId::kInvalid, type.length, type.precision, type.scale);
    }

}

std::shared_ptr<LogicalOperator>
Planner::CreateLogicalOperator(const hsql::SQLStatement &statement) {
    switch (statement.type()) {
        case hsql::kStmtSelect:
            return BuildSelect(static_cast<const hsql::SelectStatement &>(statement));
        case hsql::kStmtInsert:
            return BuildInsert(static_cast<const hsql::InsertStatement&>(statement));
        case hsql::kStmtDelete:
            return BuildDelete(static_cast<const hsql::DeleteStatement&>(statement));
        case hsql::kStmtUpdate:
            return BuildUpdate(static_cast<const hsql::UpdateStatement&>(statement));
        case hsql::kStmtShow:
            return BuildShow(static_cast<const hsql::ShowStatement&>(statement));
        case hsql::kStmtCreate:
            return BuildCreate(static_cast<const hsql::CreateStatement &>(statement));
        case hsql::kStmtDrop:
            return BuildDrop(static_cast<const hsql::DropStatement &>(statement));
        case hsql::kStmtPrepare:
            return BuildPrepare(static_cast<const hsql::PrepareStatement&>(statement));
        case hsql::kStmtExecute:
            return BuildExecute(static_cast<const hsql::ExecuteStatement&>(statement));
        case hsql::kStmtImport:
            return BuildImport(static_cast<const hsql::ImportStatement&>(statement));
        case hsql::kStmtExport:
            return BuildExport(static_cast<const hsql::ExportStatement&>(statement));
        case hsql::kStmtTransaction:
            return BuildTransaction(static_cast<const hsql::TransactionStatement&>(statement));
        case hsql::kStmtAlter:
            return BuildAlter(static_cast<const hsql::AlterStatement&>(statement));
        case hsql::kStmtError:
        case hsql::kStmtRename:
            ResponseError("Statement type not supported");
        default:
            ResponseError("Invalid statement type");
    }
}

// Build create statement to logical create operator
// Including: create table / view
std::shared_ptr<LogicalOperator>
Planner::BuildCreate(const hsql::CreateStatement& statement) {
    switch (statement.type) {
        case hsql::CreateType::kCreateTable: {
            return BuildCreateTable(statement);
        }
        case hsql::CreateType::kCreateView: {
            return BuildCreateView(statement);
        }
        case hsql::CreateType::kCreateTableFromTbl: {
            return BuildCreateTableFromTable(statement);
        }
        case hsql::CreateType::kCreateIndex: {
            return BuildCreateIndex(statement);;
        }
        default: {
            ResponseError("Creating type isn't supported");
        }
    }
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildCreateTable(const hsql::CreateStatement& statement) {
    // Check if columns is given.
    std::vector<ColumnDefinition> columns;
    for(size_t idx = 0; idx < statement.columns->size(); ++ idx) {
        const hsql::ColumnDefinition* statement_column = statement.columns->at(idx);
        std::string column_name(statement_column->name);
        bool nullable = statement_column->nullable;
        LogicalType logical_type = TypeConversion(statement_column->type);
        std::set<ConstrainType> constraints;
        for(hsql::ConstraintType constraint_type: *(statement_column->column_constraints)) {
            switch (constraint_type) {
                case hsql::ConstraintType::None:
                    break;
                case hsql::ConstraintType::NotNull:
                    break;
                case hsql::ConstraintType::Null:
                    break;
                case hsql::ConstraintType::PrimaryKey:
                    constraints.insert(ConstrainType::kPrimaryKey);
                    break;
                case hsql::ConstraintType::Unique:
                    constraints.insert(ConstrainType::kUnique);
                    break;
            }
        }

        columns.emplace_back(column_name, idx, logical_type, nullable, constraints);
    }

    std::shared_ptr<std::string> schema_name_ptr = std::make_shared<std::string>("Default");
    if(statement.schema != nullptr) {
        schema_name_ptr = std::make_shared<std::string>(statement.schema);
    }

    std::shared_ptr<TableDefinition> table_def_ptr
        = std::make_shared<TableDefinition>(statement.tableName, columns, statement.ifNotExists);
    std::shared_ptr<LogicalOperator> logical_create_table_operator
        = std::make_shared<LogicalCreateTable>(LogicalOperator::get_new_id(),
                schema_name_ptr,
                table_def_ptr);
    if(statement.select != nullptr) {
        std::shared_ptr<LogicalOperator> select_node = BuildSelect(*statement.select);
        logical_create_table_operator->set_left_node(select_node);
    }
    return logical_create_table_operator;
}

std::shared_ptr<LogicalOperator> Planner::BuildCreateTableFromTable(const hsql::CreateStatement &statement) {
    ResponseError("Creating table from table isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildCreateView(const hsql::CreateStatement &statement) {
    ResponseError("Creating view isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildCreateIndex(const hsql::CreateStatement &statement) {
    ResponseError("Creating index isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildDrop(const hsql::DropStatement &statement) {
    switch(statement.type) {
        case hsql::kDropTable:
            return BuildDropTable(statement);
        case hsql::kDropSchema:
            return BuildDropSchema(statement);
        case hsql::kDropIndex:
            return BuildDropIndex(statement);
        case hsql::kDropView:
            return BuildDropView(statement);
        case hsql::kDropPreparedStatement:
            return BuildDropPreparedStatement(statement);
        default: {
            ResponseError("Deleting type isn't supported");
        }
    }

    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildDropTable(const hsql::DropStatement &statement) {

    std::shared_ptr<std::string> schema_name_ptr = std::make_shared<std::string>("Default");
    if(statement.schema != nullptr) {
        schema_name_ptr = std::make_shared<std::string>(statement.schema);
    }

    std::shared_ptr<LogicalOperator> logical_drop_table
        = std::make_shared<LogicalDropTable>(LogicalOperator::get_new_id(),
                                             schema_name_ptr,
                                             std::make_shared<std::string>(statement.name));

    return logical_drop_table;
}

std::shared_ptr<LogicalOperator> Planner::BuildDropSchema(const hsql::DropStatement &statement) {
    ResponseError("Dropping schema isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildDropIndex(const hsql::DropStatement &statement) {
    ResponseError("Dropping index isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildDropView(const hsql::DropStatement &statement) {
    ResponseError("Dropping view isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildDropPreparedStatement(const hsql::DropStatement &statement) {
    ResponseError("Dropping prepared statement isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildInsert(const hsql::InsertStatement &statement) {
    switch(statement.type) {
        case hsql::kInsertValues:
            return BuildInsertValue(statement);
        case hsql::kInsertSelect:
            return BuildInsertSelect(statement);
        default:
            ResponseError("Inserting type isn't supported");
    }
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildInsertValue(const hsql::InsertStatement &statement) {
    ResponseError("Inserting values isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildInsertSelect(const hsql::InsertStatement &statement) {
    ResponseError("Inserting select isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildDelete(const hsql::DeleteStatement &statement) {
    ResponseError("Delete isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildUpdate(const hsql::UpdateStatement &statement) {
    ResponseError("Update isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildSelect(const hsql::SelectStatement &statement) {
    ResponseError("Select isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildShow(const hsql::ShowStatement &statement) {
    switch(statement.type) {
        case hsql::kShowTables : {
            return BuildShowTables(statement);
        }
        case hsql::kShowColumns : {
            return BuildShowColumns(statement);
        }
        default:
            ResponseError("Don't support show type.");
    }
//    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildShowColumns(const hsql::ShowStatement &statement) {
    ResponseError("Show columns isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildShowTables(const hsql::ShowStatement &statement) {
    std::vector<ColumnDefinition> column_defs = {
            {"table_name", 0, LogicalType(LogicalTypeId::kVarchar), false, std::set<ConstrainType>()},
            {"column_count", 1, LogicalType(LogicalTypeId::kBigInt), false, std::set<ConstrainType>()},
            {"row_count", 2, LogicalType(LogicalTypeId::kBigInt), false, std::set<ConstrainType>()},
            {"chunk_count", 3, LogicalType(LogicalTypeId::kBigInt), false, std::set<ConstrainType>()},
            {"chunk_size", 4, LogicalType(LogicalTypeId::kBigInt), false, std::set<ConstrainType>()},
    };

    std::shared_ptr<TableDefinition> table_def_ptr = std::make_shared<TableDefinition>("Tables", column_defs, false);

    std::shared_ptr<Table> table_ptr = std::make_shared<FixedRowCountTable>(table_def_ptr);
    // TODO: Insert tables into table

    std::shared_ptr<LogicalOperator> logical_chunk_scan =
            std::make_shared<LogicalChunkScan>(table_ptr, LogicalOperator::get_new_id());

    return logical_chunk_scan;

//    TableDefinition table_def =

//    Infinity::instance().catalog()->
    ResponseError("Show tables isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildImport(const hsql::ImportStatement &statement) {
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
    ResponseError("Invalid import type.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildImportCsv(const hsql::ImportStatement &statement) {
    ResponseError("Import from CSV file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildImportTbl(const hsql::ImportStatement &statement) {
    ResponseError("Import from other table file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildImportBinary(const hsql::ImportStatement &statement) {
    ResponseError("Import from binary file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildImportAuto(const hsql::ImportStatement &statement) {
    ResponseError("Import from automatically identified format isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildExport(const hsql::ExportStatement &statement) {
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
    ResponseError("Invalid export type.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildExportCsv(const hsql::ExportStatement &statement) {
    ResponseError("Exporting to CSV file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildExportTbl(const hsql::ExportStatement &statement) {
    ResponseError("Exporting to table file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildExportBinary(const hsql::ExportStatement &statement) {
    ResponseError("Exporting to binary file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildExportAuto(const hsql::ExportStatement &statement) {
    ResponseError("Exporting in auto mode isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildTransaction(const hsql::TransactionStatement &statement) {
    switch(statement.command) {
        case hsql::kBeginTransaction:
            return BuildTransactionBegin(statement);
        case hsql::kCommitTransaction:
            return BuildTransactionCommit(statement);
        case hsql::kRollbackTransaction:
            return BuildTransactionRollback(statement);
    }
    ResponseError("Invalid transaction command");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildTransactionBegin(const hsql::TransactionStatement &statement) {
    ResponseError("Transaction command: BEGIN isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildTransactionCommit(const hsql::TransactionStatement &statement) {
    ResponseError("Transaction command: COMMIT isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildTransactionRollback(const hsql::TransactionStatement &statement) {
    ResponseError("Transaction command: ROLLBACK isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildAlter(const hsql::AlterStatement &statement) {
    switch(statement.action->type) {
        case hsql::DropColumn:
            return BuildAlterDropColumn(statement);
    }
    ResponseError("Invalid ALTER command");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildAlterDropColumn(const hsql::AlterStatement &statement) {
    ResponseError("Alter: DROP COLUMN isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildPrepare(const hsql::PrepareStatement &statement) {
    ResponseError("Prepare statement isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildExecute(const hsql::ExecuteStatement &statement) {
    ResponseError("Execute statement isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}


}