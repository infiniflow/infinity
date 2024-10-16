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

export module cached_node_base;

import stl;
import logical_node_type;

namespace infinity {

export class CachedNodeBase {
public:
    CachedNodeBase(LogicalNodeType type, SharedPtr<Vector<String>> output_names) : type_(type), output_names_(output_names) {}

    virtual ~CachedNodeBase() = default;

    virtual u64 Hash() const {
        u64 h = 0;
        h ^= std::hash<LogicalNodeType>{}(type_);
        return h;
    }

    virtual bool Eq(const CachedNodeBase &other) const = 0;

    LogicalNodeType type() const { return type_; }

    SharedPtr<Vector<String>> output_names() const { return output_names_; }

protected:
    LogicalNodeType type_;
    SharedPtr<Vector<String>> output_names_;
};

} // namespace infinity