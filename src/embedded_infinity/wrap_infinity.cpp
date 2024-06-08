module;
#include <cassert>
#include <cstring>
#include <string>
#include <nanobind/nanobind.h>

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
import data_block;
import value;
import data_type;
import type_info;
import logical_type;
import constant_expr;
import column_expr;
import function_expr;
import between_expr;
import parsed_expr;
import search_expr;
import infinity_exception;
import column_vector;
import internal_types;
import table_def;
import third_party;
import logger;

namespace infinity {

ParsedExpr *WrapConstantExpr::GetParsedExpr() {
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

ParsedExpr *WrapColumnExpr::GetParsedExpr() {
    auto column_expr = new ColumnExpr();
    column_expr->names_.reserve(names.size());
    for (SizeT i = 0; i < names.size(); ++i) {
        column_expr->names_.emplace_back(names[i]);
    }
    column_expr->star_ = star;
    column_expr->generated_ = generated;
    return column_expr;
}

ParsedExpr *WrapFunctionExpr::GetParsedExpr() {
    auto function_expr = new FunctionExpr();
    function_expr->func_name_ = func_name;
    function_expr->distinct_ = distinct;
    Vector<ParsedExpr *> *func_arguments = new Vector<ParsedExpr *>();
    func_arguments->reserve(arguments.size());

    for (SizeT i = 0; i < arguments.size(); ++i) {
        func_arguments->emplace_back(arguments[i]->GetParsedExpr());
    }
    function_expr->arguments_ = func_arguments;
    return function_expr;
}

ParsedExpr *WrapBetweenExpr::GetParsedExpr() {
    auto between_expr = new BetweenExpr();
    between_expr->value_ = value->GetParsedExpr();
    between_expr->lower_bound_ = lower_bound->GetParsedExpr();
    between_expr->upper_bound_ = upper_bound->GetParsedExpr();
    return between_expr;
}

ParsedExpr *WrapKnnExpr::GetParsedExpr() {
    auto knn_expr = new KnnExpr(own_memory);
    fmt::print("use unimpl knn expr!");
    return knn_expr;
}

ParsedExpr *WrapMatchExpr::GetParsedExpr() {
    auto match_expr = new MatchExpr();
    match_expr->fields_ = fields;
    match_expr->matching_text_ = matching_text;
    match_expr->options_text_ = options_text;
    return match_expr;
}

ParsedExpr *WrapFusionExpr::GetParsedExpr() {
    auto fusion_expr = new FusionExpr();
    fusion_expr->method_ = method;
    fusion_expr->options_ = MakeShared<SearchOptions>(options_text);
    return fusion_expr;
}

ParsedExpr *WrapMatchTensorExpr::GetParsedExpr() {
    auto match_tensor_expr = new MatchTensorExpr(own_memory);
    return match_tensor_expr;
}

ParsedExpr *WrapSearchExpr::GetParsedExpr() {
    auto search_expr = new SearchExpr();
    search_expr->match_exprs_.reserve(match_exprs.size());
    for (SizeT i = 0; i < match_exprs.size(); ++i) {
        search_expr->match_exprs_.emplace_back(dynamic_cast<MatchExpr *>(match_exprs[i]->GetParsedExpr()));
    }
    search_expr->knn_exprs_.reserve(knn_exprs.size());
    for (SizeT i = 0; i < knn_exprs.size(); ++i) {
        search_expr->knn_exprs_.emplace_back(dynamic_cast<KnnExpr *>(knn_exprs[i]->GetParsedExpr()));
    }
    search_expr->match_tensor_exprs_.reserve(match_tensor_exprs.size());
    for (SizeT i = 0; i < match_tensor_exprs.size(); ++i) {
        search_expr->match_tensor_exprs_.emplace_back(dynamic_cast<MatchTensorExpr *>(match_tensor_exprs[i]->GetParsedExpr()));
    }
    search_expr->fusion_exprs_.reserve(fusion_exprs.size());
    for (SizeT i = 0; i < fusion_exprs.size(); ++i) {
        search_expr->fusion_exprs_.emplace_back(dynamic_cast<FusionExpr *>(fusion_exprs[i]->GetParsedExpr()));
    }
    return search_expr;
}

ParsedExpr *WrapParsedExpr::GetParsedExpr() {
    if (type == ParsedExprType::kConstant) {
        return constant_expr.GetParsedExpr();
    } else if (type == ParsedExprType::kColumn) {
        return column_expr.GetParsedExpr();
    } else if (type == ParsedExprType::kFunction) {
        return function_expr.GetParsedExpr();
    } else if (type == ParsedExprType::kBetween) {
        return between_expr.GetParsedExpr();
    } else if (type == ParsedExprType::kKnn) {
        return knn_expr.GetParsedExpr();
    } else if (type == ParsedExprType::kMatch) {
        return match_expr.GetParsedExpr();
    } else if (type == ParsedExprType::kFusion) {
        return fusion_expr.GetParsedExpr();
    } else if (type == ParsedExprType::kSearch) {
        return search_expr.GetParsedExpr();
    } else {
        throw UnrecoverableException("Unsupported ParsedExprType");
    }
}

UpdateExpr *WrapUpdateExpr::GetUpdateExpr() {
    auto update_expr = new UpdateExpr();
    update_expr->column_name = column_name;
    update_expr->value = value.GetParsedExpr();
    return update_expr;
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
    auto &names = result.names;
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
    if (query_result.IsOk()) {
        SharedPtr<DataBlock> data_block = query_result.result_table_->GetDataBlockById(0);
        auto row_count = data_block->row_count();
        if (row_count != 3) {
            String error_message = "ShowDatabase: query result is invalid.";
            UnrecoverableError(error_message);
        }
        {
            Value value = data_block->GetValue(1, 0);
            result.database_name = value.GetVarchar();
        }
        {
            Value value = data_block->GetValue(1, 1);
            result.store_dir = value.GetVarchar();
        }
        {
            Value value = data_block->GetValue(1, 2);
            result.table_count = value.value_.big_int;
        }
    }
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
                                const CreateTableOptions &create_table_options) {
    fmt::print("begin wrap create table\n");
    Vector<TableConstraint *> constraints;
    Vector<ColumnDef *> column_defs_ptr;
    for (SizeT i = 0; i < column_defs.size(); ++i) {
        auto &wrap_column_def = column_defs[i];
        SharedPtr<TypeInfo> type_info_ptr = nullptr;
        auto& logical_type = wrap_column_def.column_type.logical_type;
        if (logical_type == LogicalType::kEmbedding || logical_type == LogicalType::kTensor || logical_type == LogicalType::kTensorArray) {
            auto &embedding_type = wrap_column_def.column_type.embedding_type;
            type_info_ptr = MakeShared<EmbeddingInfo>(embedding_type.element_type, embedding_type.dimension);
        }
        auto column_type = MakeShared<DataType>(wrap_column_def.column_type.logical_type, type_info_ptr);

        SharedPtr<ParsedExpr> default_expr(wrap_column_def.constant_expr.GetParsedExpr());

        auto column_def = new ColumnDef(wrap_column_def.id, column_type, wrap_column_def.column_name, wrap_column_def.constraints, default_expr);
        column_defs_ptr.push_back(column_def);
    }
    fmt::print("begin create table\n");
    auto query_result = instance.CreateTable(db_name, table_name, std::move(column_defs_ptr), constraints, create_table_options);
    fmt::print("create table success\n");
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

WrapQueryResult WrapListTableIndexes(Infinity &instance, const String &db_name, const String &table_name) {
    auto query_result = instance.ListTableIndexes(db_name, table_name);
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
            for (SizeT j = 0; j < wrap_index_info.index_param_list.size(); ++j) {
                auto &wrap_init_param = wrap_index_info.index_param_list[j];
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
WrapInsert(Infinity &instance, const String &db_name, const String &table_name, Vector<String> &columns, Vector<Vector<WrapConstantExpr>> &values) {
    Vector<Vector<ParsedExpr *> *> *value_ptr = new Vector<Vector<ParsedExpr *> *>(values.size());
    for (SizeT i = 0; i < values.size(); ++i) {
        auto value_list = new Vector<ParsedExpr *>(values[i].size());
        for (SizeT j = 0; j < values[i].size(); ++j) {
            auto &wrap_constant_expr = values[i][j];
            (*value_list)[j] = wrap_constant_expr.GetParsedExpr();
        }
        (*value_ptr)[i] = value_list;
    }
    Vector<String> *insert_columns = new Vector<String>(columns);

    auto query_result = instance.Insert(db_name, table_name, insert_columns, value_ptr);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapImport(Infinity &instance, const String &db_name, const String &table_name, const String &path, ImportOptions import_options) {
    auto query_result = instance.Import(db_name, table_name, path, import_options);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapDelete(Infinity &instance, const String &db_name, const String &table_name, WrapParsedExpr *wrap_filter) {
    ParsedExpr *filter = nullptr;
    if (wrap_filter != nullptr) {
        filter = wrap_filter->GetParsedExpr();
    }
    auto query_result = instance.Delete(db_name, table_name, filter);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapUpdate(Infinity &instance,
                           const String &db_name,
                           const String &table_name,
                           WrapParsedExpr *wrap_filter,
                           Vector<WrapUpdateExpr> *wrap_update_list) {
    ParsedExpr *filter = nullptr;
    if (wrap_filter != nullptr) {
        filter = wrap_filter->GetParsedExpr();
    }
    Vector<UpdateExpr *> *update_list = nullptr;
    if (wrap_update_list != nullptr) {
        update_list = new Vector<UpdateExpr *>(wrap_update_list->size());
        for (SizeT i = 0; i < wrap_update_list->size(); ++i) {
            auto &wrap_update_expr = (*wrap_update_list)[i];
            UpdateExpr *update_expr = wrap_update_expr.GetUpdateExpr();
            (*update_list)[i] = update_expr;
        }
    }
    auto query_result = instance.Update(db_name, table_name, filter, update_list);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapExplain(Infinity &instance,
                            const String &db_name,
                            const String &table_name,
                            ExplainType explain_type,
                            Vector<WrapParsedExpr> wrap_output_columns,
                            WrapSearchExpr *wrap_search_expr,
                            WrapParsedExpr *wrap_filter) {
    SearchExpr *search_expr = nullptr;
    if (wrap_search_expr != nullptr) {
        search_expr = dynamic_cast<SearchExpr *>(wrap_search_expr->GetParsedExpr());
    }
    ParsedExpr *filter = nullptr;
    if (wrap_filter != nullptr) {
        filter = wrap_filter->GetParsedExpr();
    }
    Vector<ParsedExpr *> *output_columns = new Vector<ParsedExpr *>();
    output_columns->reserve(wrap_output_columns.size());
    for (SizeT i = 0; i < wrap_output_columns.size(); ++i) {
        output_columns->emplace_back(wrap_output_columns[i].GetParsedExpr());
    }

    auto query_result = instance.Explain(db_name, table_name, explain_type, search_expr, filter, output_columns);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

// WrapSearch related function
void HandleBoolType(ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector) {
    String dst;
    dst.reserve(row_count);
    for (SizeT index = 0; index < row_count; ++index) {
        const char c = column_vector->buffer_->GetCompactBit(index) ? 1 : 0;
        dst.push_back(c);
    }
    output_column_field.column_vectors.emplace_back(dst.c_str(), dst.size());
}

void HandlePodType(ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector) {
    auto size = column_vector->data_type()->Size() * row_count;
    String dst;
    dst.resize(size);
    std::memcpy(dst.data(), column_vector->data(), size);
    fmt::print("HandlePodType func, dst size = {}\n", dst.size());
    for (SizeT i = 0; i < dst.size(); ++i) {
        fmt::print("{:02x} ", unsigned(dst[i]));
    }
    fmt::print("\n");
    fmt::print("column vector:\n {}\n", column_vector->ToString());
//    fmt::print("HandlePodType, dst = {}\n", dst.c_str());
//    std::cout << "HandlePodType, dst = " << std::endl;
//    std::cout << "size = " << size << " dst = " << dst.c_str() << std::endl;
    output_column_field.column_vectors.emplace_back(dst.c_str(), dst.size());
}

void HandleVarcharType(ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector) {
    String dst;
    SizeT total_varchar_data_size = 0;
    for (SizeT index = 0; index < row_count; ++index) {
        VarcharT &varchar = ((VarcharT *)column_vector->data())[index];
        total_varchar_data_size += varchar.length_;
    }

    auto all_size = total_varchar_data_size + row_count * sizeof(i32);
    dst.resize(all_size);

    i32 current_offset = 0;
    for (SizeT index = 0; index < row_count; ++index) {
        VarcharT &varchar = ((VarcharT *)column_vector->data())[index];
        i32 length = varchar.length_;
        if (varchar.IsInlined()) {
            std::memcpy(dst.data() + current_offset, &length, sizeof(i32));
            std::memcpy(dst.data() + current_offset + sizeof(i32), varchar.short_.data_, varchar.length_);
        } else {
            auto varchar_ptr = MakeUnique<char[]>(varchar.length_ + 1);
            column_vector->buffer_->fix_heap_mgr_->ReadFromHeap(varchar_ptr.get(),
                                                                varchar.vector_.chunk_id_,
                                                                varchar.vector_.chunk_offset_,
                                                                varchar.length_);
            std::memcpy(dst.data() + current_offset, &length, sizeof(i32));
            std::memcpy(dst.data() + current_offset + sizeof(i32), varchar_ptr.get(), varchar.length_);
        }
        current_offset += sizeof(i32) + varchar.length_;
    }

    output_column_field.column_vectors.emplace_back(dst.c_str(), dst.size());
    output_column_field.column_type = column_vector->data_type()->type();
}

void HandleEmbeddingType(ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector) {
    auto size = column_vector->data_type()->Size() * row_count;
    String dst;
    dst.resize(size);
    std::memcpy(dst.data(), column_vector->data(), size);
    output_column_field.column_vectors.emplace_back(dst.c_str(), dst.size());
    output_column_field.column_type = column_vector->data_type()->type();
}

void HandleTensorType(ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector) {
    String dst;
    SizeT total_tensor_embedding_num = 0;
    for (SizeT index = 0; index < row_count; ++index) {
        TensorT &tensor = ((TensorT *)column_vector->data())[index];
        total_tensor_embedding_num += tensor.embedding_num_;
    }
    const auto embedding_info = static_cast<const EmbeddingInfo *>(column_vector->data_type()->type_info().get());
    const auto unit_embedding_byte_size = embedding_info->Size();
    dst.resize(total_tensor_embedding_num * unit_embedding_byte_size + row_count * sizeof(i32));

    i32 current_offset = 0;
    for (SizeT index = 0; index < row_count; ++index) {
        TensorT &tensor = ((TensorT *)column_vector->data())[index];
        i32 length = tensor.embedding_num_ * unit_embedding_byte_size;
        std::memcpy(dst.data() + current_offset, &length, sizeof(i32));
        current_offset += sizeof(i32);
        const auto raw_data_ptr = column_vector->buffer_->fix_heap_mgr_->GetRawPtrFromChunk(tensor.chunk_id_, tensor.chunk_offset_);
        std::memcpy(dst.data() + current_offset, raw_data_ptr, length);
        current_offset += length;
    }

    output_column_field.column_vectors.emplace_back(dst.c_str(), dst.size());
    output_column_field.column_type = column_vector->data_type()->type();
}

void HandleTensorArrayType(ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector) {
    const auto embedding_info = static_cast<const EmbeddingInfo *>(column_vector->data_type()->type_info().get());
    const auto unit_embedding_byte_size = embedding_info->Size();
    Vector<Vector<TensorT>> tensors_v(row_count);
    SizeT expect_offset = 0;
    for (SizeT index = 0; index < row_count; ++index) {
        const auto &tensorarray = ((const TensorArrayT *)column_vector->data())[index];
        const i32 tensor_num = tensorarray.tensor_num_;
        expect_offset += sizeof(i32);
        Vector<TensorT> &tensors = tensors_v[index];
        tensors.resize(tensor_num);
        column_vector->buffer_->fix_heap_mgr_->ReadFromHeap(reinterpret_cast<char *>(tensors.data()),
                                                            tensorarray.chunk_id_,
                                                            tensorarray.chunk_offset_,
                                                            tensor_num * sizeof(TensorT));
        for (SizeT i = 0; i < tensors.size(); ++i) {
            const auto &tensor = tensors[i];
            expect_offset += sizeof(i32) + tensor.embedding_num_ * unit_embedding_byte_size;
        }
    }
    String dst;
    dst.resize(expect_offset);

    i32 current_offset = 0;
    for (SizeT i = 0; i < tensors_v.size(); ++i) {
        auto &tensors = tensors_v[i];
        const i32 tensor_num = tensors.size();
        std::memcpy(dst.data() + current_offset, &tensor_num, sizeof(i32));
        current_offset += sizeof(i32);
        for (SizeT j = 0; j < tensors.size(); ++j) {
            auto tensor = tensors[j];
            i32 length = tensor.embedding_num_ * unit_embedding_byte_size;
            std::memcpy(dst.data() + current_offset, &length, sizeof(i32));
            current_offset += sizeof(i32);
            const auto raw_data_ptr = column_vector->buffer_->fix_heap_mgr_1_->GetRawPtrFromChunk(tensor.chunk_id_, tensor.chunk_offset_);
            std::memcpy(dst.data() + current_offset, raw_data_ptr, length);
            current_offset += length;
        }
    }

    output_column_field.column_vectors.emplace_back(dst.c_str(), dst.size());
    output_column_field.column_type = column_vector->data_type()->type();
}

void HandleRowIDType(ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector) {
    auto size = column_vector->data_type()->Size() * row_count;
    String dst;
    dst.resize(size);
    std::memcpy(dst.data(), column_vector->data(), size);
    output_column_field.column_vectors.emplace_back(dst.c_str(), dst.size());
    output_column_field.column_type = column_vector->data_type()->type();
}

void ProcessColumnFieldType(ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector) {
    switch (column_vector->data_type()->type()) {
        case LogicalType::kBoolean: {
            HandleBoolType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kTinyInt:
        case LogicalType::kSmallInt:
        case LogicalType::kInteger:
        case LogicalType::kBigInt:
        case LogicalType::kHugeInt:
        case LogicalType::kFloat:
        case LogicalType::kDouble: {
            HandlePodType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kVarchar: {
            HandleVarcharType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kEmbedding: {
            HandleEmbeddingType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kTensor: {
            HandleTensorType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kTensorArray: {
            HandleTensorArrayType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kRowID: {
            HandleRowIDType(output_column_field, row_count, column_vector);
            break;
        }
        default: {
            throw UnrecoverableException("Unsupported column type");
        }
    }
}

void ProcessColumns(const SharedPtr<DataBlock> &data_block, SizeT column_count, Vector<ColumnField> &columns) {
    auto row_count = data_block->row_count();
    for (SizeT col_index = 0; col_index < column_count; ++col_index) {
        auto &result_column_vector = data_block->column_vectors[col_index];
        ColumnField &output_column_field = columns[col_index];
        output_column_field.column_type = result_column_vector->data_type()->type();
        ProcessColumnFieldType(output_column_field, row_count, result_column_vector);
    }
}

void DataTypeToWrapDataType(WrapDataType &proto_data_type, const SharedPtr<DataType> &data_type) {
    switch (data_type->type()) {
        case LogicalType::kBoolean:
        case LogicalType::kTinyInt:
        case LogicalType::kSmallInt:
        case LogicalType::kInteger:
        case LogicalType::kBigInt:
        case LogicalType::kFloat:
        case LogicalType::kDouble: {
            proto_data_type.logical_type = data_type->type();
            break;
        }
        case LogicalType::kVarchar: {
            // need physical type?
            proto_data_type.logical_type = data_type->type();
            break;
        }
        case LogicalType::kTensor:
        case LogicalType::kTensorArray:
        case LogicalType::kEmbedding: {
            proto_data_type.logical_type = data_type->type();
            WrapEmbeddingType& embedding_type = proto_data_type.embedding_type;

            auto embedding_info = static_cast<EmbeddingInfo *>(data_type->type_info().get());
            embedding_type.dimension = embedding_info->Dimension();
            embedding_type.element_type = embedding_info->Type();
            break;
        }
        default: {
            String error_message = fmt::format("Invalid logical data type: {}", data_type->ToString());
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }
}

void HandleColumnDef(WrapQueryResult &wrap_query_result, SizeT column_count, SharedPtr<TableDef> table_def, Vector<ColumnField> &all_column_vectors) {
    assert(column_count == all_column_vectors.size());

    for (SizeT col_index = 0; col_index < column_count; ++col_index) {
        auto column_def = table_def->columns()[col_index];
        WrapColumnDef proto_column_def;
        proto_column_def.id = column_def->id();
        proto_column_def.column_name = column_def->name();

        WrapDataType &proto_data_type = proto_column_def.column_type;
        DataTypeToWrapDataType(proto_data_type, column_def->type());
        // proto_data_type.logical_type = column_def->type()->type();

        wrap_query_result.column_defs.emplace_back(proto_column_def);
    }
}

void ProcessDataBlocks(QueryResult &query_result, WrapQueryResult &wrap_query_result, Vector<ColumnField> &columns) {
    SizeT blocks_count = query_result.result_table_->DataBlockCount();
    for (SizeT block_idx = 0; block_idx < blocks_count; ++block_idx) {
        auto data_block = query_result.result_table_->GetDataBlockById(block_idx);
        ProcessColumns(data_block, query_result.result_table_->ColumnCount(), columns);
    }
    HandleColumnDef(wrap_query_result, query_result.result_table_->ColumnCount(), query_result.result_table_->definition_ptr_, columns);
}

WrapQueryResult WrapSearch(Infinity &instance,
                           const String &db_name,
                           const String &table_name,
                           Vector<WrapParsedExpr> select_list,
                           WrapSearchExpr *wrap_search_expr,
                           WrapParsedExpr *where_expr,
                           WrapParsedExpr *limit_expr,
                           WrapParsedExpr *offset_expr) {
    SearchExpr *search_expr = nullptr;
    if (wrap_search_expr != nullptr) {
        search_expr = dynamic_cast<SearchExpr *>(wrap_search_expr->GetParsedExpr());
    }
    ParsedExpr *filter = nullptr;
    if (where_expr != nullptr) {
        filter = where_expr->GetParsedExpr();
    }
    Vector<ParsedExpr *> *output_columns = nullptr;

    if (select_list.empty()) {
        return WrapQueryResult(ErrorCode::kEmptySelectFields, "[error] Select fields are empty");
    } else {
        output_columns = new Vector<ParsedExpr *>();
        output_columns->reserve(select_list.size());
        for (SizeT i = 0; i < select_list.size(); ++i) {
            output_columns->emplace_back(select_list[i].GetParsedExpr());
        }
    }

    fmt::print("before search, select list size = {}\n", select_list.size());
    auto query_result = instance.Search(db_name, table_name, search_expr, filter, output_columns);
    fmt::print("after search\n");
    if (!query_result.IsOk()) {
        return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
    }
    auto wrap_query_result = WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
    auto &columns = wrap_query_result.column_fields;
    columns.resize(query_result.result_table_->ColumnCount());
    ProcessDataBlocks(query_result, wrap_query_result, columns);
    return wrap_query_result;
}

WrapQueryResult WrapShowColumns(Infinity &instance, const String &db_name, const String &table_name) {
    auto query_result = instance.ShowColumns(db_name, table_name);
    if (!query_result.IsOk()) {
        return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
    }
    auto wrap_query_result = WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
    auto &columns = wrap_query_result.column_fields;
    columns.resize(query_result.result_table_->ColumnCount());
    ProcessDataBlocks(query_result, wrap_query_result, columns);
    return wrap_query_result;
}

WrapQueryResult WrapShowTables(Infinity &instance, const String &db_name) {
    auto query_result = instance.ShowTables(db_name);
    if (!query_result.IsOk()) {
        return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
    }
    auto wrap_query_result = WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
    auto &columns = wrap_query_result.column_fields;
    columns.resize(query_result.result_table_->ColumnCount());
    ProcessDataBlocks(query_result, wrap_query_result, columns);
    return wrap_query_result;
}

WrapQueryResult WrapOptimize(Infinity &instance, const String &db_name, const String &table_name) {
    auto query_result = instance.Optimize(db_name, table_name);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

} // namespace infinity
