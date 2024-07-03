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

#include "constant_expr.h"
#include "definition/column_def.h"
#include "expr/parsed_expr.h"
#include "parser_assert.h"
#include "spdlog/fmt/fmt.h"
#include "type/datetime/interval_type.h"
#include "type/info/sparse_info.h"
#include "type/serialize.h"
#include <cstdint>
#include <cstring>
#include <sstream>

namespace infinity {

ConstantExpr::~ConstantExpr() {
    switch (literal_type_) {
        case LiteralType::kString: {
            free(str_value_);
            break;
        }
        case LiteralType::kDate:
        case LiteralType::kTime:
        case LiteralType::kDateTime:
        case LiteralType::kTimestamp: {
            free(date_value_);
            break;
        }
        default:
            break;
    }
}

std::string ConstantExpr::ToString() const {
    switch (literal_type_) {
        case LiteralType::kBoolean:
            return fmt::format("{}", bool_value_);
        case LiteralType::kDouble:
            return fmt::format("{}", double_value_);
        case LiteralType::kString:
            return fmt::format("{}", str_value_);
        case LiteralType::kInteger:
            return fmt::format("{}", integer_value_);
        case LiteralType::kNull: {
            return fmt::format("Null");
        }
        case LiteralType::kDate:
        case LiteralType::kTime:
        case LiteralType::kDateTime:
        case LiteralType::kTimestamp: {
            return fmt::format("{}", date_value_);
        }
        case LiteralType::kInterval: {
            switch (interval_type_) {
                case TimeUnit::kSecond: {
                    return fmt::format("{} seconds", integer_value_);
                }
                case TimeUnit::kMinute: {
                    return fmt::format("{} minutes", integer_value_);
                }
                case TimeUnit::kHour: {
                    return fmt::format("{} hours", integer_value_);
                }
                case TimeUnit::kDay: {
                    return fmt::format("{} days", integer_value_);
                }
                case TimeUnit::kMonth: {
                    return fmt::format("{} months", integer_value_);
                }
                case TimeUnit::kYear: {
                    return fmt::format("{} years", integer_value_);
                }
                default: {
                    ParserError("Invalid interval type.");
                }
            }
        }
        case LiteralType::kIntegerArray: {
            std::stringstream ss;
            size_t len = long_array_.size();
            if (len <= 0) {
                ParserError("Invalid long array length");
            }
            for (size_t i = 0; i < len - 1; ++i) {
                ss << long_array_[i] << ',';
            }
            ss << long_array_.back();
            return ss.str();
        }
        case LiteralType::kDoubleArray: {
            std::stringstream ss;
            size_t len = double_array_.size();
            if (len <= 0) {
                ParserError("Invalid double array length");
            }
            for (size_t i = 0; i < len - 1; ++i) {
                ss << double_array_[i] << ',';
            }
            ss << double_array_.back();
            return ss.str();
        }
        case LiteralType::kSubArrayArray: {
            std::stringstream ss;
            size_t len = sub_array_array_.size();
            if (len <= 0) {
                ParserError("Invalid array array length");
            }
            for (size_t i = 0; i < len - 1; ++i) {
                ss << '[' << sub_array_array_[i]->ToString() << "],";
            }
            ss << '[' << sub_array_array_.back()->ToString() << ']';
            return ss.str();
        }
        case LiteralType::kLongSparseArray: {
            size_t nnz = long_sparse_array_.first.size();
            return SparseT::Sparse2StringT2(long_sparse_array_.first.data(), long_sparse_array_.first.data(), nnz);
        }
        case LiteralType::kDoubleSparseArray: {
            size_t nnz = double_sparse_array_.first.size();
            return SparseT::Sparse2StringT2(double_sparse_array_.first.data(), double_sparse_array_.second.data(), nnz);
        }
        case LiteralType::kEmptyArray: {
            return {};
        }
    }
}

int32_t ConstantExpr::GetSizeInBytes() const {
    int32_t size = sizeof(LiteralType);
    switch (literal_type_) {
        case LiteralType::kBoolean: {
            size += sizeof(bool);
            break;
        }
        case LiteralType::kDouble: {
            size += sizeof(double);
            break;
        }
        case LiteralType::kString: {
            size += sizeof(int32_t) + (std::string(str_value_)).length();
            break;
        }
        case LiteralType::kInteger: {
            size += sizeof(int64_t);
            break;
        }
        case LiteralType::kEmptyArray:
        case LiteralType::kNull: {
            break;
        }
        case LiteralType::kDate:
        case LiteralType::kTime:
        case LiteralType::kDateTime:
        case LiteralType::kTimestamp: {
            size += sizeof(int32_t) + (std::string(date_value_)).length();
            break;
        }
        case LiteralType::kIntegerArray: {
            size += sizeof(int64_t);
            size += sizeof(int64_t) * long_array_.size();
            break;
        }
        case LiteralType::kDoubleArray: {
            size += sizeof(int64_t);
            size += sizeof(double) * double_array_.size();
            break;
        }
        case LiteralType::kSubArrayArray: {
            size += sizeof(int64_t);
            for (const auto &val : sub_array_array_) {
                size += val->GetSizeInBytes();
            }
            break;
        }
        case LiteralType::kLongSparseArray: {
            size += sizeof(int64_t) + sizeof(int64_t) * long_sparse_array_.first.size();
            size += sizeof(int64_t) + sizeof(int64_t) * long_sparse_array_.second.size();
            break;
        }
        case LiteralType::kDoubleSparseArray: {
            size += sizeof(int64_t) + sizeof(int64_t) * double_sparse_array_.first.size();
            size += sizeof(int64_t) + sizeof(double) * double_sparse_array_.second.size();
            break;
        }
        case LiteralType::kInterval: {
            size += sizeof(int32_t);
            size += sizeof(int64_t);
            break;
        }
    }
    return size;
}

void ConstantExpr::WriteAdv(char *&ptr) const {
    WriteBufAdv<LiteralType>(ptr, literal_type_);
    switch (literal_type_) {
        case LiteralType::kBoolean: {
            WriteBufAdv<bool>(ptr, bool_value_);
            break;
        }
        case LiteralType::kDouble: {
            WriteBufAdv<double>(ptr, double_value_);
            break;
        }
        case LiteralType::kString: {
            WriteBufAdv<std::string>(ptr, std::string(str_value_));
            break;
        }
        case LiteralType::kInteger: {
            WriteBufAdv<int64_t>(ptr, integer_value_);
            break;
        }
        case LiteralType::kEmptyArray:
        case LiteralType::kNull: {
            break;
        }
        case LiteralType::kDate:
        case LiteralType::kTime:
        case LiteralType::kDateTime:
        case LiteralType::kTimestamp: {
            WriteBufAdv<std::string>(ptr, std::string(date_value_));
            break;
        }
        case LiteralType::kIntegerArray: {
            WriteBufAdv<int64_t>(ptr, long_array_.size());
            for (const auto &val : long_array_) {
                WriteBufAdv<int64_t>(ptr, val);
            }
            break;
        }
        case LiteralType::kDoubleArray: {
            WriteBufAdv<int64_t>(ptr, double_array_.size());
            for (const auto &val : double_array_) {
                WriteBufAdv<double>(ptr, val);
            }
            break;
        }
        case LiteralType::kSubArrayArray: {
            WriteBufAdv<int64_t>(ptr, sub_array_array_.size());
            for (const auto &val : sub_array_array_) {
                val->WriteAdv(ptr);
            }
            break;
        }
        case LiteralType::kInterval: {
            WriteBufAdv<TimeUnit>(ptr, interval_type_);
            WriteBufAdv<int64_t>(ptr, integer_value_);
            break;
        }
        case LiteralType::kLongSparseArray: {
            WriteBufAdv<int64_t>(ptr, long_sparse_array_.first.size());
            for (size_t i = 0; i < long_sparse_array_.first.size(); ++i) {
                WriteBufAdv<int64_t>(ptr, long_sparse_array_.first[i]);
            }
            WriteBufAdv<int64_t>(ptr, long_sparse_array_.second.size());
            for (size_t i = 0; i < long_sparse_array_.second.size(); ++i) {
                WriteBufAdv<int64_t>(ptr, long_sparse_array_.second[i]);
            }
            break;
        }
        case LiteralType::kDoubleSparseArray: {
            WriteBufAdv<int64_t>(ptr, double_sparse_array_.first.size());
            for (size_t i = 0; i < double_sparse_array_.first.size(); ++i) {
                WriteBufAdv<int64_t>(ptr, double_sparse_array_.first[i]);
            }
            WriteBufAdv<int64_t>(ptr, double_sparse_array_.second.size());
            for (size_t i = 0; i < double_sparse_array_.second.size(); ++i) {
                WriteBufAdv<double>(ptr, double_sparse_array_.second[i]);
            }
            break;
        }
    }
}

std::shared_ptr<ParsedExpr> ConstantExpr::ReadAdv(char *&ptr, int32_t maxbytes) {
    char *const ptr_end = ptr + maxbytes;
    LiteralType literal_type = ReadBufAdv<LiteralType>(ptr);
    auto const_expr = new ConstantExpr(literal_type);
    switch (literal_type) {
        case LiteralType::kBoolean: {
            bool bool_value = ReadBufAdv<bool>(ptr);
            const_expr->bool_value_ = bool_value;
            break;
        }
        case LiteralType::kDouble: {
            double double_value = ReadBufAdv<double>(ptr);
            const_expr->double_value_ = double_value;
            break;
        }
        case LiteralType::kString: {
            std::string str_value = ReadBufAdv<std::string>(ptr);
            const_expr->str_value_ = strdup(str_value.c_str());
            break;
        }
        case LiteralType::kInteger: {
            int64_t integer_value = ReadBufAdv<int64_t>(ptr);
            const_expr->integer_value_ = integer_value;
            break;
        }
        case LiteralType::kEmptyArray:
        case LiteralType::kNull: {
            break;
        }
        case LiteralType::kDate:
        case LiteralType::kTime:
        case LiteralType::kDateTime:
        case LiteralType::kTimestamp: {
            std::string date_value = ReadBufAdv<std::string>(ptr);
            const_expr->date_value_ = strdup(date_value.c_str());
            break;
        }
        case LiteralType::kIntegerArray: {
            size_t len = ReadBufAdv<int64_t>(ptr);
            for (size_t i = 0; i < len; ++i) {
                int64_t val = ReadBufAdv<int64_t>(ptr);
                const_expr->long_array_.push_back(val);
            }
            break;
        }
        case LiteralType::kDoubleArray: {
            size_t len = ReadBufAdv<int64_t>(ptr);
            for (size_t i = 0; i < len; ++i) {
                double val = ReadBufAdv<double>(ptr);
                const_expr->double_array_.push_back(val);
            }
            break;
        }
        case LiteralType::kSubArrayArray: {
            size_t len = ReadBufAdv<int64_t>(ptr);
            for (size_t i = 0; i < len; ++i) {
                auto sub_arr = std::static_pointer_cast<ConstantExpr>(ReadAdv(ptr, ptr_end - ptr));
                const_expr->sub_array_array_.push_back(std::move(sub_arr));
            }
            break;
        }
        case LiteralType::kInterval: {
            TimeUnit interval_type = ReadBufAdv<TimeUnit>(ptr);
            int64_t integer_value = ReadBufAdv<int64_t>(ptr);
            const_expr->interval_type_ = interval_type;
            const_expr->integer_value_ = integer_value;
            break;
        }
        case LiteralType::kLongSparseArray: {
            size_t len = ReadBufAdv<int64_t>(ptr);
            const_expr->long_sparse_array_.first.resize(len);
            for (size_t i = 0; i < len; ++i) {
                const_expr->long_sparse_array_.first[i] = ReadBufAdv<int64_t>(ptr);
            }
            len = ReadBufAdv<int64_t>(ptr);
            const_expr->long_sparse_array_.second.resize(len);
            for (size_t i = 0; i < len; ++i) {
                const_expr->long_sparse_array_.second[i] = ReadBufAdv<int64_t>(ptr);
            }
            break;
        }
        case LiteralType::kDoubleSparseArray: {
            size_t len = ReadBufAdv<int64_t>(ptr);
            const_expr->double_sparse_array_.first.resize(len);
            for (size_t i = 0; i < len; ++i) {
                const_expr->double_sparse_array_.first[i] = ReadBufAdv<int64_t>(ptr);
            }
            len = ReadBufAdv<int64_t>(ptr);
            const_expr->double_sparse_array_.second.resize(len);
            for (size_t i = 0; i < len; ++i) {
                const_expr->double_sparse_array_.second[i] = ReadBufAdv<double>(ptr);
            }
            break;
        }
    }
    maxbytes = ptr_end - ptr;
    ParserAssert(maxbytes >= 0, "ptr goes out of range when reading constant expression");
    return std::shared_ptr<ParsedExpr>(const_expr);
}

nlohmann::json ConstantExpr::Serialize() const {
    nlohmann::json j;
    j["type"] = static_cast<int32_t>(literal_type_);
    switch (literal_type_) {
        case LiteralType::kBoolean: {
            j["value"] = bool_value_;
            break;
        }
        case LiteralType::kDouble: {
            j["value"] = double_value_;
            break;
        }
        case LiteralType::kString: {
            j["value"] = str_value_;
            break;
        }
        case LiteralType::kInteger: {
            j["value"] = integer_value_;
            break;
        }
        case LiteralType::kEmptyArray:
        case LiteralType::kNull: {
            break;
        }
        case LiteralType::kDate:
        case LiteralType::kTime:
        case LiteralType::kDateTime:
        case LiteralType::kTimestamp: {
            j["value"] = date_value_;
            break;
        }
        case LiteralType::kIntegerArray: {
            j["value"] = long_array_;
            break;
        }
        case LiteralType::kDoubleArray: {
            j["value"] = double_array_;
            break;
        }
        case LiteralType::kSubArrayArray: {
            nlohmann::json sub_array_j(nlohmann::detail::value_t::array);
            for (const auto &val : sub_array_array_) {
                sub_array_j.emplace_back(val->Serialize());
            }
            j["value"] = std::move(sub_array_j);
            break;
        }
        case LiteralType::kInterval: {
            ParserError("Interval type is not supported in JSON serialization");
        }
        case LiteralType::kLongSparseArray: {
            nlohmann::json sub_array_j;
            sub_array_j["indices"] = long_sparse_array_.first;
            sub_array_j["data"] = long_sparse_array_.second;
            j["value"] = std::move(sub_array_j);
            break;
        }
        case LiteralType::kDoubleSparseArray: {
            nlohmann::json sub_array_j;
            sub_array_j["indices"] = double_sparse_array_.first;
            sub_array_j["data"] = double_sparse_array_.second;
            j["value"] = std::move(sub_array_j);
            break;
        }
    }
    return j;
}

std::shared_ptr<ParsedExpr> ConstantExpr::Deserialize(const nlohmann::json &constant_expr) {
    LiteralType literal_type = static_cast<LiteralType>(constant_expr["type"].get<int32_t>());
    auto const_expr = new ConstantExpr(literal_type);
    switch (literal_type) {
        case LiteralType::kBoolean: {
            const_expr->bool_value_ = constant_expr["value"].get<bool>();
            break;
        }
        case LiteralType::kDouble: {
            const_expr->double_value_ = constant_expr["value"].get<double>();
            break;
        }
        case LiteralType::kString: {
            const_expr->str_value_ = strdup(constant_expr["value"].get<std::string>().c_str());
            break;
        }
        case LiteralType::kInteger: {
            const_expr->integer_value_ = constant_expr["value"].get<int64_t>();
            break;
        }
        case LiteralType::kEmptyArray:
        case LiteralType::kNull: {
            break;
        }
        case LiteralType::kDate:
        case LiteralType::kTime:
        case LiteralType::kDateTime:
        case LiteralType::kTimestamp: {
            const_expr->date_value_ = strdup(constant_expr["value"].get<std::string>().c_str());
            break;
        }
        case LiteralType::kIntegerArray: {
            const_expr->long_array_ = constant_expr["value"].get<std::vector<int64_t>>();
            break;
        }
        case LiteralType::kDoubleArray: {
            const_expr->double_array_ = constant_expr["value"].get<std::vector<double>>();
            break;
        }
        case LiteralType::kSubArrayArray: {
            for (const nlohmann::json &array = constant_expr["value"]; const auto &val : array) {
                auto sub_arr = std::static_pointer_cast<ConstantExpr>(Deserialize(val));
                const_expr->sub_array_array_.push_back(std::move(sub_arr));
            }
            break;
        }
        case LiteralType::kInterval: {
            ParserError("Interval type is not supported in JSON serialization");
        }
        case LiteralType::kLongSparseArray: {
            const_expr->long_sparse_array_.first = constant_expr["value"]["indices"].get<std::vector<int64_t>>();
            const_expr->long_sparse_array_.second = constant_expr["value"]["data"].get<std::vector<int64_t>>();
            break;
        }
        case LiteralType::kDoubleSparseArray: {
            const_expr->double_sparse_array_.first = constant_expr["value"]["indices"].get<std::vector<int64_t>>();
            const_expr->double_sparse_array_.second = constant_expr["value"]["data"].get<std::vector<double>>();
            break;
        }
    }
    return std::shared_ptr<ParsedExpr>(const_expr);
}

void ConstantExpr::TrySortSparseVec(const ColumnDef *col_def) {
    if (literal_type_ != LiteralType::kLongSparseArray && literal_type_ != LiteralType::kDoubleSparseArray) {
        return;
    }
    if (col_def != nullptr) {
        auto data_type = col_def->type()->type();
        if (data_type != LogicalType::kSparse) {
            return;
        }
        const auto *sparse_info = static_cast<const SparseInfo *>(col_def->type()->type_info().get());
        if (sparse_info->StoreType() == SparseStoreType::kSorted) {
            return;
        }
    }
    if (literal_type_ == LiteralType::kLongSparseArray) {
        if (long_sparse_array_.second.empty()) {
            // bit sparse vec
            std::sort(long_sparse_array_.first.begin(), long_sparse_array_.first.end());
        } else {
            std::vector<std::pair<int64_t, int64_t>> pairs;
            for (size_t i = 0; i < long_sparse_array_.first.size(); ++i) {
                pairs.emplace_back(long_sparse_array_.first[i], long_sparse_array_.second[i]);
            }
            std::sort(pairs.begin(), pairs.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
        }
    } else {
        std::vector<std::pair<int64_t, double>> pairs;
        for (size_t i = 0; i < double_sparse_array_.first.size(); ++i) {
            pairs.emplace_back(double_sparse_array_.first[i], double_sparse_array_.second[i]);
        }
        std::sort(pairs.begin(), pairs.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
    }
}

} // namespace infinity
