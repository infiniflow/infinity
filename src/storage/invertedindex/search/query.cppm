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

export module term_queries;

import stl;

namespace infinity {

export struct ColumnInfo {
    String column_name_;
    u64 column_id_;
};

export class TermQuery {
public:
    TermQuery() = default;
    virtual ~TermQuery() = default;
    TermQuery(const TermQuery &) = delete;
    TermQuery &operator=(const TermQuery &) = delete;

    void SetParent(TermQuery *parent) { parent_ = parent; }
    TermQuery *GetParent() const { return parent_; }

    void SetColumn(const String &column) { column_.column_name_ = column; }

protected:
    virtual void NotifyChange() {
        if (parent_ != nullptr) {
            parent_->NotifyChange();
        }
    }
    TermQuery *parent_{nullptr};
    ColumnInfo column_;
};

export class MultiQuery : public TermQuery {
public:
    MultiQuery() = default;
    ~MultiQuery() = default;

    const Vector<UniquePtr<TermQuery>> &GetChildren() const { return children_; }

    const TermQuery &GetChild(u32 n) const { return *children_[n]; }

    TermQuery &GetChild(u32 n) { return *children_[n]; }

    MultiQuery &AddChild(UniquePtr<TermQuery> child);

    void Reserve(u32 n) { children_.reserve(n); }

protected:
    Vector<UniquePtr<TermQuery>> children_;
};

export class AndQuery : public MultiQuery {};
export class AndNotQuery : public MultiQuery {};
export class OrQuery : public MultiQuery {};
export class WandQuery : public MultiQuery {};
} // namespace infinity