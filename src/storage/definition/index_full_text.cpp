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

#include <sstream>
#include <string>
#include <vector>

module index_full_text;

import stl;
import index_base;
import third_party;
import status;
import serialize;
import infinity_exception;
import statement_common;
import base_table_ref;
import logical_type;
import index_defines;
import analyzer_pool;
import analyzer;

namespace infinity {

void ToLowerString(String &lower) { std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower); }

SharedPtr<IndexBase> IndexFullText::Make(SharedPtr<String> index_name,
                                         const String &file_name,
                                         Vector<String> column_names,
                                         const Vector<InitParameter *> &index_param_list) {
    String analyzer{};
    u64 flag = OPTION_FLAG_ALL;
    SizeT param_count = index_param_list.size();
    for (SizeT param_idx = 0; param_idx < param_count; ++param_idx) {
        InitParameter *parameter = index_param_list[param_idx];
        String para_name = parameter->param_name_;
        ToLowerString(para_name);
        if (para_name == "analyzer") {
            analyzer = parameter->param_value_;
        } else if (para_name == "flag") {
            flag = std::strtoul(parameter->param_value_.c_str(), nullptr, 10);
        }
    }
    if (analyzer.empty()) {
        analyzer = "standard";
    }
    UniquePtr<Analyzer> ana = AnalyzerPool::instance().Get(analyzer);
    if (ana.get() == nullptr) {
        RecoverableError(Status::InvalidIndexDefinition(fmt::format("Attempt to create full-text index using invalid analyer: {}.", analyzer)));
    }
    return MakeShared<IndexFullText>(index_name, file_name, std::move(column_names), analyzer, (optionflag_t)flag);
}

bool IndexFullText::operator==(const IndexFullText &other) const {
    if (this->index_type_ != other.index_type_ || this->file_name_ != other.file_name_ || this->column_names_ != other.column_names_) {
        return false;
    }
    return analyzer_ == other.analyzer_;
}

bool IndexFullText::operator!=(const IndexFullText &other) const { return !(*this == other); }

i32 IndexFullText::GetSizeInBytes() const {
    SizeT size = IndexBase::GetSizeInBytes();
    size += sizeof(int32_t) + analyzer_.length();
    size += sizeof(optionflag_t); // for flag_
    return size;
}

void IndexFullText::WriteAdv(char *&ptr) const {
    IndexBase::WriteAdv(ptr);
    WriteBufAdv(ptr, analyzer_);
    WriteBufAdv(ptr, u8(flag_));
}

String IndexFullText::ToString() const {
    std::stringstream ss;
    String output_str = IndexBase::ToString();
    if (!analyzer_.empty()) {
        output_str += ", " + analyzer_;
    }
    return output_str;
}


String IndexFullText::BuildOtherParamsString() const {
    std::stringstream ss;
    ss << "analyzer = " << analyzer_;
    return ss.str();
}


nlohmann::json IndexFullText::Serialize() const {
    nlohmann::json res = IndexBase::Serialize();
    res["analyzer"] = analyzer_;
    return res;
}

SharedPtr<IndexFullText> IndexFullText::Deserialize(const nlohmann::json &) {
    RecoverableError(Status::NotSupport("Not implemented"));
    return nullptr;
}

void IndexFullText::ValidateColumnDataType(const SharedPtr<BaseTableRef> &base_table_ref, const String &column_name) {
    auto &column_names_vector = *(base_table_ref->column_names_);
    auto &column_types_vector = *(base_table_ref->column_types_);
    SizeT column_id = std::find(column_names_vector.begin(), column_names_vector.end(), column_name) - column_names_vector.begin();
    if (column_id == column_names_vector.size()) {
        RecoverableError(Status::ColumnNotExist(column_name));
    } else if (auto &data_type = column_types_vector[column_id]; data_type->type() != LogicalType::kVarchar) {
        RecoverableError(Status::InvalidIndexDefinition(
            fmt::format("Attempt to create full-text index on column: {}, data type: {}.", column_name, data_type->ToString())));
    }
}

} // namespace infinity
