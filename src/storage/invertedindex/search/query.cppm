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
import doc_iterator;
import match_data;
import column_index_reader;

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

    static UniquePtr<TermQuery> Optimize(UniquePtr<TermQuery> query);

    virtual void Optimize(TermQuery *&self) {}

    virtual void OptimizeSelf() {}

    virtual bool IsAnd() const { return false; }

    virtual bool IsAndNot() const { return false; }

    virtual bool IsOr() const { return false; }

    virtual UniquePtr<DocIterator> CreateSearch(IndexReader &index_reader, Scorer *scorer);

protected:
    virtual void NotifyChange() {
        if (parent_ != nullptr) {
            parent_->NotifyChange();
        }
    }
    TermQuery *parent_{nullptr};
    String term_;
    ColumnInfo column_;
};

export class MultiQuery : public TermQuery {
public:
    MultiQuery() = default;
    virtual ~MultiQuery() = default;

    const Vector<UniquePtr<TermQuery>> &GetChildren() const { return children_; }

    u32 GetChildrenCount() const { return children_.size(); }

    const TermQuery *GetChild(u32 i) const { return children_[i].get(); }

    TermQuery *GetChild(u32 i) { return children_[i].get(); }

    MultiQuery &AddChild(UniquePtr<TermQuery> child);

    MultiQuery &InsertChild(u32 i, UniquePtr<TermQuery> child);

    UniquePtr<TermQuery> RemoveChild(u32 i);

    UniquePtr<TermQuery> RemoveLastChild() { return RemoveChild(children_.size() - 1); }

    void Reserve(u32 n) { children_.reserve(n); }

    void Optimize(TermQuery *&self) override;

    UniquePtr<DocIterator> CreateSearch(IndexReader &index_reader, Scorer *scorer) override;

    virtual UniquePtr<DocIterator> CreateMultiSearch(Vector<UniquePtr<DocIterator>> sub_doc_iters) = 0;

protected:
    Vector<UniquePtr<TermQuery>> children_;
};

export class AndQuery : public MultiQuery {
public:
    void OptimizeSelf() override;

    bool IsAnd() const override { return true; }

    UniquePtr<DocIterator> CreateMultiSearch(Vector<UniquePtr<DocIterator>> sub_doc_iters) override;
};
export class AndNotQuery : public MultiQuery {
public:
    void OptimizeSelf() override;

    bool IsAndNot() const override { return true; }

    UniquePtr<DocIterator> CreateMultiSearch(Vector<UniquePtr<DocIterator>> sub_doc_iters) override;
};
export class OrQuery : public MultiQuery {
public:
    void OptimizeSelf() override;

    bool IsOr() const override { return true; }

    UniquePtr<DocIterator> CreateMultiSearch(Vector<UniquePtr<DocIterator>> sub_doc_iters) override;
};
export class WandQuery : public MultiQuery {
public:
    UniquePtr<DocIterator> CreateMultiSearch(Vector<UniquePtr<DocIterator>> sub_doc_iters) override;
};

} // namespace infinity