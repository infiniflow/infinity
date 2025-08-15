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

export module infinity_core:index_full_text;

import :stl;
import :index_base;
import :index_base;
import :base_table_ref;
import :index_defines;

import third_party;

import create_index_info;
import statement_common;

namespace infinity {

export class IndexFullText final : public IndexBase {
public:
    static std::shared_ptr<IndexBase> Make(std::shared_ptr<std::string> index_name,
                                     std::shared_ptr<std::string> index_comment,
                                     const std::string &file_name,
                                     std::vector<std::string> column_names,
                                     const std::vector<InitParameter *> &index_param_list);

    IndexFullText(std::shared_ptr<std::string> index_name,
                  std::shared_ptr<std::string> index_comment,
                  const std::string &file_name,
                  std::vector<std::string> column_names,
                  const std::string &analyzer,
                  optionflag_t flag = OPTION_FLAG_ALL)
        : IndexBase(IndexType::kFullText, index_name, index_comment, file_name, std::move(column_names)), analyzer_(analyzer), flag_(flag) {};

    ~IndexFullText() final = default;

    bool operator==(const IndexFullText &other) const;

    bool operator!=(const IndexFullText &other) const;

public:
    virtual i32 GetSizeInBytes() const override;

    virtual void WriteAdv(char *&ptr) const override;

    static std::shared_ptr<IndexBase> ReadAdv(char *&ptr, i32 maxbytes);

    virtual std::string ToString() const override;

    virtual std::string BuildOtherParamsString() const override;

    virtual nlohmann::json Serialize() const override;

    static std::shared_ptr<IndexFullText> Deserialize(std::string_view index_def_str);

    bool IsRealtime() const { return FlagIsRealtime(flag_); }

public:
    static void ValidateColumnDataType(const std::shared_ptr<BaseTableRef> &base_table_ref, const std::string &column_name);

public:
    std::string analyzer_{};
    optionflag_t flag_{OPTION_FLAG_ALL};
};

} // namespace infinity
