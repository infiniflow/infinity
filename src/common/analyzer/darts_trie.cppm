// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

export module darts_trie;

import stl;
import darts;
import status;

namespace infinity {

export class POSTable {
public:
    POSTable(const String &path);

    ~POSTable() = default;

    Status Load();

    const char *GetPOS(i32 index) const;

    i32 GetPOSIndex(const String &tag) const;

private:
    String file_;
    i32 table_size_{0};
    Vector<String> pos_vec_;
    Map<String, i32> pos_map_;
};

export using DartsCore = DoubleArrayImpl<void, void, i64, void>;

export struct DartsTuple {
    DartsTuple(const String &k, const i64 &v) : key_(k), value_(v) {}
    String key_;
    i64 value_;
};

export class DartsTrie {
    UniquePtr<DartsCore> darts_;
    Vector<DartsTuple> buffer_;

public:
    DartsTrie();

    void Add(const String &key, const i64 &value);

    void Build();

    void Load(const String &file_name);

    void Save(const String &file_name);

    bool HasKeysWithPrefix(const String &key);

    i64 Get(const String &key);
};

} // namespace infinity