module;

#include <cstring>

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
import data_type;
import type_info;
import logical_type;
import constant_expr;
import knn_expr;
import embedding_info;
import statement_common;
import search_options;
import match_expr;
import fusion_expr;
import match_tensor_expr;

// wrap Infinity function for nanobind infinity
namespace infinity {

export struct WrapQueryResult {
    ErrorCode error_code;
    String error_msg;
    Vector<String> names;
    WrapQueryResult() = default;
    WrapQueryResult(ErrorCode error_code, const char *error_msg) : error_code(error_code) {
        if (error_msg != nullptr) {
            this->error_msg = String(error_msg);
        } else {
            this->error_msg = "";
        }
    }
};

export struct WrapEmbeddingType {
    EmbeddingDataType element_type;
    size_t dimension;
};

export struct WrapDataType {
    LogicalType logical_type;
    WrapEmbeddingType embedding_type;
};

export struct WrapConstantExpr {
    LiteralType literal_type;
    bool bool_value;
    i64 i64_value;
    f64 f64_value;
    String str_value;
    Vector<i64> i64_array_value;
    Vector<f64> f64_array_value;

    ParsedExpr* GetParsedExpr();
};

export struct WrapColumnDef {
    i64 id;
    WrapDataType column_type;
    String column_name;
    Set<ConstraintType> constraints;
    WrapConstantExpr constant_expr;

    // ParsedExpr* default_expr;
};

export struct WrapIndexInfo {
    IndexType index_type;
    String column_name;
    Vector<InitParameter> index_param_list;
};

export struct WrapColumnExpr {
    Vector<String> names{};
    bool star{false};
    bool generated{false};
};

export struct WrapFunctionExpr {
    String func_name{};
    Vector<ParsedExpr *> *arguments{nullptr};
    bool distinct{false};
};

export struct WrapBetweenExpr {
    ParsedExpr *value{nullptr};
    ParsedExpr *upper_bound{nullptr};
    ParsedExpr *lower_bound{nullptr};
};

export struct WrapKnnExpr {
    const bool own_memory;

    ParsedExpr *column_expr{};
    void *embedding_data_ptr{}; // Pointer to the embedding data ,the data type include float, int ,char ...., so we use void* here
    i64 dimension{};
    EmbeddingDataType embedding_data_type{EmbeddingDataType::kElemInvalid};
    KnnDistanceType distance_type{KnnDistanceType::kInvalid};
    i64 topn{};
    Vector<InitParameter *> *opt_params{};
};

export struct WrapMatchExpr {
    String fields_;
    String matching_text_;
    String options_text_;
};

export struct WrapFusionExpr {
    String method_{};
    SharedPtr<SearchOptions> options_{};
};

export struct WrapSearchExpr {
    Vector<MatchExpr *> match_exprs_{};
    Vector<KnnExpr *> knn_exprs_{};
    Vector<MatchTensorExpr *> match_tensor_exprs_{};
    FusionExpr *fusion_expr_{};
};

export struct WrapParsedExpr {
    ParsedExprType type;
    WrapConstantExpr constant_expr;
    WrapColumnExpr column_expr;
    WrapFunctionExpr function_expr;
    WrapBetweenExpr between_expr;
    WrapKnnExpr knn_expr;
    WrapMatchExpr match_expr;
    WrapFusionExpr fusion_expr;
    WrapSearchExpr search_expr;
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
                                       Vector<WrapColumnDef> column_defs,
//                                       Vector<TableConstraint *> constraints,
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
                                       Vector<WrapIndexInfo> &wrap_index_info_list,
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
WrapInsert(Infinity& instance, const String& db_name, const String& table_name, Vector<String>& columns, Vector<Vector<WrapConstantExpr>>& values);

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