module;

#include <cstring>
#include <nanobind/nanobind.h>
#include <string>

export module infinity_core:wrap_infinity;

import :stl;
import :config;
import :resource_manager;
import :task_scheduler;
import :storage;
import :status;
import :query_result;
import :query_options;
import :infinity_context;
import :session;
import parsed_expr;
import search_expr;
import insert_row_expr;
import column_def;
import create_index_info;
import update_statement;
import explain_statement;
import command_statement;
import select_statement;
import :infinity;
import data_type;
import type_info;
import logical_type;
import constant_expr;
import knn_expr;
import embedding_info;
import sparse_info;
import statement_common;
import search_options;
import match_expr;
import fusion_expr;
import match_tensor_expr;
import match_sparse_expr;
import column_expr;
import function_expr;
import between_expr;
import parsed_expr;
import search_expr;
import internal_types;
import extra_ddl_info;
import embedding_type;

namespace nb = nanobind;

// wrap Infinity function for nanobind infinity
namespace infinity {

export struct ColumnField {
    LogicalType column_type;
    Vector<nb::bytes> column_vectors;
    String column_name;
};

export struct WrapColumnField {
    WrapColumnField() = default;
    WrapColumnField(const String &column_name, const String &column_data) : column_name(column_name), column_data(column_data) {}
    String column_name;
    String column_data;
};

export struct WrapEmbeddingType {
    EmbeddingDataType element_type;
    size_t dimension;
};

export struct WrapSparseType {
    EmbeddingDataType element_type;
    EmbeddingDataType index_type;
    size_t dimension;
};

export struct WrapDataType {
    LogicalType logical_type;
    WrapEmbeddingType embedding_type;
    WrapSparseType sparse_type;
    SharedPtr<WrapDataType> array_type;
};

export struct WrapConstantExpr {
    LiteralType literal_type;
    bool bool_value;
    i64 i64_value;
    f64 f64_value;
    String str_value;
    Vector<i64> i64_array_value;
    Vector<f64> f64_array_value;
    Vector<Vector<i64>> i64_tensor_value;
    Vector<Vector<f64>> f64_tensor_value;
    Vector<Vector<Vector<i64>>> i64_tensor_array_value;
    Vector<Vector<Vector<f64>>> f64_tensor_array_value;
    Vector<i64> i64_array_idx;
    Vector<SharedPtr<WrapConstantExpr>> curly_brackets_array;

    ParsedExpr *GetParsedExpr(Status &status);
};

export struct WrapColumnDef {
    i64 id;
    WrapDataType column_type;
    String column_name;
    Set<ConstraintType> constraints;
    WrapConstantExpr constant_expr;
    String comment;
};

export struct WrapCreateTableOptions {
    ConflictType conflict_type_{ConflictType::kError};
    Vector<InitParameter> properties_;
};

export struct WrapOptimizeOptions {
    String index_name_;
    Vector<InitParameter> opt_params_;
};

export struct WrapQueryResult {
    ErrorCode error_code;
    String error_msg;
    Vector<String> names;
    Vector<WrapColumnDef> column_defs;
    Vector<ColumnField> column_fields;
    String extra_result;
    // show database
    String database_name;
    String store_dir;
    BigIntT table_count;
    String comment;
    // show index
    String table_name;
    String index_name;
    String index_type;
    BigIntT deleted_rows;

    WrapQueryResult() = default;
    WrapQueryResult(ErrorCode error_code, const char *error_msg) : error_code(error_code) {
        if (error_msg != nullptr) {
            this->error_msg = String(error_msg);
        } else {
            this->error_msg = "";
        }
    }
};

export struct WrapIndexInfo {
    IndexType index_type;
    String column_name;
    Vector<InitParameter> index_param_list;
};

export struct WrapParsedExpr;

export struct WrapOrderByExpr;

export struct WrapColumnExpr {
    WrapColumnExpr() = default;
    bool star{false};
    Vector<String> names{};
    bool generated{false};

    ParsedExpr *GetParsedExpr(Status &status);
};

export struct WrapInExpr {
    WrapInExpr() = default;
    SharedPtr<WrapParsedExpr> left;
    Vector<SharedPtr<WrapParsedExpr>> arguments;
    bool in{true};

    ParsedExpr *GetParsedExpr(Status &status);
};

export struct WrapFunctionExpr {
    WrapFunctionExpr() = default;
    String func_name{};
    Vector<SharedPtr<WrapParsedExpr>> arguments;
    bool distinct{false};

    ParsedExpr *GetParsedExpr(Status &status);
};

export struct WrapBetweenExpr {
    SharedPtr<WrapParsedExpr> value{nullptr};
    SharedPtr<WrapParsedExpr> upper_bound{nullptr};
    SharedPtr<WrapParsedExpr> lower_bound{nullptr};
    ParsedExpr *GetParsedExpr(Status &status);
};

export struct EmbeddingData {
    Vector<bool> bool_array_value;
    Vector<int16_t> u8_array_value;
    Vector<int16_t> i8_array_value;
    Vector<int16_t> i16_array_value;
    Vector<int32_t> i32_array_value;
    Vector<int64_t> i64_array_value;
    Vector<double> f32_array_value;
    Vector<double> f64_array_value;
    Vector<double> f16_array_value;
    Vector<double> bf16_array_value;
};

export struct WrapKnnExpr {
    WrapColumnExpr column_expr;
    EmbeddingData embedding_data;
    EmbeddingDataType embedding_data_type{EmbeddingDataType::kElemInvalid};
    KnnDistanceType distance_type{KnnDistanceType::kInvalid};
    i64 topn{};
    Vector<InitParameter> opt_params{};
    SharedPtr<WrapParsedExpr> filter_expr{};

    ParsedExpr *GetParsedExpr(Status &status);
};

export struct WrapMatchExpr {
    String fields;
    String matching_text;
    String options_text;
    SharedPtr<WrapParsedExpr> filter_expr{};

    ParsedExpr *GetParsedExpr(Status &status);
};

export struct WrapMatchTensorExpr {
    bool own_memory;
    String search_method{};
    WrapColumnExpr column_expr;
    EmbeddingData embedding_data;
    EmbeddingDataType embedding_data_type;
    String options_text;
    SharedPtr<WrapParsedExpr> filter_expr{};

    ParsedExpr *GetParsedExpr(Status &status);
};

export struct WrapFusionExpr {
    String method{};
    String options_text{};
    WrapMatchTensorExpr match_tensor_expr;
    bool has_match_tensor_expr{false};

    ParsedExpr *GetParsedExpr(Status &status);
};

export struct WrapMatchSparseExpr {
    bool own_memory;
    WrapColumnExpr column_expr;
    WrapConstantExpr sparse_expr;
    String metric_type;
    i64 topn;
    Vector<InitParameter> opt_params{};
    SharedPtr<WrapParsedExpr> filter_expr{};
    ParsedExpr *GetParsedExpr(Status &status);
};

export struct WrapSearchExpr {
    Vector<WrapParsedExpr> match_exprs;
    Vector<WrapFusionExpr> fusion_exprs;

    ParsedExpr *GetParsedExpr(Status &status);
};

export struct WrapParsedExpr {
    WrapParsedExpr() = default;
    WrapParsedExpr(ParsedExprType expr_type) : type(expr_type) {};
    ParsedExprType type;
    WrapConstantExpr constant_expr;
    WrapColumnExpr column_expr;
    WrapFunctionExpr function_expr;
    WrapBetweenExpr between_expr;
    WrapKnnExpr knn_expr;
    WrapMatchSparseExpr match_sparse_expr;
    WrapMatchTensorExpr match_tensor_expr;
    WrapMatchExpr match_expr;
    WrapFusionExpr fusion_expr;
    WrapSearchExpr search_expr;
    WrapInExpr in_expr;
    String alias_name;

    ParsedExpr *GetParsedExpr(Status &status);
};

export struct WrapOrderByExpr {
    WrapOrderByExpr(WrapParsedExpr expr, bool asc) : expr(expr), asc(asc) {}
    WrapParsedExpr expr;
    bool asc{true};

    OrderByExpr *GetOrderByExpr(Status &status);
};

export struct WrapUpdateExpr {
    String column_name;
    WrapParsedExpr value;
    UpdateExpr *GetUpdateExpr(Status &status);
};

export struct WrapInsertRowExpr {
    Vector<String> columns;
    Vector<WrapConstantExpr> values;

    InsertRowExpr *GetInsertRowExpr(Status &status);
};

export WrapQueryResult WrapCreateDatabase(Infinity &instance, const String &db_name, const CreateDatabaseOptions &options, const String &comment);

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

export WrapQueryResult WrapShowInfo(Infinity &instance, const String &info_name);

// For embedded sqllogictest
export WrapQueryResult WrapQuery(Infinity &instance, const String &query_text);

// Database related functions
export WrapQueryResult WrapCreateTable(Infinity &instance,
                                       const String &db_name,
                                       const String &table_name,
                                       Vector<WrapColumnDef> column_defs,
                                       WrapCreateTableOptions create_table_options);

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
                                       WrapIndexInfo &wrap_index_info,
                                       const CreateIndexOptions &create_index_options,
                                       const String &index_comment);

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

export WrapQueryResult WrapShowCurrentNode(Infinity &instance);

export WrapQueryResult WrapInsert(Infinity &instance, const String &db_name, const String &table_name, Vector<WrapInsertRowExpr> &insert_rows);

export WrapQueryResult
WrapImport(Infinity &instance, const String &db_name, const String &table_name, const String &path, ImportOptions import_options);

export WrapQueryResult WrapExport(Infinity &instance,
                                  const String &db_name,
                                  const String &table_name,
                                  Vector<String> &columns,
                                  const String &path,
                                  ExportOptions export_options);

export WrapQueryResult WrapDelete(Infinity &instance, const String &db_name, const String &table_name, WrapParsedExpr *filter);

export WrapQueryResult WrapUpdate(Infinity &instance,
                                  const String &db_name,
                                  const String &table_name,
                                  WrapParsedExpr *wrap_filter,
                                  Vector<WrapUpdateExpr> *wrap_update_list);

export WrapQueryResult WrapExplain(Infinity &instance,
                                   const String &db_name,
                                   const String &table_name,
                                   ExplainType explain_type,
                                   Vector<WrapParsedExpr> select_list,
                                   Vector<WrapParsedExpr> highlight_list,
                                   Vector<WrapOrderByExpr> order_by_list,
                                   Vector<WrapParsedExpr> group_by_list,
                                   WrapParsedExpr *having_expr,
                                   WrapSearchExpr *wrap_search_expr,
                                   WrapParsedExpr *filter_expr,
                                   WrapParsedExpr *limit_expr,
                                   WrapParsedExpr *offset_expr);

export WrapQueryResult WrapSearch(Infinity &instance,
                                  const String &db_name,
                                  const String &table_name,
                                  Vector<WrapParsedExpr> select_list,
                                  Vector<WrapParsedExpr> highlight_list,
                                  Vector<WrapOrderByExpr> order_by_list,
                                  Vector<WrapParsedExpr> group_by_list,
                                  WrapParsedExpr *having_expr,
                                  bool total_hits_count_flag,
                                  WrapSearchExpr *wrap_search_expr = nullptr,
                                  WrapParsedExpr *where_expr = nullptr,
                                  WrapParsedExpr *limit_expr = nullptr,
                                  WrapParsedExpr *offset_expr = nullptr);

export WrapQueryResult WrapOptimize(Infinity &instance, const String &db_name, const String &table_name, WrapOptimizeOptions optimize_options);

export WrapQueryResult WrapAddColumns(Infinity &instance, const String &db_name, const String &table_name, Vector<WrapColumnDef> column_defs);

export WrapQueryResult WrapDropColumns(Infinity &instance, const String &db_name, const String &table_name, Vector<String> column_names);

} // namespace infinity
