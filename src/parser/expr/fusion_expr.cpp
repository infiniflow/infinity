#include "fusion_expr.h"
#include "column_expr.h"
#include "search_options.h"
#include <cstring>

#ifndef PARESER_USE_STD_MODULE
#define PARESER_USE_STD_MODULE 1
import std;
import std.compat;
#endif

#ifndef PARESER_USE_THIRD_PARTY_MODULE
#define PARESER_USE_THIRD_PARTY_MODULE 1
import third_party;
#endif

namespace infinity {

FusionExpr::FusionExpr() : ParsedExpr(ParsedExprType::kFusion) {}

FusionExpr::~FusionExpr() {}

std::string FusionExpr::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }
    std::ostringstream oss;
    oss << "FUSION('";
    oss << method_;
    oss << "', '";
    oss << options_->ToString();
    if (match_tensor_expr_ != nullptr) {
        oss << "', '";
        oss << match_tensor_expr_->ToString();
    }
    oss << "')";
    return oss.str();
}

void FusionExpr::SetOptions(const std::string &options) { options_ = std::make_shared<SearchOptions>(options); }

std::unique_ptr<MatchTensorExpr> GetFusionMatchTensorExpr(SearchOptions &search_options);

void FusionExpr::JobAfterParser() {
    std::string to_lower_method_;
    std::transform(method_.begin(), method_.end(), std::back_inserter(to_lower_method_), [](unsigned char c) { return std::tolower(c); });
    if (to_lower_method_ == "match_tensor") {
        match_tensor_expr_ = GetFusionMatchTensorExpr(*options_);
    }
}

std::shared_ptr<ConstantExpr> BuildConstantExprFromJson(std::string_view json_str) {
    simdjson::padded_string json(json_str);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(json);
    switch (doc.type()) {
        case simdjson::json_type::boolean: {
            auto res = std::make_shared<ConstantExpr>(LiteralType::kBoolean);
            res->bool_value_ = doc.get<bool>();
            return res;
        }
        case simdjson::json_type::number: {
            simdjson::number num = doc.get_number();
            switch (num.get_number_type()) {
                case simdjson::number_type::signed_integer:
                case simdjson::number_type::unsigned_integer: {
                    auto res = std::make_shared<ConstantExpr>(LiteralType::kInteger);
                    res->integer_value_ = (int64_t)num;
                    return res;
                }
                case simdjson::number_type::floating_point_number: {
                    auto res = std::make_shared<ConstantExpr>(LiteralType::kDouble);
                    res->double_value_ = (double)num;
                    return res;
                }
                default: {
                    const auto error_info = fmt::format("Unrecognized json object type in number");
                    ParserError(error_info);
                    return nullptr;
                }
            }
        }
        case simdjson::json_type::string: {
            auto res = std::make_shared<ConstantExpr>(LiteralType::kString);
            auto str = doc.get<std::string>();
            res->str_value_ = strdup(((std::string)doc.get<std::string>()).c_str());
            return res;
        }
        case simdjson::json_type::array: {
            const uint32_t array_size = doc.count_elements();
            if (array_size == 0) {
                const auto error_info = "Empty json array!";
                ParserError(error_info);
                return nullptr;
            }
            std::vector<std::string_view> json_strs(array_size);
            std::vector<simdjson::value> values(array_size);
            for (size_t index = 0; auto field : doc.get_array()) {
                values[index] = field.value();
                json_strs[index++] = values[index].raw_json();
            }
            switch (values[0].type()) {
                case simdjson::json_type::boolean:
                case simdjson::json_type::number: {
                    std::vector<simdjson::number> nums(array_size);
                    for (size_t index = 0; auto item : values) {
                        nums[index++] = item.get_number();
                    }
                    switch (nums[0].get_number_type()) {
                        case simdjson::number_type::signed_integer:
                        case simdjson::number_type::unsigned_integer: {
                            auto res = std::make_shared<ConstantExpr>(LiteralType::kIntegerArray);
                            res->long_array_.resize(array_size);
                            for (uint32_t i = 0; i < array_size; ++i) {
                                res->long_array_[i] = (int64_t)nums[i];
                            }
                            return res;
                        }
                        case simdjson::number_type::floating_point_number: {
                            auto res = std::make_shared<ConstantExpr>(LiteralType::kDoubleArray);
                            res->double_array_.resize(array_size);
                            for (uint32_t i = 0; i < array_size; ++i) {
                                res->double_array_[i] = (double)nums[i];
                            }
                            return res;
                        }
                        default: {
                            const auto error_info = fmt::format("Unrecognized json object type in array");
                            ParserError(error_info);
                            return nullptr;
                        }
                    }
                }
                case simdjson::json_type::array: {
                    auto res = std::make_shared<ConstantExpr>(LiteralType::kSubArrayArray);
                    res->sub_array_array_.resize(array_size);
                    for (uint32_t i = 0; i < array_size; ++i) {
                        res->sub_array_array_[i] = BuildConstantExprFromJson(json_strs[i]);
                    }
                    return res;
                }
                default: {
                    const auto error_info = fmt::format("Unrecognized json object type in array");
                    ParserError(error_info);
                    return nullptr;
                }
            }
        }
        case simdjson::json_type::object: {
            std::shared_ptr<ConstantExpr> res = nullptr;
            for (auto field : doc.get_object()) {
                int64_t field_key = std::stoll(std::string((std::string_view)field.unescaped_key()));
                auto field_value = field.value();
                if (field_value.type() != simdjson::json_type::number) {
                    const auto error_info = fmt::format("Unrecognized json object type in array");
                    ParserError(error_info);
                    return nullptr;
                }

                simdjson::number num = field_value.get_number();
                switch (num.get_number_type()) {
                    case simdjson::number_type::signed_integer:
                    case simdjson::number_type::unsigned_integer: {
                        if (res.get() == nullptr) {
                            res = std::make_shared<ConstantExpr>(LiteralType::kLongSparseArray);
                        } else if (res->literal_type_ != LiteralType::kLongSparseArray) {
                            const auto error_info = "Invalid json object type in sparse array!";
                            ParserError(error_info);
                            return nullptr;
                        }
                        res->long_sparse_array_.first.push_back(field_key);
                        res->long_sparse_array_.second.push_back((int64_t)num);
                        break;
                    }
                    case simdjson::number_type::floating_point_number: {
                        if (res.get() == nullptr) {
                            res = std::make_shared<ConstantExpr>(LiteralType::kDoubleSparseArray);
                        } else if (res->literal_type_ != LiteralType::kDoubleSparseArray) {
                            const auto error_info = "Invalid json object type in sparse array!";
                            ParserError(error_info);
                            return nullptr;
                        }
                        res->double_sparse_array_.first.push_back(field_key);
                        res->double_sparse_array_.second.push_back((double)num);
                        break;
                    }
                    default: {
                        const auto error_info = fmt::format("Unrecognized json object type in array");
                        ParserError(error_info);
                        return nullptr;
                    }
                }
            }
            return res;
        }
        default: {
            const auto error_info = fmt::format("Unrecognized json object type");
            ParserError(error_info);
            return nullptr;
        }
    }
}

std::unique_ptr<MatchTensorExpr> GetFusionMatchTensorExpr(SearchOptions &search_options) {
    auto match_tensor_expr = std::make_unique<MatchTensorExpr>();
    std::string column_name;
    std::string search_tensor;
    std::string tensor_data_type;
    std::string match_method;
    // find parameters
    const auto &options = search_options.options_;
    if (const auto it = options.find("match_method"); it != options.end()) {
        match_method = it->second;
    }
    if (const auto it = options.find("column_name"); it != options.end()) {
        column_name = it->second;
    }
    if (const auto it = options.find("search_tensor"); it != options.end()) {
        search_tensor = it->second;
    }
    if (const auto it = options.find("tensor_data_type"); it != options.end()) {
        tensor_data_type = it->second;
        // to_lower
        std::transform(tensor_data_type.begin(), tensor_data_type.end(), tensor_data_type.begin(), [](unsigned char c) { return std::tolower(c); });
    }
    match_tensor_expr->SetSearchMethodStr(std::move(match_method));
    auto column_expr = std::make_unique<ColumnExpr>();
    column_expr->names_.emplace_back(column_name);
    auto cast_column_expr = static_cast<ParsedExpr *>(column_expr.release());
    match_tensor_expr->SetSearchColumn(cast_column_expr);
    const auto tensor_expr = BuildConstantExprFromJson(search_tensor);
    match_tensor_expr->SetQueryTensorStr(std::move(tensor_data_type), tensor_expr.get());
    return match_tensor_expr;
}

} // namespace infinity
