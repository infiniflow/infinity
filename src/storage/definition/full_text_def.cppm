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

import stl;
import index_def;
import parser;
import third_party;
import base_index;

export module full_text_def;

namespace infinity {

export class FullTextDef : public BaseIndex {
public:
    static SharedPtr<BaseIndex> Make(String file_name, Vector<String> column_names, const Vector<InitParameter *> &index_param_list);

    FullTextDef(String file_name, Vector<String> column_names, String analyzer)
        : BaseIndex(file_name, IndexType::kIRSFullText, Move(column_names)), analyzer_(Move(analyzer)) {}

    ~FullTextDef() = default;

    virtual bool operator==(const BaseIndex &other) const override;

    virtual bool operator!=(const BaseIndex &other) const override;

public:
    virtual i32 GetSizeInBytes() const override;

    virtual void WriteAdv(char *&ptr) const override;

    virtual String ToString() const override;

    virtual Json Serialize() const override;

public:
    String analyzer_{};
};

} // namespace infinity
