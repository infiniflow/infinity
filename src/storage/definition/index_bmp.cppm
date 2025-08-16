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

export module infinity_core:index_bmp;

import :index_base;
import :base_table_ref;
import :bmp_util;

import third_party;

import create_index_info;
import statement_common;

namespace infinity {

export class IndexBMP final : public IndexBase {
public:
    static std::shared_ptr<IndexBMP> Make(std::shared_ptr<std::string> index_name,
                                    std::shared_ptr<std::string> index_comment,
                                    const std::string &file_name,
                                    std::vector<std::string> column_names,
                                    const std::vector<InitParameter *> &index_param_list);

    static void ValidateColumnDataType(const std::shared_ptr<BaseTableRef> &base_table_ref, const std::string &column_name);

    IndexBMP(std::shared_ptr<std::string> index_name,
             std::shared_ptr<std::string> index_comment,
             const std::string &file_name,
             std::vector<std::string> column_names,
             size_t block_size,
             BMPCompressType compress_type)
        : IndexBase(IndexType::kBMP, std::move(index_name), index_comment, file_name, column_names), block_size_(block_size),
          compress_type_(compress_type) {}

public:
    virtual i32 GetSizeInBytes() const override;

    virtual void WriteAdv(char *&ptr) const override;

    virtual std::string ToString() const override;

    virtual std::string BuildOtherParamsString() const override;

    virtual nlohmann::json Serialize() const override;

public:
    size_t block_size_{};
    BMPCompressType compress_type_{BMPCompressType::kCompressed};
};

} // namespace infinity
