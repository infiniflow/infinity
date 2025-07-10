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

#include "column_def.h"
#include "expr/constant_expr.h"
#include "expr/parsed_expr.h"
#include "statement/statement_common.h"
#include "type/data_type.h"
#include "type/info/array_info.h"
#include "type/info/decimal_info.h"
#include "type/info/embedding_info.h"
#include "type/info/sparse_info.h"
#include "type/serialize.h"

namespace infinity {

std::pair<std::shared_ptr<DataType>, std::string>
ColumnType::GetDataTypeFromColumnType(const ColumnType &column_type, const std::vector<std::unique_ptr<InitParameter>> &index_param_list) {
    std::shared_ptr<TypeInfo> type_info_ptr{};
    switch (column_type.logical_type_) {
        case LogicalType::kDecimal: {
            type_info_ptr = DecimalInfo::Make(column_type.precision, column_type.scale);
            if (!type_info_ptr) {
                return {nullptr, "Fail to create decimal info."};
            }
            break;
        }
        case LogicalType::kEmbedding:
        case LogicalType::kMultiVector:
        case LogicalType::kTensor:
        case LogicalType::kTensorArray: {
            type_info_ptr = EmbeddingInfo::Make(column_type.embedding_type_, column_type.width);
            break;
        }
        case LogicalType::kSparse: {
            const auto store_type = SparseInfo::ParseStoreType(index_param_list);
            type_info_ptr = SparseInfo::Make(column_type.embedding_type_, column_type.width, store_type);
            if (!type_info_ptr) {
                return {nullptr, "Fail to create sparse info."};
            }
            break;
        }
        case LogicalType::kArray: {
            if (column_type.element_types_.size() != 1) {
                return {nullptr, "ArrayInfo::Make: column_type.element_types_ is nullptr or size is not 1"};
            }
            const auto [element_result, fail_reason] = GetDataTypeFromColumnType(*(column_type.element_types_.front()), index_param_list);
            if (!element_result) {
                return {nullptr, std::format("Fail to create element type for array. Reason: {}", fail_reason)};
            }
            type_info_ptr = ArrayInfo::Make(std::move(*element_result));
            break;
        }
        default: {
            break;
        }
    }
    return std::make_pair(std::make_shared<DataType>(column_type.logical_type_, std::move(type_info_ptr)), std::string{});
}

std::unordered_map<std::string, ConstraintType> string_to_constraint_type = {
    {"primary key", ConstraintType::kPrimaryKey},
    {"unique", ConstraintType::kUnique},
    {"null", ConstraintType::kNull},
    {"not null", ConstraintType::kNotNull},
    {"invalid", ConstraintType::kInvalid},
};

std::string ConstrainTypeToString(ConstraintType type) {
    switch (type) {
        case ConstraintType::kPrimaryKey:
            return "PrimaryKey";
        case ConstraintType::kUnique:
            return "Unique";
        case ConstraintType::kNull:
            return "Nullable";
        case ConstraintType::kNotNull:
            return "Not nullable";
        case ConstraintType::kInvalid:
            return "Invalid";
    }
    ParserError("Unexpected error.");
}

ConstraintType StringToConstraintType(const std::string &type) {
    auto iter = string_to_constraint_type.find(type);
    if (iter != string_to_constraint_type.end()) {
        return iter->second;
    } else {
        return ConstraintType::kInvalid;
    }
}

ColumnDef::ColumnDef(int64_t id,
                     std::shared_ptr<DataType> column_type,
                     std::string column_name,
                     std::set<ConstraintType> constraints,
                     std::string comment,
                     std::shared_ptr<ParsedExpr> default_expr)
    : TableElement(TableElementType::kColumn), id_(id), column_type_(std::move(column_type)), name_(std::move(column_name)),
      constraints_(std::move(constraints)), comment_(std::move(comment)), default_expr_(std::move(default_expr)) {
    // TODO: type check for default_expr
    if (!default_expr_) {
        default_expr_ = std::make_shared<ConstantExpr>(LiteralType::kNull);
    }
}

ColumnDef::ColumnDef(int64_t id,
                     std::shared_ptr<DataType> column_type,
                     std::string column_name,
                     std::set<ConstraintType> constraints,
                     std::shared_ptr<ParsedExpr> default_expr)
    : TableElement(TableElementType::kColumn), id_(id), column_type_(std::move(column_type)), name_(std::move(column_name)),
      constraints_(std::move(constraints)), default_expr_(std::move(default_expr)) {
    // TODO: type check for default_expr
    if (!default_expr_) {
        default_expr_ = std::make_shared<ConstantExpr>(LiteralType::kNull);
    }
}

ColumnDef::ColumnDef(std::shared_ptr<DataType> column_type, std::shared_ptr<ParsedExpr> default_expr)
    : TableElement(TableElementType::kColumn), column_type_(std::move(column_type)), default_expr_(std::move(default_expr)) {
    if (!default_expr_) {
        default_expr_ = std::make_shared<ConstantExpr>(LiteralType::kNull);
    }
}

ColumnDef::ColumnDef(std::shared_ptr<DataType> column_type, std::string comment, std::shared_ptr<ParsedExpr> default_expr)
    : TableElement(TableElementType::kColumn), column_type_(std::move(column_type)), comment_(std::move(comment)),
      default_expr_(std::move(default_expr)) {
    if (!default_expr_) {
        default_expr_ = std::make_shared<ConstantExpr>(LiteralType::kNull);
    }
}

bool ColumnDef::operator==(const ColumnDef &other) const {
    bool res = type_ == other.type_ && id_ == other.id_ && name_ == other.name_ && column_type_ != nullptr && other.column_type_ != nullptr &&
               *column_type_ == *other.column_type_ && constraints_.size() == other.constraints_.size() &&
               build_bloom_filter_ == other.build_bloom_filter_ && comment_ == other.comment_;
    if (!res) {
        return false;
    }
    for (auto &con : constraints_) {
        if (!other.constraints_.contains(con)) {
            return false;
        }
    }
    return true;
}

int32_t ColumnDef::GetSizeInBytes() const {
    int32_t size = 0;
    size += sizeof(int64_t); // id_
    size += column_type_->GetSizeInBytes();
    size += sizeof(int32_t) + name_.size();
    size += sizeof(int32_t) + constraints_.size() * sizeof(ConstraintType);
    size += sizeof(int32_t) + comment_.size();
    size += (dynamic_cast<ConstantExpr *>(default_expr_.get()))->GetSizeInBytes();
    size += sizeof(uint8_t); // build_bloom_filter_
    return size;
}

void ColumnDef::WriteAdv(char *&ptr) const {
    WriteBufAdv(ptr, id_);
    column_type_->WriteAdv(ptr);
    WriteBufAdv(ptr, name_);
    WriteBufAdv(ptr, (int32_t)constraints_.size());
    for (const auto &cons : constraints_) {
        WriteBufAdv(ptr, cons);
    }
    WriteBufAdv(ptr, comment_);
    (dynamic_cast<ConstantExpr *>(default_expr_.get()))->WriteAdv(ptr);
    uint8_t bf = build_bloom_filter_ ? 1 : 0;
    WriteBufAdv(ptr, bf);
}

std::shared_ptr<ColumnDef> ColumnDef::ReadAdv(const char *&ptr, int32_t maxbytes) {
    int64_t id = ReadBufAdv<int64_t>(ptr);
    std::shared_ptr<DataType> column_type = DataType::ReadAdv(ptr, maxbytes);
    std::string name = ReadBufAdv<std::string>(ptr);
    int32_t constraint_size = ReadBufAdv<int32_t>(ptr);
    std::set<ConstraintType> constraints;
    for (int32_t i = 0; i < constraint_size; i++) {
        constraints.insert(ReadBufAdv<ConstraintType>(ptr));
    }
    std::string comment = ReadBufAdv<std::string>(ptr);
    std::shared_ptr<ParsedExpr> default_expr = ConstantExpr::ReadAdv(ptr, maxbytes);
    auto column_def = std::make_shared<ColumnDef>(id, column_type, name, constraints, comment, default_expr);
    uint8_t bf = ReadBufAdv<uint8_t>(ptr);
    column_def->build_bloom_filter_ = bf == 1;
    return column_def;
}

std::string ColumnDef::ToString() const {
    std::stringstream ss;
    ss << name_ << " " << column_type_->ToString();
    for (auto &constraint : constraints_) {
        ss << " " << ConstrainTypeToString(constraint);
    }
    if (!comment_.empty()) {
        ss << " comment " << comment_;
    }
    if (default_expr_ != nullptr) {
        ss << " default " << default_expr_->ToString();
    }
    return ss.str();
}

void ColumnDef::ToJson(rapidjson::Writer<rapidjson::StringBuffer>& writer) const {
    writer.Key("column_type");
    writer.StartObject();
    this->type()->Serialize(writer);
    writer.EndObject();

    writer.Key("column_id");
    writer.Int64(this->id());
    writer.Key("column_name");
    writer.String(this->name().c_str());

    writer.Key("constraints");
    writer.StartArray();
    for (const auto &column_constraint : this->constraints_) {
        writer.Int((int32_t)column_constraint);
    }
    writer.EndArray();

    if (!(this->comment().empty())) {
        writer.Key("column_comment");
        writer.String(this->comment().c_str());
    }

    if (this->has_default_value()) {
        writer.Key("default");
        auto default_expr = dynamic_pointer_cast<ConstantExpr>(this->default_expr_);
        writer.StartObject();
        default_expr->Serialize(writer);
        writer.EndObject();
    }
}

std::shared_ptr<ColumnDef> ColumnDef::FromJson(std::string_view col_def_str) {
    simdjson::ondemand::parser parser;
    simdjson::padded_string col_def_json(col_def_str);
    simdjson::ondemand::document doc = parser.iterate(col_def_json);

    auto column_type = DataType::Deserialize(doc["column_type"].raw_json());
    int64_t column_id = doc["column_id"].get<int64_t>();
    std::string column_name = doc["column_name"].get<std::string>();

    std::set<ConstraintType> constraints;
    if (simdjson::ondemand::array constraints_json; doc["constraints"].get(constraints_json) == simdjson::SUCCESS) {
        for (auto item : constraints_json) {
            ConstraintType constraint = static_cast<ConstraintType>(static_cast<char>(item.get<char>()));
            constraints.insert(constraint);
        }
    }

    std::string column_comment;
    if (std::string column_comment_json; doc["column_comment"].get<std::string>(column_comment_json) == simdjson::SUCCESS) {
        column_comment = column_comment_json;
    }

    std::shared_ptr<ParsedExpr> default_expr = nullptr;
    if (auto default_expr_json = doc["default"]; default_expr_json.error() == simdjson::SUCCESS) {
        default_expr = ConstantExpr::Deserialize(default_expr_json.raw_json());
    }

    return std::make_shared<ColumnDef>(column_id, column_type, column_name, constraints, column_comment, default_expr);
}

} // namespace infinity