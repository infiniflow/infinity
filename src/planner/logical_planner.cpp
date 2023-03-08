//
// Created by jinhai on 23-1-17.
//

#include "logical_planner.h"
#include "main/infinity.h"
#include "query_binder.h"
#include "planner/node/logical_create_table.h"
#include "planner/node/logical_drop_table.h"
#include "planner/node/logical_drop_schema.h"
#include "planner/binder/insert_binder.h"
#include "expression/cast_expression.h"
#include "planner/node/logical_insert.h"
#include "planner/node/logical_chunk_scan.h"
#include "common/types/info/varchar_info.h"
#include "planner/node/logical_drop_index.h"
#include "planner/node/logical_drop_view.h"
#include "planner/node/logical_drop_collection.h"
#include "planner/node/logical_create_collection.h"
#include "planner/node/logical_create_schema.h"
#include "planner/node/logical_create_view.h"
#include "function/cast/cast_function.h"

namespace infinity {

void
LogicalPlanner::Build(const BaseStatement* statement) {
    switch (statement->Type()) {
        case StatementType::kSelect: {
            BuildSelect(static_cast<const SelectStatement*>(statement));
            break;
        }
        case StatementType::kInsert: {
            BuildInsert(static_cast<const InsertStatement*>(statement));
            break;
        }
        case StatementType::kUpdate: {
            BuildUpdate(static_cast<const UpdateStatement*>(statement));
            break;
        }
        case StatementType::kDelete: {
            BuildDelete(static_cast<const DeleteStatement*>(statement));
            break;
        }
        case StatementType::kCreate: {
            BuildCreate(static_cast<const CreateStatement*>(statement));
            break;
        }
        case StatementType::kDrop: {
            BuildDrop(static_cast<const DropStatement*>(statement));
            break;
        }
        case StatementType::kShow: {
            BuildShow(static_cast<const ShowStatement*>(statement));
            break;
        }
        case StatementType::kCopy: {
            BuildCopy(static_cast<const CopyStatement*>(statement));
            break;
        }
        case StatementType::kExplain: {
            BuildExplain(static_cast<const ExplainStatement*>(statement));
            break;
        }
        case StatementType::kPrepare: {
            BuildPrepare(static_cast<const PrepareStatement*>(statement));
            break;
        }
        case StatementType::kExecute: {
            BuildExecute(static_cast<const ExecuteStatement*>(statement));
            break;
        }
        case StatementType::kAlter: {
            BuildAlter(static_cast<const AlterStatement*>(statement));
            break;
        }
        case StatementType::kInvalidStmt: {
            PlannerError("Invalid statement type.");
            break;
        }
    }
}

void
LogicalPlanner::BuildSelect(const SelectStatement* statement) {
    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);
    SharedPtr<QueryBinder> query_binder_ptr = MakeShared<QueryBinder>(this->query_context_ptr_,
                                                                      bind_context_ptr);
    SharedPtr<BoundSelectStatement> bound_statement_ptr = query_binder_ptr->BindSelect(*statement);
    this->logical_plan_ = bound_statement_ptr->BuildPlan(bind_context_ptr);
}

void
LogicalPlanner::BuildInsert(const InsertStatement* statement) {
    if(statement->select_ == nullptr) {
        return BuildInsertValue(statement);
    } else {
        return BuildInsertSelect(statement);
    }
}

void
LogicalPlanner::BuildInsertValue(const InsertStatement* statement) {
    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);
    bind_context_ptr->expression_binder_ = MakeShared<InsertBinder>(query_context_ptr_);

    // Get schema name
    const String& schema_name = statement->schema_name_;
    const String& table_name = statement->table_name_;

    // Get table name
    if(table_name.empty()) {
        PlannerError("Insert statement missing table name.");
    }
    // Check schema and table in the catalog
    SharedPtr<Table> table_ptr = Infinity::instance().catalog()->GetTableByName(schema_name,
                                                                                table_name);
    if(table_ptr == nullptr) { PlannerError(schema_name + "." + table_name + " not exists.")}

    // Create value list
    Vector<SharedPtr<BaseExpression>> value_list;
    value_list.reserve(statement->values_->size());
    for (const auto* parsed_expr : *statement->values_) {
        SharedPtr<BaseExpression> value_expr
                = bind_context_ptr->expression_binder_->BuildExpression(*parsed_expr,
                                                                        bind_context_ptr.get(),
                                                                        0,
                                                                        true);
        value_list.emplace_back(value_expr);
    }

    // Rearrange the inserted value to match the table.
    // SELECT INTO TABLE (c, a) VALUES (1, 2); => SELECT INTO TABLE (a, b, c) VALUES( 2, NULL, 1);
    if (statement->columns_ != nullptr) {
        SizeT statement_column_count = statement->columns_->size();
        PlannerAssert(statement_column_count == value_list.size(),
                      "INSERT: Target column count and input column count mismatch");

//        Value null_value = Value::MakeNullData();
//        SharedPtr<BaseExpression> null_value_expr = MakeShared<ValueExpression>(null_value);

        SizeT table_column_count = table_ptr->ColumnCount();

        // Create value list with table column size and null value
        Vector<SharedPtr<BaseExpression>> rewrite_value_list(table_column_count, nullptr);

        SizeT column_idx = 0;
        for(const auto& column_name : *statement->columns_) {
            // Get table index from the inserted value column name;
            SizeT table_column_id = table_ptr->GetColumnIdByName(column_name);
            DataType table_column_type = table_ptr->GetColumnTypeById(table_column_id);
            DataType value_type = value_list[column_idx]->Type();
            if(value_type == table_column_type) {
                rewrite_value_list[table_column_id] = value_list[column_idx];
            } else {
                if(LogicalInsert::NeedCastInInsert(value_type, table_column_type)) {
                    // If the inserted value type mismatches with table column type, cast the inserted value type to correct one.
                    BoundCastFunc cast = CastFunction::GetBoundFunc(value_type, table_column_type);
                    SharedPtr<BaseExpression> cast_expr = MakeShared<CastExpression>(cast,
                                                                                     value_list[column_idx],
                                                                                     table_column_type);
                    rewrite_value_list[table_column_id] = cast_expr;
                } else {
                    // LogicalType are same and type info is also OK.
                    rewrite_value_list[column_idx] = value_list[column_idx];
                }
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
                if(LogicalInsert::NeedCastInInsert(value_type, table_column_type)) {
                    BoundCastFunc cast = CastFunction::GetBoundFunc(value_type, table_column_type);
                    SharedPtr<BaseExpression> cast_expr = MakeShared<CastExpression>(cast,
                                                                                     value_list[column_idx],
                                                                                     table_column_type);
                    rewrite_value_list[column_idx] = cast_expr;
                } else {
                    // LogicalType are same and type info is also OK.
                    rewrite_value_list[column_idx] = value_list[column_idx];
                }
            }
        }
        value_list = rewrite_value_list;
    }

    // Create logical insert node.
    SharedPtr<LogicalNode> logical_insert = MakeShared<LogicalInsert>(bind_context_ptr->GetNewLogicalNodeId(),
                                                                      table_ptr,
                                                                      bind_context_ptr->GenerateTableIndex(),
                                                                      value_list);

    // FIXME: check if we need to append operator
//    this->AppendOperator(logical_insert, bind_context_ptr);

    this->logical_plan_ = logical_insert;
}

void
LogicalPlanner::BuildInsertSelect(const InsertStatement* statement) {
    PlannerError("Inserting select isn't supported.");
}

void
LogicalPlanner::BuildUpdate(const UpdateStatement* statement) {
    PlannerError("Update isn't supported.");
}

void
LogicalPlanner::BuildDelete(const DeleteStatement* statement) {
    PlannerError("Delete isn't supported.");
}

void
LogicalPlanner::BuildCreate(const CreateStatement* statement) {
    switch (statement->ddl_type()) {
        case DDLType::kTable: {
            BuildCreateTable(statement);
            break;
        }
        case DDLType::kCollection: {
            BuildCreateCollection(statement);
            break;
        }
        case DDLType::kView: {
            BuildCreateView(statement);
            break;
        }
        case DDLType::kIndex: {
            BuildCreateIndex(statement);
            break;
        }
        case DDLType::kSchema: {
            BuildCreateSchema(statement);
            break;
        }
        default: {
            PlannerError("Creating type isn't supported");
        }
    }
}

void
LogicalPlanner::BuildCreateTable(const CreateStatement* statement) {
    auto* create_table_info = (CreateTableInfo*)statement->create_info_.get();

    // Check if columns is given.
    Vector<SharedPtr<ColumnDef>> columns;
    SizeT column_count = create_table_info->column_defs_.size();
    columns.reserve(column_count);
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        SharedPtr<ColumnDef> column_def = MakeShared<ColumnDef>(create_table_info->column_defs_[idx]->id(),
                                                                create_table_info->column_defs_[idx]->type(),
                                                                create_table_info->column_defs_[idx]->name(),
                                                                create_table_info->column_defs_[idx]->constraints_);
        columns.emplace_back(column_def);
    }

    SharedPtr<String> schema_name_ptr = MakeShared<String>(create_table_info->schema_name_);

    SharedPtr<TableDef> table_def_ptr
            = MakeShared<TableDef>(create_table_info->table_name_, columns);

    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);
    SharedPtr<LogicalNode> logical_create_table_operator
            = LogicalCreateTable::Make(bind_context_ptr->GetNewLogicalNodeId(),
                                       schema_name_ptr,
                                       table_def_ptr,
                                       bind_context_ptr->GenerateTableIndex(),
                                       create_table_info->conflict_type_);

    if(create_table_info->select_ != nullptr) {
        SharedPtr<BindContext> select_bind_context_ptr = BindContext::Make(nullptr);
        QueryBinder select_query_binder(this->query_context_ptr_, select_bind_context_ptr);
        SharedPtr<BoundSelectStatement> bound_statement_ptr = select_query_binder.BindSelect(*create_table_info->select_);
        logical_create_table_operator->set_left_node(bound_statement_ptr->BuildPlan(bind_context_ptr));
    }

    this->logical_plan_ = logical_create_table_operator;
    this->names_ptr_->emplace_back(String("OK"));
    this->types_ptr_->emplace_back(DataType(LogicalType::kInteger));
}

void
LogicalPlanner::BuildCreateCollection(const CreateStatement* statement) {
    auto* create_collection_info = (CreateCollectionInfo*)statement->create_info_.get();

    SharedPtr<String> schema_name_ptr = MakeShared<String>(create_collection_info->schema_name_);
    SharedPtr<String> collection_name_ptr = MakeShared<String>(create_collection_info->collection_name_);

    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);
    SharedPtr<LogicalNode> logical_create_collection_operator
            = LogicalCreateCollection::Make(bind_context_ptr->GetNewLogicalNodeId(),
                                            schema_name_ptr,
                                            collection_name_ptr,
                                            bind_context_ptr->GenerateTableIndex(),
                                            create_collection_info->conflict_type_);

    this->logical_plan_ = logical_create_collection_operator;
    this->names_ptr_->emplace_back(String("OK"));
    this->types_ptr_->emplace_back(DataType(LogicalType::kInteger));
}

void
LogicalPlanner::BuildCreateSchema(const CreateStatement* statement) {
    auto* create_schema_info = (CreateSchemaInfo*)statement->create_info_.get();

    SharedPtr<String> schema_name_ptr = MakeShared<String>(create_schema_info->schema_name_);

    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);
    SharedPtr<LogicalNode> logical_create_schema_operator
            = LogicalCreateSchema::Make(bind_context_ptr->GetNewLogicalNodeId(),
                                        schema_name_ptr,
                                        create_schema_info->conflict_type_);

    this->logical_plan_ = logical_create_schema_operator;
    this->names_ptr_->emplace_back(String("OK"));
    this->types_ptr_->emplace_back(DataType(LogicalType::kInteger));
}

void
LogicalPlanner::BuildCreateView(const CreateStatement* statement) {
    auto* create_view_info = (CreateViewInfo*)statement->create_info_.get();
    SharedPtr<String> schema_name_ptr = MakeShared<String>(create_view_info->schema_name_);
    SharedPtr<String> view_name_ptr = MakeShared<String>(create_view_info->view_name_);

    // Check if columns is given.
    Vector<SharedPtr<String>> columns;
    SizeT column_count = create_view_info->view_columns_->size();
    columns.reserve(column_count);
    for(SizeT idx = 0; idx < column_count; ++ idx) {
        SharedPtr<String> column_name = MakeShared<String>((*(create_view_info->view_columns_))[idx]);
        columns.emplace_back(column_name);
    }


    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);
    SharedPtr<LogicalNode> logical_create_view_operator
            = LogicalCreateView::Make(bind_context_ptr->GetNewLogicalNodeId(),
                                            schema_name_ptr,
                                            view_name_ptr,
                                            columns,
                                            create_view_info->conflict_type_);

    this->logical_plan_ = logical_create_view_operator;
    this->names_ptr_->emplace_back(String("OK"));
    this->types_ptr_->emplace_back(DataType(LogicalType::kInteger));
}

void
LogicalPlanner::BuildCreateIndex(const CreateStatement* statement) {
    PlannerError("Creating index isn't supported.");
}

void
LogicalPlanner::BuildDrop(const DropStatement* statement) {
    switch(statement->ddl_type()) {
        case DDLType::kTable: {
            BuildDropTable(statement);
            break;
        }
        case DDLType::kCollection: {
            BuildDropCollection(statement);
            break;
        }
        case DDLType::kSchema: {
            BuildDropSchema(statement);
            break;
        }
        case DDLType::kIndex: {
            BuildDropIndex(statement);
            break;
        }
        case DDLType::kView: {
            BuildDropView(statement);
            break;
        }
        case DDLType::kInvalid: {
            PlannerError("Invalid drop statement type.");
        }
    }
}

void
LogicalPlanner::BuildDropTable(const DropStatement* statement) {
    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);
    auto* drop_table_info = (DropTableInfo*)statement->drop_info_.get();

    SharedPtr<String> schema_name_ptr{nullptr};
    if(drop_table_info->schema_name_.empty()) {
        schema_name_ptr = MakeShared<String>("Default");
    } else{
        schema_name_ptr = MakeShared<String>(drop_table_info->schema_name_);
    }

    SharedPtr<String> table_name_ptr = MakeShared<String>(drop_table_info->table_name_);
    SharedPtr<LogicalNode> logical_drop_table
            = MakeShared<LogicalDropTable>(bind_context_ptr->GetNewLogicalNodeId(),
                                           schema_name_ptr,
                                           table_name_ptr);

    this->logical_plan_ = logical_drop_table;
    this->names_ptr_->emplace_back(String("OK"));
    this->types_ptr_->emplace_back(DataType(LogicalType::kInteger));
}

void
LogicalPlanner::BuildDropCollection(const DropStatement* statement) {
    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);
    auto* drop_collection_info = (DropCollectionInfo*)statement->drop_info_.get();

    SharedPtr<String> schema_name_ptr{nullptr};
    if(drop_collection_info->schema_name_.empty()) {
        schema_name_ptr = MakeShared<String>("Default");
    } else{
        schema_name_ptr = MakeShared<String>(drop_collection_info->schema_name_);
    }

    SharedPtr<LogicalNode> logical_drop_collection
            = MakeShared<LogicalDropCollection>(bind_context_ptr->GetNewLogicalNodeId(),
                                                schema_name_ptr,
                                                MakeShared<String>(drop_collection_info->collection_name_));

    this->logical_plan_ = logical_drop_collection;
    this->names_ptr_->emplace_back(String("OK"));
    this->types_ptr_->emplace_back(DataType(LogicalType::kInteger));
}

void
LogicalPlanner::BuildDropSchema(const DropStatement* statement) {
    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);
    auto* drop_schema_info = (DropSchemaInfo*)statement->drop_info_.get();

    SharedPtr<String> schema_name_ptr = MakeShared<String>(drop_schema_info->schema_name_);

    SharedPtr<LogicalNode> logical_drop_schema
            = MakeShared<LogicalDropSchema>(bind_context_ptr->GetNewLogicalNodeId(),
                                           schema_name_ptr);

    this->logical_plan_ = logical_drop_schema;
    this->names_ptr_->emplace_back(String("OK"));
    this->types_ptr_->emplace_back(DataType(LogicalType::kInteger));
}

void
LogicalPlanner::BuildDropIndex(const DropStatement* statement) {
    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);
    auto* drop_index_info = (DropIndexInfo*)statement->drop_info_.get();

    SharedPtr<String> schema_name_ptr = MakeShared<String>(drop_index_info->schema_name_);
    SharedPtr<String> index_name_ptr = MakeShared<String>(drop_index_info->index_name_);

    SharedPtr<LogicalNode> logical_drop_index
            = MakeShared<LogicalDropIndex>(bind_context_ptr->GetNewLogicalNodeId(),
                                           schema_name_ptr,
                                           index_name_ptr);

    this->logical_plan_ = logical_drop_index;
    this->names_ptr_->emplace_back(String("OK"));
    this->types_ptr_->emplace_back(DataType(LogicalType::kInteger));
}

void
LogicalPlanner::BuildDropView(const DropStatement* statement) {
    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);
    auto* drop_view_info = (DropViewInfo*)statement->drop_info_.get();

    SharedPtr<String> schema_name_ptr = MakeShared<String>(drop_view_info->schema_name_);
    SharedPtr<String> view_name_ptr = MakeShared<String>(drop_view_info->view_name_);

    SharedPtr<LogicalNode> logical_drop_view
            = MakeShared<LogicalDropView>(bind_context_ptr->GetNewLogicalNodeId(),
                                          schema_name_ptr,
                                          view_name_ptr);

    this->logical_plan_ = logical_drop_view;
    this->names_ptr_->emplace_back(String("OK"));
    this->types_ptr_->emplace_back(DataType(LogicalType::kInteger));
}

void
LogicalPlanner::BuildPrepare(const PrepareStatement* statement) {
    PlannerError("Prepare statement isn't supported.");
}

void
LogicalPlanner::BuildExecute(const ExecuteStatement* statement) {
    PlannerError("Execute statement isn't supported.");
}

void
LogicalPlanner::BuildCopy(const CopyStatement* statement) {
    if(statement->copy_from_) {
        // Import
        BuildImport(statement);
    } else {
        // Export
        BuildExport(statement);
    }
}

void
LogicalPlanner::BuildExport(const CopyStatement* statement) {
    switch(statement->copy_file_type_) {
        case CopyFileType::kCSV:
            return BuildExportCsv(statement);
        default: {
            PlannerError("Export data to unsupported file type.")
        }
    }
}

void
LogicalPlanner::BuildExportCsv(const CopyStatement* statement) {
    PlannerError("Exporting to CSV file isn't supported.");
}

void
LogicalPlanner::BuildImport(const CopyStatement* statement) {
    switch(statement->copy_file_type_) {
        case CopyFileType::kCSV:
            return BuildImportCsv(statement);
        default: {
            PlannerError("Export data to unsupported file type.")
        }
    }
}

void
LogicalPlanner::BuildImportCsv(const CopyStatement* statement) {
    PlannerError("Import from CSV file isn't supported.");
}

void
LogicalPlanner::BuildAlter(const AlterStatement* statement) {
    NotImplementError("BuildAlter")
}

void
LogicalPlanner::BuildShow(const ShowStatement* statement) {
    switch(statement->show_type_) {
        case ShowStmtType::kTables : {
            return BuildShowTables(statement);
        }
        case ShowStmtType::kColumns : {
            return BuildShowColumns(statement);
        }
        default:
            PlannerError("Unexpected show statement type.");
    }
}

void
LogicalPlanner::BuildShowColumns(const ShowStatement* statement) {
    PlannerError("Show columns isn't supported.");
}

void
LogicalPlanner::BuildShowTables(const ShowStatement* statement) {
    SharedPtr<BindContext> bind_context_ptr = BindContext::Make(nullptr);

    SharedPtr<LogicalNode> logical_chunk_scan =
            MakeShared<LogicalChunkScan>(bind_context_ptr->GetNewLogicalNodeId(),
                                         ChunkScanType::kShowTables,
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
LogicalPlanner::BuildExplain(const ExplainStatement* statement) {
    NotImplementError("Build explain statement plan.");
}

}
