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

export module index_secondary;

import stl;

import index_base;
import base_table_ref;
import create_index_info;

namespace infinity {

// Does not need any extra member.
export class IndexSecondary final : public IndexBase {
public:
    static SharedPtr<IndexBase> Make(SharedPtr<String> index_name, const String &file_name, Vector<String> column_names) {
        return MakeShared<IndexSecondary>(index_name, file_name, std::move(column_names));
    }

    IndexSecondary(SharedPtr<String> index_name, const String &file_name, Vector<String> column_names)
        : IndexBase(IndexType::kSecondary, index_name, file_name, std::move(column_names)) {}

    ~IndexSecondary() final = default;

    virtual String BuildOtherParamsString() const override;

    static void ValidateColumnDataType(const SharedPtr<BaseTableRef> &base_table_ref, const String &column_name);
};

} // namespace infinity