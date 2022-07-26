//
// Created by JinHai on 2022/7/23.
//

#include "storage/table_definition.h"
#include "planner/operator/logical_create_table.h"
#include "storage/column_definition.h"
#include "common/utility/asserter.h"
#include "planner.h"

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

void
Planner::CreateLogicalPlan(hsql::SQLStatement *statement) {
    root_operator_ = CreateLogicalOperator(*statement);
}

std::shared_ptr<LogicalOperator>
Planner::CreateLogicalOperator(const hsql::SQLStatement &statement) {
    switch (statement.type()) {
        case hsql::kStmtSelect:
            return BuildSelect(static_cast<const hsql::SelectStatement &>(statement));
        case hsql::kStmtInsert:
            return build_insert(static_cast<const hsql::InsertStatement&>(statement));
        case hsql::kStmtDelete:
            return build_delete(static_cast<const hsql::DeleteStatement&>(statement));
        case hsql::kStmtUpdate:
            return build_update(static_cast<const hsql::UpdateStatement&>(statement));
        case hsql::kStmtShow:
            return build_show(static_cast<const hsql::ShowStatement&>(statement));
        case hsql::kStmtCreate:
            return BuildCreate(static_cast<const hsql::CreateStatement &>(statement));
        case hsql::kStmtDrop:
            return BuildDrop(static_cast<const hsql::DropStatement &>(statement));
        case hsql::kStmtPrepare:
            return build_prepare(static_cast<const hsql::PrepareStatement&>(statement));
        case hsql::kStmtExecute:
            return build_execute(static_cast<const hsql::ExecuteStatement&>(statement));
        case hsql::kStmtImport:
            return build_import(static_cast<const hsql::ImportStatement&>(statement));
        case hsql::kStmtExport:
            return build_export(static_cast<const hsql::ExportStatement&>(statement));
        case hsql::kStmtTransaction:
            return build_transaction(static_cast<const hsql::TransactionStatement&>(statement));
        case hsql::kStmtAlter:
            return build_alter(static_cast<const hsql::AlterStatement&>(statement));
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

    std::shared_ptr<TableDefinition> table_def_ptr
        = std::make_shared<TableDefinition>(statement.tableName, columns, statement.ifNotExists);
    std::shared_ptr<LogicalOperator> logical_create_table_operator
        = std::make_shared<LogicalCreateTable>(
                LogicalOperatorType::kCreateTable,
                LogicalOperator::get_new_id(),
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
            return build_drop_index(statement);
        case hsql::kDropView:
            return build_drop_view(statement);
        case hsql::kDropPreparedStatement:
            return build_drop_prepared_statement(statement);
        default: {
            ResponseError("Deleting type isn't supported");
        }
    }

    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildDropTable(const hsql::DropStatement &statement) {
    ResponseError("Dropping table isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildDropSchema(const hsql::DropStatement &statement) {
    ResponseError("Dropping schema isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_drop_index(const hsql::DropStatement &statement) {
    ResponseError("Dropping index isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_drop_view(const hsql::DropStatement &statement) {
    ResponseError("Dropping view isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_drop_prepared_statement(const hsql::DropStatement &statement) {
    ResponseError("Dropping prepared statement isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_insert(const hsql::InsertStatement &statement) {
    switch(statement.type) {
        case hsql::kInsertValues:
            return build_insert_value(statement);
        case hsql::kInsertSelect:
            return build_insert_select(statement);
        default:
            ResponseError("Inserting type isn't supported");
    }
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_insert_value(const hsql::InsertStatement &statement) {
    ResponseError("Inserting values isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_insert_select(const hsql::InsertStatement &statement) {
    ResponseError("Inserting select isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_delete(const hsql::DeleteStatement &statement) {
    ResponseError("Delete isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_update(const hsql::UpdateStatement &statement) {
    ResponseError("Update isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::BuildSelect(const hsql::SelectStatement &statement) {
    ResponseError("Select isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_show(const hsql::ShowStatement &statement) {
    ResponseError("Show isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_show_columns(const hsql::ShowStatement &statement) {
    ResponseError("Show columns isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_show_tables(const hsql::ShowStatement &statement) {
    ResponseError("Show tables isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_import(const hsql::ImportStatement &statement) {
    switch(statement.type) {
        case hsql::kImportCSV:
            return build_import_csv(statement);
        case hsql::kImportTbl:
            return build_import_tbl(statement);
        case hsql::kImportBinary:
            return build_import_binary(statement);
        case hsql::kImportAuto:
            return build_import_auto(statement);
    }
    ResponseError("Invalid import type.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_import_csv(const hsql::ImportStatement &statement) {
    ResponseError("Import from CSV file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_import_tbl(const hsql::ImportStatement &statement) {
    ResponseError("Import from other table file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_import_binary(const hsql::ImportStatement &statement) {
    ResponseError("Import from binary file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_import_auto(const hsql::ImportStatement &statement) {
    ResponseError("Import from automatically identified format isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_export(const hsql::ExportStatement &statement) {
    switch(statement.type) {
        case hsql::kImportCSV:
            return build_export_csv(statement);
        case hsql::kImportTbl:
            return build_export_tbl(statement);
        case hsql::kImportBinary:
            return build_export_binary(statement);
        case hsql::kImportAuto:
            return build_export_auto(statement);
    }
    ResponseError("Invalid export type.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_export_csv(const hsql::ExportStatement &statement) {
    ResponseError("Exporting to CSV file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_export_tbl(const hsql::ExportStatement &statement) {
    ResponseError("Exporting to table file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_export_binary(const hsql::ExportStatement &statement) {
    ResponseError("Exporting to binary file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_export_auto(const hsql::ExportStatement &statement) {
    ResponseError("Exporting in auto mode isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_transaction(const hsql::TransactionStatement &statement) {
    switch(statement.command) {
        case hsql::kBeginTransaction:
            return build_transaction_begin(statement);
        case hsql::kCommitTransaction:
            return build_transaction_commit(statement);
        case hsql::kRollbackTransaction:
            return build_transaction_rollback(statement);
    }
    ResponseError("Invalid transaction command");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_transaction_begin(const hsql::TransactionStatement &statement) {
    ResponseError("Transaction command: BEGIN isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_transaction_commit(const hsql::TransactionStatement &statement) {
    ResponseError("Transaction command: COMMIT isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_transaction_rollback(const hsql::TransactionStatement &statement) {
    ResponseError("Transaction command: ROLLBACK isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_alter(const hsql::AlterStatement &statement) {
    switch(statement.action->type) {
        case hsql::DropColumn:
            return build_alter_drop_column(statement);
    }
    ResponseError("Invalid ALTER command");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_alter_drop_column(const hsql::AlterStatement &statement) {
    ResponseError("Alter: DROP COLUMN isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_prepare(const hsql::PrepareStatement &statement) {
    ResponseError("Prepare statement isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator> Planner::build_execute(const hsql::ExecuteStatement &statement) {
    ResponseError("Execute statement isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::string Planner::ToString() const {
    return std::string();
}


}