//
// Created by JinHai on 2022/7/23.
//

#include "planner.h"

#include "main/infinity.h"

#include "planner/node/logical_create_table.h"
#include "planner/node/logical_drop_table.h"
#include "planner/node/logical_chunk_scan.h"
#include "planner/node/logical_table_scan.h"
#include "planner/node/logical_view_scan.h"
#include "planner/node/logical_insert.h"
#include "planner/node/logical_filter.h"

#include "binder/aggregate_binder.h"
#include "binder/group_binder.h"
#include "binder/having_binder.h"
#include "binder/insert_binder.h"
#include "binder/select_binder.h"
#include "binder/where_binder.h"

#include "storage/table_definition.h"
#include "storage/column_definition.h"
#include "storage/table_with_fix_row.h"

#include "common/utility/infinity_assert.h"

#include "expression/value_expression.h"
#include "expression/cast_expression.h"
#include "expression/column_expression.h"

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

std::shared_ptr<LogicalNode>
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
            PlannerError("Statement type not supported");
        default:
            PlannerError("Invalid statement type");
    }
}

// Build create statement to logical create operator
// Including: create table / view
std::shared_ptr<LogicalNode>
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
            PlannerError("Creating type isn't supported");
        }
    }
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
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
    std::shared_ptr<LogicalNode> logical_create_table_operator
        = std::make_shared<LogicalCreateTable>(schema_name_ptr, table_def_ptr);
    this->AppendOperator(logical_create_table_operator, bind_context_ptr);

    if(statement.select != nullptr) {
        // TODO: build select here, bind context is needed.
//        std::shared_ptr<LogicalOperator> select_node = BuildSelect(*statement.select);
//        logical_create_table_operator->set_left_node(select_node);
    }
    return logical_create_table_operator;
}

std::shared_ptr<LogicalNode>
Planner::BuildCreateTableFromTable(const hsql::CreateStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Creating table from table isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildCreateView(const hsql::CreateStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Creating view isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildCreateIndex(const hsql::CreateStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Creating index isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
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
            PlannerError("Deleting type isn't supported");
        }
    }

    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildDropTable(const hsql::DropStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {

    std::shared_ptr<std::string> schema_name_ptr = std::make_shared<std::string>("Default");
    if(statement.schema != nullptr) {
        schema_name_ptr = std::make_shared<std::string>(statement.schema);
    }

    std::shared_ptr<LogicalNode> logical_drop_table
        = std::make_shared<LogicalDropTable>(schema_name_ptr,
                                             std::make_shared<std::string>(statement.name));
    this->AppendOperator(logical_drop_table, bind_context_ptr);

    return logical_drop_table;
}

std::shared_ptr<LogicalNode>
Planner::BuildDropSchema(const hsql::DropStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Dropping schema isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildDropIndex(const hsql::DropStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Dropping index isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildDropView(const hsql::DropStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Dropping view isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildDropPreparedStatement(const hsql::DropStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Dropping prepared statement isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildInsert(const hsql::InsertStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    bind_context_ptr->binder_ = std::make_shared<InsertBinder>();
    switch(statement.type) {
        case hsql::kInsertValues:{
            return BuildInsertValue(statement, bind_context_ptr);
        }
        case hsql::kInsertSelect:
            return BuildInsertSelect(statement, bind_context_ptr);
        default:
            PlannerError("Inserting type isn't supported");
    }
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildInsertValue(const hsql::InsertStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    // Get schema name
    std::string schema_name = statement.schema == nullptr? "Default" : statement.schema;

    // Get table name
    if(statement.tableName == nullptr) {
        PlannerError("Insert statement missing table name.");
    }
    std::string table_name = std::string{statement.tableName};
    // Check schema and table in the catalog
    std::shared_ptr<Table> table_ptr = Infinity::instance().catalog()->GetTableByName(schema_name, table_name);
    if(table_ptr == nullptr) { PlannerError(schema_name + "." + table_name + " not exists.")}

    // Create value list
    std::vector<std::shared_ptr<BaseExpression>> value_list;
    value_list.reserve(statement.values->size());
    for (const auto* expr : *statement.values) {
        std::shared_ptr<BaseExpression> value_expr
            = bind_context_ptr->binder_->BuildExpression(*expr, current_bind_context_ptr_);
        value_list.emplace_back(value_expr);
    }

    // Rearrange the inserted value to match the table.
    // SELECT INTO TABLE (c, a) VALUES (1, 2); => SELECT INTO TABLE (a, b, c) VALUES( 2, NULL, 1);
    if (statement.columns != nullptr) {
        size_t statement_column_count = statement.columns->size();
        PlannerAssert(statement_column_count == value_list.size(),
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
    std::shared_ptr<LogicalNode> logical_insert =
            std::make_shared<LogicalInsert>(table_ptr, value_list);
    this->AppendOperator(logical_insert, bind_context_ptr);

    return logical_insert;
}

std::shared_ptr<LogicalNode>
Planner::BuildInsertSelect(const hsql::InsertStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Inserting select isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildDelete(const hsql::DeleteStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Delete isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildUpdate(const hsql::UpdateStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Update isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildSelect(const hsql::SelectStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {

    PlannerAssert(statement.selectList != nullptr, "SELECT list is needed");
    PlannerAssert(!statement.selectList->empty(), "SELECT list can't be empty");


    std::shared_ptr<LogicalNode> root_node_ptr;
    // 1. WITH clause
    if (statement.withDescriptions != nullptr) {
        for(const auto& with_description: *statement.withDescriptions) {
            std::string name = with_description->alias;
            if(bind_context_ptr->CTE_map_.contains(name)) {
                PlannerError("WITH query name: " + name + " occurs more than once.");
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

    current_bind_context_ptr_->binder_ = std::make_shared<WhereBinder>();

    // 5. SELECT list (aliases)
    // Check all select list items to transfer all columns into table.column_name style.
    // Can we also bind them all?
    std::vector<SelectListElement> select_list
        = BuildSelectList(*statement.selectList, current_bind_context_ptr_);

    // 6. WHERE
    if (statement.whereClause) {
        std::shared_ptr<LogicalNode> filter_operator = BuildFilter(statement.whereClause, current_bind_context_ptr_);
        filter_operator->set_left_node(root_node_ptr);
        root_node_ptr = filter_operator;
    }

    // 7. GROUP BY
    // 8. WITH CUBE / WITH ROLLUP
    // 9. HAVING
    // 10. DISTINCT
    root_node_ptr = BuildGroupByHaving(statement, current_bind_context_ptr_, root_node_ptr);

    // 11. SELECT (not flatten subquery)
    current_bind_context_ptr_->binder_ = std::make_shared<SelectBinder>();
    for (const SelectListElement& select_element : select_list) {
        // Make up the column identifier to: table.column_name;

        // Bind the expr use SelectBinder

        // Set project alias to the project list index.
    }


    // 12. ORDER BY
    if(statement.order != nullptr && statement.limit == nullptr) {
        std::shared_ptr<LogicalNode> order_operator = BuildOrderBy(*statement.order, current_bind_context_ptr_);
        order_operator->set_left_node(root_node_ptr);
        root_node_ptr = order_operator;
    }
    // 13. LIMIT
    if(statement.limit !=nullptr && statement.order == nullptr) {
        std::shared_ptr<LogicalNode> limit_operator = BuildLimit(*statement.limit, current_bind_context_ptr_);
        limit_operator->set_left_node(root_node_ptr);
        root_node_ptr = limit_operator;
    }
    // 14. TOP
    if(statement.limit != nullptr && statement.order != nullptr) {
        std::shared_ptr<LogicalNode> top_operator = BuildTop(*statement.order, *statement.limit, current_bind_context_ptr_);
        top_operator->set_left_node(root_node_ptr);
        root_node_ptr = top_operator;
    }
    // 15. UNION/INTERSECT/EXCEPT
    // 16. LIMIT
    // 17. ORDER BY
    // 18. TOP

    // Append projection node, also flatten subquery.
    PlannerError("Select isn't supported.");
    return root_node_ptr;
}

std::shared_ptr<LogicalNode>
Planner::BuildShow(const hsql::ShowStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    switch(statement.type) {
        case hsql::kShowTables : {
            return BuildShowTables(statement, bind_context_ptr);
        }
        case hsql::kShowColumns : {
            return BuildShowColumns(statement, bind_context_ptr);
        }
        default:
            PlannerError("Don't support show type.");
    }
//    return std::shared_ptr<LogicalOperator>();
}

std::shared_ptr<LogicalNode>
Planner::BuildShowColumns(const hsql::ShowStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Show columns isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildShowTables(const hsql::ShowStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    std::shared_ptr<LogicalNode> logical_chunk_scan =
            std::make_shared<LogicalChunkScan>(ChunkScanType::kShowTables);
    this->AppendOperator(logical_chunk_scan, bind_context_ptr);

    return logical_chunk_scan;
}

std::shared_ptr<LogicalNode>
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
    PlannerError("Invalid import type.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildImportCsv(const hsql::ImportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Import from CSV file isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildImportTbl(const hsql::ImportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Import from other table file isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildImportBinary(const hsql::ImportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Import from binary file isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildImportAuto(const hsql::ImportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Import from automatically identified format isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
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
    PlannerError("Invalid export type.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildExportCsv(const hsql::ExportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Exporting to CSV file isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildExportTbl(const hsql::ExportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Exporting to table file isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildExportBinary(const hsql::ExportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Exporting to binary file isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildExportAuto(const hsql::ExportStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Exporting in auto mode isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildTransaction(const hsql::TransactionStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    switch(statement.command) {
        case hsql::kBeginTransaction:
            return BuildTransactionBegin(statement, bind_context_ptr);
        case hsql::kCommitTransaction:
            return BuildTransactionCommit(statement, bind_context_ptr);
        case hsql::kRollbackTransaction:
            return BuildTransactionRollback(statement, bind_context_ptr);
    }
    PlannerError("Invalid transaction command");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildTransactionBegin(const hsql::TransactionStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Transaction command: BEGIN isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildTransactionCommit(const hsql::TransactionStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Transaction command: COMMIT isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildTransactionRollback(const hsql::TransactionStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Transaction command: ROLLBACK isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildAlter(const hsql::AlterStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    switch(statement.action->type) {
        case hsql::DropColumn:
            return BuildAlterDropColumn(statement, bind_context_ptr);
    }
    PlannerError("Invalid ALTER command");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildAlterDropColumn(const hsql::AlterStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Alter: DROP COLUMN isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildPrepare(const hsql::PrepareStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Prepare statement isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildExecute(const hsql::ExecuteStatement &statement, const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Execute statement isn't supported.");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
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
            PlannerError("BuildFromClause: Table select");
            break;
        }
        case hsql::kTableJoin: {
            // select t1.b, t2.c from t1 join t2 on t1.a = t2.a
            PlannerError("BuildFromClause: Table join");
            break;
        }
        case hsql::kTableCrossProduct: {
            // select t1.b, t2.c from t1, t2;
            PlannerError("BuildFromClause: Cross product");
            break;
        }
    }


    PlannerError("BuildFromClause is not implemented");
    return std::shared_ptr<LogicalNode>();
}

std::vector<SelectListElement>
Planner::BuildSelectList(const std::vector<hsql::Expr*>& select_list, const std::shared_ptr<BindContext>& bind_context_ptr) {

    std::vector<SelectListElement> select_lists;
    select_lists.reserve(select_list.size());
    for(const hsql::Expr* select_expr: select_list) {
        switch(select_expr->type) {
            case hsql::kExprStar: {
                std::shared_ptr<Table> table_ptr;
                if(select_expr->table == nullptr) {
                    // select * from t1;
                    PlannerAssert(!bind_context_ptr->tables_.empty(), "No table is bound.");

                    // Use first table as the default table: select * from t1, t2; means select t1.* from t1, t2;
                    table_ptr = bind_context_ptr->tables_[0];
                } else {
                    // select t1.* from t1;
                    std::string table_name(select_expr->table);
                    if (bind_context_ptr->tables_by_name_.contains(table_name)) {
                        table_ptr = bind_context_ptr->tables_by_name_[table_name];
                    } else {
                        PlannerAssert(!bind_context_ptr->tables_.empty(), "Table: '" + table_name + "' not found in select list.");
                    }
                }

                std::shared_ptr<std::string> table_name_ptr = std::make_shared<std::string>(table_ptr->table_def()->name());

                // Get corresponding column definition from binding context.
                const std::vector<ColumnDefinition>& columns_def = bind_context_ptr->tables_[0]->table_def()->columns();

                // Reserve more data in select list
                select_lists.reserve(select_list.size() + columns_def.size());

                // Build select list
                uint64_t column_index = 0;
                for(const ColumnDefinition& column_def: columns_def) {
                    ColumnBinding column_binding(table_name_ptr,
                                                 std::make_shared<std::string>(column_def.name()),
                                                 0,
                                                 column_index);

                    std::shared_ptr<BoundColumnExpression> bound_column_expr_ptr
                            = std::make_shared<BoundColumnExpression>(column_def.logical_type(), column_binding);
                    ColumnIdentifier column_identifier(*table_name_ptr, column_def.name());
                    select_lists.emplace_back(bound_column_expr_ptr);
                    select_lists.back().AddColumnIdentifier(column_identifier);

                    // Add the output heading of this bind context
                    bind_context_ptr->heading_.emplace_back(column_def.name());
                }

                break;
            }
            default: {
                std::shared_ptr<BaseExpression> expr
                    = bind_context_ptr->binder_->BuildExpression(*select_expr, bind_context_ptr);

                select_lists.emplace_back(expr);

                // Generator column identifier
                // TODO: This is duplicate code from Build Expression, think about how to remove it.
                std::optional<std::string> table_name = nullptr;
                if(select_expr->table != nullptr) {
                    table_name = std::optional<std::string>(std::string(select_expr->table));
                }
                ColumnIdentifier column_identifier(table_name, select_expr->name);

                // Add column identifier
                select_lists.back().AddColumnIdentifier(column_identifier);

                // Add the output heading of this bind context
                bind_context_ptr->heading_.emplace_back(select_expr->name);
            }
        }
    }

    return select_lists;
}

std::shared_ptr<LogicalNode>
Planner::BuildFilter(const hsql::Expr* whereClause, const std::shared_ptr<BindContext>& bind_context_ptr) {
    std::shared_ptr<BaseExpression> where_expr =
            bind_context_ptr->binder_->BuildExpression(*whereClause, current_bind_context_ptr_);
    std::shared_ptr<LogicalFilter> logical_filter = std::make_shared<LogicalFilter>(where_expr);
    return logical_filter;
}

std::shared_ptr<LogicalNode>
Planner::BuildGroupByHaving(
        const hsql::SelectStatement& select,
        const std::shared_ptr<BindContext>& bind_context_ptr,
        const std::shared_ptr<LogicalNode>& root_operator) {

    if(select.groupBy != nullptr) {
        // Start to bind GROUP BY clause
        // Set group binder
        bind_context_ptr->binder_ = std::make_shared<GroupBinder>();

        bind_context_ptr->groups_.reserve(select.groupBy->columns->size());
        for (const hsql::Expr* expr: *select.groupBy->columns) {

            // Call GroupBinder BuildExpression
            std::shared_ptr<BaseExpression> group_by_expr =
                    bind_context_ptr->binder_->BuildExpression(*expr, bind_context_ptr);
        }
    }

    // All having expr must appear in group by list or aggregate function list.
    if(select.groupBy != nullptr && select.groupBy->having != nullptr) {
        // Start to bind Having clause
        // Set having binder
        bind_context_ptr->binder_ = std::make_shared<GroupBinder>();
        for (const hsql::Expr* expr: *select.groupBy->having->exprList) {
            bind_context_ptr->binder_->BuildExpression(*expr, bind_context_ptr);
        }
    }

    PlannerError("BuildGroupByHaving is not implemented");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildOrderBy(
        const std::vector<hsql::OrderDescription*>& order_by_clause,
        const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("BuildOrderBy is not implemented");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildLimit(
        const hsql::LimitDescription& limit_description,
        const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("BuildLimit is not implemented");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildTop(const std::vector<hsql::OrderDescription*>& order_by_clause,
         const hsql::LimitDescription& limit_description,
         const std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("BuildTop is not implemented");
    return std::shared_ptr<LogicalNode>();
}

std::shared_ptr<LogicalNode>
Planner::BuildTable(const hsql::TableRef* from_table, const std::shared_ptr<BindContext>& bind_context_ptr) {
    std::string name = from_table->name;
    std::string schema_name = from_table->schema == nullptr ? "Default" : std::string(from_table->schema);
    if(bind_context_ptr->CTE_map_.contains(name)) {
        // Table is from CTE
        PlannerError("Table can't be CTE table now.");
    }

    std::shared_ptr<Table> table_ptr = Infinity::instance().catalog()->GetTableByName(schema_name, name);
    if(table_ptr != nullptr) {
        // Build table scan operator
        std::shared_ptr<LogicalTableScan> logical_table_scan = std::make_shared<LogicalTableScan>(table_ptr);
        this->AppendOperator(logical_table_scan, bind_context_ptr);

        // Insert the table in the binding context
        bind_context_ptr->AddTable(table_ptr);

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

    PlannerError("BuildTable: trying to build an supported table");
}

uint64_t Planner::AppendOperator(std::shared_ptr<LogicalNode> op, const std::shared_ptr<BindContext>& bind_context) {
    uint64_t node_id = operator_array_.size();
    op->set_node_id(node_id);
    operator_array_.emplace_back(OperatorContext {std::move(op), bind_context->id_});
    return node_id;
}

}