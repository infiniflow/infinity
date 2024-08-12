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

module index_diskann;

import stl;
import status;
import index_base;
import third_party;
import infinity_exception;
import serialize;
import default_values;
import index_base;
import logical_type;
import statement_common;
import logger;

namespace infinity {

String DiskAnnEncodeTypeToString(DiskAnnEncodeType encode_type){
    switch (encode_type) {
        case DiskAnnEncodeType::kPlain:
            return "plain";
        default:
            return "invalid";
    }
}

DiskAnnEncodeType StringToDiskAnnEncodeType(const String &str){
    if (str == "plain") {
        return DiskAnnEncodeType::kPlain;
    } else {
        return DiskAnnEncodeType::kInvalid;
    }
}

SharedPtr<IndexBase>
IndexDiskAnn::Make(SharedPtr<String> index_name, const String &file_name, Vector<String> column_names, const Vector<InitParameter *> &index_param_list) {
    SizeT R = DISKANN_R;
    SizeT L = DISKANN_L;
    SizeT num_pq_chunks = DISKANN_NUM_PQ_CHUNKS;
    SizeT num_parts = DISKANN_NUM_PARTS;

    DiskAnnEncodeType encode_type = DiskAnnEncodeType::kPlain;
    MetricType metric_type = MetricType::kInvalid;

    for (const auto &param : index_param_list) {
        if (param->param_name_ == "metric") {
            metric_type = StringToMetricType(param->param_value_);
        } else if (param->param_name_ == "encode") {
            encode_type = StringToDiskAnnEncodeType(param->param_value_);
        } else if (param->param_name_ == "r") {
            R = std::stoul(param->param_value_);
        } else if (param->param_name_ == "l") {
            L = std::stoul(param->param_value_);
        } else if (param->param_name_ == "num_pq_chunks") {
            num_pq_chunks = std::stoul(param->param_value_);
        } else if (param->param_name_ == "num_parts") {
            num_parts = std::stoul(param->param_value_);
        } else {
            Status status = Status::InvalidIndexParam(param->param_name_);
            RecoverableError(status);
        }
    }

    if (metric_type == MetricType::kInvalid) {
        Status status = Status::InvalidIndexParam("Metric type");
        RecoverableError(status);
    }

    if (encode_type == DiskAnnEncodeType::kInvalid) {
        Status status = Status::InvalidIndexParam("Encode type");
        RecoverableError(status);
    }

    return std::make_shared<IndexDiskAnn>(index_name, file_name, std::move(column_names), metric_type, encode_type, R, L, num_pq_chunks, num_parts);
}

bool IndexDiskAnn::operator==(const IndexDiskAnn &other) const {
    if (this->index_type_ != other.index_type_ || this->file_name_ != other.file_name_ || this->column_names_ != other.column_names_) {
        return false;
    }
    return metric_type_ == other.metric_type_ && encode_type_ == other.encode_type_ &&  R_ == other.R_ && L_ == other.L_ && 
    num_pq_chunks_ == other.num_pq_chunks_ && num_parts_ == other.num_parts_;
}

bool IndexDiskAnn::operator!=(const IndexDiskAnn &other) const {
    return !(*this == other);
}

i32 IndexDiskAnn::GetSizeInBytes() const {
    SizeT size = IndexBase::GetSizeInBytes();
    size += sizeof(metric_type_);
    size += sizeof(encode_type_);
    size += sizeof(R_);
    size += sizeof(L_);
    size += sizeof(num_pq_chunks_);
    size += sizeof(num_parts_);
    return size;
}

void IndexDiskAnn::WriteAdv(char *&ptr) const {
    IndexBase::WriteAdv(ptr);
    WriteBufAdv(ptr, metric_type_);
    WriteBufAdv(ptr, encode_type_);
    WriteBufAdv(ptr, R_);
    WriteBufAdv(ptr, L_);
    WriteBufAdv(ptr, num_pq_chunks_);
    WriteBufAdv(ptr, num_parts_);
}

String IndexDiskAnn::ToString() const{
    std::stringstream ss;
    ss << IndexBase::ToString() << ", " << MetricTypeToString(metric_type_) << ", " << R_ << ", " << L_ << ", " << num_pq_chunks_ << ", " << num_parts_;
    return ss.str();
}

String IndexDiskAnn::BuildOtherParamsString() const {
    std::stringstream ss;
    ss << "metric_type=" << MetricTypeToString(metric_type_) << ", encode_type=" << DiskAnnEncodeTypeToString(encode_type_) << ", R=" << R_ << ", L=" << L_ << ", num_pq_chunks=" << num_pq_chunks_ << ", num_parts=" << num_parts_;
    return ss.str();
}

nlohmann::json IndexDiskAnn::Serialize() const {
    nlohmann::json res = IndexBase::Serialize();
    res["metric_type"] = MetricTypeToString(metric_type_);
    res["encode_type"] = DiskAnnEncodeTypeToString(encode_type_);
    res["R"] = R_;
    res["L"] = L_;
    res["num_pq_chunks"] = num_pq_chunks_;
    res["num_parts"] = num_parts_;
    return res;
}

void IndexDiskAnn::ValidateColumnDataType(const SharedPtr<BaseTableRef> &base_table_ref, const String &column_name) {
    auto &column_names_vector = *(base_table_ref->column_names_);
    auto &column_types_vector = *(base_table_ref->column_types_);
    SizeT column_id = std::find(column_names_vector.begin(), column_names_vector.end(), column_name) - column_names_vector.begin();
    if (column_id == column_names_vector.size()) {
        Status status = Status::ColumnNotExist(column_name);
        RecoverableError(status);
    } else if (auto &data_type = column_types_vector[column_id]; data_type->type() != LogicalType::kEmbedding) {
        Status status = Status::InvalidIndexDefinition(
            fmt::format("Attempt to create DsikAnn index on column: {}, data type: {}.", column_name, data_type->ToString()));
        RecoverableError(status);
    }

}

} // namespace infinity