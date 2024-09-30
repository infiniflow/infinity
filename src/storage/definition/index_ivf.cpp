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

#include "json.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

module index_ivf;

import infinity_exception;
import stl;
import index_base;
import status;
import serialize;
import logical_type;
import statement_common;
import logger;
import embedding_info;
import internal_types;
import data_type;
import create_index_info;

namespace infinity {

bool IndexIVF::operator==(const IndexIVF &other) const {
    return (*static_cast<const IndexBase *>(this) == static_cast<const IndexBase &>(other)) && (ivf_option_ == other.ivf_option_);
}

bool IndexIVF::operator!=(const IndexIVF &other) const { return !(*this == other); }

i32 IndexIVF::GetSizeInBytes() const {
    SizeT size = IndexBase::GetSizeInBytes();
    size += sizeof(ivf_option_);
    return size;
}

void IndexIVF::WriteAdv(char *&ptr) const {
    IndexBase::WriteAdv(ptr);
    WriteBufAdv(ptr, ivf_option_);
}

String IndexIVF::ToString() const {
    std::stringstream ss;
    ss << IndexBase::ToString() << ", " << BuildOtherParamsString();
    return std::move(ss).str();
}

IndexIVF::IndexIVF(SharedPtr<String> index_name, const String &file_name, Vector<String> column_names, const IndexIVFOption &ivf_option)
    : IndexBase(IndexType::kIVF, std::move(index_name), file_name, std::move(column_names)), ivf_option_(ivf_option) {}

template <std::integral T, T min_v = std::numeric_limits<T>::min(), T max_v = std::numeric_limits<T>::max()>
    requires(min_v <= max_v)
T GetIntegerFromNodeHandler(const auto &nh) {
    const auto val = DataType::StringToValue<BigIntT>(nh.mapped());
    if (val < min_v || val > max_v) {
        RecoverableError(Status::InvalidIndexDefinition(std::format("'{}' parameter value '{}' is out of range.", nh.key(), val)));
    }
    return val;
}

auto GetMandatoryParamNodeHandler(Map<String, String> &params_map, std::string_view param_name) {
    auto nh = params_map.extract(String{param_name});
    if (!nh) {
        RecoverableError(Status::InvalidIndexDefinition(std::format("Missing parameter '{}'.", param_name)));
    }
    return nh;
}

SharedPtr<IndexIVF>
IndexIVF::Make(SharedPtr<String> index_name, const String &file_name, Vector<String> column_names, const Vector<InitParameter *> &index_param_list) {
    Map<String, String> params_map;
    for (const auto *para : index_param_list) {
        String param_name = para->param_name_;
        const auto &param_v = para->param_value_;
        ToLower(param_name);
        if (const auto [_, success] = params_map.emplace(std::move(param_name), param_v); !success) {
            RecoverableError(Status::InvalidIndexDefinition("Duplicate parameter names"));
        }
    }
    IndexIVFOption ivf_option;
    {
        const auto nh = GetMandatoryParamNodeHandler(params_map, "metric");
        ToLower(nh.mapped());
        ivf_option.metric_ = StringToMetricType(nh.mapped());
    }
    {
        // IndexIVFCentroidOption
        if (const auto nh = params_map.extract("centroids_num_ratio"); nh) {
            ivf_option.centroid_option_.centroids_num_ratio_ = DataType::StringToValue<FloatT>(nh.mapped());
        }
        if (const auto nh = params_map.extract("min_points_per_centroid"); nh) {
            ivf_option.centroid_option_.min_points_per_centroid_ = GetIntegerFromNodeHandler<u32>(nh);
        }
        if (const auto nh = params_map.extract("max_points_per_centroid"); nh) {
            ivf_option.centroid_option_.max_points_per_centroid_ = GetIntegerFromNodeHandler<u32>(nh);
        }
    }
    {
        // IndexIVFStorageOption
        if (const auto nh = params_map.extract("storage_type"); nh) {
            ToLower(nh.mapped());
            if (nh.mapped() == "plain") {
                ivf_option.storage_option_.type_ = IndexIVFStorageOption::Type::kPlain;
            } else if (nh.mapped() == "scalar_quantization") {
                ivf_option.storage_option_.type_ = IndexIVFStorageOption::Type::kScalarQuantization;
            } else if (nh.mapped() == "product_quantization") {
                ivf_option.storage_option_.type_ = IndexIVFStorageOption::Type::kProductQuantization;
            } else {
                RecoverableError(Status::InvalidIndexDefinition(
                    std::format("Unrecognized storage_type '{}', valid choices: 'plain', 'scalar_quantization', 'product_quantization'",
                                nh.mapped())));
            }
        }
        switch (ivf_option.storage_option_.type_) {
            case IndexIVFStorageOption::Type::kPlain: {
                if (const auto nh = params_map.extract("plain_storage_data_type"); nh) {
                    ToUpper(nh.mapped());
                    ivf_option.storage_option_.plain_storage_data_type_ = EmbeddingT::String2EmbeddingDataType(nh.mapped());
                }
                break;
            }
            case IndexIVFStorageOption::Type::kScalarQuantization: {
                const auto nh = GetMandatoryParamNodeHandler(params_map, "scalar_quantization_bits");
                ivf_option.storage_option_.scalar_quantization_bits_ = GetIntegerFromNodeHandler<u32>(nh);
                break;
            }
            case IndexIVFStorageOption::Type::kProductQuantization: {
                const auto nh1 = GetMandatoryParamNodeHandler(params_map, "product_quantization_subspace_num");
                ivf_option.storage_option_.product_quantization_subspace_num_ = GetIntegerFromNodeHandler<u32, 1>(nh1);
                const auto nh2 = GetMandatoryParamNodeHandler(params_map, "product_quantization_subspace_bits");
                ivf_option.storage_option_.product_quantization_subspace_bits_ = GetIntegerFromNodeHandler<u32, 4, 16>(nh2);
                break;
            }
        }
    }
    if (!params_map.empty()) {
        std::ostringstream oss;
        oss << "Following parameters are left unused:";
        for (const auto &[k, v] : params_map) {
            oss << " (" << k << ", " << v << ')';
        }
        oss << '.';
        RecoverableError(Status::InvalidIndexDefinition(std::move(oss).str()));
    }
    return MakeShared<IndexIVF>(std::move(index_name), file_name, std::move(column_names), ivf_option);
}

void CheckIndexIVFStorageOption(IndexIVFStorageOption &storage_option, const DataType *column_data_type) {
    const auto *embedding_type_info = static_cast<const EmbeddingInfo *>(column_data_type->type_info().get());
    const auto embedding_data_type = embedding_type_info->Type();
    const auto embedding_dimension = embedding_type_info->Dimension();
    switch (storage_option.type_) {
        case IndexIVFStorageOption::Type::kPlain: {
            // check plain_storage_data_type_
            switch (storage_option.plain_storage_data_type_) {
                case EmbeddingDataType::kElemInt8:
                case EmbeddingDataType::kElemUInt8: {
                    if (storage_option.plain_storage_data_type_ != embedding_data_type) {
                        RecoverableError(
                            Status::InvalidIndexDefinition(std::format("Invalid plain storage type: {} on Embedding column: {}",
                                                                       EmbeddingT::EmbeddingDataType2String(storage_option.plain_storage_data_type_),
                                                                       column_data_type->ToString())));
                    }
                    break;
                }
                case EmbeddingDataType::kElemFloat:
                case EmbeddingDataType::kElemFloat16:
                case EmbeddingDataType::kElemBFloat16: {
                    switch (embedding_data_type) {
                        case EmbeddingDataType::kElemFloat:
                        case EmbeddingDataType::kElemDouble:
                        case EmbeddingDataType::kElemFloat16:
                        case EmbeddingDataType::kElemBFloat16: {
                            break;
                        }
                        case EmbeddingDataType::kElemBit:
                        case EmbeddingDataType::kElemInt8:
                        case EmbeddingDataType::kElemInt16:
                        case EmbeddingDataType::kElemInt32:
                        case EmbeddingDataType::kElemInt64:
                        case EmbeddingDataType::kElemUInt8:
                        case EmbeddingDataType::kElemInvalid: {
                            RecoverableError(Status::InvalidIndexDefinition(
                                std::format("Attempt to use floating point plain storage on embedding column with type: {}.",
                                            column_data_type->ToString())));
                            break;
                        }
                    }
                    break;
                }
                case EmbeddingDataType::kElemDouble:
                case EmbeddingDataType::kElemBit:
                case EmbeddingDataType::kElemInt16:
                case EmbeddingDataType::kElemInt32:
                case EmbeddingDataType::kElemInt64:{
                    RecoverableError(Status::InvalidIndexDefinition(
                        std::format("Invalid plain storage type: {}.", EmbeddingT::EmbeddingDataType2String(storage_option.plain_storage_data_type_)) +
                        " Can only choose: int8 for int8, uint8 for uint8, float, float16 or bfloat16 for floating type"));
                    break;
                }
                case EmbeddingDataType::kElemInvalid: {
                    // i8 -> i8, u8 -> u8, floating -> f32
                    switch (embedding_data_type) {
                        case EmbeddingDataType::kElemUInt8:
                        case EmbeddingDataType::kElemInt8: {
                            storage_option.plain_storage_data_type_ = embedding_data_type;
                            break;
                        }
                        case EmbeddingDataType::kElemFloat:
                        case EmbeddingDataType::kElemDouble:
                        case EmbeddingDataType::kElemFloat16:
                        case EmbeddingDataType::kElemBFloat16: {
                            storage_option.plain_storage_data_type_ = EmbeddingDataType::kElemFloat;
                            break;
                        }
                        case EmbeddingDataType::kElemBit:
                        case EmbeddingDataType::kElemInt16:
                        case EmbeddingDataType::kElemInt32:
                        case EmbeddingDataType::kElemInt64:
                        case EmbeddingDataType::kElemInvalid: {
                            RecoverableError(Status::InvalidIndexDefinition(
                                std::format("Attempt to use floating point plain storage on embedding column with type: {}.",
                                            column_data_type->ToString())));
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        }
        case IndexIVFStorageOption::Type::kScalarQuantization:
        case IndexIVFStorageOption::Type::kProductQuantization: {
            // only allowed on float32, float16, bfloat16, float64 embedding
            switch (embedding_data_type) {
                case EmbeddingDataType::kElemFloat:
                case EmbeddingDataType::kElemDouble:
                case EmbeddingDataType::kElemFloat16:
                case EmbeddingDataType::kElemBFloat16: {
                    break;
                }
                case EmbeddingDataType::kElemBit:
                case EmbeddingDataType::kElemInt8:
                case EmbeddingDataType::kElemInt16:
                case EmbeddingDataType::kElemInt32:
                case EmbeddingDataType::kElemInt64:
                case EmbeddingDataType::kElemUInt8:
                case EmbeddingDataType::kElemInvalid: {
                    RecoverableError(Status::InvalidIndexDefinition(
                        std::format("Attempt to use scalar quantization on embedding column with type: {}.", column_data_type->ToString()) +
                        " Can only use scalar quantization on float32, float16, bfloat16, float64 embedding."));
                }
            }
            if (storage_option.type_ == IndexIVFStorageOption::Type::kScalarQuantization) {
                // check scalar_quantization_bits_
                if (const auto scalar_quantization_bits = storage_option.scalar_quantization_bits_;
                    scalar_quantization_bits != 4 && scalar_quantization_bits != 8) {
                    RecoverableError(Status::InvalidIndexDefinition(
                        std::format("scalar_quantization_bits can only be 4 or 8, now it is {}.", scalar_quantization_bits)));
                }
            } else if (storage_option.type_ == IndexIVFStorageOption::Type::kProductQuantization) {
                // check product_quantization_subspace_num_, product_quantization_subspace_bits_
                if (const auto product_quantization_subspace_num = storage_option.product_quantization_subspace_num_;
                    product_quantization_subspace_num > embedding_dimension) {
                    RecoverableError(
                        Status::InvalidIndexDefinition(std::format("product_quantization_subspace_num ({}) is larger than embedding_dimension({}).",
                                                                   product_quantization_subspace_num,
                                                                   embedding_dimension)));
                }
                if (const auto product_quantization_subspace_bits = storage_option.product_quantization_subspace_bits_;
                    product_quantization_subspace_bits < 4 || product_quantization_subspace_bits > 16) {
                    RecoverableError(
                        Status::InvalidIndexDefinition(std::format("product_quantization_subspace_bits can only be in range [4, 16], now it is {}.",
                                                                   product_quantization_subspace_bits)));
                }
            } else {
                UnrecoverableError("Unexpected case");
            }
            break;
        }
    }
}

void CheckIndexIVFCentroidOption(const IndexIVFCentroidOption &centroid_option) {
    if (centroid_option.centroids_num_ratio_ < 0.0f) {
        RecoverableError(Status::InvalidIndexDefinition(
            std::format("centroids_num_ratio parameter value '{}' is not positive.", centroid_option.centroids_num_ratio_)));
    }
    if (centroid_option.min_points_per_centroid_ > centroid_option.max_points_per_centroid_) {
        RecoverableError(Status::InvalidIndexDefinition(std::format("min_points_per_centroid ({}) is greater than max_points_per_centroid ({}).",
                                                                    centroid_option.min_points_per_centroid_,
                                                                    centroid_option.max_points_per_centroid_)));
    }
}

void IndexIVF::ValidateColumnDataType(const SharedPtr<BaseTableRef> &base_table_ref, const String &column_name) {
    const auto &column_names_vector = *(base_table_ref->column_names_);
    const auto &column_types_vector = *(base_table_ref->column_types_);
    const auto name_it = std::find(column_names_vector.begin(), column_names_vector.end(), column_name);
    if (name_it == column_names_vector.end()) {
        RecoverableError(Status::ColumnNotExist(column_name));
    }
    const auto column_id = std::distance(column_names_vector.begin(), name_it);
    if (const auto &data_type = column_types_vector[column_id];
        data_type->type() != LogicalType::kEmbedding && data_type->type() != LogicalType::kMultiVector) {
        RecoverableError(Status::InvalidIndexDefinition(
            std::format("Attempt to create IVF index on column: {}, data type: {}.", column_name, data_type->ToString())));
    } else {
        if (ivf_option_.metric_ == MetricType::kInvalid) {
            RecoverableError(Status::InvalidIndexDefinition("Invalid metric type"));
        }
        CheckIndexIVFCentroidOption(ivf_option_.centroid_option_);
        CheckIndexIVFStorageOption(ivf_option_.storage_option_, data_type.get());
    }
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(IndexIVFCentroidOption, centroids_num_ratio_, min_points_per_centroid_, max_points_per_centroid_);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(IndexIVFStorageOption,
                                   type_,
                                   plain_storage_data_type_,
                                   scalar_quantization_bits_,
                                   product_quantization_subspace_num_,
                                   product_quantization_subspace_bits_);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(IndexIVFOption, metric_, centroid_option_, storage_option_);

nlohmann::json IndexIVF::Serialize() const {
    nlohmann::json res = IndexBase::Serialize();
    res["ivf_option"] = ivf_option_;
    return res;
}

IndexIVFOption IndexIVF::DeserializeIndexIVFOption(const nlohmann::json &ivf_option_json) { return ivf_option_json; }

String BuildIndexIVFStorageOptionStr();

String IndexIVFCentroidOption::ToString() const {
    return std::format("IndexIVFCentroidOption: [centroids_num_ratio: {}, min_points_per_centroid: {}, max_points_per_centroid: {}]",
                       centroids_num_ratio_,
                       min_points_per_centroid_,
                       max_points_per_centroid_);
}

String IndexIVFStorageOption::ToString() const {
    std::ostringstream oss;
    oss << "IndexIVFStorageOption: [";
    switch (type_) {
        case Type::kPlain: {
            oss << std::format("Type: Plain, plain_storage_data_type: {}", EmbeddingT::EmbeddingDataType2String(plain_storage_data_type_));
            break;
        }
        case Type::kProductQuantization: {
            oss << std::format("Type: ProductQuantization, scalar_quantization_bits: {}", scalar_quantization_bits_);
            break;
        }
        case Type::kScalarQuantization: {
            oss << std::format("Type: ScalarQuantization, product_quantization_subspace_num: {}, product_quantization_subspace_bits: {}",
                               product_quantization_subspace_num_,
                               product_quantization_subspace_bits_);
            break;
        }
    }
    oss << ']';
    return std::move(oss).str();
}

String IndexIVF::BuildOtherParamsString() const {
    return std::format("metric: {}, {}, {}",
                       MetricTypeToString(ivf_option_.metric_),
                       ivf_option_.centroid_option_.ToString(),
                       ivf_option_.storage_option_.ToString());
}

} // namespace infinity
