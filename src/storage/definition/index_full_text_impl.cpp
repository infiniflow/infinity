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

module;

module infinity_core:index_full_text.impl;

import :index_full_text;
import :index_base;
import :status;
import :infinity_exception;
import :base_table_ref;
import :index_defines;
import :analyzer_pool;
import :analyzer;
import :logger;

import std;
import std.compat;
import third_party;

import logical_type;
import statement_common;
import serialize;

namespace infinity {

void ToLowerString(std::string &lower) {
    for (auto &c : lower) {
        c = tolower(c);
    }
}

std::shared_ptr<IndexBase> IndexFullText::Make(std::shared_ptr<std::string> index_name,
                                         std::shared_ptr<std::string> index_comment,
                                         const std::string &file_name,
                                         std::vector<std::string> column_names,
                                         const std::vector<InitParameter *> &index_param_list) {
    std::string analyzer_name{};
    optionflag_t flag = OPTION_FLAG_ALL;
    size_t param_count = index_param_list.size();
    for (size_t param_idx = 0; param_idx < param_count; ++param_idx) {
        InitParameter *parameter = index_param_list[param_idx];
        std::string para_name = parameter->param_name_;
        ToLowerString(para_name);
        if (para_name == "analyzer") {
            analyzer_name = parameter->param_value_;
        } else if (para_name == "flag") {
            flag = std::strtoul(parameter->param_value_.c_str(), nullptr, 10);
        } else if (para_name == "realtime") {
            std::string realtime_str = parameter->param_value_;
            ToLowerString(realtime_str);
            if (realtime_str == "true") {
                FlagAddRealtime(flag);
            } else if (realtime_str != "false") {
                LOG_WARN(fmt::format("Unknown parameter value: {}, {}", para_name, parameter->param_value_));
            }
        } else {
            LOG_WARN(fmt::format("Unknown parameter: {}", para_name));
        }
    }
    if (analyzer_name.empty()) {
        analyzer_name = "standard";
    }
    auto [analyzer, status] = AnalyzerPool::instance().GetAnalyzer(analyzer_name);
    if (!status.ok()) {
        RecoverableError(status);
    }
    return std::make_shared<IndexFullText>(index_name, index_comment, file_name, std::move(column_names), analyzer_name, (optionflag_t)flag);
}

bool IndexFullText::operator==(const IndexFullText &other) const {
    if (this->index_type_ != other.index_type_ || this->file_name_ != other.file_name_ || this->column_names_ != other.column_names_) {
        return false;
    }
    return analyzer_ == other.analyzer_;
}

bool IndexFullText::operator!=(const IndexFullText &other) const { return !(*this == other); }

i32 IndexFullText::GetSizeInBytes() const {
    size_t size = IndexBase::GetSizeInBytes();
    size += sizeof(int32_t) + analyzer_.length();
    size += sizeof(optionflag_t); // for flag_
    return size;
}

void IndexFullText::WriteAdv(char *&ptr) const {
    IndexBase::WriteAdv(ptr);
    WriteBufAdv(ptr, analyzer_);
    WriteBufAdv(ptr, u8(flag_));
}

std::string IndexFullText::ToString() const {
    std::stringstream ss;
    std::string output_str = IndexBase::ToString();
    if (!analyzer_.empty()) {
        output_str += ", " + analyzer_;
    }
    return output_str;
}

std::string IndexFullText::BuildOtherParamsString() const {
    std::stringstream ss;
    ss << "analyzer = " << analyzer_;
    return ss.str();
}

nlohmann::json IndexFullText::Serialize() const {
    nlohmann::json res = IndexBase::Serialize();
    res["analyzer"] = analyzer_;
    res["flag"] = flag_;
    return res;
}

std::shared_ptr<IndexFullText> IndexFullText::Deserialize(std::string_view index_def_str) {
    Status status = Status::NotSupport("Not implemented");
    RecoverableError(status);
    return nullptr;
}

void IndexFullText::ValidateColumnDataType(const std::shared_ptr<BaseTableRef> &base_table_ref, const std::string &column_name) {
    auto &column_names_vector = *(base_table_ref->column_names_);
    auto &column_types_vector = *(base_table_ref->column_types_);
    size_t column_id = std::find(column_names_vector.begin(), column_names_vector.end(), column_name) - column_names_vector.begin();
    if (column_id == column_names_vector.size()) {
        Status status = Status::ColumnNotExist(column_name);
        RecoverableError(status);
    } else if (auto &data_type = column_types_vector[column_id]; data_type->type() != LogicalType::kVarchar) {
        Status status = Status::InvalidIndexDefinition(
            fmt::format("Attempt to create full-text index on column: {}, data type: {}.", column_name, data_type->ToString()));
        RecoverableError(status);
    }
}

} // namespace infinity
