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

module index_hnsw;

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
import data_type;
import embedding_info;
import internal_types;

namespace infinity {

HnswEncodeType StringToHnswEncodeType(const String &str) {
    if (str == "plain") {
        return HnswEncodeType::kPlain;
    } else if (str == "lvq") {
        return HnswEncodeType::kLVQ;
    } else {
        return HnswEncodeType::kInvalid;
    }
}

String HnswEncodeTypeToString(HnswEncodeType encode_type) {
    switch (encode_type) {
        case HnswEncodeType::kPlain:
            return "plain";
        case HnswEncodeType::kLVQ:
            return "lvq";
        default:
            return "invalid";
    }
}

SharedPtr<IndexBase>
IndexHnsw::Make(SharedPtr<String> index_name, const String &file_name, Vector<String> column_names, const Vector<InitParameter *> &index_param_list) {
    SizeT M = HNSW_M;
    SizeT ef_construction = HNSW_EF_CONSTRUCTION;
    SizeT block_size = HNSW_BLOCK_SIZE;
    MetricType metric_type = MetricType::kInvalid;
    HnswEncodeType encode_type = HnswEncodeType::kPlain;
    for (const auto *param : index_param_list) {
        if (param->param_name_ == "m") {
            M = std::stoi(param->param_value_);
        } else if (param->param_name_ == "ef_construction") {
            ef_construction = std::stoi(param->param_value_);
        } else if (param->param_name_ == "metric") {
            metric_type = StringToMetricType(param->param_value_);
        } else if (param->param_name_ == "encode") {
            encode_type = StringToHnswEncodeType(param->param_value_);
        } else if (param->param_name_ == "block_size") {
            block_size = std::stoi(param->param_value_);
        } else {
            Status status = Status::InvalidIndexParam(param->param_name_);
            RecoverableError(status);
        }
    }

    if (metric_type == MetricType::kInvalid) {
        Status status = Status::InvalidIndexParam("Metric type");
        RecoverableError(status);
    }

    if (encode_type == HnswEncodeType::kInvalid) {
        Status status = Status::InvalidIndexParam("Encode type");
        RecoverableError(status);
    }

    return MakeShared<IndexHnsw>(index_name, file_name, std::move(column_names), metric_type, encode_type, M, ef_construction, block_size);
}

bool IndexHnsw::operator==(const IndexHnsw &other) const {
    if (this->index_type_ != other.index_type_ || this->file_name_ != other.file_name_ || this->column_names_ != other.column_names_) {
        return false;
    }
    return metric_type_ == other.metric_type_ && encode_type_ == other.encode_type_ && M_ == other.M_ && ef_construction_ == other.ef_construction_ &&
           block_size_ == other.block_size_;
}

bool IndexHnsw::operator!=(const IndexHnsw &other) const { return !(*this == other); }

i32 IndexHnsw::GetSizeInBytes() const {
    SizeT size = IndexBase::GetSizeInBytes();
    size += sizeof(metric_type_);
    size += sizeof(encode_type_);
    size += sizeof(M_);
    size += sizeof(ef_construction_);
    size += sizeof(block_size_);
    return size;
}

void IndexHnsw::WriteAdv(char *&ptr) const {
    IndexBase::WriteAdv(ptr);
    WriteBufAdv(ptr, metric_type_);
    WriteBufAdv(ptr, encode_type_);
    WriteBufAdv(ptr, M_);
    WriteBufAdv(ptr, ef_construction_);
    WriteBufAdv(ptr, block_size_);
}

String IndexHnsw::ToString() const {
    std::stringstream ss;
    ss << IndexBase::ToString() << ", " << MetricTypeToString(metric_type_) << ", " << M_ << ", " << ef_construction_ << ", " << block_size_;
    return ss.str();
}

String IndexHnsw::BuildOtherParamsString() const {
    std::stringstream ss;
    ss << "metric = " << MetricTypeToString(metric_type_) << ", encode_type = " << HnswEncodeTypeToString(encode_type_) << ", M = " << M_
       << ", ef_construction = " << ef_construction_ << ", block_size = " << block_size_;
    return ss.str();
}

nlohmann::json IndexHnsw::Serialize() const {
    nlohmann::json res = IndexBase::Serialize();
    res["metric_type"] = MetricTypeToString(metric_type_);
    res["encode_type"] = HnswEncodeTypeToString(encode_type_);
    res["M"] = M_;
    res["ef_construction"] = ef_construction_;
    res["block_size"] = block_size_;
    return res;
}

void IndexHnsw::ValidateColumnDataType(const SharedPtr<BaseTableRef> &base_table_ref,
                                       const String &column_name,
                                       const Vector<InitParameter *> &index_param_list) {
    const auto &column_names_vector = *(base_table_ref->column_names_);
    const auto &column_types_vector = *(base_table_ref->column_types_);
    const SizeT column_id = std::find(column_names_vector.begin(), column_names_vector.end(), column_name) - column_names_vector.begin();
    if (column_id == column_names_vector.size()) {
        RecoverableError(Status::ColumnNotExist(column_name));
    }
    const DataType *data_type_ptr = column_types_vector[column_id].get();
    switch (data_type_ptr->type()) {
        case LogicalType::kEmbedding:
        case LogicalType::kMultiVector: {
            break;
        }
        default: {
            RecoverableError(Status::InvalidIndexDefinition(
                fmt::format("Attempt to create HNSW index on column: {}, data type: {}.", column_name, data_type_ptr->ToString())));
        }
    }
    const auto embedding_info = dynamic_cast<const EmbeddingInfo *>(data_type_ptr->type_info().get());
    const EmbeddingDataType embedding_data_type = embedding_info->Type();
    for (const auto *param : index_param_list) {
        if (param->param_name_ == "encode" && StringToHnswEncodeType(param->param_value_) == HnswEncodeType::kLVQ) {
            // TODO: now only support float?
            if (embedding_data_type != EmbeddingDataType::kElemFloat) {
                RecoverableError(
                    Status::InvalidIndexDefinition(fmt::format("Attempt to create HNSW index with LVQ encoding on column: {}, data type: {}.",
                                                               column_name,
                                                               data_type_ptr->ToString())));
            }
        }
    }
}

} // namespace infinity