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

#include <string>
#include <vector>

module index_ivfflat;

import infinity_exception;
import stl;
import index_def;
import status;
import third_party;
import serialize;
import index_base;
import logical_type;
import statement_common;

namespace infinity {

SharedPtr<IndexBase> IndexIVFFlat::Make(String file_name, Vector<String> column_names, const Vector<InitParameter *> &index_param_list) {
    SizeT centroids_count = 0;
    MetricType metric_type = MetricType::kInvalid;
    for (auto para : index_param_list) {
        if (para->param_name_ == "centroids_count") {
            centroids_count = std::stoi(para->param_value_);
        } else if (para->param_name_ == "metric") {
            metric_type = StringToMetricType(para->param_value_);
        }
    }
    if (metric_type == MetricType::kInvalid) {
        RecoverableError(Status::LackIndexParam());
    }
    return MakeShared<IndexIVFFlat>(std::move(file_name), std::move(column_names), centroids_count, metric_type);
}

bool IndexIVFFlat::operator==(const IndexIVFFlat &other) const {
    if (this->index_type_ != other.index_type_ || this->file_name_ != other.file_name_ || this->column_names_ != other.column_names_) {
        return false;
    }
    return centroids_count_ == other.centroids_count_ && metric_type_ == other.metric_type_;
}

bool IndexIVFFlat::operator!=(const IndexIVFFlat &other) const { return !(*this == other); }

i32 IndexIVFFlat::GetSizeInBytes() const {
    SizeT size = IndexBase::GetSizeInBytes();
    size += sizeof(centroids_count_);
    size += sizeof(metric_type_);
    return size;
}

void IndexIVFFlat::WriteAdv(char *&ptr) const {
    IndexBase::WriteAdv(ptr);
    WriteBufAdv(ptr, centroids_count_);
    WriteBufAdv(ptr, metric_type_);
}

SharedPtr<IndexBase> IndexIVFFlat::ReadAdv(char *&, int32_t) {
    UnrecoverableError("Not implemented");
    return nullptr;
}

String IndexIVFFlat::ToString() const {
    std::stringstream ss;
    ss << IndexBase::ToString() << ", " << centroids_count_ << ", " << MetricTypeToString(metric_type_);
    return ss.str();
}

nlohmann::json IndexIVFFlat::Serialize() const {
    nlohmann::json res = IndexBase::Serialize();
    res["centroids_count"] = centroids_count_;
    res["metric_type"] = MetricTypeToString(metric_type_);
    return res;
}

SharedPtr<IndexIVFFlat> IndexIVFFlat::Deserialize(const nlohmann::json &) {
    UnrecoverableError("Not implemented");
    return nullptr;
}

void IndexIVFFlat::ValidateColumnDataType(const SharedPtr<BaseTableRef> &base_table_ref, const String &column_name) {
    auto &column_names_vector = *(base_table_ref->column_names_);
    auto &column_types_vector = *(base_table_ref->column_types_);
    SizeT column_id = std::find(column_names_vector.begin(), column_names_vector.end(), column_name) - column_names_vector.begin();
    if (column_id == column_names_vector.size()) {
        RecoverableError(Status::ColumnNotExist(column_name));
    } else if (auto &data_type = column_types_vector[column_id]; data_type->type() != LogicalType::kEmbedding) {
        RecoverableError(Status::InvalidIndexDefinition(
            fmt::format("Invalid parameter for IVFFlat index: column name: {}, data type not supported: {}.", column_name, data_type->ToString())));
    }
}

} // namespace infinity