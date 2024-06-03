module;
#include <cstring>

module wrap_infinity;

import stl;
import config;
import resource_manager;
import task_scheduler;
import storage;
import status;
import query_result;
import query_options;
import infinity_context;
import session;
import parsed_expr;
import search_expr;
import column_def;
import create_index_info;
import update_statement;
import explain_statement;
import command_statement;
import infinity;
import third_party;
import data_block;
import value;
import data_type;
import type_info;
import logical_type;
import constant_expr;

namespace infinity {

ParsedExpr* WrapConstantExpr::GetParsedExpr() {
    auto constant_expr = new ConstantExpr(literal_type);
    switch (literal_type) {
        case LiteralType::kBoolean: {
            constant_expr->bool_value_ = bool_value;
            return constant_expr;
        }
        case LiteralType::kDouble: {
            constant_expr->double_value_ = f64_value;
            return constant_expr;
        }
        case LiteralType::kString: {
            constant_expr->str_value_ = strdup(str_value.c_str());
            return constant_expr;
        }
        case LiteralType::kInteger: {
            constant_expr->integer_value_ = i64_value;
            return constant_expr;
        }
        case LiteralType::kNull: {
            return constant_expr;
        }
        case LiteralType::kIntegerArray: {
            constant_expr->long_array_.reserve(i64_array_value.size());
            for (SizeT i = 0; i < i64_array_value.size(); ++i) {
                constant_expr->long_array_.emplace_back(i64_array_value[i]);
            }
            return constant_expr;
        }
        case LiteralType::kDoubleArray: {
            constant_expr->double_array_.reserve(f64_array_value.size());
            for (SizeT i = 0; i < f64_array_value.size(); ++i) {
                constant_expr->double_array_.emplace_back(f64_array_value[i]);
            }
            return constant_expr;
        }
        default: {
            return new ConstantExpr(LiteralType::kNull);
        }
    }
    return constant_expr;

}

WrapQueryResult WrapCreateDatabase(Infinity &instance, const String &db_name, const CreateDatabaseOptions &options) {
    auto query_result = instance.CreateDatabase(db_name, options);
    WrapQueryResult result(query_result.ErrorCode(), query_result.ErrorMsg());
    return result;
}

WrapQueryResult WrapDropDatabase(Infinity &instance, const String &db_name, const DropDatabaseOptions &options) {
    auto query_result = instance.DropDatabase(db_name, options);
    WrapQueryResult result(query_result.ErrorCode(), query_result.ErrorMsg());
    return result;
}

WrapQueryResult WrapListDatabases(Infinity &instance) {
    auto query_result = instance.ListDatabases();
    WrapQueryResult result(query_result.ErrorCode(), query_result.ErrorMsg());

    SharedPtr<DataBlock> data_block = query_result.result_table_->GetDataBlockById(0);
    auto& names = result.names;
    names.resize(data_block->row_count());
    for (SizeT i = 0; i < names.size(); ++i) {
        Value value = data_block->GetValue(0, i);
        names[i] = value.GetVarchar();
    }
    return result;
}

WrapQueryResult WrapGetDatabase(Infinity &instance, const String &db_name) {
    auto query_result = instance.GetDatabase(db_name);
    WrapQueryResult result(query_result.ErrorCode(), query_result.ErrorMsg());
    return result;
}

WrapQueryResult WrapShowDatabase(Infinity &instance, const String &db_name) {
    auto query_result = instance.ShowDatabase(db_name);
    WrapQueryResult result(query_result.ErrorCode(), query_result.ErrorMsg());
    return result;
}

WrapQueryResult WrapFlush(Infinity &instance) {
    auto query_result = instance.Flush();
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapSetVariableOrConfig(Infinity &instance, const String &name, bool value, SetScope scope) {
    auto query_result = instance.SetVariableOrConfig(name, value, scope);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapSetVariableOrConfig(Infinity &instance, const String &name, i64 value, SetScope scope) {
    auto query_result = instance.SetVariableOrConfig(name, value, scope);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapSetVariableOrConfig(Infinity &instance, const String &name, double value, SetScope scope) {
    auto query_result = instance.SetVariableOrConfig(name, value, scope);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapSetVariableOrConfig(Infinity &instance, const String &name, String value, SetScope scope) {
    auto query_result = instance.SetVariableOrConfig(name, value, scope);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapShowVariable(Infinity &instance, const String &variable_name, SetScope scope) {
    auto query_result = instance.ShowVariable(variable_name, scope);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapShowVariables(Infinity &instance, SetScope scope) {
    auto query_result = instance.ShowVariables(scope);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapShowConfig(Infinity &instance, const String &config_name) {
    auto query_result = instance.ShowConfig(config_name);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapShowConfigs(Infinity &instance) {
    auto query_result = instance.ShowConfigs();
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapQuery(Infinity &instance, const String &query_text) {
    auto query_result = instance.Query(query_text);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapCreateTable(Infinity &instance,
                                const String &db_name,
                                const String &table_name,
                                Vector<WrapColumnDef> column_defs,
                                // Vector<TableConstraint *> constraints,
                                const CreateTableOptions &create_table_options) {
    // std::cout << "receive CreateTable success, column size: " << column_defs.size() << std::endl;
    // return WrapQueryResult{ErrorCode::kOk, ""};
    Vector<TableConstraint *> constraints;
    Vector<ColumnDef *> column_defs_ptr;
    for (SizeT i = 0; i < column_defs.size(); ++i) {
        auto& wrap_column_def = column_defs[i];
        SharedPtr<TypeInfo> type_info_ptr = nullptr;
        if (wrap_column_def.column_type.logical_type == LogicalType::kEmbedding) {
            auto &embedding_type = wrap_column_def.column_type.embedding_type;
            type_info_ptr = MakeShared<EmbeddingInfo>(embedding_type.element_type, embedding_type.dimension);
        }
        auto column_type = MakeShared<DataType>(wrap_column_def.column_type.logical_type, type_info_ptr);

        SharedPtr<ParsedExpr> default_expr(wrap_column_def.constant_expr.GetParsedExpr());

        auto column_def = new ColumnDef(wrap_column_def.id, column_type, wrap_column_def.column_name, wrap_column_def.constraints, default_expr);
        column_defs_ptr.push_back(column_def);
    }
    auto query_result = instance.CreateTable(db_name, table_name, std::move(column_defs_ptr), constraints, create_table_options);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapDropTable(Infinity &instance, const String &db_name, const String &table_name, const DropTableOptions &drop_table_options) {
    auto query_result = instance.DropTable(db_name, table_name, drop_table_options);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapListTables(Infinity &instance, const String &db_name) {
    auto query_result = instance.ListTables(db_name);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapShowTable(Infinity &instance, const String &db_name, const String &table_name) {
    auto query_result = instance.ShowTable(db_name, table_name);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapShowColumns(Infinity &instance, const String &db_name, const String &table_name) {
    auto query_result = instance.ShowColumns(db_name, table_name);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapListTableIndexes(Infinity &instance, const String &db_name, const String &table_name) {
    auto query_result = instance.ListTableIndexes(db_name, table_name);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapShowTables(Infinity &instance, const String &db_name) {
    auto query_result = instance.ShowTables(db_name);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapGetTable(Infinity &instance, const String &db_name, const String &table_name) {
    auto query_result = instance.GetTable(db_name, table_name);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapCreateIndex(Infinity &instance,
                                const String &db_name,
                                const String &table_name,
                                const String &index_name,
                                Vector<WrapIndexInfo> &wrap_index_info_list,
                                const CreateIndexOptions &create_index_options) {
    auto index_info_list = new Vector<IndexInfo *>();
    for (SizeT i = 0; i < wrap_index_info_list.size(); ++i) {
        auto wrap_index_info = wrap_index_info_list[i];
        auto index_info = new IndexInfo();
        index_info->index_type_ = wrap_index_info.index_type;
        index_info->column_name_ = wrap_index_info.column_name;
        {
            auto index_param_list = new Vector<InitParameter *>();
            for(SizeT j = 0; j < wrap_index_info.index_param_list.size(); ++j) {
                auto& wrap_init_param = wrap_index_info.index_param_list[j];
                index_param_list->emplace_back(new InitParameter(wrap_init_param.param_name_, wrap_init_param.param_value_));
            }
            index_info->index_param_list_ = index_param_list;
        }
        index_info_list->emplace_back(index_info);
    }
    auto query_result = instance.CreateIndex(db_name, table_name, index_name, index_info_list, create_index_options);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapDropIndex(Infinity &instance,
                              const String &db_name,
                              const String &table_name,
                              const String &index_name,
                              const DropIndexOptions &drop_index_option) {
    auto query_result = instance.DropIndex(db_name, table_name, index_name, drop_index_option);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapShowIndex(Infinity &instance, const String &db_name, const String &table_name, const String &index_name) {
    auto query_result = instance.ShowIndex(db_name, table_name, index_name);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapShowSegment(Infinity &instance, const String &db_name, const String &table_name, const SegmentID &segment_id) {
    auto query_result = instance.ShowSegment(db_name, table_name, segment_id);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapShowSegments(Infinity &instance, const String &db_name, const String &table_name) {
    auto query_result = instance.ShowSegments(db_name, table_name);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult
WrapShowBlock(Infinity &instance, const String &db_name, const String &table_name, const SegmentID &segment_id, const BlockID &block_id) {
    auto query_result = instance.ShowBlock(db_name, table_name, segment_id, block_id);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapShowBlocks(Infinity &instance, const String &db_name, const String &table_name, const SegmentID &segment_id) {
    auto query_result = instance.ShowBlocks(db_name, table_name, segment_id);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapShowBlockColumn(Infinity &instance,
                                    const String &db_name,
                                    const String &table_name,
                                    const SegmentID &segment_id,
                                    const BlockID &block_id,
                                    const SizeT &column_id) {
    auto query_result = instance.ShowBlockColumn(db_name, table_name, segment_id, block_id, column_id);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult
WrapInsert(Infinity& instance, const String& db_name, const String& table_name, Vector<String>& columns, Vector<Vector<WrapConstantExpr>>& values) {
    Vector<Vector<ParsedExpr *> *> *value_ptr = new Vector<Vector<ParsedExpr *> *>(values.size());
    for (SizeT i = 0; i < values.size(); ++i) {
        auto value_list = new Vector<ParsedExpr *>(values[i].size());
        for (SizeT j = 0; j < values[i].size(); ++j) {
            auto& wrap_constant_expr = values[i][j];
            (*value_list)[j] = wrap_constant_expr.GetParsedExpr();
        }
        (*value_ptr)[i] = value_list;
    }
    Vector<String> *insert_columns = new Vector<String>(columns);

    auto query_result = instance.Insert(db_name, table_name, insert_columns, value_ptr);
    std::cout << "Wrap Insert success" << std::endl;
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapImport(Infinity &instance, const String &db_name, const String &table_name, const String &path, ImportOptions import_options) {
    auto query_result = instance.Import(db_name, table_name, path, import_options);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapDelete(Infinity &instance, const String &db_name, const String &table_name, ParsedExpr *filter) {
    auto query_result = instance.Delete(db_name, table_name, filter);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult
WrapUpdate(Infinity &instance, const String &db_name, const String &table_name, ParsedExpr *filter, Vector<UpdateExpr *> *update_list) {
    auto query_result = instance.Update(db_name, table_name, filter, update_list);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapExplain(Infinity &instance,
                            const String &db_name,
                            const String &table_name,
                            ExplainType explain_type,
                            SearchExpr *search_expr,
                            ParsedExpr *filter,
                            Vector<ParsedExpr *> *output_columns) {
    auto query_result = instance.Explain(db_name, table_name, explain_type, search_expr, filter, output_columns);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapSearch(Infinity &instance,
                           const String &db_name,
                           const String &table_name,
                           SearchExpr *search_expr,
                           ParsedExpr *filter,
                           Vector<ParsedExpr *> *output_columns) {
    auto query_result = instance.Search(db_name, table_name, search_expr, filter, output_columns);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapOptimize(Infinity &instance, const String &db_name, const String &table_name) {
    auto query_result = instance.Optimize(db_name, table_name);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

} // namespace infinity
