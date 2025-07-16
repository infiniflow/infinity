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

module infinity_core:index_base.impl;

import :index_base;
import :stl;
import serialize;
import :index_ivf;
import :index_hnsw;
import :index_diskann;
import :index_full_text;
import :index_secondary;
import :index_emvb;
import :index_bmp;
import :bmp_util;
// import :third_party;
// import :status;

import :infinity_exception;
import create_index_info;
import :index_defines;
import :logger;

namespace infinity {

String MetricTypeToString(MetricType metric_type) {
    switch (metric_type) {
        case MetricType::kMetricCosine: {
            return "cosine";
        }
        case MetricType::kMetricInnerProduct: {
            return "ip";
        }
        case MetricType::kMetricL2: {
            return "l2";
        }
        case MetricType::kInvalid: {
            return "Invalid";
        }
    }
}

MetricType StringToMetricType(const String &str) {
    if (str == "cos" or str == "cosine") {
        return MetricType::kMetricCosine;
    } else if (str == "ip") {
        return MetricType::kMetricInnerProduct;
    } else if (str == "l2") {
        return MetricType::kMetricL2;
    } else {
        return MetricType::kInvalid;
    }
}
} // namespace infinity

//--------------------------------------------------

namespace infinity {

bool IndexBase::operator==(const IndexBase &other) const { return index_type_ == other.index_type_ && column_names_ == other.column_names_; }

bool IndexBase::operator!=(const IndexBase &other) const { return !(*this == other); }

int32_t IndexBase::GetSizeInBytes() const {
    int32_t size = 0;
    size += sizeof(index_type_);
    size += sizeof(int32_t);
    size += index_name_->length();
    size += sizeof(int32_t);
    size += index_comment_->length();
    size += sizeof(int32_t);
    size += file_name_.length();
    size += sizeof(int32_t);
    for (const String &column_name : column_names_) {
        size += sizeof(int32_t) + column_name.length();
    }
    return size;
}

void IndexBase::WriteAdv(char *&ptr) const {
    WriteBufAdv(ptr, index_type_);
    WriteBufAdv(ptr, *index_name_);
    WriteBufAdv(ptr, *index_comment_);
    WriteBufAdv(ptr, file_name_);
    WriteBufAdv(ptr, static_cast<int32_t>(column_names_.size()));
    for (const String &column_name : column_names_) {
        WriteBufAdv(ptr, column_name);
    }
}

SharedPtr<IndexBase> IndexBase::ReadAdv(const char *&ptr, int32_t maxbytes) {
    const char *const ptr_end = ptr + maxbytes;
    if (maxbytes <= 0) {
        String error_message = "ptr goes out of range when reading IndexBase";
        UnrecoverableError(error_message);
    }
    IndexType index_type = ReadBufAdv<IndexType>(ptr);
    Vector<String> column_names;
    SharedPtr<String> index_name = MakeShared<String>(ReadBufAdv<String>(ptr));
    SharedPtr<String> index_comment = MakeShared<String>(ReadBufAdv<String>(ptr));
    String file_name = ReadBufAdv<String>(ptr);
    int32_t column_names_size = ReadBufAdv<int32_t>(ptr);
    for (int32_t i = 0; i < column_names_size; ++i) {
        column_names.emplace_back(ReadBufAdv<String>(ptr));
    }
    SharedPtr<IndexBase> res = nullptr;
    switch (index_type) {
        case IndexType::kIVF: {
            const auto ivf_option = ReadBufAdv<IndexIVFOption>(ptr);
            res = MakeShared<IndexIVF>(index_name, index_comment, file_name, column_names, ivf_option);
            break;
        }
        case IndexType::kHnsw: {
            MetricType metric_type = ReadBufAdv<MetricType>(ptr);
            HnswEncodeType encode_type = ReadBufAdv<HnswEncodeType>(ptr);
            HnswBuildType build_type = ReadBufAdv<HnswBuildType>(ptr);
            SizeT M = ReadBufAdv<SizeT>(ptr);
            SizeT ef_construction = ReadBufAdv<SizeT>(ptr);
            SizeT block_size = ReadBufAdv<SizeT>(ptr);
            Optional<LSGConfig> lsg_config = None;
            if (ReadBufAdv<bool>(ptr)) {
                lsg_config = LSGConfig::ReadAdv(ptr);
            }
            res = MakeShared<IndexHnsw>(index_name,
                                        index_comment,
                                        file_name,
                                        column_names,
                                        metric_type,
                                        encode_type,
                                        build_type,
                                        M,
                                        ef_construction,
                                        block_size,
                                        lsg_config);
            break;
        }
        case IndexType::kDiskAnn: {
            MetricType metric_type = ReadBufAdv<MetricType>(ptr);
            DiskAnnEncodeType encode_type = ReadBufAdv<DiskAnnEncodeType>(ptr);
            SizeT R = ReadBufAdv<SizeT>(ptr);
            SizeT L = ReadBufAdv<SizeT>(ptr);
            SizeT num_pq_chunks = ReadBufAdv<SizeT>(ptr);
            SizeT num_parts = ReadBufAdv<SizeT>(ptr);
            res = MakeShared<
                IndexDiskAnn>(index_name, index_comment, file_name, column_names, metric_type, encode_type, R, L, num_pq_chunks, num_parts);
            break;
        }
        case IndexType::kFullText: {
            String analyzer = ReadBufAdv<String>(ptr);
            u8 flag = ReadBufAdv<u8>(ptr);
            res = MakeShared<IndexFullText>(index_name, index_comment, file_name, column_names, analyzer, optionflag_t(flag));
            break;
        }
        case IndexType::kSecondary: {
            res = MakeShared<IndexSecondary>(index_name, index_comment, file_name, std::move(column_names));
            break;
        }
        case IndexType::kEMVB: {
            u32 residual_pq_subspace_num = ReadBufAdv<u32>(ptr);
            u32 residual_pq_subspace_bits = ReadBufAdv<u32>(ptr);
            res = MakeShared<IndexEMVB>(index_name,
                                        index_comment,
                                        file_name,
                                        std::move(column_names),
                                        residual_pq_subspace_num,
                                        residual_pq_subspace_bits);
            break;
        }
        case IndexType::kBMP: {
            SizeT block_size = ReadBufAdv<SizeT>(ptr);
            BMPCompressType compress_type = ReadBufAdv<BMPCompressType>(ptr);
            res = MakeShared<IndexBMP>(index_name, index_comment, file_name, std::move(column_names), block_size, compress_type);
            break;
        }
        case IndexType::kInvalid: {
            String error_message = "Error index method while reading";
            UnrecoverableError(error_message);
        }
        default: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
    }
    if (ptr_end < ptr) {
        String error_message = "ptr goes out of range when reading IndexBase";
        UnrecoverableError(error_message);
    }
    return res;
}

String IndexBase::ToString() const {
    std::stringstream ss;
    ss << "IndexBase: " << IndexInfo::IndexTypeToString(index_type_) << ", [";
    for (size_t i = 0; i < column_names_.size(); ++i) {
        ss << column_names_[i];
        if (i != column_names_.size() - 1) {
            ss << ", ";
        }
    }
    ss << "]";
    return ss.str();
}

nlohmann::json IndexBase::Serialize() const {
    nlohmann::json res;
    res["index_type"] = IndexInfo::IndexTypeToString(index_type_);
    res["index_name"] = *index_name_;
    res["index_comment"] = *index_comment_;
    res["file_name"] = file_name_;
    res["column_names"] = column_names_;
    return res;
}

SharedPtr<IndexBase> IndexBase::Deserialize(std::string_view index_def_str) {
    simdjson::padded_string index_def_json(index_def_str);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(index_def_json);

    SharedPtr<IndexBase> res = nullptr;
    String index_type_name = doc["index_type"].get<String>();
    IndexType index_type = IndexInfo::StringToIndexType(index_type_name);
    SharedPtr<String> index_name = MakeShared<String>(doc["index_name"].get<String>());

    SharedPtr<String> index_comment;
    if (String index_comment_json; doc["index_comment"].get<String>(index_comment_json) == simdjson::SUCCESS) {
        index_comment = MakeShared<String>(index_comment_json);
    } else {
        index_comment = MakeShared<String>();
    }

    String file_name = doc["file_name"].get<String>();
    Vector<String> column_names = doc["column_names"].get<Vector<String>>();
    switch (index_type) {
        case IndexType::kIVF: {
            const auto ivf_option = IndexIVF::DeserializeIndexIVFOption(doc["ivf_option"].raw_json());
            res = MakeShared<IndexIVF>(index_name, index_comment, file_name, std::move(column_names), ivf_option);
            break;
        }
        case IndexType::kHnsw: {
            SizeT M = doc["M"].get<SizeT>();
            SizeT ef_construction = doc["ef_construction"].get<SizeT>();
            SizeT block_size = doc["block_size"].get<SizeT>();
            MetricType metric_type = StringToMetricType(doc["metric_type"].get<String>());
            HnswEncodeType encode_type = StringToHnswEncodeType(doc["encode_type"].get<String>());
            HnswBuildType build_type = HnswBuildType::kPlain;
            if (String build_type_json; doc["build_type"].get<String>(build_type_json) == simdjson::SUCCESS) {
                build_type = StringToHnswBuildType(build_type_json);
            }
            Optional<LSGConfig> lsg_config = None;
            if (String lsg_config_json; doc["lsg_config"].get<String>(lsg_config_json) == simdjson::SUCCESS) {
                lsg_config = LSGConfig::FromString(lsg_config_json);
            }
            res = MakeShared<IndexHnsw>(index_name,
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
            break;
        }
        case IndexType::kDiskAnn: {
            SizeT R = doc["R"].get<SizeT>();
            SizeT L = doc["L"].get<SizeT>();
            SizeT num_pq_chunks = doc["num_pq_chunks"].get<SizeT>();
            SizeT num_parts = doc["num_parts"].get<SizeT>();
            MetricType metric_type = StringToMetricType(doc["metric_type"].get<String>());
            DiskAnnEncodeType encode_type = StringToDiskAnnEncodeType(doc["encode_type"].get<String>());
            res = MakeShared<IndexDiskAnn>(index_name,
                                           index_comment,
                                           file_name,
                                           std::move(column_names),
                                           metric_type,
                                           encode_type,
                                           R,
                                           L,
                                           num_pq_chunks,
                                           num_parts);
            break;
        }
        case IndexType::kFullText: {
            String analyzer = doc["analyzer"].get<String>();
            auto ft_res = MakeShared<IndexFullText>(index_name, index_comment, file_name, std::move(column_names), analyzer);
            u8 flag_json;
            simdjson::error_code error = doc["flag"].get<u8>(flag_json);
            if (error == simdjson::SUCCESS) {
                u8 flag = flag_json;
                ft_res->flag_ = flag;
            }
            res = ft_res;
            break;
        }
        case IndexType::kSecondary: {
            res = MakeShared<IndexSecondary>(index_name, index_comment, file_name, std::move(column_names));
            break;
        }
        case IndexType::kEMVB: {
            u32 residual_pq_subspace_num = doc["pq_subspace_num"].get<u32>();
            u32 residual_pq_subspace_bits = doc["pq_subspace_bits"].get<u32>();
            res = MakeShared<IndexEMVB>(index_name,
                                        index_comment,
                                        file_name,
                                        std::move(column_names),
                                        residual_pq_subspace_num,
                                        residual_pq_subspace_bits);
            break;
        }
        case IndexType::kBMP: {
            SizeT block_size = doc["block_size"].get<SizeT>();
            auto compress_type = (BMPCompressType)(i8)doc["compress_type"].get<i8>();
            res = MakeShared<IndexBMP>(index_name, index_comment, file_name, std::move(column_names), block_size, compress_type);
            break;
        }
        case IndexType::kInvalid: {
            String error_message = "Error index method while deserializing";
            UnrecoverableError(error_message);
        }
        default: {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
    }
    return res;
}

bool IndexBase::ContainsColumn(const String &column_name) const {
    for (SizeT i = 0; i < column_names_.size(); ++i) {
        if (column_names_[i] == column_name) {
            return true;
        }
    }
    return false;
}

} // namespace infinity
