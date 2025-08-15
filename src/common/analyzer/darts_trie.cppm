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

#include <cstdlib>

export module infinity_core:darts_trie;

import :stl;
import :darts;
import :status;

// import std;

namespace infinity {

export class POSTable {
public:
    POSTable(const std::string &path);

    ~POSTable() = default;

    Status Load();

    const char *GetPOS(i32 index) const;

    i32 GetPOSIndex(const std::string &tag) const;

private:
    std::string file_;
    i32 table_size_{0};
    std::vector<std::string> pos_vec_;
    std::map<std::string, i32> pos_map_;
};

export using DartsCore = DoubleArrayImpl<void, void, int, void>;

export struct DartsTuple {
    DartsTuple(const std::string &k, const int &v) : key_(k), value_(v) {}
    std::string key_;
    int value_;
};

export class DartsTrie {
    std::unique_ptr<DartsCore> darts_;
    std::vector<DartsTuple> buffer_;

public:
    DartsTrie();

    void Add(const std::string &key, const int &value);

    void Build();

    void Load(const std::string &file_name);

    void Save(const std::string &file_name);

    bool HasKeysWithPrefix(std::string_view key) const;

    int Traverse(const char *key, std::size_t &node_pos, std::size_t &key_pos, std::size_t length) const;

    int Get(std::string_view key) const;
};

} // namespace infinity