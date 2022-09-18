//
// Created by JinHai on 2022/9/11.
//

#include "plan_builder.h"

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
#include "binder/join_binder.h"

#include "storage/table_definition.h"
#include "storage/column_definition.h"
#include "storage/table_with_fix_row.h"

#include "common/utility/infinity_assert.h"

#include "expression/value_expression.h"
#include "expression/cast_expression.h"
#include "expression/column_expression.h"
#include "expression/conjunction_expression.h"

#include "expression/expression_transformer.h"


namespace infinity {

static LogicalType
TypeConversion(hsql::ColumnType type) {
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


PlanContext
PlanBuilder::BuildPlan(const hsql::SQLStatement &statement) {

    // Create the bind context and insert it into the root bind context array and assign an id to it.
    // TODO: consider to move the bind context construct into BuildXXXX method.
    std::shared_ptr<BindContext> bind_context_ptr = std::make_shared<BindContext>(nullptr);
    this->AddBindContextArray(bind_context_ptr);

    PlanContext plan_context;
    switch (statement.type()) {
        case hsql::kStmtSelect: {
            auto bound_select_node = BuildSelect(static_cast<const hsql::SelectStatement &>(statement), bind_context_ptr);

            // Need to create plan and construct the plan context;
            break;
        }
        case hsql::kStmtInsert:
            plan_context = BuildInsert(static_cast<const hsql::InsertStatement&>(statement), bind_context_ptr);
            break;
        case hsql::kStmtDelete:
            plan_context = BuildDelete(static_cast<const hsql::DeleteStatement&>(statement), bind_context_ptr);
            break;
        case hsql::kStmtUpdate:
            plan_context = BuildUpdate(static_cast<const hsql::UpdateStatement&>(statement), bind_context_ptr);
            break;
        case hsql::kStmtShow:
            plan_context = BuildShow(static_cast<const hsql::ShowStatement&>(statement), bind_context_ptr);
            break;
        case hsql::kStmtCreate:
            plan_context = BuildCreate(static_cast<const hsql::CreateStatement &>(statement), bind_context_ptr);
            break;
        case hsql::kStmtDrop:
            plan_context = BuildDrop(static_cast<const hsql::DropStatement &>(statement), bind_context_ptr);
            break;
        case hsql::kStmtPrepare:
            plan_context = BuildPrepare(static_cast<const hsql::PrepareStatement&>(statement), bind_context_ptr);
            break;
        case hsql::kStmtExecute:
            plan_context = BuildExecute(static_cast<const hsql::ExecuteStatement&>(statement), bind_context_ptr);
            break;
        case hsql::kStmtImport:
            plan_context = BuildImport(static_cast<const hsql::ImportStatement&>(statement), bind_context_ptr);
            break;
        case hsql::kStmtExport:
            plan_context = BuildExport(static_cast<const hsql::ExportStatement&>(statement), bind_context_ptr);
            break;
        case hsql::kStmtTransaction:
            plan_context = BuildTransaction(static_cast<const hsql::TransactionStatement&>(statement), bind_context_ptr);
            break;
        case hsql::kStmtAlter:
            plan_context = BuildAlter(static_cast<const hsql::AlterStatement&>(statement), bind_context_ptr);
            break;
        case hsql::kStmtError:
        case hsql::kStmtRename:
            PlannerError("Statement type not supported");
        default:
            PlannerError("Invalid statement type");
    }

    return plan_context;
}

std::vector<std::shared_ptr<BindContext>>&
PlanBuilder::BindContextArray() {
    return bind_context_array_;
}

void
PlanBuilder::AddBindContextArray(std::shared_ptr<BindContext>& bind_context_ptr) {
    std::vector<std::shared_ptr<BindContext>>& root_bind_context_array = this->BindContextArray();
    bind_context_ptr->id_ = root_bind_context_array.size();
    root_bind_context_array.emplace_back(bind_context_ptr);
}

// Build create statement to logical create operator
// Including: create table / view
PlanContext
PlanBuilder::BuildCreate(const hsql::CreateStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr) {
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

    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildCreateTable(const hsql::CreateStatement& statement, std::shared_ptr<BindContext>& bind_context_ptr) {
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

    // FIXME: check if we need to append operator
//    this->AppendOperator(logical_create_table_operator, bind_context_ptr);

    if(statement.select != nullptr) {
        // TODO: build select here, bind context is needed.
//        std::shared_ptr<LogicalOperator> select_node = BuildSelect(*statement.select);
//        logical_create_table_operator->set_left_node(select_node);
    }

    PlanContext res;
    res.plan = logical_create_table_operator;

    return res;
}

PlanContext
PlanBuilder::BuildCreateTableFromTable(const hsql::CreateStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Creating table from table isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildCreateView(const hsql::CreateStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Creating view isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildCreateIndex(const hsql::CreateStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Creating index isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildDrop(const hsql::DropStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
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

    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildDropTable(const hsql::DropStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {

    std::shared_ptr<std::string> schema_name_ptr = std::make_shared<std::string>("Default");
    if(statement.schema != nullptr) {
        schema_name_ptr = std::make_shared<std::string>(statement.schema);
    }

    std::shared_ptr<LogicalNode> logical_drop_table
            = std::make_shared<LogicalDropTable>(schema_name_ptr,
                                                 std::make_shared<std::string>(statement.name));
    // FIXME: check if we need to append operator
    //    this->AppendOperator(logical_drop_table, bind_context_ptr);

    PlanContext res;
    res.plan = logical_drop_table;

    return res;
}

PlanContext
PlanBuilder::BuildDropSchema(const hsql::DropStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Dropping schema isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildDropIndex(const hsql::DropStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Dropping index isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildDropView(const hsql::DropStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Dropping view isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildDropPreparedStatement(const hsql::DropStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Dropping prepared statement isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildInsert(const hsql::InsertStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
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
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildInsertValue(const hsql::InsertStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
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
                = bind_context_ptr->binder_->BuildExpression(*expr, bind_context_ptr);
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

    // FIXME: check if we need to append operator
//    this->AppendOperator(logical_insert, bind_context_ptr);

    PlanContext res;
    res.plan = logical_insert;

    return res;
}

PlanContext
PlanBuilder::BuildInsertSelect(const hsql::InsertStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Inserting select isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildDelete(const hsql::DeleteStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Delete isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildUpdate(const hsql::UpdateStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Update isn't supported.");
    PlanContext res;
    return res;
}

std::shared_ptr<BoundSelectNode>
PlanBuilder::BuildSelect(const hsql::SelectStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {

    PlannerAssert(statement.selectList != nullptr, "SELECT list is needed");
    PlannerAssert(!statement.selectList->empty(), "SELECT list can't be empty");

    // TODO: Move constructing bind context here ?
    auto bound_select_node = std::make_shared<BoundSelectNode>(bind_context_ptr);

    std::shared_ptr<LogicalNode> root_node_ptr;
    // 1. WITH clause
    if (statement.withDescriptions != nullptr) {

        // Prepare to store the with statement
        int64_t with_stmt_count = statement.withDescriptions->size();
        bind_context_ptr->CTE_map_.reserve(with_stmt_count);

        // Hash set to restrict the with statement name visibility
        std::unordered_set<std::string> masked_name_set;

        for(int64_t i = with_stmt_count - 1; i >= 0; -- i) {
            hsql::WithDescription* with_desc = (*statement.withDescriptions)[i];
            std::string name = with_desc->alias;
            if(bind_context_ptr->CTE_map_.contains(name)) {
                PlannerError("WITH query name: " + name + " occurs more than once.");
            }

            masked_name_set.insert(name);
            std::shared_ptr<CommonTableExpressionInfo> cte_info_ptr
                    = std::make_shared<CommonTableExpressionInfo>(name, with_desc->select, masked_name_set);

            bind_context_ptr->CTE_map_[name] = cte_info_ptr;
        }
    }

    // 2. FROM clause (BaseTable, Join and Subquery)
    // 3. ON
    // 4. JOIN
    if (statement.fromTable != nullptr) {
        // Build table reference
        bound_select_node->table_ref_ptr_ = BuildFromClause(statement.fromTable, bind_context_ptr);
    } else {
        // No table reference, just evaluate the expr of the select list.
    }

    bind_context_ptr->binder_ = std::make_shared<WhereBinder>();

    // 5. SELECT list (aliases)
    // Check all select list items to transfer all columns into table.column_name style.
    // Can we also bind them all?
    std::vector<SelectListElement> select_list
            = BuildSelectList(*statement.selectList, bind_context_ptr);

    // 6. WHERE
    if (statement.whereClause) {
        std::shared_ptr<LogicalNode> filter_operator = BuildFilter(statement.whereClause, bind_context_ptr).plan;
        filter_operator->set_left_node(root_node_ptr);
        root_node_ptr = filter_operator;
    }

    // 7. GROUP BY
    // 8. WITH CUBE / WITH ROLLUP
    // 9. HAVING
    // 10. DISTINCT
    root_node_ptr = BuildGroupByHaving(statement, bind_context_ptr, root_node_ptr).plan;

    // 11. SELECT (not flatten subquery)
    bind_context_ptr->binder_ = std::make_shared<SelectBinder>();
    for (const SelectListElement& select_element : select_list) {
        // Make up the column identifier to: table.column_name;

        // Bind the expr use SelectBinder

        // Set project alias to the project list index.
    }


    // 12. ORDER BY
    if(statement.order != nullptr && statement.limit == nullptr) {
        std::shared_ptr<LogicalNode> order_operator = BuildOrderBy(*statement.order, bind_context_ptr).plan;
        order_operator->set_left_node(root_node_ptr);
        root_node_ptr = order_operator;
    }
    // 13. LIMIT
    if(statement.limit !=nullptr && statement.order == nullptr) {
        std::shared_ptr<LogicalNode> limit_operator = BuildLimit(*statement.limit, bind_context_ptr).plan;
        limit_operator->set_left_node(root_node_ptr);
        root_node_ptr = limit_operator;
    }
    // 14. TOP
    if(statement.limit != nullptr && statement.order != nullptr) {
        std::shared_ptr<LogicalNode> top_operator = BuildTop(*statement.order, *statement.limit, bind_context_ptr).plan;
        top_operator->set_left_node(root_node_ptr);
        root_node_ptr = top_operator;
    }
    // 15. UNION/INTERSECT/EXCEPT
    // 16. LIMIT
    // 17. ORDER BY
    // 18. TOP

    // Append projection node, also flatten subquery.
    PlannerError("Select isn't supported.");
    return bound_select_node;
}

PlanContext
PlanBuilder::BuildShow(const hsql::ShowStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
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
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildShowColumns(const hsql::ShowStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Show columns isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildShowTables(const hsql::ShowStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    std::shared_ptr<LogicalNode> logical_chunk_scan =
            std::make_shared<LogicalChunkScan>(ChunkScanType::kShowTables);

    // FIXME: check if we need to append operator
//    this->AppendOperator(logical_chunk_scan, bind_context_ptr);
    PlanContext res;
    res.plan = logical_chunk_scan;
    return res;
}

PlanContext
PlanBuilder::BuildImport(const hsql::ImportStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
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
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildImportCsv(const hsql::ImportStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Import from CSV file isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildImportTbl(const hsql::ImportStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Import from other table file isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildImportBinary(const hsql::ImportStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Import from binary file isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildImportAuto(const hsql::ImportStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Import from automatically identified format isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildExport(const hsql::ExportStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
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
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildExportCsv(const hsql::ExportStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Exporting to CSV file isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildExportTbl(const hsql::ExportStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Exporting to table file isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildExportBinary(const hsql::ExportStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Exporting to binary file isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildExportAuto(const hsql::ExportStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Exporting in auto mode isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildTransaction(const hsql::TransactionStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    switch(statement.command) {
        case hsql::kBeginTransaction:
            return BuildTransactionBegin(statement, bind_context_ptr);
        case hsql::kCommitTransaction:
            return BuildTransactionCommit(statement, bind_context_ptr);
        case hsql::kRollbackTransaction:
            return BuildTransactionRollback(statement, bind_context_ptr);
    }
    PlannerError("Invalid transaction command");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildTransactionBegin(const hsql::TransactionStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Transaction command: BEGIN isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildTransactionCommit(const hsql::TransactionStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Transaction command: COMMIT isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildTransactionRollback(const hsql::TransactionStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Transaction command: ROLLBACK isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildAlter(const hsql::AlterStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    switch(statement.action->type) {
        case hsql::DropColumn:
            return BuildAlterDropColumn(statement, bind_context_ptr);
    }
    PlannerError("Invalid ALTER command");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildAlterDropColumn(const hsql::AlterStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Alter: DROP COLUMN isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildPrepare(const hsql::PrepareStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Prepare statement isn't supported.");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildExecute(const hsql::ExecuteStatement &statement, std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("Execute statement isn't supported.");
    PlanContext res;
    return res;
}

std::shared_ptr<TableRef>
PlanBuilder::BuildFromClause(const hsql::TableRef* from_table, std::shared_ptr<BindContext>& bind_context_ptr) {
    switch(from_table->type) {
        case hsql::kTableName: {
            // Only one table: select * from t1;
            return BuildTable(from_table, bind_context_ptr);
        }
        case hsql::kTableSelect: {
            // select t1.a from (select * from t2 as t1);
            std::string name = from_table->name != nullptr ? from_table->name : std::string();
            return BuildSubquery(name, *(from_table->select), bind_context_ptr);
        }
        case hsql::kTableJoin: {
            // select t1.b, t2.c from t1 join t2 on t1.a = t2.a
            return BuildJoin(from_table, bind_context_ptr);
        }
        case hsql::kTableCrossProduct: {
            // select t1.b, t2.c from t1, t2;
            return BuildCrossProduct(from_table, bind_context_ptr);
        }

        // TODO: No case currently, since parser doesn't support it.
//        case hsql::kExpressionList: {
//            break;
//        }
    }


    PlannerError("BuildFromClause is not implemented");
    return std::make_shared<TableRef>(TableRefType::kInvalid);
}

std::vector<SelectListElement>
PlanBuilder::BuildSelectList(const std::vector<hsql::Expr*>& select_list, std::shared_ptr<BindContext>& bind_context_ptr) {

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
                    std::shared_ptr<ColumnExpression> bound_column_expr_ptr
                            = std::make_shared<ColumnExpression>(column_def.logical_type(),
                                                                 *table_name_ptr,
                                                                 0, // TODO: need to generate a table index
                                                                 column_def.name(),
                                                                 column_index,
                                                                 0);
                    ++ column_index;
                    std::shared_ptr<std::string> column_name_ptr = std::make_shared<std::string>(column_def.name());
                    ColumnIdentifier column_identifier(table_name_ptr, column_name_ptr, nullptr);
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

                std::shared_ptr<std::string> table_name_ptr;
                if(select_expr->table != nullptr) {
                    table_name_ptr = std::make_shared<std::string>(select_expr->table);
                }
                ColumnIdentifier column_identifier(table_name_ptr,
                                                   std::make_shared<std::string>(select_expr->name),
                                                   nullptr);

                // Add column identifier
                select_lists.back().AddColumnIdentifier(column_identifier);

                // Add the output heading of this bind context
                bind_context_ptr->heading_.emplace_back(select_expr->name);
            }
        }
    }

    return select_lists;
}

PlanContext
PlanBuilder::BuildFilter(const hsql::Expr* whereClause, std::shared_ptr<BindContext>& bind_context_ptr) {
    std::shared_ptr<BaseExpression> where_expr =
            bind_context_ptr->binder_->BuildExpression(*whereClause, bind_context_ptr);
    std::shared_ptr<LogicalFilter> logical_filter = std::make_shared<LogicalFilter>(where_expr);
    PlanContext res;
    res.plan = logical_filter;
    return res;
}

PlanContext
PlanBuilder::BuildGroupByHaving(
        const hsql::SelectStatement& select,
        std::shared_ptr<BindContext>& bind_context_ptr,
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
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildOrderBy(
        const std::vector<hsql::OrderDescription*>& order_by_clause,
        std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("BuildOrderBy is not implemented");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildLimit(
        const hsql::LimitDescription& limit_description,
        std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("BuildLimit is not implemented");
    PlanContext res;
    return res;
}

PlanContext
PlanBuilder::BuildTop(const std::vector<hsql::OrderDescription*>& order_by_clause,
                  const hsql::LimitDescription& limit_description,
                  std::shared_ptr<BindContext>& bind_context_ptr) {
    PlannerError("BuildTop is not implemented");
    PlanContext res;
    return res;
}

std::shared_ptr<TableRef>
PlanBuilder::BuildTable(const hsql::TableRef* from_table, std::shared_ptr<BindContext>& bind_context_ptr) {
    // There are five cases here:
    // CTE*, which is subquery (may include correlated expression).
    // Recursive CTE (not supported by parser.)
    // View*, which is also a subquery (may include correlated expression).
    // Table*
    // External Table (not supported by parser.)

    // In AST, name is the alias of CTE and name of table
    // In AST, alias is the alias of table

    std::string name = from_table->name;
    std::string schema_name = from_table->schema == nullptr ? "Default" : std::string(from_table->schema);

    // Try to get CTE info from the cte container Query builder saved CTEs into before.
    auto cte = bind_context_ptr->GetCTE(name);
    if(cte != nullptr) {
        return BuildCTE(name, cte, bind_context_ptr);
    }

    // Base Table
    std::shared_ptr<Table> table_ptr = Infinity::instance().catalog()->GetTableByName(schema_name, name);
    if(table_ptr != nullptr) {
        std::string table_name = schema_name + "." + name;
        int64_t table_index = bind_context_ptr->GetNewTableIndex();
        int64_t node_id = bind_context_ptr->GetNewLogicalNodeId();
        // Build table scan operator
        std::shared_ptr<LogicalTableScan> logical_table_scan =
                std::make_shared<LogicalTableScan>(table_index, node_id, table_ptr);

        // TODO: Handle table and column alias
//        if(from_table->alias != nullptr) {
//            logical_table_scan->table_alias_ = from_table->alias->name;
//            logical_table_scan->column_aliases_.reserve((from_table->alias->columns)->size());
//            for(const char* column_alias: *(from_table->alias->columns)) {
//                logical_table_scan->column_aliases_.emplace_back(column_alias);
//            }
//        }
        auto& columns = table_ptr->table_def()->columns();
        std::vector<LogicalType> types;
        std::vector<std::string> names;
        types.reserve(columns.size());
        names.reserve(columns.size());
        for(auto& column : columns) {
            types.emplace_back(column.logical_type());
            names.emplace_back(column.name());
        }

        auto table_ref = std::make_shared<BaseTableRef>(logical_table_scan);

        // Insert the table in the binding context
        bind_context_ptr->AddTableBinding(table_name, table_index, node_id, logical_table_scan, types, names);

        return table_ref;
    }

    // View
    std::shared_ptr<View> view_ptr = Infinity::instance().catalog()->GetViewByName(schema_name, name);
    if(view_ptr != nullptr) {
        // Build view statement
        std::string view_name = schema_name + "." + name;
        BuildView(view_name, view_ptr, bind_context_ptr);
    }

    PlannerError("BuildTable: trying to build an supported table");
}

std::shared_ptr<TableRef>
PlanBuilder::BuildSubquery(const std::string &name, const hsql::SelectStatement& select_stmt, std::shared_ptr<BindContext>& bind_context_ptr) {
    // Create new bind context and add into context array;
    std::shared_ptr<BindContext> cte_bind_context_ptr = std::make_shared<BindContext>(bind_context_ptr);
    this->AddBindContextArray(cte_bind_context_ptr);

    // Create bound select node and subquery table reference
    auto bound_select_node_ptr = this->BuildSelect(select_stmt, cte_bind_context_ptr);
    auto subquery_table_ref_ptr = std::make_shared<SubqueryTableRef>(bound_select_node_ptr);

    // Get the table index of the sub query output
    int64_t table_index = bound_select_node_ptr->GetTableIndex();

    std::string binding_name = name.empty() ? "subquery" + std::to_string(table_index) : name;
    // Add binding into bind context
    bind_context_ptr->AddSubqueryBinding(binding_name, table_index, bound_select_node_ptr->types, bound_select_node_ptr->names);

    // TODO: Not care about the correlated expression

    // return subquery table reference
    return subquery_table_ref_ptr;
}

std::shared_ptr<TableRef>
PlanBuilder::BuildCTE(const std::string &name, const std::shared_ptr<CommonTableExpressionInfo>& cte, std::shared_ptr<BindContext>& bind_context_ptr) {
    // Table is from CTE
    if(bind_context_ptr->IsCTEBound(cte)) {
        // The CTE is bound before.
        PlannerError("CTE can only be bound only once");
    }

    // Build CTE(subquery)
    // insert into CTE Bound in bind context
    bind_context_ptr->BoundCTE(cte);

    // Create new bind context and add into context array;
    std::shared_ptr<BindContext> cte_bind_context_ptr = std::make_shared<BindContext>(bind_context_ptr);
    this->AddBindContextArray(cte_bind_context_ptr);

    // Create bound select node and subquery table reference
    auto bound_select_node_ptr = this->BuildSelect(*cte->select_statement_, cte_bind_context_ptr);
    auto subquery_table_ref_ptr = std::make_shared<SubqueryTableRef>(bound_select_node_ptr);

    // Get the table index of the sub query output
    int64_t table_index = bound_select_node_ptr->GetTableIndex();

    // Add binding into bind context
    bind_context_ptr->AddSubqueryBinding(name, table_index, bound_select_node_ptr->types, bound_select_node_ptr->names);

    // TODO: Not care about the correlated expression

    return subquery_table_ref_ptr;
}

std::shared_ptr<TableRef>
PlanBuilder::BuildView(const std::string &view_name, const std::shared_ptr<View>& view_ptr, std::shared_ptr<BindContext>& bind_context_ptr) {
    // Build view scan operator
    PlannerAssert(!(bind_context_ptr->IsViewBound(view_name)), "View: " + view_name + " is bound before!");
    bind_context_ptr->BoundView(view_name);

    const hsql::SQLStatement* sql_statement = view_ptr->GetSQLStatement();
    PlannerAssert(sql_statement->type() != hsql::StatementType::kStmtSelect, "View related statement isn't a select statement.");

    auto* select_stmt_ptr = static_cast<const hsql::SelectStatement*>(sql_statement);

    // Create new bind context and add into context array;
    std::shared_ptr<BindContext> cte_bind_context_ptr = std::make_shared<BindContext>(bind_context_ptr);
    this->AddBindContextArray(cte_bind_context_ptr);

    // Create bound select node and subquery table reference
    auto bound_select_node_ptr = this->BuildSelect(*select_stmt_ptr, cte_bind_context_ptr);
    auto subquery_table_ref_ptr = std::make_shared<SubqueryTableRef>(bound_select_node_ptr);

    // Get the table index of the sub query output
    int64_t table_index = bound_select_node_ptr->GetTableIndex();

    // Add binding into bind context
    bind_context_ptr->AddViewBinding(view_name, table_index, bound_select_node_ptr->types, bound_select_node_ptr->names);

    // TODO: Not care about the correlated expression

    // return subquery table reference
    return subquery_table_ref_ptr;
}

std::shared_ptr<TableRef>
PlanBuilder::BuildCrossProduct(const hsql::TableRef* from_table, std::shared_ptr<BindContext>& bind_context_ptr) {
    // Create left and right bind context
    // Build left and right table with new bind context
    // Merge two bind context into their parent context

    std::shared_ptr<CrossProductTableRef> result;

    const std::vector<hsql::TableRef*>& tables = *from_table->list;
    int64_t table_count = tables.size();

    hsql::TableRef* left_table_ptr = tables[0];
    std::shared_ptr<BindContext> left_bind_context_ptr = std::make_shared<BindContext>(bind_context_ptr);
    AddBindContextArray(left_bind_context_ptr);
    std::shared_ptr<TableRef> left_bound_table_ref = BuildFromClause(left_table_ptr, left_bind_context_ptr);

    for(int64_t idx = 1; idx < table_count - 1; ++ idx) {
        result = std::make_shared<CrossProductTableRef>();
        result->left_bind_context_ = left_bind_context_ptr;
        result->left_table_ref_ = left_bound_table_ref;

        hsql::TableRef* right_table_ptr = tables[idx];
        std::shared_ptr<BindContext> right_bind_context_ptr = std::make_shared<BindContext>(bind_context_ptr);
        AddBindContextArray(right_bind_context_ptr);
        std::shared_ptr<TableRef> right_bound_table_ref = BuildFromClause(right_table_ptr, right_bind_context_ptr);

        result->right_bind_context_ = right_bind_context_ptr;
        result->right_table_ref_ = right_bound_table_ref;

        std::shared_ptr<BindContext> cross_product_bind_context_ptr = std::make_shared<BindContext>(bind_context_ptr);
        AddBindContextArray(cross_product_bind_context_ptr);
        cross_product_bind_context_ptr->AddBindContext(left_bind_context_ptr);
        cross_product_bind_context_ptr->AddBindContext(right_bind_context_ptr);

        left_bind_context_ptr = cross_product_bind_context_ptr;
        left_bound_table_ref = result;
    }

    result = std::make_shared<CrossProductTableRef>();
    result->left_bind_context_ = left_bind_context_ptr;
    result->left_table_ref_ = left_bound_table_ref;

    hsql::TableRef* right_table_ptr = tables.back();
    std::shared_ptr<BindContext> right_bind_context_ptr = std::make_shared<BindContext>(bind_context_ptr);
    AddBindContextArray(right_bind_context_ptr);
    std::shared_ptr<TableRef> right_bound_table_ref = BuildFromClause(right_table_ptr, right_bind_context_ptr);

    result->right_bind_context_ = right_bind_context_ptr;
    result->right_table_ref_ = right_bound_table_ref;

    bind_context_ptr->AddBindContext(left_bind_context_ptr);
    bind_context_ptr->AddBindContext(right_bind_context_ptr);

    // TODO: Not care about the correlated expression

    return result;
}

std::shared_ptr<TableRef>
PlanBuilder::BuildJoin(const hsql::TableRef *from_table, std::shared_ptr<BindContext> &bind_context_ptr) {
    auto result = std::make_shared<JoinTableRef>();

    switch (from_table->join->type) {
        case hsql::JoinType::kJoinCross: result->join_type_ = JoinType::kCross; break;
        case hsql::JoinType::kJoinFull: result->join_type_ = JoinType::kFull; break;
        case hsql::JoinType::kJoinInner: result->join_type_ = JoinType::kInner; break;
        case hsql::JoinType::kJoinNatural: result->join_type_ = JoinType::kNatural; break;
        case hsql::JoinType::kJoinLeft: result->join_type_ = JoinType::kLeft; break;
        case hsql::JoinType::kJoinRight: result->join_type_ = JoinType::kRight; break;
        default:
            PlannerError("Unsupported join type.")
    }

    // Build left child
    std::shared_ptr<BindContext> left_bind_context_ptr = std::make_shared<BindContext>(bind_context_ptr);
    AddBindContextArray(left_bind_context_ptr);
    std::shared_ptr<TableRef> left_bound_table_ref = BuildFromClause(from_table->join->left, left_bind_context_ptr);

    // Build right child
    std::shared_ptr<BindContext> right_bind_context_ptr = std::make_shared<BindContext>(bind_context_ptr);
    AddBindContextArray(right_bind_context_ptr);
    std::shared_ptr<TableRef> right_bound_table_ref = BuildFromClause(from_table->join->right, right_bind_context_ptr);

    // Merge left/right bind context into current bind context
    bind_context_ptr->AddBindContext(left_bind_context_ptr);
    bind_context_ptr->AddBindContext(right_bind_context_ptr);

    // Current parser doesn't support On using column syntax, so only consider the case of natural join.
    if(result->join_type_ == JoinType::kNatural) {
        std::unordered_set<std::string> left_binding_column_names;
        // TODO: Is there any way to get all_column_names size ? Collect all left binding columns numbers at very beginning?
        for(auto& left_binding: left_bind_context_ptr->bindings_) {
            for(auto& left_column_name: left_binding->column_names_) {
                left_binding_column_names.emplace(left_column_name);
            }
        }

        std::vector<std::string> using_column_names;
        // TODO: column count of left binding tables and right binding tables is using_column_names size
        for(auto& right_binding: right_bind_context_ptr->bindings_) {
            for(auto& right_column_name: right_binding->column_names_) {
                if(left_binding_column_names.contains(right_column_name)) {
                    using_column_names.emplace_back(right_column_name);
                }
            }
        }

        if(using_column_names.empty()) {
            // It is cross product, but not a natural join
            auto cross_product_table_ref = std::make_shared<CrossProductTableRef>();
            cross_product_table_ref->left_bind_context_ = left_bind_context_ptr;
            cross_product_table_ref->left_table_ref_ = left_bound_table_ref;

            cross_product_table_ref->right_bind_context_ = right_bind_context_ptr;
            cross_product_table_ref->right_table_ref_ = right_bound_table_ref;
            return cross_product_table_ref;
        } else {
            // It is a natural join, we only consider the inner natural join case.
            result->join_type_ = JoinType::kInner;

            // Reserve the on condition space for the expression.
            result->on_conditions_.reserve(using_column_names.size());

            // TODO: Construct join condition: left_column_expr = right_column_expr AND left_column_expr = right_column_expr;
            for(auto& column_name: using_column_names) {
                // Create left bound column expression
                auto& left_binding_ptr = left_bind_context_ptr->bindings_by_column_[column_name];
                PlannerAssert(left_binding_ptr != nullptr, "Column: " + column_name + " doesn't exist in left table");
                auto left_column_index = left_binding_ptr->name2index_[column_name];
                auto left_column_type = left_binding_ptr->column_types_[left_column_index];

                std::shared_ptr<ColumnExpression> left_column_expression_ptr =
                        std::make_shared<ColumnExpression>(left_column_type, left_binding_ptr->table_name_, left_binding_ptr->table_index_, column_name, left_column_index, 0);

                auto& right_binding_ptr = right_bind_context_ptr->bindings_by_column_[column_name];
                PlannerAssert(right_binding_ptr != nullptr, "Column: " + column_name + " doesn't exist in right table");
                auto right_column_index = right_binding_ptr->name2index_[column_name];
                auto right_column_type = right_binding_ptr->column_types_[right_column_index];

                std::shared_ptr<ColumnExpression> right_column_expression_ptr =
                        std::make_shared<ColumnExpression>(right_column_type, right_binding_ptr->table_name_, right_binding_ptr->table_index_, column_name, right_column_index, 0);

                auto condition = std::make_shared<ConjunctionExpression>(ExpressionType::kConjunction, ConjunctionType::kAnd, left_column_expression_ptr, right_column_expression_ptr);
                result->on_conditions_.emplace_back(condition);

                // For natural join, we can return now.
                return result;
            }
        }
    }

    // Inner / Full / Left / Right join
    // TODO: Bind all join condition with where expression binder.

    // std::shared_ptr<JoinBinder> join_binder
    auto join_binder = std::make_shared<JoinBinder>();

    // std::shared_ptr<BaseExpression> on_condition_ptr
    auto on_condition_ptr = join_binder->BuildExpression(*from_table->join->condition, bind_context_ptr);
//    std::shared_ptr<BaseExpression> where_expr =
//            bind_context_ptr->binder_->BuildExpression(*whereClause, bind_context_ptr);
//
//    std::shared_ptr<BaseExpression> condition_ptr =
//    from_table->join->condition
//    for(hsql::Expr* expr: from_table->join->condition) {
//
//    }
    result->on_conditions_ = SplitExpressionByDelimiter(on_condition_ptr, ConjunctionType::kAnd);

    return result;
}


}
