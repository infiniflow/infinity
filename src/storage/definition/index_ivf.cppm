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

export module index_ivf;

import stl;
import index_base;
import third_party;
import base_table_ref;
import statement_common;
import internal_types;

namespace infinity {

// always use float for centroids
export struct IndexIVFCentroidOption {
    float centroids_num_ratio_ = 1.0f;  // centroid_num = ratio * sqrt(embedding_num)
    u32 min_points_per_centroid_ = 32;  // for training centroids
    u32 max_points_per_centroid_ = 256; // for training centroids
    bool operator==(const IndexIVFCentroidOption &other) const = default;
    String ToString() const;
};

export struct IndexIVFStorageOption {
    enum class Type {
        kPlain,               // for floating-point, i8, u8
        kScalarQuantization,  // for floating-point, quantization for every dimension
        kProductQuantization, // for floating-point, centroid tag for several subspaces
    };
    Type type_ = Type::kPlain;
    // kPlain
    EmbeddingDataType plain_storage_data_type_ = EmbeddingDataType::kElemInvalid; // float32, float16, bfloat16, int8 or uint8
    // kScalarQuantization
    u32 scalar_quantization_bits_ = 0; // 4 or 8
    // kProductQuantization
    u32 product_quantization_subspace_num_ = 0;  // divisor of embedding dimension
    u32 product_quantization_subspace_bits_ = 0; // in range [4, 16]
    bool operator==(const IndexIVFStorageOption &other) const = default;
    String ToString() const;
};

export struct IndexIVFOption {
    MetricType metric_ = MetricType::kInvalid;
    IndexIVFCentroidOption centroid_option_;
    IndexIVFStorageOption storage_option_;
    bool operator==(const IndexIVFOption &) const = default;
};

export class IndexIVF final : public IndexBase {
public:
    static SharedPtr<IndexIVF>
    Make(SharedPtr<String> index_name, const String &file_name, Vector<String> column_names, const Vector<InitParameter *> &index_param_list);

    IndexIVF(SharedPtr<String> index_name, const String &file_name, Vector<String> column_names, const IndexIVFOption &ivf_option);

    ~IndexIVF() override = default;

    bool operator==(const IndexIVF &other) const;

    bool operator!=(const IndexIVF &other) const;

    i32 GetSizeInBytes() const override;

    void WriteAdv(char *&ptr) const override;

    String ToString() const override;

    String BuildOtherParamsString() const override;

    nlohmann::json Serialize() const override;

    static IndexIVFOption DeserializeIndexIVFOption(const nlohmann::json &ivf_option_json);

    void ValidateColumnDataType(const SharedPtr<BaseTableRef> &base_table_ref, const String &column_name);

    IndexIVFOption ivf_option_;
};

} // namespace infinity
