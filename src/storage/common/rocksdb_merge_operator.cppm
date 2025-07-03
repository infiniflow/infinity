// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

export module rocksdb_merge_operator;

import stl;
import third_party;

namespace infinity {

export class String2UInt64AddOperator : public rocksdb::AssociativeMergeOperator {
public:
    static SharedPtr<String2UInt64AddOperator> Create() { return MakeShared<String2UInt64AddOperator>(); }

    static const char *kClassName() { return "String2UInt64AddOperator"; }
    static const char *kNickName() { return "string2uint64add"; }
    const char *Name() const override { return kClassName(); }
    const char *NickName() const override { return kNickName(); }

    bool Merge(const rocksdb::Slice & /*key*/,
               const rocksdb::Slice *existing_value,
               const rocksdb::Slice &value,
               std::string *new_value,
               rocksdb::Logger *logger) const override;

private:
    u64 DecodeInteger(const rocksdb::Slice &value, rocksdb::Logger *logger) const;
};

} // namespace infinity
