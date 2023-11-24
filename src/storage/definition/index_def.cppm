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
import third_party;
import base_index;

export module index_def;

namespace infinity {

export class IndexDef {
public:
    explicit IndexDef(SharedPtr<String> index_name): index_name_(Move(index_name)) {}

    virtual ~IndexDef() = default;

    virtual bool operator==(const IndexDef &other) const;

    virtual bool operator!=(const IndexDef &other) const;

public:
    // Estimated serialized size in bytes, ensured be no less than Write requires, allowed be larger.
    i32 GetSizeInBytes() const;

    // Write to a char buffer
    void WriteAdv(char *&ptr) const;

    // Read char from buffer
    static SharedPtr<IndexDef> ReadAdv(char *&ptr, i32 maxbytes);

    String ToString() const;

    Json Serialize() const;

    static SharedPtr<IndexDef> Deserialize(const Json &index_def_json);

public:
    SharedPtr<String> index_name_{};
    Vector<SharedPtr<BaseIndex>> index_array_{};
};
} // namespace infinity