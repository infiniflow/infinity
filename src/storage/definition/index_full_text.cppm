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

export module index_full_text;

import stl;
import index_base;

import third_party;
import index_base;
import create_index_info;
import statement_common;
import base_table_ref;
import index_defines;

namespace infinity {

export class IndexFullText final : public IndexBase {
public:
    static SharedPtr<IndexBase>
    Make(SharedPtr<String> index_name, const String &file_name, Vector<String> column_names, const Vector<InitParameter *> &index_param_list);

    IndexFullText(SharedPtr<String> index_name,
                  const String &file_name,
                  Vector<String> column_names,
                  const String &analyzer,
                  optionflag_t flag = OPTION_FLAG_ALL)
        : IndexBase(IndexType::kFullText, index_name, file_name, std::move(column_names)), analyzer_(analyzer), flag_(flag){};

    ~IndexFullText() final = default;

    bool operator==(const IndexFullText &other) const;

    bool operator!=(const IndexFullText &other) const;

public:
    virtual i32 GetSizeInBytes() const override;

    virtual void WriteAdv(char *&ptr) const override;

    static SharedPtr<IndexBase> ReadAdv(char *&ptr, i32 maxbytes);

    virtual String ToString() const override;

    virtual String BuildOtherParamsString() const override;

    virtual nlohmann::json Serialize() const override;

    static SharedPtr<IndexFullText> Deserialize(const nlohmann::json &index_def_json);

public:
    static void ValidateColumnDataType(const SharedPtr<BaseTableRef> &base_table_ref, const String &column_name);

public:
    String analyzer_{};
    optionflag_t flag_{OPTION_FLAG_ALL};
};

} // namespace infinity
