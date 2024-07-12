#include "fusion_expr.h"
#include "column_expr.h"
#include "search_options.h"
#include "spdlog/fmt/fmt.h"
#include "sstream"
#include <cmath>
#include <utility>

namespace infinity {

FusionExpr::FusionExpr() : ParsedExpr(ParsedExprType::kFusion) {}

FusionExpr::~FusionExpr(){}

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

std::shared_ptr<ConstantExpr> BuildConstantExprFromJson(const nlohmann::json &json_object) {
    switch (json_object.type()) {
        case nlohmann::json::value_t::boolean: {
            auto res = std::make_shared<ConstantExpr>(LiteralType::kBoolean);
            res->bool_value_ = json_object.get<bool>();
            return res;
        }
        case nlohmann::json::value_t::number_unsigned:
        case nlohmann::json::value_t::number_integer: {
            auto res = std::make_shared<ConstantExpr>(LiteralType::kInteger);
            res->integer_value_ = json_object.get<int64_t>();
            return res;
        }
        case nlohmann::json::value_t::number_float: {
            auto res = std::make_shared<ConstantExpr>(LiteralType::kDouble);
            res->double_value_ = json_object.get<double>();
            return res;
        }
        case nlohmann::json::value_t::string: {
            auto res = std::make_shared<ConstantExpr>(LiteralType::kString);
            auto str = json_object.get<std::string>();
            res->str_value_ = strdup(json_object.get<std::string>().c_str());
            return res;
        }
        case nlohmann::json::value_t::array: {
            const uint32_t array_size = json_object.size();
            if (array_size == 0) {
                const auto error_info = "Empty json array!";
                ParserError(error_info);
                return nullptr;
            }
            switch (json_object[0].type()) {
                case nlohmann::json::value_t::boolean:
                case nlohmann::json::value_t::number_unsigned:
                case nlohmann::json::value_t::number_integer: {
                    auto res = std::make_shared<ConstantExpr>(LiteralType::kIntegerArray);
                    res->long_array_.resize(array_size);
                    for (uint32_t i = 0; i < array_size; ++i) {
                        res->long_array_[i] = json_object[i].get<int64_t>();
                    }
                    return res;
                }
                case nlohmann::json::value_t::number_float: {
                    auto res = std::make_shared<ConstantExpr>(LiteralType::kDoubleArray);
                    res->double_array_.resize(array_size);
                    for (uint32_t i = 0; i < array_size; ++i) {
                        res->double_array_[i] = json_object[i].get<double>();
                    }
                    return res;
                }
                case nlohmann::json::value_t::array: {
                    auto res = std::make_shared<ConstantExpr>(LiteralType::kSubArrayArray);
                    res->sub_array_array_.resize(array_size);
                    for (uint32_t i = 0; i < array_size; ++i) {
                        res->sub_array_array_[i] = BuildConstantExprFromJson(json_object[i]);
                    }
                    return res;
                }
                default: {
                    const auto error_info = fmt::format("Unrecognized json object type in array: {}", json_object.type_name());
                    ParserError(error_info);
                    return nullptr;
                }
            }
        }
        case nlohmann::json::value_t::object: {
            std::shared_ptr<ConstantExpr> res = nullptr;
            for (auto iter = json_object.begin(); iter != json_object.end(); ++iter) {
                int64_t key = std::stoll(iter.key());
                const auto &value_obj = iter.value();
                switch (value_obj.type()) {
                    case nlohmann::json::value_t::number_unsigned:
                    case nlohmann::json::value_t::number_integer: {
                        if (res.get() == nullptr) {
                            res = std::make_shared<ConstantExpr>(LiteralType::kLongSparseArray);
                        } else if (res->literal_type_ != LiteralType::kLongSparseArray) {
                            const auto error_info = "Invalid json object type in sparse array!";
                            ParserError(error_info);
                            return nullptr;
                        }
                        res->long_sparse_array_.first.push_back(key);
                        res->long_sparse_array_.second.push_back(value_obj.get<int64_t>());
                        break;
                    }
                    case nlohmann::json::value_t::number_float: {
                        if (res.get() == nullptr) {
                            res = std::make_shared<ConstantExpr>(LiteralType::kDoubleSparseArray);
                        } else if (res->literal_type_ != LiteralType::kDoubleSparseArray) {
                            const auto error_info = "Invalid json object type in sparse array!";
                            ParserError(error_info);
                            return nullptr;
                        }
                        res->double_sparse_array_.first.push_back(key);
                        res->double_sparse_array_.second.push_back(value_obj.get<double>());
                        break;
                    }
                    default: {
                        const auto error_info = fmt::format("Unrecognized json object type in array: {}", json_object.type_name());
                        ParserError(error_info);
                        return nullptr;
                    }
                }
            }
            return res;
        }
        default: {
            const auto error_info = fmt::format("Unrecognized json object type: {}", json_object.type_name());
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
    const auto json_obj = nlohmann::json::parse(search_tensor);
    const auto tensor_expr = BuildConstantExprFromJson(json_obj);
    match_tensor_expr->SetQueryTensorStr(std::move(tensor_data_type), tensor_expr.get());
    return match_tensor_expr;
}

} // namespace infinity
