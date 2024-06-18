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

export module index_bmp;

import stl;
import index_base;
import statement_common;
import third_party;
import base_table_ref;
import create_index_info;

namespace infinity {

export class IndexBMP final : public IndexBase {
public:
    static SharedPtr<IndexBMP>
    Make(SharedPtr<String> index_name, const String &file_name, Vector<String> column_names, const Vector<InitParameter *> &index_param_list);

    static void ValidateColumnDataType(const SharedPtr<BaseTableRef> &base_table_ref, const String &column_name);

    IndexBMP(SharedPtr<String> index_name, const String &file_name, Vector<String> column_names, SizeT block_size)
        : IndexBase(IndexType::kBMP, std::move(index_name), file_name, column_names), block_size_(block_size) {}

public:
    virtual i32 GetSizeInBytes() const override;

    virtual void WriteAdv(char *&ptr) const override;

    virtual String ToString() const override;

    virtual String BuildOtherParamsString() const override;

    virtual nlohmann::json Serialize() const override;

public:
    SizeT block_size_{};
};

} // namespace infinity
