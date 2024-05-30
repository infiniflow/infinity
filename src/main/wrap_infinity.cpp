module;

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

namespace infinity {
WrapQueryResult WrapCreateDatabase(Infinity &instance, const String &db_name, const CreateDatabaseOptions &options) {
    WrapQueryResult result;
    auto query_result = instance.CreateDatabase(db_name, options);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapDropDatabase(Infinity &instance, const String &db_name, const DropDatabaseOptions &options) {
    WrapQueryResult result;
    auto query_result = instance.DropDatabase(db_name, options);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapListDatabases(Infinity &instance) {
    WrapQueryResult result;
    auto query_result = instance.ListDatabases();
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapGetDatabase(Infinity &instance, const String &db_name) {
    WrapQueryResult result;
    auto query_result = instance.GetDatabase(db_name);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapShowDatabase(Infinity &instance, const String &db_name) {
    WrapQueryResult result;
    auto query_result = instance.ShowDatabase(db_name);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapFlush(Infinity &instance) {
    WrapQueryResult result;
    auto query_result = instance.Flush();
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapSetVariableOrConfig(Infinity &instance, const String &name, bool value, SetScope scope) {
    WrapQueryResult result;
    auto query_result = instance.SetVariableOrConfig(name, value, scope);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapSetVariableOrConfig(Infinity &instance, const String &name, i64 value, SetScope scope) {
    WrapQueryResult result;
    auto query_result = instance.SetVariableOrConfig(name, value, scope);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapSetVariableOrConfig(Infinity &instance, const String &name, double value, SetScope scope) {
    WrapQueryResult result;
    auto query_result = instance.SetVariableOrConfig(name, value, scope);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapSetVariableOrConfig(Infinity &instance, const String &name, String value, SetScope scope) {
    WrapQueryResult result;
    auto query_result = instance.SetVariableOrConfig(name, value, scope);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapShowVariable(Infinity &instance, const String &variable_name, SetScope scope) {
    WrapQueryResult result;
    auto query_result = instance.ShowVariable(variable_name, scope);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapShowVariables(Infinity &instance, SetScope scope) {
    WrapQueryResult result;
    auto query_result = instance.ShowVariables(scope);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapShowConfig(Infinity &instance, const String &config_name) {
    WrapQueryResult result;
    auto query_result = instance.ShowConfig(config_name);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapShowConfigs(Infinity &instance) {
    WrapQueryResult result;
    auto query_result = instance.ShowConfigs();
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapQuery(Infinity &instance, const String &query_text) {
    WrapQueryResult result;
    auto query_result = instance.Query(query_text);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapCreateTable(Infinity &instance,
                                const String &db_name,
                                const String &table_name,
                                Vector<ColumnDef *> column_defs,
                                Vector<TableConstraint *> constraints,
                                const CreateTableOptions &create_table_options) {
    WrapQueryResult result;
    auto query_result = instance.CreateTable(db_name, table_name, column_defs, constraints, create_table_options);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapDropTable(Infinity &instance, const String &db_name, const String &table_name, const DropTableOptions &drop_table_options) {
    WrapQueryResult result;
    auto query_result = instance.DropTable(db_name, table_name, drop_table_options);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapListTables(Infinity &instance, const String &db_name) {
    WrapQueryResult result;
    auto query_result = instance.ListTables(db_name);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapShowTable(Infinity &instance, const String &db_name, const String &table_name) {
    WrapQueryResult result;
    auto query_result = instance.ShowTable(db_name, table_name);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapShowColumns(Infinity &instance, const String &db_name, const String &table_name) {
    WrapQueryResult result;
    auto query_result = instance.ShowColumns(db_name, table_name);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapListTableIndexes(Infinity &instance, const String &db_name, const String &table_name) {
    WrapQueryResult result;
    auto query_result = instance.ListTableIndexes(db_name, table_name);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapShowTables(Infinity &instance, const String &db_name) {
    WrapQueryResult result;
    auto query_result = instance.ShowTables(db_name);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapGetTable(Infinity &instance, const String &db_name, const String &table_name) {
    WrapQueryResult result;
    auto query_result = instance.GetTable(db_name, table_name);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapCreateIndex(Infinity &instance,
                                const String &db_name,
                                const String &table_name,
                                const String &index_name,
                                Vector<IndexInfo *> *index_info_list,
                                const CreateIndexOptions &create_index_options) {
    WrapQueryResult result;
    auto query_result = instance.CreateIndex(db_name, table_name, index_name, index_info_list, create_index_options);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapDropIndex(Infinity &instance,
                              const String &db_name,
                              const String &table_name,
                              const String &index_name,
                              const DropIndexOptions &drop_index_option) {
    WrapQueryResult result;
    auto query_result = instance.DropIndex(db_name, table_name, index_name, drop_index_option);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapShowIndex(Infinity &instance, const String &db_name, const String &table_name, const String &index_name) {
    WrapQueryResult result;
    auto query_result = instance.ShowIndex(db_name, table_name, index_name);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapShowSegment(Infinity &instance, const String &db_name, const String &table_name, const SegmentID &segment_id) {
    WrapQueryResult result;
    auto query_result = instance.ShowSegment(db_name, table_name, segment_id);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapShowSegments(Infinity &instance, const String &db_name, const String &table_name) {
    WrapQueryResult result;
    auto query_result = instance.ShowSegments(db_name, table_name);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult
WrapShowBlock(Infinity &instance, const String &db_name, const String &table_name, const SegmentID &segment_id, const BlockID &block_id) {
    WrapQueryResult result;
    auto query_result = instance.ShowBlock(db_name, table_name, segment_id, block_id);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapShowBlocks(Infinity &instance, const String &db_name, const String &table_name, const SegmentID &segment_id) {
    WrapQueryResult result;
    auto query_result = instance.ShowBlocks(db_name, table_name, segment_id);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapShowBlockColumn(Infinity &instance,
                                    const String &db_name,
                                    const String &table_name,
                                    const SegmentID &segment_id,
                                    const BlockID &block_id,
                                    const SizeT &column_id) {
    WrapQueryResult result;
    auto query_result = instance.ShowBlockColumn(db_name, table_name, segment_id, block_id, column_id);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult
WrapInsert(Infinity& instance, const String& db_name, const String& table_name, Vector<String> columns, Vector<Vector<ParsedExpr*>> values) {
    Vector<Vector<ParsedExpr*>*> value_ptr;
    for (SizeT i = 0; i < values.size(); ++i) {
        value_ptr.push_back(&values[i]);
    }
    auto query_result = instance.Insert(db_name, table_name, &columns, &value_ptr);

    WrapQueryResult result;
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapImport(Infinity &instance, const String &db_name, const String &table_name, const String &path, ImportOptions import_options) {
    WrapQueryResult result;
    auto query_result = instance.Import(db_name, table_name, path, import_options);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapDelete(Infinity &instance, const String &db_name, const String &table_name, ParsedExpr *filter) {
    WrapQueryResult result;
    auto query_result = instance.Delete(db_name, table_name, filter);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult
WrapUpdate(Infinity &instance, const String &db_name, const String &table_name, ParsedExpr *filter, Vector<UpdateExpr *> *update_list) {
    WrapQueryResult result;
    auto query_result = instance.Update(db_name, table_name, filter, update_list);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapExplain(Infinity &instance,
                            const String &db_name,
                            const String &table_name,
                            ExplainType explain_type,
                            SearchExpr *search_expr,
                            ParsedExpr *filter,
                            Vector<ParsedExpr *> *output_columns) {
    WrapQueryResult result;
    auto query_result = instance.Explain(db_name, table_name, explain_type, search_expr, filter, output_columns);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapSearch(Infinity &instance,
                           const String &db_name,
                           const String &table_name,
                           SearchExpr *search_expr,
                           ParsedExpr *filter,
                           Vector<ParsedExpr *> *output_columns) {
    WrapQueryResult result;
    auto query_result = instance.Search(db_name, table_name, search_expr, filter, output_columns);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

WrapQueryResult WrapOptimize(Infinity &instance, const String &db_name, const String &table_name) {
    WrapQueryResult result;
    auto query_result = instance.Optimize(db_name, table_name);
    result.error_code = query_result.ErrorCode();
    result.error_msg = query_result.ErrorMsg();
    return result;
}

} // namespace infinity
