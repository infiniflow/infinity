// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

module infinity_core:explain_logical_plan.impl;

import :explain_logical_plan;
import :logical_node;
import :logical_create_schema;
import :logical_create_table;
import :logical_create_index;
import :logical_create_collection;
import :logical_create_view;
import :logical_drop_schema;
import :logical_drop_table;
import :logical_drop_collection;
import :logical_drop_view;
import :logical_insert;
import :logical_delete;
import :logical_update;
import :logical_project;
import :logical_filter;
import :logical_table_scan;
import :logical_index_scan;
import :logical_knn_scan;
import :logical_aggregate;
import :logical_sort;
import :logical_limit;
import :logical_top;
import :logical_cross_product;
import :logical_join;
import :logical_show;
import :logical_import;
import :logical_export;
import :logical_flush;
import :logical_optimize;
import :logical_match;
import :logical_match_tensor_scan;
import :logical_match_sparse_scan;
import :logical_fusion;
import :logical_unnest;
import :logical_unnest_aggregate;
import :logical_node_type;
import :logical_check;
import :base_expression;
import :expression_type;
import :knn_expression;
import :aggregate_expression;
import :cast_expression;
import :case_expression;
import :column_expression;
import :function_expression;
import :between_expression;
import :in_expression;
import :value_expression;
import :reference_expression;
import :infinity_exception;

import third_party;

import internal_types;
import knn_expr;
import select_statement;
import extra_ddl_info;
import column_def;
import join_reference;
import statement_common;
import flush_statement;
import optimize_statement;
import show_statement;
import check_statement;

namespace infinity {

Status
ExplainLogicalPlan::Explain(const LogicalNode *statement, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    switch (statement->operator_type()) {
        case LogicalNodeType::kAggregate: {
            Explain((LogicalAggregate *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kExcept:
            break;
        case LogicalNodeType::kUnion:
            break;
        case LogicalNodeType::kIntersect:
            break;
        case LogicalNodeType::kJoin: {
            Explain((LogicalJoin *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCrossProduct: {
            Explain((LogicalCrossProduct *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kLimit: {
            Explain((LogicalLimit *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kFilter: {
            Explain((LogicalFilter *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kUnnest: {
            Explain((LogicalUnnest *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kUnnestAggregate: {
            Explain((LogicalUnnestAggregate *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kProjection: {
            Explain((LogicalProject *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kSort: {
            Explain((LogicalSort *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kTop: {
            Explain((LogicalTop *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDelete:
            Explain((LogicalDelete *)statement, result, intent_size);
            break;
        case LogicalNodeType::kUpdate:
            Explain((LogicalUpdate *)statement, result, intent_size);
            break;
        case LogicalNodeType::kInsert: {
            Explain((LogicalInsert *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kImport: {
            Explain((LogicalImport *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kExport: {
            Explain((LogicalExport *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kAlter:
            break;
        case LogicalNodeType::kCreateTable: {
            Explain((LogicalCreateTable *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCreateIndex: {
            Explain((LogicalCreateIndex *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCreateCollection: {
            Explain((LogicalCreateCollection *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCreateSchema: {
            Explain((LogicalCreateSchema *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kCreateView: {
            Explain((LogicalCreateView *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDropTable: {
            Explain((LogicalDropTable *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDropCollection: {
            Explain((LogicalDropCollection *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDropSchema: {
            Explain((LogicalDropSchema *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kDropView: {
            Explain((LogicalDropView *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kFlush: {
            Explain((LogicalFlush *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kOptimize: {
            Explain((LogicalOptimize *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kShow: {
            Explain((LogicalShow *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kTableScan: {
            Explain((LogicalTableScan *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kIndexScan: {
            Explain((LogicalIndexScan *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kKnnScan: {
            Explain((LogicalKnnScan *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kMatchSparseScan: {
            Explain((LogicalMatchSparseScan *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kMatch: {
            Explain((LogicalMatch *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kMatchTensorScan: {
            Explain((LogicalMatchTensorScan *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kFusion: {
            Explain((LogicalFusion *)statement, result, intent_size);
            break;
        }
        case LogicalNodeType::kViewScan:
            break;
        case LogicalNodeType::kDummyScan:
            break;
        case LogicalNodeType::kExplain:
            break;
        case LogicalNodeType::kPrepare:
            break;
        case LogicalNodeType::kCheck: {
            Explain((LogicalCheck *)statement, result, intent_size);
            break;
        }
        default: {
            UnrecoverableError("Unexpected logical node type");
        }
    }
    if (statement->left_node().get() != nullptr) {
        ExplainLogicalPlan::Explain(statement->left_node().get(), result, intent_size + 2);
    }
    if (statement->right_node().get() != nullptr) {
        ExplainLogicalPlan::Explain(statement->right_node().get(), result, intent_size + 2);
    }
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const LogicalCreateSchema *create_node,
                                   std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                   i64 intent_size) {
    {
        std::string create_header_str;
        if (intent_size != 0) {
            create_header_str = fmt::format("{}-> CREATE SCHEMA ", std::string(intent_size - 2, ' '));
        } else {
            create_header_str = "CREATE SCHEMA ";
        }

        create_header_str += fmt::format("({})", create_node->node_id());
        result->emplace_back(std::make_shared<std::string>(create_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = fmt::format("{} - database name: {}", std::string(intent_size, ' '), *create_node->schema_name());
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // Conflict type
    {
        std::string conflict_type_str =
            fmt::format("{} - conflict type: {}", std::string(intent_size, ' '), ConflictTypeToStr(create_node->conflict_type()));
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    // Output column
    {
        std::string output_columns_str = fmt::format("{} - output columns: [OK]", std::string(intent_size, ' '));
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const LogicalCreateTable *create_node,
                                   std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                   i64 intent_size) {
    {
        std::string create_header_str;
        if (intent_size != 0) {
            create_header_str = fmt::format("{}-> CREATE TABLE ", std::string(intent_size - 2, ' '));
        } else {
            create_header_str = "CREATE TABLE ";
        }

        create_header_str += fmt::format("({})", create_node->node_id());
        result->emplace_back(std::make_shared<std::string>(create_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = fmt::format("{} - database name: {}", std::string(intent_size, ' '), *create_node->schema_name());
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // Table name
    {
        std::string table_name_str =
            fmt::format("{} - table name: {}", std::string(intent_size, ' '), *create_node->table_definitions()->table_name());
        result->emplace_back(std::make_shared<std::string>(table_name_str));
    }

    // Column definition
    {
        size_t column_count = create_node->table_definitions()->column_count();
        if (column_count == 0) {
            UnrecoverableError("No columns in the table");
        }
        const std::vector<std::shared_ptr<ColumnDef>> &columns = create_node->table_definitions()->columns();

        std::string columns_str;
        columns_str.append(std::string(intent_size, ' ')).append(" - columns: [");
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            columns_str.append(columns[idx]->ToString()).append(", ");
        }
        columns_str.append(columns.back()->ToString()).append("]");
        result->emplace_back(std::make_shared<std::string>(columns_str));
    }

    // Conflict type
    {
        std::string conflict_type_str =
            fmt::format("{} - conflict type: {}", std::string(intent_size, ' '), ConflictTypeToStr(create_node->conflict_type()));
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    // Output column
    {
        std::string output_columns_str = fmt::format("{} - output columns: [OK]", std::string(intent_size, ' '));
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const LogicalCreateIndex *create_node,
                                   std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                   i64 intent_size) {
    {
        std::string create_header_str;
        if (intent_size != 0) {
            create_header_str = std::string(intent_size - 2, ' ') + "-> CREATE INDEX ";
        } else {
            create_header_str = "CREATE INDEX ";
        }

        create_header_str += "(" + std::to_string(create_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(create_header_str));
    }

    {
        std::string schema_name_str = std::string(intent_size, ' ') + " - database name: " + *create_node->base_table_ref()->db_name();
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    {
        std::string table_name_str = std::string(intent_size, ' ') + " - table name: " + *create_node->base_table_ref()->table_name();
        result->emplace_back(std::make_shared<std::string>(table_name_str));
    }

    // Index definition
    {
        std::string index_def_str = std::string(intent_size, ' ') + " - index definition: " + create_node->index_definition()->ToString();
        result->emplace_back(std::make_shared<std::string>(index_def_str));
    }

    {
        std::string conflict_type_str = std::string(intent_size, ' ') + " - conflict type: " + ConflictTypeToStr(create_node->conflict_type());
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [OK]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const LogicalCreateCollection *create_node,
                                   std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                   i64 intent_size) {
    {
        std::string create_header_str;
        if (intent_size != 0) {
            create_header_str = fmt::format("{}-> CREATE COLLECTION ", std::string(intent_size - 2, ' '));
        } else {
            create_header_str = "CREATE COLLECTION ";
        }

        create_header_str += fmt::format("({})", create_node->node_id());
        result->emplace_back(std::make_shared<std::string>(create_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = fmt::format("{} - database name: {}", std::string(intent_size, ' '), *create_node->schema_name());
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // Collection name
    {
        std::string collection_name_str = fmt::format("{} - collection name: {}", std::string(intent_size, ' '), *create_node->collection_name());
        result->emplace_back(std::make_shared<std::string>(collection_name_str));
    }

    // Conflict type
    {
        std::string conflict_type_str =
            fmt::format("{} - conflict type: {}", std::string(intent_size, ' '), ConflictTypeToStr(create_node->conflict_type()));
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    // Output column
    {
        std::string output_columns_str = fmt::format("{} - output columns: [OK]", std::string(intent_size, ' '));
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const LogicalCreateView *create_node,
                                   std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                   i64 intent_size) {
    {
        std::string create_header_str;
        if (intent_size != 0) {
            create_header_str = fmt::format("{}-> CREATE VIEW ", std::string(intent_size - 2, ' '));
        } else {
            create_header_str = "CREATE VIEW ";
        }
        create_header_str += fmt::format("({})", create_node->node_id());
        result->emplace_back(std::make_shared<std::string>(create_header_str));
    }

    // Schema name
    {
        std::string schema_name_str =
            fmt::format("{} - database name: {}", std::string(intent_size, ' '), create_node->create_view_info()->schema_name_);
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // View name
    {
        std::string view_name_str = fmt::format("{} - view name: {}", std::string(intent_size, ' '), create_node->create_view_info()->view_name_);
        result->emplace_back(std::make_shared<std::string>(view_name_str));
    }

    // Column definition
    {
        size_t column_count = create_node->names_ptr()->size();
        if (column_count == 0) {
            UnrecoverableError("No columns in the table");
        }
        std::string columns_str;
        columns_str.append(std::string(intent_size, ' ')).append(" - columns: [");
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            columns_str.append(create_node->names_ptr()->at(idx)).append(" ").append(create_node->types_ptr()->at(idx)->ToString()).append(", ");
        }
        columns_str.append(create_node->names_ptr()->back()).append(" ").append(create_node->types_ptr()->back()->ToString()).append("]");
        result->emplace_back(std::make_shared<std::string>(columns_str));

        result->emplace_back(std::make_shared<std::string>(columns_str));
    }

    // Conflict type
    {
        std::string conflict_type_str =
            fmt::format("{} - conflict type: {}", std::string(intent_size, ' '), ConflictTypeToStr(create_node->create_view_info()->conflict_type_));
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    // Text
    {
        std::string sql_text = fmt::format("{} - text: Not implemented", std::string(intent_size, ' '));
        result->emplace_back(std::make_shared<std::string>(sql_text));
    }

    // Output column
    {
        std::string output_columns_str = fmt::format("{} - output columns: [OK]", std::string(intent_size, ' '));
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalDropSchema *drop_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string drop_header_str;
        if (intent_size != 0) {
            drop_header_str = fmt::format("{}-> CREATE SCHEMA ", std::string(intent_size - 2, ' '));
        } else {
            drop_header_str = "DROP SCHEMA ";
        }

        drop_header_str += fmt::format("({})", drop_node->node_id());
        result->emplace_back(std::make_shared<std::string>(drop_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = fmt::format("{} - database name: {}", std::string(intent_size, ' '), *drop_node->schema_name());
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // Conflict type
    {
        std::string conflict_type_str =
            fmt::format("{} - conflict type: {}", std::string(intent_size, ' '), ConflictTypeToStr(drop_node->conflict_type()));
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    // Output column
    {
        std::string output_columns_str = fmt::format("{} - output columns: [OK]", std::string(intent_size, ' '));
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalDropTable *drop_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string drop_header_str;
        if (intent_size != 0) {
            drop_header_str = fmt::format("{}-> DROP TABLE ", std::string(intent_size - 2, ' '));
        } else {
            drop_header_str = "DROP TABLE ";
        }

        drop_header_str += fmt::format("({})", drop_node->node_id());
        result->emplace_back(std::make_shared<std::string>(drop_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = fmt::format("{} - database name: {}", std::string(intent_size, ' '), *drop_node->schema_name());
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // Table name
    {
        std::string table_name_str = fmt::format("{} - table: {}", std::string(intent_size, ' '), *drop_node->table_name());
        result->emplace_back(std::make_shared<std::string>(table_name_str));
    }

    // Conflict type
    {
        std::string conflict_type_str =
            fmt::format("{} - conflict type: {}", std::string(intent_size, ' '), ConflictTypeToStr(drop_node->conflict_type()));
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    // Output column
    {
        std::string output_columns_str = fmt::format("{} - output columns: [OK]", std::string(intent_size, ' '));
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const LogicalDropCollection *drop_node,
                                   std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                   i64 intent_size) {
    {
        std::string drop_header_str;
        if (intent_size != 0) {
            drop_header_str = fmt::format("{}-> DROP COLLECTION ", std::string(intent_size - 2, ' '));
        } else {
            drop_header_str = "DROP COLLECTION ";
        }

        drop_header_str += fmt::format("({})", drop_node->node_id());
        result->emplace_back(std::make_shared<std::string>(drop_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = fmt::format("{} - database name: {}", std::string(intent_size, ' '), *drop_node->schema_name());
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // Collection name
    {
        std::string table_name_str = fmt::format("{} - collection: {}", std::string(intent_size, ' '), *drop_node->collection_name());
        result->emplace_back(std::make_shared<std::string>(table_name_str));
    }

    // Conflict type
    {
        std::string conflict_type_str =
            fmt::format("{} - conflict type: {}", std::string(intent_size, ' '), ConflictTypeToStr(drop_node->conflict_type()));
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    // Output column
    {
        std::string output_columns_str = fmt::format("{} - output columns: [OK]", std::string(intent_size, ' '));
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalDropView *drop_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    return Status::NotSupport("DROP VIEW isn't supported");
#if 0
    {
        std::string drop_header_str;
        if (intent_size != 0) {
            drop_header_str = fmt::format("{}-> DROP VIEW ", std::string(intent_size - 2, ' '));
        } else {
            drop_header_str = "DROP VIEW ";
        }

        drop_header_str += fmt::format("({})", drop_node->node_id());
        result->emplace_back(std::make_shared<std::string>(drop_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = fmt::format("{} - database name: {}", std::string(intent_size, ' '), *drop_node->schema_name());
        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // View name
    {
        std::string table_name_str = fmt::format("{} - view name: {}", std::string(intent_size, ' '), *drop_node->view_name());
        result->emplace_back(std::make_shared<std::string>(table_name_str));
    }

    // Conflict type
    {
        std::string conflict_type_str = fmt::format("{} - conflict type: {}", std::string(intent_size, ' '), ConflictTypeToStr(drop_node->conflict_type()));
        result->emplace_back(std::make_shared<std::string>(conflict_type_str));
    }

    // Output column
    {
        std::string output_columns_str = fmt::format("{} - output columns: [OK]", std::string(intent_size, ' '));
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
#endif
}

Status
ExplainLogicalPlan::Explain(const LogicalInsert *insert_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string insert_header_str;
        if (intent_size != 0) {
            insert_header_str = fmt::format("{}-> INSERT ", std::string(intent_size - 2, ' '));
        } else {
            insert_header_str = "INSERT ";
        }

        insert_header_str += fmt::format("({})", insert_node->node_id());
        result->emplace_back(std::make_shared<std::string>(insert_header_str));
    }

    // Schema name
    {
        std::string schema_name_str = fmt::format("{} - database name: {}", std::string(intent_size, ' '), *(insert_node->table_info()->db_name_));

        result->emplace_back(std::make_shared<std::string>(schema_name_str));
    }

    // Table name
    {
        std::string table_name_str = fmt::format("{} - table name: {}", std::string(intent_size, ' '), *(insert_node->table_info()->table_name_));
        result->emplace_back(std::make_shared<std::string>(table_name_str));
    }

    // Values
    {
        std::string insert_str;
        insert_str = " - values ";
        size_t value_count = insert_node->value_list().size();
        if (value_count == 0) {
            UnrecoverableError("No value list in insert statement");
        }
        for (size_t idx = 0; idx < value_count; ++idx) {
            if (idx != 0)
                insert_str += ", ";
            const std::vector<std::shared_ptr<BaseExpression>> &value = insert_node->value_list()[idx];
            size_t column_count = value.size();
            for (size_t col = 0; col < column_count; ++col) {
                auto &value_expr = value[idx];
                if (col == 0)
                    insert_str += "(";
                else
                    insert_str += ", ";
                insert_str += value_expr->Name();
                if (col == column_count - 1)
                    insert_str += ")";
            }
        }

        result->emplace_back(std::make_shared<std::string>(insert_str));
    }
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalDelete *delete_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string header = delete_node->ToString(intent_size);
    result->emplace_back(std::make_shared<std::string>(header));
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalUpdate *update_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string header = update_node->ToString(intent_size);
    result->emplace_back(std::make_shared<std::string>(header));
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalProject *project_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string project_header;
        if (intent_size != 0) {
            project_header = fmt::format("{}-> PROJECT ", std::string(intent_size - 2, ' '));
        } else {
            project_header = "PROJECT ";
        }

        project_header += fmt::format("({})", project_node->node_id());
        result->emplace_back(std::make_shared<std::string>(project_header));
    }

    // Table index
    {
        std::string table_index = fmt::format("{} - table index: #{}", std::string(intent_size, ' '), project_node->table_index_);
        result->emplace_back(std::make_shared<std::string>(table_index));
    }

    // Expressions
    {
        std::string expression_str;
        expression_str.append(std::string(intent_size, ' ')).append(" - expressions: [");
        size_t expr_count = project_node->expressions_.size();
        if (expr_count == 0) {
            UnrecoverableError("No expression list in projection node.");
        }
        for (size_t idx = 0; idx < expr_count - 1; ++idx) {
            Explain(project_node->expressions_[idx].get(), expression_str);
            expression_str += ", ";
        }
        Explain(project_node->expressions_.back().get(), expression_str);
        expression_str += "]";
        result->emplace_back(std::make_shared<std::string>(expression_str));
    }
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalFilter *filter_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string filter_node_header;
    if (intent_size != 0) {
        filter_node_header = fmt::format("{}-> FILTER ", std::string(intent_size - 2, ' '));
    } else {
        filter_node_header = "FILTER ";
    }

    filter_node_header += fmt::format("({})", filter_node->node_id());
    result->emplace_back(std::make_shared<std::string>(filter_node_header));

    // filter expression
    std::string filter_str = std::string(intent_size, ' ');
    filter_str += " - filter: ";
    Explain(filter_node->expression().get(), filter_str);
    result->emplace_back(std::make_shared<std::string>(filter_str));

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ');
        output_columns_str += " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = filter_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx);
            output_columns_str += ", ";
        }
        output_columns_str += output_columns->back();
        output_columns_str += "]";

        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalUnnest *unnest_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string unnest_node_header_str;
    if (intent_size != 0) {
        unnest_node_header_str = fmt::format("{}-> UNNEST ", std::string(intent_size - 2, ' '));
    } else {
        unnest_node_header_str = "UNNEST ";
    }

    unnest_node_header_str += fmt::format("({})", unnest_node->node_id());
    result->emplace_back(std::make_shared<std::string>(unnest_node_header_str));

    // Unnest expression
    {
        std::string unnest_expression_str = std::string(intent_size, ' ') + " - unnest expression: [";
        const std::vector<std::shared_ptr<BaseExpression>> &expression_list = unnest_node->expression_list();
        size_t expression_count = expression_list.size();
        for (size_t idx = 0; idx < expression_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(expression_list[idx].get(), unnest_expression_str);
            unnest_expression_str += ", ";
        }
        ExplainLogicalPlan::Explain(expression_list.back().get(), unnest_expression_str);
        unnest_expression_str += "]";
        result->emplace_back(std::make_shared<std::string>(unnest_expression_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = unnest_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const LogicalUnnestAggregate *unnest_aggregate_node,
                                   std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                   i64 intent_size) {
    std::string unnest_node_header_str;
    if (intent_size != 0) {
        unnest_node_header_str = fmt::format("{}-> UNNEST AGGREGATE ", std::string(intent_size - 2, ' '));
    } else {
        unnest_node_header_str = "UNNEST AGGREGATE ";
    }

    unnest_node_header_str += "(" + std::to_string(unnest_aggregate_node->node_id()) + ")";
    result->emplace_back(std::make_shared<std::string>(unnest_node_header_str));

    // Unnest expression
    {
        std::string unnest_expression_str = std::string(intent_size, ' ') + " - unnest expression: [";
        const std::vector<std::shared_ptr<BaseExpression>> &expression_list = unnest_aggregate_node->unnest_expression_list();
        size_t expression_count = expression_list.size();
        for (size_t idx = 0; idx < expression_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(expression_list[idx].get(), unnest_expression_str);
            unnest_expression_str += ", ";
        }
        ExplainLogicalPlan::Explain(expression_list.back().get(), unnest_expression_str);
        unnest_expression_str += "]";
        result->emplace_back(std::make_shared<std::string>(unnest_expression_str));
    }

    size_t groups_count = unnest_aggregate_node->groups_.size();
    size_t aggregates_count = unnest_aggregate_node->aggregates_.size();

    //    // Aggregate Table index
    //    {
    //        std::string aggregate_table_index =
    //            std::string(intent_size, ' ') + " - aggregate table index: #" + std::to_string(unnest_aggregate_node->AggregateTableIndex());
    //        result->emplace_back(std::make_shared<std::string>(aggregate_table_index));
    //    }

    // Aggregate expressions
    {
        std::string aggregate_expression_str = std::string(intent_size, ' ') + " - aggregate: [";
        if (aggregates_count != 0) {
            for (size_t idx = 0; idx < aggregates_count - 1; ++idx) {
                ExplainLogicalPlan::Explain(unnest_aggregate_node->aggregates_[idx].get(), aggregate_expression_str);
                aggregate_expression_str += ", ";
            }
            ExplainLogicalPlan::Explain(unnest_aggregate_node->aggregates_.back().get(), aggregate_expression_str);
        }
        aggregate_expression_str += "]";
        result->emplace_back(std::make_shared<std::string>(aggregate_expression_str));
    }

    // Group by expressions
    if (groups_count != 0) {
        // Group by table index
        //        std::string group_table_index = std::string(intent_size, ' ') + " - group by table index: #" +
        //        std::to_string(unnest_aggregate_node->GroupTableIndex()); result->emplace_back(std::make_shared<std::string>(group_table_index));

        std::string group_by_expression_str = std::string(intent_size, ' ') + " - group by: [";
        for (size_t idx = 0; idx < groups_count - 1; ++idx) {
            ExplainLogicalPlan::Explain(unnest_aggregate_node->groups_[idx].get(), group_by_expression_str);
            group_by_expression_str += ", ";
        }
        ExplainLogicalPlan::Explain(unnest_aggregate_node->groups_.back().get(), group_by_expression_str);
        group_by_expression_str += "]";
        result->emplace_back(std::make_shared<std::string>(group_by_expression_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = unnest_aggregate_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const LogicalTableScan *table_scan_node,
                                   std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                   i64 intent_size) {
    std::string table_scan_header;
    if (intent_size != 0) {
        table_scan_header = std::string(intent_size - 2, ' ');
        table_scan_header += "-> TABLE SCAN ";
    } else {
        table_scan_header = "TABLE SCAN ";
    }

    table_scan_header += fmt::format("({})", table_scan_node->node_id());
    result->emplace_back(std::make_shared<std::string>(table_scan_header));

    // Table alias and name
    std::string table_name = std::string(intent_size, ' ');
    table_name += " - table name: ";
    table_name += table_scan_node->TableAlias();
    table_name += "(";

    table_name += *table_scan_node->table_info()->db_name_;
    table_name += ".";
    table_name += *table_scan_node->table_info()->table_name_;
    table_name += ")";
    result->emplace_back(std::make_shared<std::string>(table_name));

    // Table index
    std::string table_index = std::string(intent_size, ' ');
    table_index += " - table index: #";
    table_index += std::to_string(table_scan_node->TableIndex());
    result->emplace_back(std::make_shared<std::string>(table_index));

    // Output columns
    std::string output_columns = std::string(intent_size, ' ');
    output_columns += " - output columns: [";
    size_t column_count = table_scan_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError(fmt::format("No column in table: {}.", table_scan_node->TableAlias()));
    }
    for (size_t idx = 0; idx < column_count - 1; ++idx) {
        output_columns += table_scan_node->GetOutputNames()->at(idx);
        output_columns += ", ";
    }
    output_columns += table_scan_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(std::make_shared<std::string>(output_columns));
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const LogicalIndexScan *index_scan_node,
                                   std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                   i64 intent_size) {
    std::string index_scan_header;
    if (intent_size != 0) {
        index_scan_header = std::string(intent_size - 2, ' ');
        index_scan_header += "-> INDEX SCAN ";
    } else {
        index_scan_header = "INDEX SCAN ";
    }

    index_scan_header += fmt::format("({})", index_scan_node->node_id());
    result->emplace_back(std::make_shared<std::string>(index_scan_header));

    // Table alias and name
    std::string table_name = std::string(intent_size, ' ');
    table_name += " - table name: ";
    table_name += index_scan_node->TableAlias();
    table_name += "(";

    table_name += *index_scan_node->table_info()->db_name_;
    table_name += ".";
    table_name += *index_scan_node->table_info()->table_name_;
    table_name += ")";
    result->emplace_back(std::make_shared<std::string>(table_name));

    // Table index
    std::string table_index = std::string(intent_size, ' ');
    table_index += " - table index: #";
    table_index += std::to_string(index_scan_node->TableIndex());
    result->emplace_back(std::make_shared<std::string>(table_index));

    // filter expression
    std::string filter_str = std::string(intent_size, ' ');
    filter_str += " - filter: ";
    Explain(index_scan_node->index_filter_.get(), filter_str);
    result->emplace_back(std::make_shared<std::string>(filter_str));

    // Output columns
    std::string output_columns = std::string(intent_size, ' ');
    output_columns += " - output columns: [";
    size_t column_count = index_scan_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError(fmt::format("No column in table: {}.", index_scan_node->TableAlias()));
    }
    for (size_t idx = 0; idx < column_count - 1; ++idx) {
        output_columns += index_scan_node->GetOutputNames()->at(idx);
        output_columns += ", ";
    }
    output_columns += index_scan_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(std::make_shared<std::string>(output_columns));

    // TODO: load meta
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const LogicalKnnScan *knn_scan_node,
                                   std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                   i64 intent_size) {
    std::string knn_scan_header;
    if (intent_size != 0) {
        knn_scan_header = std::string(intent_size - 2, ' ');
        knn_scan_header += "-> KNN SCAN ";
    } else {
        knn_scan_header = "KNN SCAN ";
    }

    knn_scan_header += fmt::format("({})", knn_scan_node->node_id());
    result->emplace_back(std::make_shared<std::string>(knn_scan_header));

    // Table alias and name
    std::string table_name = std::string(intent_size, ' ');
    table_name += " - table name: ";
    table_name += knn_scan_node->TableAlias();
    table_name += "(";

    table_name += *knn_scan_node->table_info()->db_name_;
    table_name += ".";
    table_name += *knn_scan_node->table_info()->table_name_;
    table_name += ")";
    result->emplace_back(std::make_shared<std::string>(table_name));

    // Table index
    std::string table_index = std::string(intent_size, ' ');
    table_index += " - table index: #";
    table_index += std::to_string(knn_scan_node->TableIndex());
    result->emplace_back(std::make_shared<std::string>(table_index));

    KnnExpression *knn_expr_raw = knn_scan_node->knn_expression().get();
    // Embedding info
    std::string embedding_info = std::string(intent_size, ' ');
    embedding_info += " - embedding info: ";
    embedding_info += knn_expr_raw->arguments().at(0)->Name();
    result->emplace_back(std::make_shared<std::string>(embedding_info));

    std::string embedding_type_str = std::string(intent_size + 2, ' ');
    embedding_type_str += " - element type: ";
    embedding_type_str += EmbeddingT::EmbeddingDataType2String(knn_expr_raw->embedding_data_type_);
    result->emplace_back(std::make_shared<std::string>(embedding_type_str));

    std::string embedding_dimension_str = std::string(intent_size + 2, ' ');
    embedding_dimension_str += " - dimension: ";
    embedding_type_str += std::to_string(knn_expr_raw->dimension_);
    result->emplace_back(std::make_shared<std::string>(embedding_dimension_str));

    std::string distance_type_str = std::string(intent_size + 2, ' ');
    distance_type_str += " - distance type: ";
    distance_type_str += KnnExpr::KnnDistanceType2Str(knn_expr_raw->distance_type_);
    result->emplace_back(std::make_shared<std::string>(distance_type_str));

    // Query embedding
    std::string query_embedding = std::string(intent_size + 2, ' ');
    query_embedding += " - query embedding: ";
    query_embedding += EmbeddingT::Embedding2String(knn_expr_raw->query_embedding_, knn_expr_raw->embedding_data_type_, knn_expr_raw->dimension_);
    result->emplace_back(std::make_shared<std::string>(query_embedding));

    // filter expression
    if (knn_scan_node->filter_expression_.get() != nullptr) {
        std::string filter_str = std::string(intent_size, ' ');
        filter_str += " - filter: ";
        Explain(knn_scan_node->filter_expression_.get(), filter_str);
        result->emplace_back(std::make_shared<std::string>(filter_str));
    }

    // Output columns
    std::string output_columns = std::string(intent_size, ' ');
    output_columns += " - output columns: [";
    size_t column_count = knn_scan_node->GetOutputNames()->size();
    if (column_count == 0) {
        UnrecoverableError(fmt::format("No column in table: {}.", knn_scan_node->TableAlias()));
    }
    for (size_t idx = 0; idx < column_count - 1; ++idx) {
        output_columns += knn_scan_node->GetOutputNames()->at(idx);
        output_columns += ", ";
    }
    output_columns += knn_scan_node->GetOutputNames()->back();
    output_columns += "]";
    result->emplace_back(std::make_shared<std::string>(output_columns));
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const LogicalAggregate *aggregate_node,
                                   std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                   i64 intent_size) {
    size_t groups_count = aggregate_node->groups_.size();
    size_t aggregates_count = aggregate_node->aggregates_.size();
    if (groups_count == 0 && aggregate_node == 0) {
        UnrecoverableError("Both groups and aggregates are empty.");
    }

    {
        std::string agg_header;
        if (intent_size != 0) {
            agg_header = std::string(intent_size - 2, ' ');
            agg_header += "-> AGGREGATE ";
        } else {
            agg_header = "AGGREGATE ";
        }

        agg_header += "(";
        agg_header += std::to_string(aggregate_node->node_id());
        agg_header += ")";
        result->emplace_back(std::make_shared<std::string>(agg_header));
    }

    // Aggregate Table index
    {
        std::string aggregate_table_index = std::string(intent_size, ' ');
        aggregate_table_index += " - aggregate table index: #";
        aggregate_table_index += std::to_string(aggregate_node->aggregate_index_);
        result->emplace_back(std::make_shared<std::string>(aggregate_table_index));
    }

    // Aggregate expressions
    {
        std::string aggregate_expression_str = std::string(intent_size, ' ');
        aggregate_expression_str += " - aggregate: [";
        if (aggregates_count != 0) {
            for (size_t idx = 0; idx < aggregates_count - 1; ++idx) {
                Explain(aggregate_node->aggregates_[idx].get(), aggregate_expression_str);
                aggregate_expression_str += ", ";
            }
            Explain(aggregate_node->aggregates_.back().get(), aggregate_expression_str);
        }
        aggregate_expression_str += "]";
        result->emplace_back(std::make_shared<std::string>(aggregate_expression_str));
    }

    // Group by expressions
    if (groups_count != 0) {
        // Group by table index
        std::string group_table_index = std::string(intent_size, ' ');
        group_table_index += " - group by table index: #";
        group_table_index += std::to_string(aggregate_node->groupby_index_);
        result->emplace_back(std::make_shared<std::string>(group_table_index));

        std::string group_by_expression_str = std::string(intent_size, ' ');
        group_by_expression_str += " - group by: [";
        for (size_t idx = 0; idx < groups_count - 1; ++idx) {
            Explain(aggregate_node->groups_[idx].get(), group_by_expression_str);
            group_by_expression_str += ", ";
        }
        Explain(aggregate_node->groups_.back().get(), group_by_expression_str);
        group_by_expression_str += "]";
        result->emplace_back(std::make_shared<std::string>(group_by_expression_str));
    }
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalSort *sort_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string sort_header;
        if (intent_size != 0) {
            sort_header = std::string(intent_size - 2, ' ');
            sort_header += "-> SORT ";
        } else {
            sort_header = "SORT ";
        }

        sort_header += "(";
        sort_header += std::to_string(sort_node->node_id());
        sort_header += ")";
        result->emplace_back(std::make_shared<std::string>(sort_header));
    }

    {
        std::string sort_expression_str = std::string(intent_size, ' ');
        sort_expression_str += " - expressions: [";
        size_t order_by_count = sort_node->expressions_.size();
        if (order_by_count == 0) {
            UnrecoverableError("ORDER BY without any expression.");
        }

        for (size_t idx = 0; idx < order_by_count - 1; ++idx) {
            Explain(sort_node->expressions_[idx].get(), sort_expression_str);
            sort_expression_str += " ";
            sort_expression_str += SelectStatement::ToString(sort_node->order_by_types_[idx]);
            sort_expression_str += ", ";
        }
        Explain(sort_node->expressions_.back().get(), sort_expression_str);
        sort_expression_str += " ";
        sort_expression_str += SelectStatement::ToString(sort_node->order_by_types_.back());
        sort_expression_str += "]";
        result->emplace_back(std::make_shared<std::string>(sort_expression_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ');
        output_columns_str += " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = sort_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx);
            output_columns_str += ", ";
        }
        output_columns_str += output_columns->back();
        output_columns_str += "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalLimit *limit_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string limit_header;
        if (intent_size != 0) {
            limit_header = std::string(intent_size - 2, ' ');
            limit_header += "-> LIMIT ";
        } else {
            limit_header = "LIMIT ";
        }

        limit_header += "(";
        limit_header += std::to_string(limit_node->node_id());
        limit_header += ")";
        result->emplace_back(std::make_shared<std::string>(limit_header));
    }

    {
        std::string limit_value_str = std::string(intent_size, ' ');
        limit_value_str += " - limit: ";
        Explain(limit_node->limit_expression_.get(), limit_value_str);
        result->emplace_back(std::make_shared<std::string>(limit_value_str));
    }

    if (limit_node->offset_expression_.get() != nullptr) {
        std::string offset_value_str = std::string(intent_size, ' ');
        offset_value_str += " - offset: ";
        Explain(limit_node->offset_expression_.get(), offset_value_str);
        result->emplace_back(std::make_shared<std::string>(offset_value_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ');
        output_columns_str += " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = limit_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx);
            output_columns_str += ", ";
        }
        output_columns_str += output_columns->back();
        output_columns_str += "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const LogicalTop *top_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string top_header;
        if (intent_size != 0) {
            top_header = std::string(intent_size - 2, ' ') + "-> TOP ";
        } else {
            top_header = "TOP ";
        }

        top_header += "(" + std::to_string(top_node->node_id()) + ")";
        result->emplace_back(std::make_shared<std::string>(top_header));
    }

    {
        std::string sort_expression_str = std::string(intent_size, ' ') + " - sort expressions: [";
        auto &sort_expressions = top_node->sort_expressions_;
        size_t order_by_count = sort_expressions.size();
        if (order_by_count == 0) {
            UnrecoverableError("TOP without any sort expression.");
        }

        auto &order_by_types = top_node->order_by_types_;
        for (size_t idx = 0; idx < order_by_count - 1; ++idx) {
            Explain(sort_expressions[idx].get(), sort_expression_str);
            sort_expression_str += " " + SelectStatement::ToString(order_by_types[idx]) + ", ";
        }
        Explain(sort_expressions.back().get(), sort_expression_str);
        sort_expression_str += " " + SelectStatement::ToString(order_by_types.back()) + "]";
        result->emplace_back(std::make_shared<std::string>(sort_expression_str));
    }

    {
        std::string limit_value_str = std::string(intent_size, ' ') + " - limit: ";
        Explain(top_node->limit_expression_.get(), limit_value_str);
        result->emplace_back(std::make_shared<std::string>(limit_value_str));
    }

    if (top_node->offset_expression_.get() != nullptr) {
        std::string offset_value_str = std::string(intent_size, ' ') + " - offset: ";
        Explain(top_node->offset_expression_.get(), offset_value_str);
        result->emplace_back(std::make_shared<std::string>(offset_value_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ') + " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = top_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx) + ", ";
        }
        output_columns_str += output_columns->back() + "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const LogicalCrossProduct *cross_product_node,
                                   std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                   i64 intent_size) {
    {
        std::string cross_product_header;
        if (intent_size != 0) {
            cross_product_header = std::string(intent_size - 2, ' ');
            cross_product_header += "-> CROSS PRODUCT ";
        } else {
            cross_product_header = "CROSS PRODUCT ";
        }
        cross_product_header += "(";
        cross_product_header += std::to_string(cross_product_node->node_id());
        cross_product_header += ")";
        result->emplace_back(std::make_shared<std::string>(cross_product_header));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ');
        output_columns_str += " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = cross_product_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx);
            output_columns_str += ", ";
        }
        output_columns_str += output_columns->back();
        output_columns_str += "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalJoin *join_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string join_header;
        if (intent_size != 0) {
            join_header = std::string(intent_size - 2, ' ');
            join_header += "-> ";
        }
        join_header += JoinReference::ToString(join_node->join_type_);
        join_header += "(";
        join_header += std::to_string(join_node->node_id());
        join_header += ")";
        result->emplace_back(std::make_shared<std::string>(join_header));
    }

    // Conditions
    {
        std::string condition_str = std::string(intent_size, ' ');
        condition_str += " - filters: [";

        size_t conditions_count = join_node->conditions_.size();
        if (conditions_count == 0) {
            UnrecoverableError("JOIN without any condition.");
        }

        for (size_t idx = 0; idx < conditions_count - 1; ++idx) {
            Explain(join_node->conditions_[idx].get(), condition_str);
            condition_str += ", ";
        }
        Explain(join_node->conditions_.back().get(), condition_str);
        result->emplace_back(std::make_shared<std::string>(condition_str));
    }

    // Output column
    {
        std::string output_columns_str = std::string(intent_size, ' ');
        output_columns_str += " - output columns: [";
        std::shared_ptr<std::vector<std::string>> output_columns = join_node->GetOutputNames();
        size_t column_count = output_columns->size();
        for (size_t idx = 0; idx < column_count - 1; ++idx) {
            output_columns_str += output_columns->at(idx);
            output_columns_str += ", ";
        }
        output_columns_str += output_columns->back();
        output_columns_str += "]";
        result->emplace_back(std::make_shared<std::string>(output_columns_str));
    }
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalShow *show_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    switch (show_node->show_type()) {
        case ShowStmtType::kInvalid: {
            UnrecoverableError("Invalid show statement type");
            break;
        }
        case ShowStmtType::kDatabase: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW DATABASE ";
            } else {
                show_str = "SHOW DATABASE ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kTable: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW TABLES ";
            } else {
                show_str = "SHOW TABLES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kCollections: {
            UnrecoverableError("Show collections are not supported now");
            break;
        }
        case ShowStmtType::kIndex: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW INDEX ";
            } else {
                show_str = "SHOW INDEX ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kIndexSegment: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW INDEX SEGMENT ";
            } else {
                show_str = "SHOW INDEX SEGMENT ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kIndexChunk: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW INDEX CHUNK ";
            } else {
                show_str = "SHOW INDEX CHUNK ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kTables: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW TABLES ";
            } else {
                show_str = "SHOW TABLES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [database, table, type, column_count, row_count, segment_count, block_count, segment_capacity]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kTasks: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW VIEWS ";
            } else {
                show_str = "SHOW VIEWS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [schema, view, column_count]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kColumns: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> DESCRIBE TABLE/COLLECTION ";
            } else {
                show_str = "DESCRIBE TABLE/COLLECTION ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string show_column_schema_str = std::string(intent_size, ' ');
            show_column_schema_str += " - database: ";
            show_column_schema_str += show_node->schema_name();
            result->emplace_back(std::make_shared<std::string>(show_column_schema_str));

            std::string show_column_table_str = std::string(intent_size, ' ');
            show_column_table_str += " - table/collection: ";
            show_column_table_str += *(show_node->object_name());
            result->emplace_back(std::make_shared<std::string>(show_column_table_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [column_name, column_type, constraint]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kDatabases: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW DATABASES ";
            } else {
                show_str = "SHOW DATABASES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [database]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kConfigs: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW CONFIGS ";
            } else {
                show_str = "SHOW CONFIGS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [config_name, value, description]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kBuffer: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW BUFFER ";
            } else {
                show_str = "SHOW BUFFER ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [path, status, size, buffered_type, type]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kMemIndex: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW MEMINDEX ";
            } else {
                show_str = "SHOW MEMINDEX ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [index_name, table_name, db_name, size, row_count]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kProfiles: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW PROFILES ";
            } else {
                show_str = "SHOW PROFILES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [record_no, parser, logical planner, optimizer, physical planner, pipeline builder, task "
                                  "builder, executor, total_cost]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kQueries: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW QUERIES ";
            } else {
                show_str = "SHOW QUERIES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [session_id, query_id, query_kind, start_time, time_consumption]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kQuery: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW QUERY ";
            } else {
                show_str = "SHOW QUERY ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kTransactions: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW TRANSACTIONS ";
            } else {
                show_str = "SHOW TRANSACTIONS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [transaction_id, transaction_text]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kTransaction: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW TRANSACTION ";
            } else {
                show_str = "SHOW TRANSACTION ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kTransactionHistory: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW TRANSACTION HISTORY ";
            } else {
                show_str = "SHOW TRANSACTION HISTORY ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [transaction]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kIndexes: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW INDEXES ";
            } else {
                show_str = "SHOW INDEXES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [index_name, index_type, column_names, other_parameters]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kSegments: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW SEGMENTS ";
            } else {
                show_str = "SHOW SEGMENTS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            if (show_node->segment_id().has_value()) {
                std::string output_columns_str = std::string(intent_size, ' ');
                output_columns_str += " - segment: " + std::to_string(*show_node->segment_id());
                result->emplace_back(std::make_shared<std::string>(output_columns_str));
            }

            if (show_node->block_id().has_value()) {
                std::string output_columns_str = std::string(intent_size, ' ');
                output_columns_str += " - block: " + std::to_string(*show_node->block_id());
                result->emplace_back(std::make_shared<std::string>(output_columns_str));
            }

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [id, status, size]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kSegment: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW SEGMENT ";
            } else {
                show_str = "SHOW SEGMENT ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string show_segment_str = std::string(intent_size, ' ');
            show_segment_str += " - segment: " + std::to_string(*show_node->segment_id());
            result->emplace_back(std::make_shared<std::string>(show_segment_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str +=
                " - output columns: [id, status, dir, size, block_count, row_capacity, row_count, actual_row_count, room, column_count]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kBlocks: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW BLOCKS ";
            } else {
                show_str = "SHOW BLOCKS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            if (show_node->segment_id().has_value()) {
                std::string output_columns_str = std::string(intent_size, ' ');
                output_columns_str += " - segment: " + std::to_string(*show_node->segment_id());
                result->emplace_back(std::make_shared<std::string>(output_columns_str));
            }

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [id, size, row_count]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kBlock: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW BLOCK ";
            } else {
                show_str = "SHOW BLOCK ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string show_segment_str = std::string(intent_size, ' ');
            show_segment_str += " - segment: " + std::to_string(*show_node->segment_id());
            result->emplace_back(std::make_shared<std::string>(show_segment_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [id, path, size, row_capacity, row_count, checkpoint_row_count, column_count, checkpoint_ts]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kBlockColumn: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW BLOCK COLUMN";
            } else {
                show_str = "SHOW BLOCK COLUMN";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string show_segment_str = std::string(intent_size, ' ');
            show_segment_str += " - segment: " + std::to_string(*show_node->segment_id());
            result->emplace_back(std::make_shared<std::string>(show_segment_str));

            std::string show_block_str = std::string(intent_size, ' ');
            show_block_str += " - block: " + std::to_string(*show_node->block_id());
            result->emplace_back(std::make_shared<std::string>(show_block_str));

            std::string show_column_str = std::string(intent_size, ' ');
            show_column_str += " - column: " + std::to_string(*show_node->column_id());
            result->emplace_back(std::make_shared<std::string>(show_column_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [id, path, size, row_capacity, row_count, checkpoint_row_count, column_count, checkpoint_ts]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kSessionVariable: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW SESSION VARIABLE ";
            } else {
                show_str = "SHOW SESSION VARIABLE ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += fmt::format(" - variable: {}", *(show_node->object_name()));
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kSessionVariables: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW SESSION VARIABLES ";
            } else {
                show_str = "SHOW SESSION VARIABLES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));
            break;
        }
        case ShowStmtType::kGlobalVariable: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW GLOBAL VARIABLE ";
            } else {
                show_str = "SHOW GLOBAL VARIABLE ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += fmt::format(" - variable: {}", *(show_node->object_name()));
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kGlobalVariables: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW GLOBAL VARIABLES ";
            } else {
                show_str = "SHOW GLOBAL VARIABLES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));
            break;
        }
        case ShowStmtType::kConfig: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW CONFIG ";
            } else {
                show_str = "SHOW CONFIG ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += fmt::format(" - config: {}", *(show_node->object_name()));
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kLogs: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW LOGS ";
            } else {
                show_str = "SHOW LOGS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));
            break;
        }
        case ShowStmtType::kCatalog: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW CATALOG ";
            } else {
                show_str = "SHOW CATALOG ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));
            break;
        }
        case ShowStmtType::kCatalogToFile: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW CATALOG TO FILE ";
            } else {
                show_str = "SHOW CATALOG TO FILE ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));
            break;
        }
        case ShowStmtType::kPersistenceFiles: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW PERSISTENCE FILES ";
            } else {
                show_str = "SHOW PERSISTENCE FILES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));
            break;
        }
        case ShowStmtType::kPersistenceObjects: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW PERSISTENCE OBJECTS ";
            } else {
                show_str = "SHOW PERSISTENCE OBJECTS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));
            break;
        }
        case ShowStmtType::kPersistenceObject: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW PERSISTENCE OBJECT ";
            } else {
                show_str = "SHOW PERSISTENCE OBJECT ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));
            break;
        }
        case ShowStmtType::kMemory: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW MEMORY ";
            } else {
                show_str = "SHOW MEMORY ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));
            break;
        }
        case ShowStmtType::kMemoryObjects: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW MEMORY OBJECTS ";
            } else {
                show_str = "SHOW MEMORY OBJECTS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));
            break;
        }
        case ShowStmtType::kMemoryAllocation: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW MEMORY ALLOCATION ";
            } else {
                show_str = "SHOW MEMORY ALLOCATION ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));
            break;
        }
        case ShowStmtType::kFunction: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW FUNCTION";
            } else {
                show_str = "SHOW FUNCTION";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case ShowStmtType::kListSnapshots: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW SNAPSHOTS ";
            } else {
                show_str = "SHOW SNAPSHOTS ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));
            break;
        }
        case ShowStmtType::kShowSnapshot: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW SNAPSHOT ";
            } else {
                show_str = "SHOW SNAPSHOT ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));
            break;
        }
        case ShowStmtType::kListCaches: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> LIST CACHES ";
            } else {
                show_str = "LIST CACHES ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));
            break;
        }
        case ShowStmtType::kShowCache: {
            std::string show_str;
            if (intent_size != 0) {
                show_str = std::string(intent_size - 2, ' ');
                show_str += "-> SHOW CACHE ";
            } else {
                show_str = "SHOW CACHE ";
            }
            show_str += "(";
            show_str += std::to_string(show_node->node_id());
            show_str += ")";
            result->emplace_back(std::make_shared<std::string>(show_str));
            break;
        }
    }
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const BaseExpression *base_expression, std::string &expr_str, bool consider_add_parentheses) {
    switch (base_expression->type()) {
        case ExpressionType::kAggregate: {
            auto *aggregate_expression = (AggregateExpression *)base_expression;
            if (aggregate_expression->arguments().size() != 1) {
                UnrecoverableError("More than one argument in aggregate function");
            }
            expr_str += aggregate_expression->aggregate_function_.name();
            expr_str += "(";
            Explain(aggregate_expression->arguments()[0].get(), expr_str);
            expr_str += ")";
            break;
        }
        case ExpressionType::kCast: {
            auto *cast_expression = (CastExpression *)base_expression;
            if (cast_expression->arguments().size() != 1) {
                UnrecoverableError("More than one argument in cast function");
            }
            expr_str += "CAST(";
            Explain(cast_expression->arguments()[0].get(), expr_str);
            expr_str += " AS ";
            expr_str += cast_expression->Type().ToString();
            expr_str += ")";
            break;
        }
        case ExpressionType::kCase: {
            auto *case_expression = (CaseExpression *)base_expression;

            expr_str += "CASE ";
            size_t case_expr_count = case_expression->CaseExpr().size();
            for (size_t idx = 0; idx < case_expr_count; ++idx) {
                auto &check = case_expression->CaseExpr()[idx];
                expr_str += " WHEN(";
                Explain(check.when_expr_.get(), expr_str);
                expr_str += ")";

                expr_str += " THEN(";
                Explain(check.then_expr_.get(), expr_str);
                expr_str += ")";
            }

            expr_str += " ELSE";
            Explain(case_expression->ElseExpr().get(), expr_str);
            expr_str += " END";
            break;
        }
        case ExpressionType::kColumn: {
            ColumnExpression *column_expression = (ColumnExpression *)base_expression;
            expr_str += column_expression->Name();
            expr_str += " (#";
            expr_str += std::to_string(column_expression->binding().table_idx);
            expr_str += ".";
            expr_str += std::to_string(column_expression->binding().column_idx);
            expr_str += ")";
            break;
        }
        case ExpressionType::kFunction: {
            FunctionExpression *function_expression = (FunctionExpression *)base_expression;
            if (function_expression->arguments().empty()) {
                // No argument function
                expr_str += function_expression->func_.name();
                expr_str += "()";
                break;
            }

            if (function_expression->arguments().size() == 1) {
                // Unary function
                expr_str += function_expression->func_.name();
                expr_str += "(";
                Explain(function_expression->arguments()[0].get(), expr_str);
                expr_str += ")";
                break;
            }
            if (function_expression->arguments().size() == 2) {
                // Binary function
                if (consider_add_parentheses) {
                    expr_str += "(";
                }
                Explain(function_expression->arguments()[0].get(), expr_str, true);
                expr_str += " ";
                expr_str += function_expression->func_.name();
                expr_str += " ";
                Explain(function_expression->arguments()[1].get(), expr_str, true);
                if (consider_add_parentheses) {
                    expr_str += ")";
                }
                break;
            }

            // More than two arguments function
            expr_str += function_expression->func_.name();
            expr_str += "(";
            size_t argument_count = function_expression->arguments().size();
            for (size_t idx = 0; idx < argument_count - 1; ++idx) {
                Explain(function_expression->arguments()[idx].get(), expr_str);
                expr_str += ", ";
            }
            Explain(function_expression->arguments().back().get(), expr_str);
            expr_str += ")";
            break;
        }
        case ExpressionType::kBetween: {
            auto *between_expression = (BetweenExpression *)base_expression;
            if (between_expression->arguments().size() != 3) {
                UnrecoverableError("Between expression should have three arguments.");
            }
            Explain(between_expression->arguments()[0].get(), expr_str);
            expr_str += " BETWEEN ";
            Explain(between_expression->arguments()[1].get(), expr_str);
            expr_str += " AND ";
            Explain(between_expression->arguments()[2].get(), expr_str);
            break;
        }
        case ExpressionType::kIn: {
            InExpression *in_expression = (InExpression *)base_expression;
            expr_str += "IN[";
            size_t argument_count = in_expression->arguments().size();
            for (size_t idx = 0; idx < argument_count - 1; ++idx) {
                Explain(in_expression->arguments()[idx].get(), expr_str);
                expr_str += ", ";
            }
            Explain(in_expression->arguments().back().get(), expr_str);
            expr_str += "]";
            break;
        }
        case ExpressionType::kValue: {
            ValueExpression *value_expression = (ValueExpression *)base_expression;
            expr_str += value_expression->ToString();
            break;
        }
        case ExpressionType::kReference: {
            ReferenceExpression *reference_expression = (ReferenceExpression *)base_expression;
            expr_str += reference_expression->Name();
            expr_str += " (#";
            expr_str += std::to_string(reference_expression->column_index());
            expr_str += ")";
            break;
        }
        case ExpressionType::kFilterFullText:
        case ExpressionType::kUnnest: {
            expr_str += base_expression->ToString();
            break;
        }
        case ExpressionType::kSubQuery:
        case ExpressionType::kCorrelatedColumn:
        default: {
            UnrecoverableError("Unsupported expression type");
        }
    }
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalImport *import_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string import_header_str;
        if (intent_size != 0) {
            import_header_str = std::string(intent_size - 2, ' ');
            import_header_str += "-> IMPORT ";
        } else {
            import_header_str = "IMPORT ";
        }

        import_header_str += "(";
        import_header_str += std::to_string(import_node->node_id());
        import_header_str += ")";
        result->emplace_back(std::make_shared<std::string>(import_header_str));
    }

    {
        std::shared_ptr<std::string> schema_name = std::make_shared<std::string>(
            fmt::format("{} - database name: {}", std::string(intent_size, ' '), *(import_node->table_info()->db_name_)));
        result->emplace_back(schema_name);
    }

    {
        std::shared_ptr<std::string> table_name = std::make_shared<std::string>(
            fmt::format("{} - table name: {}", std::string(intent_size, ' '), *(import_node->table_info()->table_name_)));
        result->emplace_back(table_name);
    }

    {
        std::shared_ptr<std::string> path =
            std::make_shared<std::string>(fmt::format("{} - file: {}", std::string(intent_size, ' '), import_node->file_path()));
        result->emplace_back(path);
    }

    switch (import_node->FileType()) {
        case CopyFileType::kCSV: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(fmt::format("{} - type: CSV", std::string(intent_size, ' ')));
            result->emplace_back(file_type);

            std::shared_ptr<std::string> header =
                std::make_shared<std::string>(fmt::format("{} - header: {}", std::string(intent_size, ' '), (import_node->header() ? "Yes" : "No")));
            result->emplace_back(header);

            std::shared_ptr<std::string> delimiter =
                std::make_shared<std::string>(fmt::format("{} - header: {} - delimiter: ", std::string(intent_size, ' '), import_node->delimiter()));
            result->emplace_back(delimiter);
            break;
        }
        case CopyFileType::kJSON: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(fmt::format("{} - type: JSON", std::string(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kJSONL: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(fmt::format("{} - type: JSONL", std::string(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kFVECS: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(fmt::format("{} - type: FVECS", std::string(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kCSR: {
            auto file_type = std::make_shared<std::string>(fmt::format("{} - type: CSR", std::string(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kBVECS: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(fmt::format("{} - type: BVECS", std::string(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kPARQUET: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(fmt::format("{} - type: PARQUET", std::string(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kInvalid: {
            UnrecoverableError("Invalid file type");
        }
    }

    if (import_node->left_node().get() != nullptr or import_node->right_node().get() != nullptr) {
        UnrecoverableError("Import node have children nodes.");
    }
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalExport *export_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    {
        std::string export_header_str;
        if (intent_size != 0) {
            export_header_str = std::string(intent_size - 2, ' ');
            export_header_str += "-> EXPORT ";
        } else {
            export_header_str = "EXPORT ";
        }

        export_header_str += "(";
        export_header_str += std::to_string(export_node->node_id());
        export_header_str += ")";
        result->emplace_back(std::make_shared<std::string>(export_header_str));
    }

    {
        std::shared_ptr<std::string> schema_name =
            std::make_shared<std::string>(fmt::format("{} - database name: {}", std::string(intent_size, ' '), export_node->schema_name()));
        result->emplace_back(schema_name);
    }

    {
        std::shared_ptr<std::string> table_name =
            std::make_shared<std::string>(fmt::format("{} - table name: {}", std::string(intent_size, ' '), export_node->table_name()));
        result->emplace_back(table_name);
    }

    {
        std::shared_ptr<std::string> path =
            std::make_shared<std::string>(fmt::format("{} - file: {}", std::string(intent_size, ' '), export_node->file_path()));
        result->emplace_back(path);
    }

    switch (export_node->FileType()) {
        case CopyFileType::kCSV: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(fmt::format("{} - type: CSV", std::string(intent_size, ' ')));
            result->emplace_back(file_type);

            std::shared_ptr<std::string> header =
                std::make_shared<std::string>(fmt::format("{} - header: {}", std::string(intent_size, ' '), (export_node->header() ? "Yes" : "No")));
            result->emplace_back(header);

            std::shared_ptr<std::string> delimiter =
                std::make_shared<std::string>(fmt::format("{} - header: {} - delimiter: ", std::string(intent_size, ' '), export_node->delimiter()));
            result->emplace_back(delimiter);
            break;
        }
        case CopyFileType::kJSON: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(fmt::format("{} - type: JSON", std::string(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kJSONL: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(fmt::format("{} - type: JSONL", std::string(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kFVECS: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(fmt::format("{} - type: FVECS", std::string(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kCSR: {
            auto file_type = std::make_shared<std::string>(fmt::format("{} - type: CSR", std::string(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kBVECS: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(fmt::format("{} - type: BVECS", std::string(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kPARQUET: {
            std::shared_ptr<std::string> file_type = std::make_shared<std::string>(fmt::format("{} - type: PARQUET", std::string(intent_size, ' ')));
            result->emplace_back(file_type);
            break;
        }
        case CopyFileType::kInvalid: {
            UnrecoverableError("Invalid file type");
        }
    }

    if (export_node->left_node().get() != nullptr or export_node->right_node().get() != nullptr) {
        UnrecoverableError("EXPORT node have children nodes.");
    }
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalFlush *flush_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::string flush_header_str;
    if (intent_size != 0) {
        flush_header_str = std::string(intent_size - 2, ' ');
        flush_header_str += "-> FLUSH ";
    } else {
        flush_header_str = "FLUSH ";
    }

    switch (flush_node->flush_type()) {
        case FlushType::kCatalog:
            flush_header_str += "CATALOG (";
            flush_header_str += std::to_string(flush_node->node_id());
            flush_header_str += ")";
            break;
        case FlushType::kData:
            flush_header_str += "DATA (";
            flush_header_str += std::to_string(flush_node->node_id());
            flush_header_str += ")";
            break;
        case FlushType::kLog:
            flush_header_str += "LOG (";
            flush_header_str += std::to_string(flush_node->node_id());
            flush_header_str += ")";
            break;
        case FlushType::kBuffer:
            flush_header_str += "BUFFER (";
            flush_header_str += std::to_string(flush_node->node_id());
            flush_header_str += ")";
            break;
    }
    result->emplace_back(std::make_shared<std::string>(flush_header_str));
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const LogicalOptimize *optimize_node,
                                   std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                   i64 intent_size) {
    std::string optimize_header_str;
    if (intent_size != 0) {
        optimize_header_str = std::string(intent_size - 2, ' ');
        optimize_header_str += "-> OPTIMIZE ";
    } else {
        optimize_header_str = "OPTIMIZE ";
    }

    result->emplace_back(std::make_shared<std::string>(optimize_header_str));
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalMatch *match_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::istringstream iss(match_node->ToString(intent_size));
    std::string line;
    while (std::getline(iss, line)) {
        result->emplace_back(std::make_shared<std::string>(std::move(line)));
    }
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const LogicalMatchSparseScan *match_node,
                                   std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                   i64 intent_size) {
    std::istringstream iss(match_node->ToString(intent_size));
    std::string line;
    while (std::getline(iss, line)) {
        result->emplace_back(std::make_shared<std::string>(std::move(line)));
    }
    return Status::OK();
}

Status ExplainLogicalPlan::Explain(const LogicalMatchTensorScan *match_tensor_node,
                                   std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result,
                                   i64 intent_size) {
    std::istringstream iss(match_tensor_node->ToString(intent_size));
    std::string line;
    while (std::getline(iss, line)) {
        result->emplace_back(std::make_shared<std::string>(std::move(line)));
    }
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalFusion *fusion_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    std::istringstream iss(fusion_node->ToString(intent_size));
    std::string line;
    while (std::getline(iss, line)) {
        result->emplace_back(std::make_shared<std::string>(std::move(line)));
    }
    return Status::OK();
}

Status
ExplainLogicalPlan::Explain(const LogicalCheck *check_node, std::shared_ptr<std::vector<std::shared_ptr<std::string>>> &result, i64 intent_size) {
    switch (check_node->check_type()) {
        case CheckStmtType::kInvalid: {
            UnrecoverableError("Invalid check statement type");
            break;
        }
        case CheckStmtType::kSystem: {
            std::string check_ptr;
            if (intent_size != 0) {
                check_ptr = std::string(intent_size - 2, ' ');
                check_ptr += "-> CHECK SYSTEM ";
            } else {
                check_ptr = "CHECK SYSTEM ";
            }
            check_ptr += "(";
            check_ptr += std::to_string(check_node->node_id());
            check_ptr += ")";
            result->emplace_back(std::make_shared<std::string>(check_ptr));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
        case CheckStmtType::kTable: {
            std::string check_ptr;
            if (intent_size != 0) {
                check_ptr = std::string(intent_size - 2, ' ');
                check_ptr += "-> CHECK TABLE ";
            } else {
                check_ptr = "CHECK TABLE ";
            }
            check_ptr += "(";
            check_ptr += std::to_string(check_node->node_id());
            check_ptr += ")";
            result->emplace_back(std::make_shared<std::string>(check_ptr));

            std::string output_columns_str = std::string(intent_size, ' ');
            output_columns_str += " - output columns: [name, value]";
            result->emplace_back(std::make_shared<std::string>(output_columns_str));
            break;
        }
    }

    return Status::OK();
}

} // namespace infinity
