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

module infinity_core:index_hnsw.impl;

import :index_hnsw;

import :stl;
import :status;
import :index_base;
import :third_party;
import :infinity_exception;
import serialize;
import :default_values;
import :index_base;
import logical_type;
import statement_common;
import :logger;
import data_type;
import embedding_info;
import internal_types;
import :hnsw_lsg_builder;

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

String HnswBuildTypeToString(HnswBuildType build_type) {
    switch (build_type) {
        case HnswBuildType::kPlain:
            return "plain";
        case HnswBuildType::kLSG:
            return "lsg";
        default:
            return "invalid";
    }
}

HnswBuildType StringToHnswBuildType(const String &str) {
    if (str == "plain") {
        return HnswBuildType::kPlain;
    } else if (str == "lsg") {
        return HnswBuildType::kLSG;
    } else {
        return HnswBuildType::kInvalid;
    }
}

void TrimSpace(std::string_view &str) {
    if (str.empty()) {
        return;
    }
    str.remove_prefix(str.find_first_not_of(' '));
    str.remove_suffix(str.size() - str.find_last_not_of(' ') - 1);
}

LSGConfig LSGConfig::FromString(const String &str) {
    // example: "sample_raito=0.01,ls_k=10,alpha=1.0"
    LSGConfig lsg_config;
    std::string_view sv(str);
    bool end = false;
    do {
        auto r = sv.find(',');
        if (r == std::string_view::npos) {
            r = sv.size();
            end = true;
        }
        auto kv = sv.substr(0, r);
        if (!end) {
            sv.remove_prefix(r + 1);
        }
        auto pos = kv.find('=');
        if (pos == std::string_view::npos) {
            Status status = Status::InvalidIndexParam(String(kv));
            RecoverableError(status);
        }
        auto key = kv.substr(0, pos);
        auto value = kv.substr(pos + 1);
        TrimSpace(key);
        TrimSpace(value);
        if (key == "sample_raito") {
            lsg_config.sample_ratio_ = std::stof(String(value));
        } else if (key == "ls_k") {
            lsg_config.ls_k_ = std::stoi(String(value));
        } else if (key == "alpha") {
            lsg_config.alpha_ = std::stof(String(value));
        } else {
            Status status = Status::InvalidIndexParam(String(key));
            RecoverableError(status);
        }
    } while (!end);
    return lsg_config;
}

SizeT LSGConfig::GetSizeInBytes() const {
    SizeT size = 0;
    size += sizeof(sample_ratio_);
    size += sizeof(ls_k_);
    size += sizeof(alpha_);
    return size;
}

void LSGConfig::WriteAdv(char *&ptr) const {
    WriteBufAdv<float>(ptr, sample_ratio_);
    WriteBufAdv<SizeT>(ptr, ls_k_);
    WriteBufAdv<float>(ptr, alpha_);
}

LSGConfig LSGConfig::ReadAdv(const char *&ptr) {
    LSGConfig lsg_config;
    lsg_config.sample_ratio_ = ReadBufAdv<float>(ptr);
    lsg_config.ls_k_ = ReadBufAdv<SizeT>(ptr);
    lsg_config.alpha_ = ReadBufAdv<float>(ptr);
    return lsg_config;
}

String LSGConfig::ToString() const {
    std::stringstream ss;
    ss << "sample_raito = " << sample_ratio_ << ", ls_k = " << ls_k_ << ", alpha = " << alpha_;
    return ss.str();
}

SharedPtr<IndexBase> IndexHnsw::Make(SharedPtr<String> index_name,
                                     SharedPtr<String> index_comment,
                                     const String &file_name,
                                     Vector<String> column_names,
                                     const Vector<InitParameter *> &index_param_list) {
    SizeT M = HNSW_M;
    SizeT ef_construction = HNSW_EF_CONSTRUCTION;
    SizeT block_size = HNSW_BLOCK_SIZE;
    MetricType metric_type = MetricType::kInvalid;
    HnswEncodeType encode_type = HnswEncodeType::kPlain;
    HnswBuildType build_type = HnswBuildType::kPlain;
    Optional<LSGConfig> lsg_config = None;
    for (const auto *param : index_param_list) {
        if (param->param_name_ == "m") {
            M = std::stoi(param->param_value_);
        } else if (param->param_name_ == "ef_construction") {
            ef_construction = std::stoi(param->param_value_);
        } else if (param->param_name_ == "metric") {
            metric_type = StringToMetricType(param->param_value_);
        } else if (param->param_name_ == "encode") {
            encode_type = StringToHnswEncodeType(param->param_value_);
        } else if (param->param_name_ == "build_type") {
            build_type = StringToHnswBuildType(param->param_value_);
        } else if (param->param_name_ == "block_size") {
            block_size = std::stoi(param->param_value_);
        } else if (param->param_name_ == "lsg_config") {
            lsg_config = LSGConfig::FromString(param->param_value_);
        } else {
            Status status = Status::InvalidIndexParam(param->param_name_);
            RecoverableError(status);
        }
    }
    if (build_type != HnswBuildType::kLSG && lsg_config) {
        lsg_config = None;
    }
    if (build_type == HnswBuildType::kLSG && !lsg_config) {
        lsg_config = LSGConfig();
    }

    if (metric_type == MetricType::kInvalid) {
        Status status = Status::InvalidIndexParam("Metric type");
        RecoverableError(status);
    }

    if (encode_type == HnswEncodeType::kInvalid) {
        Status status = Status::InvalidIndexParam("Encode type");
        RecoverableError(status);
    }

    return MakeShared<IndexHnsw>(index_name,
                                 index_comment,
                                 file_name,
                                 std::move(column_names),
                                 metric_type,
                                 encode_type,
                                 build_type,
                                 M,
                                 ef_construction,
                                 block_size,
                                 lsg_config);
}

bool IndexHnsw::operator==(const IndexHnsw &other) const {
    if (this->index_type_ != other.index_type_ || this->file_name_ != other.file_name_ || this->column_names_ != other.column_names_) {
        return false;
    }
    return metric_type_ == other.metric_type_ && encode_type_ == other.encode_type_ && build_type_ == other.build_type_ && M_ == other.M_ &&
           ef_construction_ == other.ef_construction_ && block_size_ == other.block_size_;
}

bool IndexHnsw::operator!=(const IndexHnsw &other) const { return !(*this == other); }

i32 IndexHnsw::GetSizeInBytes() const {
    SizeT size = IndexBase::GetSizeInBytes();
    size += sizeof(metric_type_);
    size += sizeof(encode_type_);
    size += sizeof(build_type_);
    size += sizeof(M_);
    size += sizeof(ef_construction_);
    size += sizeof(block_size_);
    size += sizeof(bool);
    if (lsg_config_) {
        size += lsg_config_->GetSizeInBytes();
    }
    return size;
}

void IndexHnsw::WriteAdv(char *&ptr) const {
    IndexBase::WriteAdv(ptr);
    WriteBufAdv(ptr, metric_type_);
    WriteBufAdv(ptr, encode_type_);
    WriteBufAdv(ptr, build_type_);
    WriteBufAdv(ptr, M_);
    WriteBufAdv(ptr, ef_construction_);
    WriteBufAdv(ptr, block_size_);
    WriteBufAdv(ptr, lsg_config_.has_value());
    if (lsg_config_) {
        lsg_config_->WriteAdv(ptr);
    }
}

String IndexHnsw::ToString() const {
    std::stringstream ss;
    ss << IndexBase::ToString() << ", " << MetricTypeToString(metric_type_) << HnswBuildTypeToString(build_type_) << ", " << M_ << ", "
       << ef_construction_ << ", " << block_size_;
    if (lsg_config_) {
        ss << ", " << lsg_config_->ToString();
    }
    return ss.str();
}

String IndexHnsw::BuildOtherParamsString() const {
    std::stringstream ss;
    ss << "metric = " << MetricTypeToString(metric_type_) << ", encode_type = " << HnswEncodeTypeToString(encode_type_) << ", M = " << M_
       << ", ef_construction = " << ef_construction_;
    if (lsg_config_) {
        ss << ", lsg_config = " << lsg_config_->ToString();
    }
    return ss.str();
}

nlohmann::json IndexHnsw::Serialize() const {
    nlohmann::json res = IndexBase::Serialize();
    res["metric_type"] = MetricTypeToString(metric_type_);
    res["encode_type"] = HnswEncodeTypeToString(encode_type_);
    res["build_type"] = HnswBuildTypeToString(build_type_);
    res["M"] = M_;
    res["ef_construction"] = ef_construction_;
    res["block_size"] = block_size_;
    if (lsg_config_) {
        res["lsg_config"] = lsg_config_->ToString();
    }
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
                RecoverableError(Status::InvalidIndexDefinition(
                    fmt::format("Attempt to create HNSW index with LVQ encoding on column: {}, data type: {}. now only support float element type.",
                                column_name,
                                data_type_ptr->ToString())));
            }
        }
    }
    // TODO: now only support float, int8, uint8?
    switch (embedding_data_type) {
        case EmbeddingDataType::kElemFloat:
        case EmbeddingDataType::kElemInt8:
        case EmbeddingDataType::kElemUInt8: {
            // supported
            break;
        }
        default: {
            RecoverableError(Status::InvalidIndexDefinition(
                fmt::format("Attempt to create HNSW index on column: {}, data type: {}. now only support float, int8, uint8 element type.",
                            column_name,
                            data_type_ptr->ToString())));
        }
    }
}

} // namespace infinity
