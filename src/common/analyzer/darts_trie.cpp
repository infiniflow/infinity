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

void DartsTrie::Add(const String &key, const i64 &value) { buffer_.push_back(DartsTuple(key, value)); }

void DartsTrie::Build() {
    std::sort(buffer_.begin(), buffer_.end(), [](const DartsTuple &l, const DartsTuple &r) {
        int cmp = std::strncmp(l.key_.c_str(), r.key_.c_str(), std::min(l.key_.size(), r.key_.size()));
        if (cmp == 0)
            return l.key_.size() < r.key_.size();
        return cmp < 0;
    });
    Vector<const char *> keys;
    Vector<std::size_t> lengths;
    Vector<i64> values;
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

bool DartsTrie::HasKeysWithPrefix(const String &key) {
    Vector<DartsCore::result_pair_type> result_pairs(256);
    std::size_t num_results = darts_->commonPrefixSearch(key.c_str(), &result_pairs[0], result_pairs.size());
    return num_results > 0;
}

i64 DartsTrie::Get(const String &key) {
    DartsCore::value_type value;
    darts_->exactMatchSearch(key.c_str(), value);
    return value;
}

} // namespace infinity