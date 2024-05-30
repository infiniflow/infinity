module;

export module wrap_infinity;

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

// wrap Infinity function for nanobind infinity
namespace infinity {

export struct WrapQueryResult {
    ErrorCode error_code;
    String error_msg;
};

export WrapQueryResult WrapCreateDatabase(Infinity &instance, const String &db_name, const CreateDatabaseOptions &options);

export WrapQueryResult WrapDropDatabase(Infinity &instance, const String &db_name, const DropDatabaseOptions &options);

export WrapQueryResult WrapListDatabases(Infinity &instance);

export WrapQueryResult WrapGetDatabase(Infinity &instance, const String &db_name);

export WrapQueryResult WrapShowDatabase(Infinity &instance, const String &db_name);

export WrapQueryResult WrapFlush(Infinity &instance);

export WrapQueryResult WrapSetVariableOrConfig(Infinity &instance, const String &name, bool value, SetScope scope);

export WrapQueryResult WrapSetVariableOrConfig(Infinity &instance, const String &name, i64 value, SetScope scope);

export WrapQueryResult WrapSetVariableOrConfig(Infinity &instance, const String &name, double value, SetScope scope);

export WrapQueryResult WrapSetVariableOrConfig(Infinity &instance, const String &name, String value, SetScope scope);

export WrapQueryResult WrapShowVariable(Infinity &instance, const String &variable_name, SetScope scope);

export WrapQueryResult WrapShowVariables(Infinity &instance, SetScope scope);

export WrapQueryResult WrapShowConfig(Infinity &instance, const String &config_name);

export WrapQueryResult WrapShowConfigs(Infinity &instance);

// For embedded sqllogictest
export WrapQueryResult WrapQuery(Infinity &instance, const String &query_text);

// Database related functions
export WrapQueryResult WrapCreateTable(Infinity &instance,
                                       const String &db_name,
                                       const String &table_name,
                                       Vector<ColumnDef *> column_defs,
                                       Vector<TableConstraint *> constraints,
                                       const CreateTableOptions &create_table_options);

export WrapQueryResult WrapDropTable(Infinity &instance, const String &db_name, const String &table_name, const DropTableOptions &drop_table_options);

export WrapQueryResult WrapListTables(Infinity &instance, const String &db_name);

export WrapQueryResult WrapShowTable(Infinity &instance, const String &db_name, const String &table_name);

export WrapQueryResult WrapShowColumns(Infinity &instance, const String &db_name, const String &table_name);

export WrapQueryResult WrapListTableIndexes(Infinity &instance, const String &db_name, const String &table_name);

export WrapQueryResult WrapShowTables(Infinity &instance, const String &db_name);

export WrapQueryResult WrapGetTable(Infinity &instance, const String &db_name, const String &table_name);

// Table related functions
export WrapQueryResult WrapCreateIndex(Infinity &instance,
                                       const String &db_name,
                                       const String &table_name,
                                       const String &index_name,
                                       Vector<IndexInfo *> *index_info_list,
                                       const CreateIndexOptions &create_index_options);

export WrapQueryResult WrapDropIndex(Infinity &instance,
                                     const String &db_name,
                                     const String &table_name,
                                     const String &index_name,
                                     const DropIndexOptions &drop_index_option);

export WrapQueryResult WrapShowIndex(Infinity &instance, const String &db_name, const String &table_name, const String &index_name);

export WrapQueryResult WrapShowSegment(Infinity &instance, const String &db_name, const String &table_name, const SegmentID &segment_id);

export WrapQueryResult WrapShowSegments(Infinity &instance, const String &db_name, const String &table_name);

export WrapQueryResult
WrapShowBlock(Infinity &instance, const String &db_name, const String &table_name, const SegmentID &segment_id, const BlockID &block_id);

export WrapQueryResult WrapShowBlocks(Infinity &instance, const String &db_name, const String &table_name, const SegmentID &segment_id);

export WrapQueryResult WrapShowBlockColumn(Infinity &instance,
                                           const String &db_name,
                                           const String &table_name,
                                           const SegmentID &segment_id,
                                           const BlockID &block_id,
                                           const SizeT &column_id);

export WrapQueryResult
WrapInsert(Infinity& instance, const String& db_name, const String& table_name, Vector<String> columns, Vector<Vector<ParsedExpr*>> values);

export WrapQueryResult
WrapImport(Infinity &instance, const String &db_name, const String &table_name, const String &path, ImportOptions import_options);

export WrapQueryResult WrapDelete(Infinity &instance, const String &db_name, const String &table_name, ParsedExpr *filter);

export WrapQueryResult
WrapUpdate(Infinity &instance, const String &db_name, const String &table_name, ParsedExpr *filter, Vector<UpdateExpr *> *update_list);

export WrapQueryResult WrapExplain(Infinity &instance,
                                   const String &db_name,
                                   const String &table_name,
                                   ExplainType explain_type,
                                   SearchExpr *search_expr,
                                   ParsedExpr *filter,
                                   Vector<ParsedExpr *> *output_columns);

export WrapQueryResult WrapSearch(Infinity &instance,
                                  const String &db_name,
                                  const String &table_name,
                                  SearchExpr *search_expr,
                                  ParsedExpr *filter,
                                  Vector<ParsedExpr *> *output_columns);

export WrapQueryResult WrapOptimize(Infinity &instance, const String &db_name, const String &table_name);

} // namespace infinity