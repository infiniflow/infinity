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

#include <cstddef>
#include <fstream>
#include <map>
#include <vector>

#include "string_utils.h"
module darts_trie;

import stl;
import darts;
import status;

namespace infinity {

POSTable::POSTable(const String &file_name) : file_(file_name) {}

Status POSTable::Load() {
    std::ifstream from(file_);
    if (!from.good()) {
        return Status::InvalidAnalyzerFile(file_);
    }

    String line;
    i32 index = 0;

    while (getline(from, line)) {
        line = line.substr(0, line.find('\r'));
        if (line.empty())
            continue;
        pos_map_[line] = index;
    }

    for (auto &x : pos_map_) {
        x.second = index++;
        pos_vec_.push_back(x.first);
    }

    return Status::OK();
}

const char *POSTable::GetPOS(i32 index) const {
    if (index < 0 || index >= table_size_)
        return "";

    return pos_vec_[index].c_str();
}

i32 POSTable::GetPOSIndex(const String &tag) const {
    Map<String, i32>::const_iterator it = pos_map_.find(tag);
    if (it != pos_map_.end())
        return it->second;
    return -1;
}

DartsTrie::DartsTrie() : darts_{MakeUnique<DartsCore>()} {}

void DartsTrie::Add(const String &key, const int &value) { buffer_.push_back(DartsTuple(key, value)); }

void DartsTrie::Build() {
    std::sort(buffer_.begin(), buffer_.end(), [](const DartsTuple &l, const DartsTuple &r) { return l.key_ < r.key_; });
    Vector<const char *> keys;
    Vector<std::size_t> lengths;
    Vector<int> values;
    for (auto &o : buffer_) {
        keys.push_back(o.key_.c_str());
        lengths.push_back(o.key_.size());
        values.push_back(o.value_);
    }
    darts_->build(keys.size(), keys.data(), lengths.data(), values.data(), nullptr);
    buffer_.clear();
}

void DartsTrie::Load(const String &file_name) { darts_->open(file_name.c_str()); }

void DartsTrie::Save(const String &file_name) { darts_->save(file_name.c_str()); }

// string literal "" is null-terminated
constexpr std::string_view empty_null_terminated_sv = "";

bool DartsTrie::HasKeysWithPrefix(std::string_view key) const {
    if (key.empty()) [[unlikely]] {
        key = empty_null_terminated_sv;
    }
    std::size_t id = 0;
    std::size_t key_pos = 0;
    const auto result = darts_->traverse(key.data(), id, key_pos, key.size());
    return result != -2;
}

int DartsTrie::Traverse(const char *key, std::size_t &node_pos, std::size_t &key_pos, const std::size_t length) const {
    return darts_->traverse(key, node_pos, key_pos, length);
}

int DartsTrie::Get(std::string_view key) const {
    if (key.empty()) [[unlikely]] {
        key = empty_null_terminated_sv;
    }
    return darts_->exactMatchSearch<DartsCore::value_type>(key.data(), key.size());
}

} // namespace infinity