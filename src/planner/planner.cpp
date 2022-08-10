//
// Created by JinHai on 2022/7/23.
//

#include "planner.h"

#include "main/infinity.h"

#include "planner/operator/logical_create_table.h"
#include "planner/operator/logical_drop_table.h"
#include "planner/operator/logical_chunk_scan.h"
#include "planner/operator/logical_table_scan.h"
#include "planner/operator/logical_view_scan.h"
#include "planner/operator/logical_insert.h"

#include "storage/table_definition.h"
#include "storage/column_definition.h"
#include "storage/table_with_fix_row.h"

#include "common/utility/asserter.h"

#include "expression/value_expression.h"
#include "expression/cast_expression.h"

#include <vector>


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

    current_bind_context_ptr_ = std::make_shared<BindContext>();
    current_bind_context_ptr_->id_ = bind_contexts_.size();
    bind_contexts_.emplace_back(current_bind_context_ptr_);

    switch (statement.type()) {
        case hsql::kStmtSelect:
            return BuildSelect(static_cast<const hsql::SelectStatement &>(statement), current_bind_context_ptr_);
        case hsql::kStmtInsert:
            return BuildInsert(static_cast<const hsql::InsertStatement&>(statement), current_bind_context_ptr_);
        case hsql::kStmtDelete:
            return BuildDelete(static_cast<const hsql::DeleteStatement&>(statement), current_bind_context_ptr_);
        case hsql::kStmtUpdate:
            return BuildUpdate(static_cast<const hsql::UpdateStatement&>(statement), current_bind_context_ptr_);
        case hsql::kStmtShow:
            return BuildShow(static_cast<const hsql::ShowStatement&>(statement), current_bind_context_ptr_);
        case hsql::kStmtCreate:
            return BuildCreate(static_cast<const hsql::CreateStatement &>(statement), current_bind_context_ptr_);
        case hsql::kStmtDrop:
            return BuildDrop(static_cast<const hsql::DropStatement &>(statement), current_bind_context_ptr_);
        case hsql::kStmtPrepare:
            return BuildPrepare(static_cast<const hsql::PrepareStatement&>(statement), current_bind_context_ptr_);
        case hsql::kStmtExecute:
            return BuildExecute(static_cast<const hsql::ExecuteStatement&>(statement), current_bind_context_ptr_);
        case hsql::kStmtImport:
            return BuildImport(static_cast<const hsql::ImportStatement&>(statement), current_bind_context_ptr_);
        case hsql::kStmtExport:
            return BuildExport(static_cast<const hsql::ExportStatement&>(statement), current_bind_context_ptr_);
        case hsql::kStmtTransaction:
            return BuildTransaction(static_cast<const hsql::TransactionStatement&>(statement), current_bind_context_ptr_);
        case hsql::kStmtAlter:
            return BuildAlter(static_cast<const hsql::AlterStatement&>(statement), current_bind_context_ptr_);
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
Planner::BuildCreate(const hsql::CreateStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    switch (statement.type) {
        case hsql::CreateType::kCreateTable: {
            return BuildCreateTable(statement, bind_context_ptr);
        }
        case hsql::CreateType::kCreateView: {
            return BuildCreateView(statement, bind_context_ptr);
        }
        case hsql::CreateType::kCreateTableFromTbl: {
            return BuildCreateTableFromTable(statement, bind_context_ptr);
        }
        case hsql::CreateType::kCreateIndex: {
            return BuildCreateIndex(statement, bind_context_ptr);;
        }
        default: {
            ResponseError("Creating type isn't supported");
        }
    }
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildCreateTable(const hsql::CreateStatement& statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
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
        = std::make_shared<LogicalCreateTable>(schema_name_ptr, table_def_ptr);
    this->AppendOperator(logical_create_table_operator, bind_context_ptr);

    if(statement.select != nullptr) {
        // TODO: build select here, bind context is needed.
//        std::shared_ptr<LogicalOperator> select_node = BuildSelect(*statement.select);
//        logical_create_table_operator->set_left_node(select_node);
    }
    return logical_create_table_operator;
}

std::shared_ptr<LogicalOperator>
Planner::BuildCreateTableFromTable(const hsql::CreateStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Creating table from table isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildCreateView(const hsql::CreateStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Creating view isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildCreateIndex(const hsql::CreateStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Creating index isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildDrop(const hsql::DropStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    switch(statement.type) {
        case hsql::kDropTable:
            return BuildDropTable(statement, bind_context_ptr);
        case hsql::kDropSchema:
            return BuildDropSchema(statement, bind_context_ptr);
        case hsql::kDropIndex:
            return BuildDropIndex(statement, bind_context_ptr);
        case hsql::kDropView:
            return BuildDropView(statement, bind_context_ptr);
        case hsql::kDropPreparedStatement:
            return BuildDropPreparedStatement(statement, bind_context_ptr);
        default: {
            ResponseError("Deleting type isn't supported");
        }
    }

    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildDropTable(const hsql::DropStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {

    std::shared_ptr<std::string> schema_name_ptr = std::make_shared<std::string>("Default");
    if(statement.schema != nullptr) {
        schema_name_ptr = std::make_shared<std::string>(statement.schema);
    }

    std::shared_ptr<LogicalOperator> logical_drop_table
        = std::make_shared<LogicalDropTable>(schema_name_ptr,
                                             std::make_shared<std::string>(statement.name));
    this->AppendOperator(logical_drop_table, bind_context_ptr);

    return logical_drop_table;
}

std::shared_ptr<LogicalOperator>
Planner::BuildDropSchema(const hsql::DropStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Dropping schema isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildDropIndex(const hsql::DropStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Dropping index isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildDropView(const hsql::DropStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Dropping view isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildDropPreparedStatement(const hsql::DropStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Dropping prepared statement isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildInsert(const hsql::InsertStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    switch(statement.type) {
        case hsql::kInsertValues:
            return BuildInsertValue(statement, bind_context_ptr);
        case hsql::kInsertSelect:
            return BuildInsertSelect(statement, bind_context_ptr);
        default:
            ResponseError("Inserting type isn't supported");
    }
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildInsertValue(const hsql::InsertStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    // Get schema name
    std::string schema_name = statement.schema == nullptr? "Default" : statement.schema;

    // Get table name
    if(statement.tableName == nullptr) {
        ResponseError("Insert statement missing table name.");
    }
    std::string table_name = std::string{statement.tableName};
    // Check schema and table in the catalog
    std::shared_ptr<Table> table_ptr = Infinity::instance().catalog()->GetTableByName(schema_name, table_name);
    if(table_ptr == nullptr) { ResponseError(schema_name + "." + table_name + " not exists.")}

    // Create value list
    std::vector<std::shared_ptr<BaseExpression>> value_list;
    value_list.reserve(statement.values->size());
    for (const auto* expr : *statement.values) {
        std::shared_ptr<BaseExpression> value_expr = BuildExpression(*expr, current_bind_context_ptr_);
        value_list.emplace_back(value_expr);
    }

    // Rearrange the inserted value to match the table.
    // SELECT INTO TABLE (c, a) VALUES (1, 2); => SELECT INTO TABLE (a, b, c) VALUES( 2, NULL, 1);
    if (statement.columns != nullptr) {
        size_t statement_column_count = statement.columns->size();
        Assert(statement_column_count == value_list.size(),
               "INSERT: Target column count and input column count mismatch");

        std::shared_ptr<BaseExpression> null_value_expr = std::make_shared<ValueExpression>(LogicalType(LogicalTypeId::kNull));

        size_t table_column_count = table_ptr->table_def()->columns().size();

        // Create value list with table column size and null value
        std::vector<std::shared_ptr<BaseExpression>> rewrite_value_list(table_column_count, null_value_expr);

        size_t column_idx = 0;
        for(const auto& column_name : *statement.columns) {
            // Get table index from the inserted value column name;
            size_t table_column_id = table_ptr->table_def()->GetIdByName(column_name);
            LogicalType table_column_type = table_ptr->table_def()->columns()[table_column_id].logical_type();
            LogicalType value_type = value_list[column_idx]->DataType();
            if(value_type == table_column_type) {
                rewrite_value_list[table_column_id] = value_list[column_idx];
            } else {
                // If the inserted value type mismatches with table column type, cast the inserted value type to correct one.
                std::shared_ptr<BaseExpression> cast_expr = std::make_shared<CastExpression>(value_list[column_idx], table_column_type);
                rewrite_value_list[table_column_id] = cast_expr;
            }
            ++ column_idx;
        }

        value_list = rewrite_value_list;
    }

    // Create logical insert node.
    std::shared_ptr<LogicalOperator> logical_insert =
            std::make_shared<LogicalInsert>(table_ptr, value_list);
    this->AppendOperator(logical_insert, bind_context_ptr);

    return logical_insert;
}

std::shared_ptr<LogicalOperator>
Planner::BuildInsertSelect(const hsql::InsertStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Inserting select isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildDelete(const hsql::DeleteStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Delete isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildUpdate(const hsql::UpdateStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Update isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildSelect(const hsql::SelectStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {

    Assert(statement.selectList != nullptr, "SELECT list is needed");
    Assert(!statement.selectList->empty(), "SELECT list can't be empty");


    std::shared_ptr<LogicalOperator> root_node_ptr;
    // 1. WITH clause
    if (statement.withDescriptions != nullptr) {
        for(const auto& with_description: *statement.withDescriptions) {
            std::string name = with_description->alias;
            if(bind_context_ptr->CTE_map_.contains(name)) {
                ResponseError("WITH query name: " + name + " occurs more than once.");
            }

            std::shared_ptr<CommonTableExpressionInfo> cte_info_ptr
                = std::make_shared<CommonTableExpressionInfo>(name, with_description->select);

            bind_context_ptr->CTE_map_[name] = cte_info_ptr;
        }
    }

    // 2. FROM clause (BaseTable, Join and Subquery)
    // 3. ON
    // 4. JOIN
    if (statement.fromTable != nullptr) {
        // Build table reference
        root_node_ptr = BuildFromClause(statement.fromTable, bind_context_ptr);
    } else {
        // No table reference, just evaluate the expr of the select list.
    }

    // 5. SELECT list (aliases)
    if (statement.whereClause) {
        auto where_expr = BuildExpression(*statement.whereClause, current_bind_context_ptr_);

    }
    BuildSelectList(*statement.selectList, current_bind_context_ptr_);

    // 6. WHERE
    std::shared_ptr<LogicalOperator> filter_operator = BuildFilter(statement.whereClause, current_bind_context_ptr_);
    filter_operator->set_left_node(root_node_ptr);
    root_node_ptr = filter_operator;

    // 7. GROUP BY
    // 8. WITH CUBE / WITH ROLLUP
    // 9. HAVING
    // 10. DISTINCT
    root_node_ptr = BuildGroupByHaving(statement, current_bind_context_ptr_, root_node_ptr);

    // 11. SELECT


    // 12. ORDER BY
    if(statement.order != nullptr && statement.limit == nullptr) {
        std::shared_ptr<LogicalOperator> order_operator = BuildOrderBy(*statement.order, current_bind_context_ptr_);
        order_operator->set_left_node(root_node_ptr);
        root_node_ptr = order_operator;
    }
    // 13. LIMIT
    if(statement.limit !=nullptr && statement.order == nullptr) {
        std::shared_ptr<LogicalOperator> limit_operator = BuildLimit(*statement.limit, current_bind_context_ptr_);
        limit_operator->set_left_node(root_node_ptr);
        root_node_ptr = limit_operator;
    }
    // 14. TOP
    if(statement.limit != nullptr && statement.order != nullptr) {
        std::shared_ptr<LogicalOperator> top_operator = BuildTop(*statement.order, *statement.limit, current_bind_context_ptr_);
        top_operator->set_left_node(root_node_ptr);
        root_node_ptr = top_operator;
    }
    // 15. UNION/INTERSECT/EXCEPT
    // 16. LIMIT
    // 17. ORDER BY
    // 18. TOP
    ResponseError("Select isn't supported.");
    return root_node_ptr;
}

std::shared_ptr<LogicalOperator>
Planner::BuildShow(const hsql::ShowStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    switch(statement.type) {
        case hsql::kShowTables : {
            return BuildShowTables(statement, bind_context_ptr);
        }
        case hsql::kShowColumns : {
            return BuildShowColumns(statement, bind_context_ptr);
        }
        default:
            ResponseError("Don't support show type.");
    }
//    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildShowColumns(const hsql::ShowStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Show columns isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildShowTables(const hsql::ShowStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
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
            std::make_shared<LogicalChunkScan>(table_ptr);
    this->AppendOperator(logical_chunk_scan, bind_context_ptr);

    return logical_chunk_scan;

//    TableDefinition table_def =

//    Infinity::instance().catalog()->
    ResponseError("Show tables isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildImport(const hsql::ImportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    switch(statement.type) {
        case hsql::kImportCSV:
            return BuildImportCsv(statement, bind_context_ptr);
        case hsql::kImportTbl:
            return BuildImportTbl(statement, bind_context_ptr);
        case hsql::kImportBinary:
            return BuildImportBinary(statement, bind_context_ptr);
        case hsql::kImportAuto:
            return BuildImportAuto(statement, bind_context_ptr);
    }
    ResponseError("Invalid import type.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildImportCsv(const hsql::ImportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Import from CSV file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildImportTbl(const hsql::ImportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Import from other table file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildImportBinary(const hsql::ImportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Import from binary file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildImportAuto(const hsql::ImportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Import from automatically identified format isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildExport(const hsql::ExportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    switch(statement.type) {
        case hsql::kImportCSV:
            return BuildExportCsv(statement, bind_context_ptr);
        case hsql::kImportTbl:
            return BuildExportTbl(statement, bind_context_ptr);
        case hsql::kImportBinary:
            return BuildExportBinary(statement, bind_context_ptr);
        case hsql::kImportAuto:
            return BuildExportAuto(statement, bind_context_ptr);
    }
    ResponseError("Invalid export type.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildExportCsv(const hsql::ExportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Exporting to CSV file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildExportTbl(const hsql::ExportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Exporting to table file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildExportBinary(const hsql::ExportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Exporting to binary file isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildExportAuto(const hsql::ExportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Exporting in auto mode isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildTransaction(const hsql::TransactionStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    switch(statement.command) {
        case hsql::kBeginTransaction:
            return BuildTransactionBegin(statement, bind_context_ptr);
        case hsql::kCommitTransaction:
            return BuildTransactionCommit(statement, bind_context_ptr);
        case hsql::kRollbackTransaction:
            return BuildTransactionRollback(statement, bind_context_ptr);
    }
    ResponseError("Invalid transaction command");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildTransactionBegin(const hsql::TransactionStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Transaction command: BEGIN isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildTransactionCommit(const hsql::TransactionStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Transaction command: COMMIT isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildTransactionRollback(const hsql::TransactionStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Transaction command: ROLLBACK isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildAlter(const hsql::AlterStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    switch(statement.action->type) {
        case hsql::DropColumn:
            return BuildAlterDropColumn(statement, bind_context_ptr);
    }
    ResponseError("Invalid ALTER command");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildAlterDropColumn(const hsql::AlterStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Alter: DROP COLUMN isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildPrepare(const hsql::PrepareStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Prepare statement isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildExecute(const hsql::ExecuteStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("Execute statement isn't supported.");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<BaseExpression>
Planner::BuildExpression(const hsql::Expr &expr, const std::shared_ptr<BindContext>& bind_context_ptr) {

    std::string expr_name = expr.name ? std::string(expr.name) : std::string();

    std::shared_ptr<BaseExpression> left = expr.expr ? BuildExpression(*expr.expr, bind_context_ptr) : nullptr;
    std::shared_ptr<BaseExpression> right = expr.expr2 ? BuildExpression(*expr.expr2, bind_context_ptr) : nullptr;

    LogicalType logical_type(LogicalTypeId::kNull);
    switch(expr.type) {
        case hsql::kExprLiteralFloat:
            logical_type = LogicalType(LogicalTypeId::kDouble);
            return std::make_shared<ValueExpression>(logical_type, expr.fval);
        case hsql::kExprLiteralInt:
            // TODO: int16/int8 also can be found out.
            logical_type = static_cast<int32_t>(expr.ival) == expr.ival ?
                           LogicalType(LogicalTypeId::kInteger): LogicalType(LogicalTypeId::kBigInt);
            return std::make_shared<ValueExpression>(logical_type, expr.ival);

        case hsql::kExprLiteralString:
            logical_type = LogicalType(LogicalTypeId::kVarchar);
            return std::make_shared<ValueExpression>(logical_type, expr_name);

        case hsql::kExprLiteralNull:
            logical_type = LogicalType(LogicalTypeId::kNull);
            return std::make_shared<ValueExpression>(logical_type);


        default:
            ResponseError("Unsupported expr type");
    }

    return std::shared_ptr<BaseExpression>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildFromClause(const hsql::TableRef* from_table, const std::shared_ptr<BindContext>& bind_context_ptr) {
    switch(from_table->type) {
        case hsql::kTableName: {
            // Only one table: select * from t1;
            return BuildTable(from_table, bind_context_ptr);
//            ResponseError("BuildFromClause: Only one table");
//            break;
        }
        case hsql::kTableSelect: {
            // select t1.a from (select * from t2 as t1);
            ResponseError("BuildFromClause: Table select");
            break;
        }
        case hsql::kTableJoin: {
            // select t1.b, t2.c from t1 join t2 on t1.a = t2.a
            ResponseError("BuildFromClause: Table join");
            break;
        }
        case hsql::kTableCrossProduct: {
            // select t1.b, t2.c from t1, t2;
            ResponseError("BuildFromClause: Cross product");
            break;
        }
    }


    ResponseError("BuildFromClause is not implemented");
    return std::shared_ptr<LogicalOperator>();
}

void
Planner::BuildSelectList(const std::vector<hsql::Expr*>& select_list, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("BuildSelectList is not implemented");
}

std::shared_ptr<LogicalOperator>
Planner::BuildFilter(const hsql::Expr* whereClause, const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("BuildFilter is not implemented");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildGroupByHaving(
        const hsql::SelectStatement& select,
        const std::shared_ptr<BindContext>& bind_context_ptr,
        const std::shared_ptr<LogicalOperator>& root_operator) {
    ResponseError("BuildGroupByHaving is not implemented");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildOrderBy(
        const std::vector<hsql::OrderDescription*>& order_by_clause,
        const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("BuildOrderBy is not implemented");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildLimit(
        const hsql::LimitDescription& limit_description,
        const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("BuildLimit is not implemented");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildTop(const std::vector<hsql::OrderDescription*>& order_by_clause,
         const hsql::LimitDescription& limit_description,
         const std::shared_ptr<BindContext>& bind_context_ptr) {
    ResponseError("BuildTop is not implemented");
    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalOperator>
Planner::BuildTable(const hsql::TableRef* from_table, const std::shared_ptr<BindContext>& bind_context_ptr) {
    std::string name = from_table->name;
    std::string schema_name = from_table->schema == nullptr ? "Default" : std::string(from_table->schema);
    if(bind_context_ptr->CTE_map_.contains(name)) {
        // Table is from CTE
        ResponseError("Table can't be CTE table now.");
    }

    std::shared_ptr<Table> table_ptr = Infinity::instance().catalog()->GetTableByName(schema_name, name);
    if(table_ptr != nullptr) {
        // Build table scan operator
        std::shared_ptr<LogicalTableScan> logical_table_scan = std::make_shared<LogicalTableScan>(table_ptr);
        this->AppendOperator(logical_table_scan, bind_context_ptr);

        // Handle table and column alias
        if(from_table->alias != nullptr) {
            logical_table_scan->table_alias_ = from_table->alias->name;
            logical_table_scan->column_aliases_.reserve((from_table->alias->columns)->size());
            for(const char* column_alias: *(from_table->alias->columns)) {
                logical_table_scan->column_aliases_.emplace_back(column_alias);
            }
        }

        return logical_table_scan;
    }

    std::shared_ptr<View> view_ptr = Infinity::instance().catalog()->GetViewByName(schema_name, name);
    if(view_ptr != nullptr) {
        // Build view scan operator
        std::shared_ptr<LogicalViewScan> logical_view_scan = std::make_shared<LogicalViewScan>(view_ptr);
        this->AppendOperator(logical_view_scan, bind_context_ptr);
        return logical_view_scan;
    }

    ResponseError("BuildTable: trying to build an supported table");
}

uint64_t Planner::AppendOperator(std::shared_ptr<LogicalOperator> op, const std::shared_ptr<BindContext>& bind_context) {
    uint64_t node_id = operator_array_.size();
    op->set_node_id(node_id);
    operator_array_.emplace_back(OperatorContext {std::move(op), bind_context->id_});
    return node_id;
}

}