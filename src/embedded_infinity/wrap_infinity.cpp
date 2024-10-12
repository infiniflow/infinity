module;
#include <cassert>
#include <cstring>
#include <nanobind/nanobind.h>
#include <string>

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
import in_expr;
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
import query_options;
import defer_op;

namespace infinity {

ParsedExpr *WrapConstantExpr::GetParsedExpr(Status &status) {
    status.code_ = ErrorCode::kOk;

    auto constant_expr = MakeUnique<ConstantExpr>(literal_type);
    switch (literal_type) {
        case LiteralType::kBoolean: {
            constant_expr->bool_value_ = bool_value;
            break;
        }
        case LiteralType::kDouble: {
            constant_expr->double_value_ = f64_value;
            break;
        }
        case LiteralType::kString: {
            constant_expr->str_value_ = strdup(str_value.c_str());
            break;
        }
        case LiteralType::kInteger: {
            constant_expr->integer_value_ = i64_value;
            break;
        }
        case LiteralType::kNull: {
            break;
        }
        case LiteralType::kIntegerArray: {
            constant_expr->long_array_ = i64_array_value;
            break;
        }
        case LiteralType::kDoubleArray: {
            constant_expr->double_array_ = f64_array_value;
            break;
        }
        case LiteralType::kLongSparseArray: {
            constant_expr->long_sparse_array_.first.reserve(i64_array_idx.size());
            constant_expr->long_sparse_array_.second.reserve(i64_array_value.size());
            for (SizeT i = 0; i < i64_array_idx.size(); ++i) {
                constant_expr->long_sparse_array_.first.emplace_back(i64_array_idx[i]);
                constant_expr->long_sparse_array_.second.emplace_back(i64_array_value[i]);
            }
            break;
        }
        case LiteralType::kDoubleSparseArray: {
            constant_expr->double_sparse_array_.first.reserve(i64_array_idx.size());
            constant_expr->double_sparse_array_.second.reserve(f64_array_value.size());
            for (SizeT i = 0; i < i64_array_idx.size(); ++i) {
                constant_expr->double_sparse_array_.first.emplace_back(i64_array_idx[i]);
                constant_expr->double_sparse_array_.second.emplace_back(f64_array_value[i]);
            }
            break;
        }
        case LiteralType::kEmptyArray: {
            break;
        }
        case LiteralType::kSubArrayArray: {
            auto create_vv = []<typename T>(const std::vector<std::vector<T>> &vv, ConstantExpr *output_constant_expr) {
                static_assert(std::is_same_v<T, int64_t> || std::is_same_v<T, double>);
                constexpr auto basic_type = [] {
                    if constexpr (std::is_same_v<T, int64_t>) {
                        return LiteralType::kIntegerArray;
                    } else if constexpr (std::is_same_v<T, double>) {
                        return LiteralType::kDoubleArray;
                    } else {
                        static_assert(false, "unexpected type");
                        return LiteralType::kEmptyArray;
                    }
                }();
                output_constant_expr->sub_array_array_.reserve(vv.size());
                for (const auto &value_2 : vv) {
                    auto parsed_expr_2 = MakeShared<ConstantExpr>(basic_type);
                    if constexpr (std::is_same_v<T, int64_t>) {
                        parsed_expr_2->long_array_ = value_2;
                    } else if constexpr (std::is_same_v<T, double>) {
                        parsed_expr_2->double_array_ = value_2;
                    } else {
                        static_assert(false, "unexpected type");
                    }
                    output_constant_expr->sub_array_array_.emplace_back(std::move(parsed_expr_2));
                }
            };
            auto create_vvv = [&constant_expr, &create_vv](const auto &vvv) {
                constant_expr->sub_array_array_.reserve(vvv.size());
                for (const auto &value_1 : vvv) {
                    auto parsed_expr_1 = MakeShared<ConstantExpr>(LiteralType::kSubArrayArray);
                    create_vv(value_1, parsed_expr_1.get());
                    constant_expr->sub_array_array_.emplace_back(std::move(parsed_expr_1));
                }
            };
            const auto total_have_val = static_cast<int>(!i64_tensor_value.empty()) + static_cast<int>(!f64_tensor_value.empty()) +
                                        static_cast<int>(!i64_tensor_array_value.empty()) + static_cast<int>(!f64_tensor_array_value.empty());
            if (total_have_val != 1) {
                status = Status::InvalidConstantType();
                return nullptr;
            }
            if (!i64_tensor_value.empty()) {
                create_vv(i64_tensor_value, constant_expr.get());
            } else if (!f64_tensor_value.empty()) {
                create_vv(f64_tensor_value, constant_expr.get());
            } else if (!i64_tensor_array_value.empty()) {
                create_vvv(i64_tensor_array_value);
            } else if (!f64_tensor_array_value.empty()) {
                create_vvv(f64_tensor_array_value);
            }
            break;
        }
        case LiteralType::kDate: {
            constant_expr->date_value_ = strdup(str_value.c_str());
            break;
        }
        case LiteralType::kTime: {
            constant_expr->date_value_ = strdup(str_value.c_str());
            break;
        }
        case LiteralType::kDateTime: {
            constant_expr->date_value_ = strdup(str_value.c_str());
            break;
        }
        case LiteralType::kTimestamp: {
            constant_expr->date_value_ = strdup(str_value.c_str());
            break;
        }
        case LiteralType::kInterval: {
            constant_expr->date_value_ = strdup(str_value.c_str());
            break;
        }
        default: {
            status = Status::InvalidConstantType();
            return nullptr;
        }
    }
    return constant_expr.release();
}

ParsedExpr *WrapColumnExpr::GetParsedExpr(Status &status) {
    status.code_ = ErrorCode::kOk;
    auto column_expr = new ColumnExpr();
    column_expr->names_.reserve(names.size());
    for (SizeT i = 0; i < names.size(); ++i) {
        column_expr->names_.emplace_back(names[i]);
    }
    column_expr->star_ = star;
    column_expr->generated_ = generated;
    return column_expr;
}

ParsedExpr *WrapFunctionExpr::GetParsedExpr(Status &status) {
    status.code_ = ErrorCode::kOk;

    auto function_expr = new FunctionExpr();
    function_expr->func_name_ = func_name;
    function_expr->distinct_ = distinct;
    Vector<ParsedExpr *> *func_arguments = new Vector<ParsedExpr *>();
    func_arguments->reserve(arguments.size());
    function_expr->arguments_ = func_arguments;

    for (SizeT i = 0; i < arguments.size(); ++i) {
        func_arguments->emplace_back(arguments[i]->GetParsedExpr(status));
        if (status.code_ != ErrorCode::kOk) {
            delete function_expr;
            function_expr = nullptr;
            return nullptr;
        }
    }
    return function_expr;
}

ParsedExpr *WrapBetweenExpr::GetParsedExpr(Status &status) {
    status.code_ = ErrorCode::kOk;

    auto between_expr = new BetweenExpr();
    between_expr->value_ = value->GetParsedExpr(status);
    if (status.code_ != ErrorCode::kOk) {
        delete between_expr;
        between_expr = nullptr;
        return nullptr;
    }
    between_expr->lower_bound_ = lower_bound->GetParsedExpr(status);
    if (status.code_ != ErrorCode::kOk) {
        delete between_expr;
        between_expr = nullptr;
        return nullptr;
    }
    between_expr->upper_bound_ = upper_bound->GetParsedExpr(status);
    if (status.code_ != ErrorCode::kOk) {
        delete between_expr;
        between_expr = nullptr;
        return nullptr;
    }
    return between_expr;
}

Tuple<void *, i64> GetEmbeddingDataTypeDataPtrFromProto(const EmbeddingData &embedding_data, Status &status) {
    status.code_ = ErrorCode::kOk;
    if (embedding_data.u8_array_value.size() != 0) {
        auto ptr_i16 = (int16_t *)(embedding_data.u8_array_value.data());
        auto ptr_u8 = (uint8_t *)(embedding_data.u8_array_value.data());
        for (size_t i = 0; i < embedding_data.u8_array_value.size(); ++i) {
            ptr_u8[i] = static_cast<uint8_t>(ptr_i16[i]);
        }
        return {(void *)embedding_data.u8_array_value.data(), embedding_data.u8_array_value.size()};
    } else if (embedding_data.i8_array_value.size() != 0) {
        auto ptr_i16 = (int16_t *)(embedding_data.i8_array_value.data());
        auto ptr_i8 = (int8_t *)(embedding_data.i8_array_value.data());
        for (size_t i = 0; i < embedding_data.i8_array_value.size(); ++i) {
            ptr_i8[i] = static_cast<int8_t>(ptr_i16[i]);
        }
        return {(void *)embedding_data.i8_array_value.data(), embedding_data.i8_array_value.size()};
    } else if (embedding_data.i16_array_value.size() != 0) {
        return {(void *)embedding_data.i16_array_value.data(), embedding_data.i16_array_value.size()};
    } else if (embedding_data.i32_array_value.size() != 0) {
        return {(void *)embedding_data.i32_array_value.data(), embedding_data.i32_array_value.size()};
    } else if (embedding_data.i64_array_value.size() != 0) {
        return {(void *)embedding_data.i64_array_value.data(), embedding_data.i64_array_value.size()};
    } else if (embedding_data.f32_array_value.size() != 0) {
        auto ptr_double = (double *)(embedding_data.f32_array_value.data());
        auto ptr_float = (float *)(embedding_data.f32_array_value.data());
        for (size_t i = 0; i < embedding_data.f32_array_value.size(); ++i) {
            ptr_float[i] = float(ptr_double[i]);
        }
        return {(void *)embedding_data.f32_array_value.data(), embedding_data.f32_array_value.size()};
    } else if (embedding_data.f64_array_value.size() != 0) {
        return {(void *)embedding_data.f64_array_value.data(), embedding_data.f64_array_value.size()};
    } else if (embedding_data.f16_array_value.size() != 0) {
        auto ptr_double = (double *)(embedding_data.f16_array_value.data());
        auto ptr_float16 = (Float16T *)(embedding_data.f16_array_value.data());
        for (size_t i = 0; i < embedding_data.f16_array_value.size(); ++i) {
            ptr_float16[i] = float(ptr_double[i]);
        }
        return {(void *)embedding_data.f16_array_value.data(), embedding_data.f16_array_value.size()};
    } else if (embedding_data.bf16_array_value.size() != 0) {
        auto ptr_double = (double *)(embedding_data.bf16_array_value.data());
        auto ptr_bfloat16 = (BFloat16T *)(embedding_data.bf16_array_value.data());
        for (size_t i = 0; i < embedding_data.bf16_array_value.size(); ++i) {
            ptr_bfloat16[i] = float(ptr_double[i]);
        }
        return {(void *)embedding_data.bf16_array_value.data(), embedding_data.bf16_array_value.size()};
    } else {
        status = Status::InvalidEmbeddingDataType("unknown type");
        return {nullptr, 0};
    }
}

ParsedExpr *WrapKnnExpr::GetParsedExpr(Status &status) {
    status.code_ = ErrorCode::kOk;

    auto knn_expr = MakeUnique<KnnExpr>(false);

    knn_expr->distance_type_ = distance_type;
    if (knn_expr->distance_type_ == KnnDistanceType::kInvalid) {
        status = Status::InvalidKnnDistanceType();
        return nullptr;
    }
    knn_expr->embedding_data_type_ = embedding_data_type;
    if (knn_expr->embedding_data_type_ == EmbeddingDataType::kElemInvalid) {
        status = Status::InvalidEmbeddingDataType("unknown type");
        return nullptr;
    }
    auto [embedding_data_ptr, dimension] = GetEmbeddingDataTypeDataPtrFromProto(embedding_data, status);
    if (status.code_ != ErrorCode::kOk) {
        return nullptr;
    }
    knn_expr->embedding_data_ptr_ = embedding_data_ptr;
    knn_expr->dimension_ = dimension;

    knn_expr->topn_ = topn;
    if (knn_expr->topn_ <= 0) {
        status = Status::InvalidParameterValue("topn", std::to_string(topn), "topn should be greater than 0");
        return nullptr;
    }

    knn_expr->column_expr_ = column_expr.GetParsedExpr(status);
    if (status.code_ != ErrorCode::kOk) {
        return nullptr;
    }

    knn_expr->opt_params_ = new Vector<InitParameter *>();
    for (auto &param : opt_params) {
        if (param.param_name_ == "index_name") {
            knn_expr->index_name_ = param.param_value_;
        }
        if (param.param_name_ == "ignore_index" && param.param_value_ == "true") {
            knn_expr->ignore_index_ = true;
        }

        auto init_parameter = MakeUnique<InitParameter>();
        init_parameter->param_name_ = param.param_name_;
        init_parameter->param_value_ = param.param_value_;
        knn_expr->opt_params_->emplace_back(init_parameter.release());
    }

    if (filter_expr) {
        knn_expr->filter_expr_.reset(filter_expr->GetParsedExpr(status));
    }
    if (status.code_ != ErrorCode::kOk) {
        return nullptr;
    }

    return knn_expr.release();
}

ParsedExpr *WrapMatchExpr::GetParsedExpr(Status &status) {
    status.code_ = ErrorCode::kOk;

    auto match_expr = MakeUnique<MatchExpr>();
    match_expr->fields_ = fields;
    match_expr->matching_text_ = matching_text;
    match_expr->options_text_ = options_text;
    if (filter_expr) {
        match_expr->filter_expr_.reset(filter_expr->GetParsedExpr(status));
    }
    if (status.code_ != ErrorCode::kOk) {
        return nullptr;
    }
    return match_expr.release();
}

ParsedExpr *WrapFusionExpr::GetParsedExpr(Status &status) {
    status.code_ = ErrorCode::kOk;

    auto fusion_expr = MakeUnique<FusionExpr>();
    fusion_expr->method_ = method;
    fusion_expr->options_ = MakeShared<SearchOptions>(options_text);
    if (has_match_tensor_expr) {
        fusion_expr->match_tensor_expr_.reset(dynamic_cast<MatchTensorExpr *>(match_tensor_expr.GetParsedExpr(status)));
    } else {
        fusion_expr->match_tensor_expr_ = nullptr;
    }

    return fusion_expr.release();
}

ParsedExpr *WrapMatchTensorExpr::GetParsedExpr(Status &status) {
    status.code_ = ErrorCode::kOk;

    auto match_tensor_expr = MakeUnique<MatchTensorExpr>(own_memory);
    match_tensor_expr->SetSearchMethodStr(search_method);
    match_tensor_expr->column_expr_.reset(column_expr.GetParsedExpr(status));
    match_tensor_expr->options_text_ = options_text;
    match_tensor_expr->embedding_data_type_ = embedding_data_type;

    if (status.code_ != ErrorCode::kOk) {
        return nullptr;
    }

    auto [embedding_data_ptr, dimension] = GetEmbeddingDataTypeDataPtrFromProto(embedding_data, status);
    if (status.code_ != ErrorCode::kOk) {
        return nullptr;
    }
    match_tensor_expr->dimension_ = dimension;
    const auto copy_bytes = EmbeddingT::EmbeddingSize(match_tensor_expr->embedding_data_type_, match_tensor_expr->dimension_);
    match_tensor_expr->query_tensor_data_ptr_ = MakeUniqueForOverwrite<char[]>(copy_bytes);
    std::memcpy(match_tensor_expr->query_tensor_data_ptr_.get(), embedding_data_ptr, copy_bytes);

    if (filter_expr) {
        match_tensor_expr->filter_expr_.reset(filter_expr->GetParsedExpr(status));
    }
    if (status.code_ != ErrorCode::kOk) {
        return nullptr;
    }
    return match_tensor_expr.release();
}

ParsedExpr *WrapMatchSparseExpr::GetParsedExpr(Status &status) {
    status.code_ = ErrorCode::kOk;

    auto match_sparse_expr = MakeUnique<MatchSparseExpr>(own_memory);

    auto *column_expr_ptr = column_expr.GetParsedExpr(status);
    if (status.code_ != ErrorCode::kOk) {
        return nullptr;
    }
    match_sparse_expr->SetSearchColumn(column_expr_ptr);

    auto *query_sparse_ptr = static_cast<ConstantExpr *>(sparse_expr.GetParsedExpr(status));
    if (status.code_ != ErrorCode::kOk) {
        return nullptr;
    }
    match_sparse_expr->SetQuerySparse(query_sparse_ptr);

    match_sparse_expr->SetMetricType(metric_type);

    auto *opt_params_ptr = new Vector<InitParameter *>();
    for (auto &param : opt_params) {
        auto *init_parameter = new InitParameter();
        init_parameter->param_name_ = param.param_name_;
        init_parameter->param_value_ = param.param_value_;
        opt_params_ptr->emplace_back(init_parameter);
    }
    match_sparse_expr->SetOptParams(topn, opt_params_ptr);

    if (filter_expr) {
        match_sparse_expr->filter_expr_.reset(filter_expr->GetParsedExpr(status));
    }
    if (status.code_ != ErrorCode::kOk) {
        return nullptr;
    }
    return match_sparse_expr.release();
}

ParsedExpr *WrapSearchExpr::GetParsedExpr(Status &status) {
    status.code_ = ErrorCode::kOk;

    auto search_expr = new SearchExpr();
    search_expr->match_exprs_.reserve(match_exprs.size());
    for (SizeT i = 0; i < match_exprs.size(); ++i) {
        search_expr->match_exprs_.emplace_back(match_exprs[i].GetParsedExpr(status));
        if (status.code_ != ErrorCode::kOk) {
            delete search_expr;
            search_expr = nullptr;
            return nullptr;
        }
    }
    search_expr->fusion_exprs_.reserve(fusion_exprs.size());
    for (SizeT i = 0; i < fusion_exprs.size(); ++i) {
        search_expr->fusion_exprs_.emplace_back(dynamic_cast<FusionExpr *>(fusion_exprs[i].GetParsedExpr(status)));
        if (status.code_ != ErrorCode::kOk) {
            delete search_expr;
            search_expr = nullptr;
            return nullptr;
        }
    }
    return search_expr;
}

ParsedExpr *WrapInExpr::GetParsedExpr(Status &status) {
    status.code_ = ErrorCode::kOk;
    auto in_expr = MakeUnique<InExpr>();
    in_expr->arguments_ = new Vector<ParsedExpr *>();
    in_expr->arguments_->reserve(arguments.size());
    for (SizeT i = 0; i < arguments.size(); ++i) {
        in_expr->arguments_->emplace_back(arguments[i]->GetParsedExpr(status));
        if (status.code_ != ErrorCode::kOk) {
            return nullptr;
        }
    }
    in_expr->left_ = left->GetParsedExpr(status);
    in_expr->in_ = in;
    return in_expr.release();
}

ParsedExpr *WrapParsedExpr::GetParsedExpr(Status &status) {
    status.code_ = ErrorCode::kOk;
    switch (type) {
        case ParsedExprType::kConstant:
            return constant_expr.GetParsedExpr(status);
        case ParsedExprType::kColumn:
            return column_expr.GetParsedExpr(status);
        case ParsedExprType::kFunction:
            return function_expr.GetParsedExpr(status);
        case ParsedExprType::kBetween:
            return between_expr.GetParsedExpr(status);
        case ParsedExprType::kKnn:
            return knn_expr.GetParsedExpr(status);
        case ParsedExprType::kMatch:
            return match_expr.GetParsedExpr(status);
        case ParsedExprType::kMatchSparse:
            return match_sparse_expr.GetParsedExpr(status);
        case ParsedExprType::kMatchTensor:
            return match_tensor_expr.GetParsedExpr(status);
        case ParsedExprType::kFusion:
            return fusion_expr.GetParsedExpr(status);
        case ParsedExprType::kSearch:
            return search_expr.GetParsedExpr(status);
        case ParsedExprType::kIn:
            return in_expr.GetParsedExpr(status);
        default:
            status = Status::InvalidParsedExprType();
    }
    return nullptr;
}

OrderByExpr *WrapOrderByExpr::GetOrderByExpr(Status &status) {
    auto order_by_expr = new OrderByExpr();
    order_by_expr->expr_ = expr.GetParsedExpr(status);
    if (asc) {
        order_by_expr->type_ = OrderType::kAsc;
    } else {
        order_by_expr->type_ = OrderType::kDesc;
    }
    return order_by_expr;
}

UpdateExpr *WrapUpdateExpr::GetUpdateExpr(Status &status) {
    auto update_expr = new UpdateExpr();
    update_expr->column_name = column_name;
    update_expr->value = value.GetParsedExpr(status);
    if (status.code_ != ErrorCode::kOk) {
        delete update_expr;
        update_expr = nullptr;
    }
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
            result.table_count = std::stol(value.GetVarchar());
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

WrapQueryResult WrapShowInfo(Infinity &instance, const String &info_name) {
    auto query_result = instance.ShowFunction(info_name);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapQuery(Infinity &instance, const String &query_text) {
    auto query_result = instance.Query(query_text);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

namespace {

Optional<WrapQueryResult> UnwrapColumnDefs(Vector<WrapColumnDef> &column_defs, Vector<ColumnDef *> &column_defs_ptr) {
    for (SizeT i = 0; i < column_defs.size(); ++i) {
        auto &wrap_column_def = column_defs[i];
        SharedPtr<TypeInfo> type_info_ptr = nullptr;
        switch (wrap_column_def.column_type.logical_type) {
            case LogicalType::kEmbedding:
            case LogicalType::kMultiVector:
            case LogicalType::kTensor:
            case LogicalType::kTensorArray: {
                auto &embedding_type = wrap_column_def.column_type.embedding_type;
                type_info_ptr = MakeShared<EmbeddingInfo>(embedding_type.element_type, embedding_type.dimension);
                break;
            }
            case LogicalType::kSparse: {
                auto &sparse_type = wrap_column_def.column_type.sparse_type;
                type_info_ptr = SparseInfo::Make(sparse_type.element_type, sparse_type.index_type, sparse_type.dimension, SparseStoreType::kSort);
                break;
            }
            default: {
                break;
            }
        }
        auto column_type = MakeShared<DataType>(wrap_column_def.column_type.logical_type, type_info_ptr);
        Status status;
        SharedPtr<ParsedExpr> default_expr(wrap_column_def.constant_expr.GetParsedExpr(status));
        if (status.code_ != ErrorCode::kOk) {
            for (SizeT j = 0; j < i; ++j) {
                if (column_defs_ptr[j] != nullptr) {
                    delete column_defs_ptr[j];
                    column_defs_ptr[j] = nullptr;
                }
            }
            return WrapQueryResult(status.code_, status.msg_->c_str());
        }
        auto column_def = new ColumnDef(wrap_column_def.id, column_type, wrap_column_def.column_name, wrap_column_def.constraints, default_expr);
        column_defs_ptr.push_back(column_def);
    }
    return None;
}

} // namespace

WrapQueryResult WrapCreateTable(Infinity &instance,
                                const String &db_name,
                                const String &table_name,
                                Vector<WrapColumnDef> column_defs,
                                WrapCreateTableOptions create_table_options) {
    Vector<TableConstraint *> constraints;
    Vector<ColumnDef *> column_defs_ptr;
    auto res = UnwrapColumnDefs(column_defs, column_defs_ptr);
    if (res.has_value()) {
        return res.value();
    }
    CreateTableOptions options;
    options.conflict_type_ = create_table_options.conflict_type_;
    for (auto &property : create_table_options.properties_) {
        options.properties_.emplace_back(new InitParameter(std::move(property.param_name_), std::move(property.param_value_)));
    }
    auto query_result = instance.CreateTable(db_name, table_name, std::move(column_defs_ptr), constraints, std::move(options));
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapDropTable(Infinity &instance, const String &db_name, const String &table_name, const DropTableOptions &drop_table_options) {
    auto query_result = instance.DropTable(db_name, table_name, drop_table_options);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapListTables(Infinity &instance, const String &db_name) {
    auto query_result = instance.ListTables(db_name);

    WrapQueryResult result(query_result.ErrorCode(), query_result.ErrorMsg());

    SharedPtr<DataBlock> data_block = query_result.result_table_->GetDataBlockById(0);
    auto &names = result.names;
    names.resize(data_block->row_count());
    for (SizeT i = 0; i < names.size(); ++i) {
        Value value = data_block->GetValue(1, i);
        names[i] = value.GetVarchar();
    }
    return result;
}

WrapQueryResult WrapShowTable(Infinity &instance, const String &db_name, const String &table_name) {
    auto query_result = instance.ShowTable(db_name, table_name);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapListTableIndexes(Infinity &instance, const String &db_name, const String &table_name) {
    auto query_result = instance.ListTableIndexes(db_name, table_name);
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

WrapQueryResult WrapGetTable(Infinity &instance, const String &db_name, const String &table_name) {
    auto query_result = instance.GetTable(db_name, table_name);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapCreateIndex(Infinity &instance,
                                const String &db_name,
                                const String &table_name,
                                const String &index_name,
                                WrapIndexInfo &wrap_index_info,
                                const CreateIndexOptions &create_index_options) {
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
    auto query_result = instance.CreateIndex(db_name, table_name, index_name, index_info, create_index_options);
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
    if (values.empty()) {
        return WrapQueryResult(ErrorCode::kInsertWithoutValues, "insert values is empty");
    }
    Vector<Vector<ParsedExpr *> *> *value_ptr = new Vector<Vector<ParsedExpr *> *>(values.size());
    for (SizeT i = 0; i < values.size(); ++i) {
        auto value_list = new Vector<ParsedExpr *>(values[i].size());
        for (SizeT j = 0; j < values[i].size(); ++j) {
            auto &wrap_constant_expr = values[i][j];
            Status status;
            (*value_list)[j] = wrap_constant_expr.GetParsedExpr(status);
            if (status.code_ != ErrorCode::kOk) {
                for (SizeT k = 0; k < i; ++k) {
                    if ((*value_ptr)[k] != nullptr) {
                        delete (*value_ptr)[k];
                    }
                }
                if (value_list != nullptr) {
                    delete value_list;
                }
                delete value_ptr;
                return WrapQueryResult(status.code_, status.msg_->c_str());
            }
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

WrapQueryResult WrapExport(Infinity &instance,
                           const String &db_name,
                           const String &table_name,
                           Vector<String> &columns,
                           const String &path,
                           ExportOptions export_options) {
    Vector<ParsedExpr *> *export_columns = new Vector<ParsedExpr *>();
    export_columns->reserve(columns.size());

    for (SizeT i = 0; i < columns.size(); ++i) {
        auto &column_name = columns[i];
        ToLower(column_name);
        if (column_name == "_row_id") {
            FunctionExpr *expr = new FunctionExpr();
            expr->func_name_ = "row_id";
            export_columns->emplace_back(expr);
        } else if (column_name == "_create_timestamp") {
            FunctionExpr *expr = new FunctionExpr();
            expr->func_name_ = "create_timestamp";
            export_columns->emplace_back(expr);
        } else if (column_name == "_delete_timestamp") {
            FunctionExpr *expr = new FunctionExpr();
            expr->func_name_ = "delete_timestamp";
            export_columns->emplace_back(expr);
        } else {
            ColumnExpr *expr = new ColumnExpr();
            expr->names_.emplace_back(column_name);
            export_columns->emplace_back(expr);
        }
    }

    auto query_result = instance.Export(db_name, table_name, export_columns, path, export_options);
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapDelete(Infinity &instance, const String &db_name, const String &table_name, WrapParsedExpr *wrap_filter) {
    ParsedExpr *filter = nullptr;
    if (wrap_filter != nullptr) {
        Status status;
        filter = wrap_filter->GetParsedExpr(status);
        if (status.code_ != ErrorCode::kOk) {
            if (filter != nullptr) {
                delete filter;
                filter = nullptr;
            }
            return WrapQueryResult(status.code_, status.msg_->c_str());
        }
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
        Status status;
        filter = wrap_filter->GetParsedExpr(status);
        if (status.code_ != ErrorCode::kOk) {
            if (filter != nullptr) {
                delete filter;
                filter = nullptr;
            }
            return WrapQueryResult(status.code_, status.msg_->c_str());
        }
    }
    Vector<UpdateExpr *> *update_list = nullptr;
    if (wrap_update_list != nullptr) {
        update_list = new Vector<UpdateExpr *>(wrap_update_list->size());
        for (SizeT i = 0; i < wrap_update_list->size(); ++i) {
            auto &wrap_update_expr = (*wrap_update_list)[i];
            Status status;
            UpdateExpr *update_expr = wrap_update_expr.GetUpdateExpr(status);
            (*update_list)[i] = update_expr;
            if (status.code_ != ErrorCode::kOk) {
                for (SizeT j = 0; j <= i; ++j) {
                    if ((*update_list)[j] != nullptr) {
                        delete (*update_list)[j];
                        (*update_list)[j] = nullptr;
                    }
                }
                if (update_list != nullptr) {
                    delete update_list;
                    update_list = nullptr;
                }
                return WrapQueryResult(status.code_, status.msg_->c_str());
            }
        }
    }
    auto query_result = instance.Update(db_name, table_name, filter, update_list);
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
    output_column_field.column_vectors.emplace_back(column_vector->data(), size);
}

void HandleVarcharType(ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector) {
    SizeT all_size = 0;
    Vector<Pair<const char *, SizeT>> raw_data;
    for (SizeT index = 0; index < row_count; ++index) {
        Span<const char> data = column_vector->GetVarchar(index);
        all_size += sizeof(i32) + data.size();
        raw_data.emplace_back(data.data(), data.size());
    }
    String dst;
    dst.resize(all_size);
    SizeT current_offset = 0;
    for (const auto [data_ptr, data_size] : raw_data) {
        i32 length = data_size;
        std::memcpy(dst.data() + current_offset, &length, sizeof(i32));
        std::memcpy(dst.data() + current_offset + sizeof(i32), data_ptr, data_size);
        current_offset += sizeof(i32) + length;
    }

    output_column_field.column_vectors.emplace_back(dst.c_str(), dst.size());
    output_column_field.column_type = column_vector->data_type()->type();
}

void HandleEmbeddingType(ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector) {
    auto size = column_vector->data_type()->Size() * row_count;
    output_column_field.column_vectors.emplace_back(column_vector->data(), size);
    output_column_field.column_type = column_vector->data_type()->type();
}

void HandleMultiVectorType(ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector) {
    SizeT all_size = 0;
    Vector<Pair<const char *, SizeT>> multi_vector_data(row_count);
    for (SizeT index = 0; index < row_count; ++index) {
        Span<const char> raw_data = column_vector->GetMultiVectorRaw(index).first;
        all_size += sizeof(i32) + raw_data.size();
        multi_vector_data[index] = {raw_data.data(), raw_data.size()};
    }
    String dst;
    dst.resize(all_size);

    i32 current_offset = 0;
    for (SizeT index = 0; index < row_count; ++index) {
        const auto &[data, length] = multi_vector_data[index];
        std::memcpy(dst.data() + current_offset, &length, sizeof(i32));
        std::memcpy(dst.data() + current_offset + sizeof(i32), data, length);
        current_offset += sizeof(i32) + length;
    }

    output_column_field.column_vectors.emplace_back(dst.c_str(), dst.size());
    output_column_field.column_type = column_vector->data_type()->type();
}

void HandleTensorType(ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector) {
    SizeT all_size = 0;
    Vector<Pair<const char *, SizeT>> tensor_data(row_count);
    for (SizeT index = 0; index < row_count; ++index) {
        Span<const char> raw_data = column_vector->GetTensorRaw(index).first;
        all_size += sizeof(i32) + raw_data.size();
        tensor_data[index] = {raw_data.data(), raw_data.size()};
    }
    String dst;
    dst.resize(all_size);

    i32 current_offset = 0;
    for (SizeT index = 0; index < row_count; ++index) {
        const auto &[data, length] = tensor_data[index];
        std::memcpy(dst.data() + current_offset, &length, sizeof(i32));
        std::memcpy(dst.data() + current_offset + sizeof(i32), data, length);
        current_offset += sizeof(i32) + length;
    }

    output_column_field.column_vectors.emplace_back(dst.c_str(), dst.size());
    output_column_field.column_type = column_vector->data_type()->type();
}

void HandleTensorArrayType(ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector) {
    SizeT all_size = 0;
    Vector<Vector<Pair<const char *, SizeT>>> tensor_array_data;
    for (SizeT index = 0; index < row_count; ++index) {
        all_size += sizeof(i32);
        Vector<Pair<const char *, SizeT>> tensor_data;
        Vector<Pair<Span<const char>, SizeT>> array_data = column_vector->GetTensorArrayRaw(index);
        for (const auto [raw_data, embedding_num] : array_data) {
            all_size += sizeof(i32) + raw_data.size();
            tensor_data.emplace_back(raw_data.data(), raw_data.size());
        }
        tensor_array_data.push_back(std::move(tensor_data));
    }
    String dst;
    dst.resize(all_size);

    i32 current_offset = 0;
    for (const auto &tensor_data : tensor_array_data) {
        i32 tensor_num = tensor_data.size();
        std::memcpy(dst.data() + current_offset, &tensor_num, sizeof(i32));
        current_offset += sizeof(i32);
        for (const auto [raw_data, size] : tensor_data) {
            std::memcpy(dst.data() + current_offset, &size, sizeof(i32));
            std::memcpy(dst.data() + current_offset + sizeof(i32), raw_data, size);
            current_offset += sizeof(i32) + size;
        }
    }

    output_column_field.column_vectors.emplace_back(dst.c_str(), dst.size());
    output_column_field.column_type = column_vector->data_type()->type();
}

void HandleSparseType(ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector) {
    SizeT all_size = 0;
    Vector<Tuple<Span<const char>, Span<const char>, SizeT>> raw_data;
    for (SizeT index = 0; index < row_count; ++index) {
        auto [data_span, index_span, nnz_size_t] = column_vector->GetSparseRaw(index);
        all_size += sizeof(i32) + data_span.size() + index_span.size();
        raw_data.emplace_back(data_span, index_span, nnz_size_t);
    }
    String dst;
    dst.resize(all_size);
    SizeT current_offset = 0;
    for (const auto [data_span, index_span, nnz_size_t] : raw_data) {
        i32 nnz = nnz_size_t;
        std::memcpy(dst.data() + current_offset, &nnz, sizeof(i32));
        current_offset += sizeof(i32);
        std::memcpy(dst.data() + current_offset, index_span.data(), index_span.size());
        current_offset += index_span.size();
        std::memcpy(dst.data() + current_offset, data_span.data(), data_span.size());
        current_offset += data_span.size();
    }

    output_column_field.column_vectors.emplace_back(dst.c_str(), dst.size());
    output_column_field.column_type = column_vector->data_type()->type();
}

void HandleRowIDType(ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector) {
    auto size = column_vector->data_type()->Size() * row_count;
    output_column_field.column_vectors.emplace_back(column_vector->data(), size);
    output_column_field.column_type = column_vector->data_type()->type();
}

void HandleTimeRelatedTypes(ColumnField &output_column_field, SizeT row_count, const SharedPtr<ColumnVector> &column_vector) {
    auto size = column_vector->data_type()->Size() * row_count;
    String dst;
    dst.resize(size);
    std::memcpy(dst.data(), column_vector->data(), size);
    output_column_field.column_vectors.emplace_back(dst.c_str(), dst.size());
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
        case LogicalType::kFloat16:
        case LogicalType::kBFloat16:
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
        case LogicalType::kMultiVector: {
            HandleMultiVectorType(output_column_field, row_count, column_vector);
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
        case LogicalType::kSparse: {
            HandleSparseType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kRowID: {
            HandleRowIDType(output_column_field, row_count, column_vector);
            break;
        }
        case LogicalType::kDate:
        case LogicalType::kTime:
        case LogicalType::kDateTime:
        case LogicalType::kInterval:
        case LogicalType::kTimestamp: {
            HandleTimeRelatedTypes(output_column_field, row_count, column_vector);
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
        case LogicalType::kFloat16:
        case LogicalType::kBFloat16:
        case LogicalType::kFloat:
        case LogicalType::kDouble:
        case LogicalType::kDate:
        case LogicalType::kTime:
        case LogicalType::kDateTime:
        case LogicalType::kInterval:
        case LogicalType::kTimestamp: {
            proto_data_type.logical_type = data_type->type();
            break;
        }
        case LogicalType::kVarchar: {
            proto_data_type.logical_type = data_type->type();
            break;
        }
        case LogicalType::kMultiVector:
        case LogicalType::kTensor:
        case LogicalType::kTensorArray:
        case LogicalType::kEmbedding: {
            proto_data_type.logical_type = data_type->type();
            WrapEmbeddingType &embedding_type = proto_data_type.embedding_type;

            auto embedding_info = static_cast<EmbeddingInfo *>(data_type->type_info().get());
            embedding_type.dimension = embedding_info->Dimension();
            embedding_type.element_type = embedding_info->Type();
            break;
        }
        case LogicalType::kSparse: {
            proto_data_type.logical_type = data_type->type();
            WrapSparseType &sparse_type = proto_data_type.sparse_type;

            auto sparse_info = static_cast<SparseInfo *>(data_type->type_info().get());
            sparse_type.dimension = sparse_info->Dimension();
            sparse_type.element_type = sparse_info->DataType();
            sparse_type.index_type = sparse_info->IndexType();
            break;
        }
        default: {
            String error_message = fmt::format("Invalid logical data type: {}", data_type->ToString());
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
                           Vector<WrapParsedExpr> highlight_list,
                           Vector<WrapOrderByExpr> order_by_list,
                           Vector<WrapParsedExpr> group_by_list,
                           WrapSearchExpr *wrap_search_expr,
                           WrapParsedExpr *filter_expr,
                           WrapParsedExpr *limit_expr,
                           WrapParsedExpr *offset_expr) {

    SearchExpr *search_expr = nullptr;
    DeferFn defer_fn1([&]() {
        if (search_expr != nullptr) {
            delete search_expr;
            search_expr = nullptr;
        }
    });
    if (wrap_search_expr != nullptr) {
        Status status;
        search_expr = dynamic_cast<SearchExpr *>(wrap_search_expr->GetParsedExpr(status));
        if (status.code_ != ErrorCode::kOk) {
            return WrapQueryResult(status.code_, status.msg_->c_str());
        }
    }
    ParsedExpr *filter = nullptr;
    DeferFn defer_fn2([&]() {
        if (filter != nullptr) {
            delete filter;
            filter = nullptr;
        }
    });
    if (filter_expr != nullptr) {
        Status status;
        filter = filter_expr->GetParsedExpr(status);
        if (status.code_ != ErrorCode::kOk) {
            return WrapQueryResult(status.code_, status.msg_->c_str());
        }
    }
    Vector<ParsedExpr *> *output_columns = nullptr;
    DeferFn defer_fn3([&]() {
        if (output_columns != nullptr) {
            SizeT output_column_len = output_columns->size();
            for (SizeT i = 0; i < output_column_len; ++i) {
                if ((*output_columns)[i] != nullptr) {
                    delete (*output_columns)[i];
                    (*output_columns)[i] = nullptr;
                }
            }
        }
    });
    if (select_list.empty()) {
        return WrapQueryResult(ErrorCode::kEmptySelectFields, "[error] Select fields are empty");
    } else {
        output_columns = new Vector<ParsedExpr *>();
        output_columns->reserve(select_list.size());
        for (SizeT i = 0; i < select_list.size(); ++i) {
            Status status;
            output_columns->emplace_back(select_list.at(i).GetParsedExpr(status));
            if (status.code_ != ErrorCode::kOk) {
                return WrapQueryResult(status.code_, status.msg_->c_str());
            }
        }
    }

    Vector<ParsedExpr *> *highlight = nullptr;
    DeferFn defer_fn7([&]() {
        if (highlight != nullptr) {
            SizeT highlight_column_len = highlight->size();
            for (SizeT i = 0; i < highlight_column_len; ++i) {
                if ((*highlight)[i] != nullptr) {
                    delete (*highlight)[i];
                    (*highlight)[i] = nullptr;
                }
            }
        }
    });
    if (!highlight_list.empty()) {
        highlight = new Vector<ParsedExpr *>();
        highlight->reserve(highlight_list.size());
        for (SizeT i = 0; i < highlight_list.size(); ++i) {
            Status status;
            highlight->emplace_back(highlight_list.at(i).GetParsedExpr(status));
            if (status.code_ != ErrorCode::kOk) {
                return WrapQueryResult(status.code_, status.msg_->c_str());
            }
        }
    }

    ParsedExpr *limit = nullptr;
    DeferFn defer_fn4([&]() {
        if (limit != nullptr) {
            delete limit;
            limit = nullptr;
        }
    });
    if (limit_expr != nullptr) {
        Status status;
        limit = limit_expr->GetParsedExpr(status);
        if (status.code_ != ErrorCode::kOk) {
            return WrapQueryResult(status.code_, status.msg_->c_str());
        }
    }

    ParsedExpr *offset = nullptr;
    DeferFn defer_fn5([&]() {
        if (offset != nullptr) {
            delete offset;
            offset = nullptr;
        }
    });
    if (offset_expr != nullptr) {
        Status status;
        offset = offset_expr->GetParsedExpr(status);
        if (status.code_ != ErrorCode::kOk) {
            return WrapQueryResult(status.code_, status.msg_->c_str());
        }
    }

    Vector<OrderByExpr *> *order_by_exprs = nullptr;
    DeferFn defer_fn6([&]() {
        if (order_by_exprs != nullptr) {
            SizeT order_by_expr_len = order_by_exprs->size();
            for (SizeT i = 0; i < order_by_expr_len; ++i) {
                if ((*order_by_exprs)[i] != nullptr) {
                    delete (*order_by_exprs)[i];
                    (*order_by_exprs)[i] = nullptr;
                }
            }
            delete order_by_exprs;
            order_by_exprs = nullptr;
        }
    });
    if (!order_by_list.empty()) {
        order_by_exprs = new Vector<OrderByExpr *>();
        order_by_exprs->reserve(order_by_list.size());
        for (SizeT i = 0; i < order_by_list.size(); ++i) {
            Status status;
            order_by_exprs->emplace_back(order_by_list.at(i).GetOrderByExpr(status));
            if (status.code_ != ErrorCode::kOk) {
                return WrapQueryResult(status.code_, status.msg_->c_str());
            }
        }
    }

    auto query_result = instance.Search(db_name, table_name, search_expr, filter, limit, offset, output_columns, highlight, order_by_exprs);
    search_expr = nullptr;
    filter = nullptr;
    limit = nullptr;
    offset = nullptr;
    output_columns = nullptr;
    order_by_exprs = nullptr;
    if (!query_result.IsOk()) {
        return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
    }
    auto wrap_query_result = WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
    auto &columns = wrap_query_result.column_fields;
    columns.resize(query_result.result_table_->ColumnCount());
    ProcessDataBlocks(query_result, wrap_query_result, columns);
    return wrap_query_result;
}

WrapQueryResult WrapExplain(Infinity &instance,
                            const String &db_name,
                            const String &table_name,
                            ExplainType explain_type,
                            Vector<WrapParsedExpr> select_list,
                            Vector<WrapParsedExpr> highlight_list,
                            Vector<WrapOrderByExpr> order_by_list,
                            Vector<WrapParsedExpr> group_by_list,
                            WrapSearchExpr *wrap_search_expr,
                            WrapParsedExpr *filter_expr,
                            WrapParsedExpr *limit_expr,
                            WrapParsedExpr *offset_expr) {

    SearchExpr *search_expr = nullptr;
    DeferFn defer_fn1([&]() {
        if (search_expr != nullptr) {
            delete search_expr;
            search_expr = nullptr;
        }
    });
    if (wrap_search_expr != nullptr) {
        Status status;
        search_expr = dynamic_cast<SearchExpr *>(wrap_search_expr->GetParsedExpr(status));
        if (status.code_ != ErrorCode::kOk) {
            return WrapQueryResult(status.code_, status.msg_->c_str());
        }
    }
    ParsedExpr *filter = nullptr;
    DeferFn defer_fn2([&]() {
        if (filter != nullptr) {
            delete filter;
            filter = nullptr;
        }
    });
    if (filter_expr != nullptr) {
        Status status;
        filter = filter_expr->GetParsedExpr(status);
        if (status.code_ != ErrorCode::kOk) {
            return WrapQueryResult(status.code_, status.msg_->c_str());
        }
    }
    Vector<ParsedExpr *> *output_columns = nullptr;
    DeferFn defer_fn3([&]() {
        if (output_columns != nullptr) {
            SizeT output_column_len = output_columns->size();
            for (SizeT i = 0; i < output_column_len; ++i) {
                if ((*output_columns)[i] != nullptr) {
                    delete (*output_columns)[i];
                    (*output_columns)[i] = nullptr;
                }
            }
        }
    });
    if (select_list.empty()) {
        return WrapQueryResult(ErrorCode::kEmptySelectFields, "[error] Select fields are empty");
    } else {
        output_columns = new Vector<ParsedExpr *>();
        output_columns->reserve(select_list.size());
        for (SizeT i = 0; i < select_list.size(); ++i) {
            Status status;
            output_columns->emplace_back(select_list[i].GetParsedExpr(status));
            if (status.code_ != ErrorCode::kOk) {
                return WrapQueryResult(status.code_, status.msg_->c_str());
            }
        }
    }

    Vector<OrderByExpr *> *order_by_exprs = nullptr;
    DeferFn defer_fn6([&]() {
        if (order_by_exprs != nullptr) {
            SizeT order_by_expr_len = order_by_exprs->size();
            for (SizeT i = 0; i < order_by_expr_len; ++i) {
                if ((*order_by_exprs)[i] != nullptr) {
                    delete (*order_by_exprs)[i];
                    (*order_by_exprs)[i] = nullptr;
                }
            }
            delete order_by_exprs;
            order_by_exprs = nullptr;
        }
    });
    if (!order_by_list.empty()) {
        order_by_exprs = new Vector<OrderByExpr *>();
        order_by_exprs->reserve(order_by_list.size());
        for (SizeT i = 0; i < order_by_list.size(); ++i) {
            Status status;
            order_by_exprs->emplace_back(order_by_list.at(i).GetOrderByExpr(status));
            if (status.code_ != ErrorCode::kOk) {
                return WrapQueryResult(status.code_, status.msg_->c_str());
            }
        }
    }

    auto query_result = instance.Explain(db_name, table_name, explain_type, search_expr, filter, nullptr, nullptr, output_columns, nullptr, order_by_exprs);
    search_expr = nullptr;
    filter = nullptr;
    output_columns = nullptr;
    order_by_exprs = nullptr;
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

WrapQueryResult WrapOptimize(Infinity &instance, const String &db_name, const String &table_name, WrapOptimizeOptions optimize_options) {
    OptimizeOptions options;
    options.index_name_ = std::move(optimize_options.index_name_);
    for (auto &param : optimize_options.opt_params_) {
        options.opt_params_.emplace_back(new InitParameter(std::move(param.param_name_), std::move(param.param_value_)));
    }
    auto query_result = instance.Optimize(db_name, table_name, std::move(options));
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapAddColumns(Infinity &instance, const String &db_name, const String &table_name, Vector<WrapColumnDef> wrapped_column_defs) {
    Vector<ColumnDef *> column_defs_ptr;
    auto res = UnwrapColumnDefs(wrapped_column_defs, column_defs_ptr);
    if (res.has_value()) {
        return res.value();
    }
    Vector<SharedPtr<ColumnDef>> column_defs(column_defs_ptr.size());
    std::transform(column_defs_ptr.begin(), column_defs_ptr.end(), column_defs.begin(), [](ColumnDef *column_def) {
        return SharedPtr<ColumnDef>(column_def);
    });
    auto query_result = instance.AddColumns(db_name, table_name, std::move(column_defs));
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

WrapQueryResult WrapDropColumns(Infinity &instance, const String &db_name, const String &table_name, Vector<String> column_names) {
    auto query_result = instance.DropColumns(db_name, table_name, std::move(column_names));
    return WrapQueryResult(query_result.ErrorCode(), query_result.ErrorMsg());
}

} // namespace infinity
