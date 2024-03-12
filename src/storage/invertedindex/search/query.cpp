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

module term_queries;

import stl;
import doc_iterator;
import term_doc_iterator;
import multi_query_iterator;
import and_iterator;
import and_not_iterator;
import or_iterator;
import column_index_reader;
import posting_iterator;
import match_data;

namespace infinity {

UniquePtr<TermQuery> TermQuery::Optimize(UniquePtr<TermQuery> query) {
    TermQuery *root = query.release();
    root->Optimize(root);
    return UniquePtr<TermQuery>(root);
}

UniquePtr<DocIterator> TermQuery::CreateSearch(IndexReader &index_reader, Scorer *scorer) {
    ColumnIndexReader *column_index_reader = index_reader.GetColumnIndexReader(column_.column_id_);
    if (!column_index_reader)
        return nullptr;
    PostingIterator *posting_iterator = column_index_reader->Lookup(term_, index_reader.session_pool_.get());
    if (posting_iterator == nullptr)
        return nullptr;
    UniquePtr<TermDocIterator> search = MakeUnique<TermDocIterator>(posting_iterator, column_.column_id_);
    scorer->AddDocIterator(search.get(), column_.column_id_);
    return std::move(search);
}

MultiQuery &MultiQuery::AddChild(UniquePtr<TermQuery> child) {
    child->SetParent(this);
    children_.push_back(std::move(child));
    NotifyChange();
    return *this;
}

MultiQuery &MultiQuery::InsertChild(u32 i, UniquePtr<TermQuery> child) {
    child->SetParent(this);
    children_.insert(children_.begin() + i, std::move(child));
    NotifyChange();
    return *this;
}

UniquePtr<TermQuery> MultiQuery::RemoveChild(u32 i) {
    UniquePtr<TermQuery> ret = std::move(children_[i]);
    children_.erase(children_.begin() + i);
    ret->SetParent(nullptr);
    NotifyChange();
    return ret;
}

void MultiQuery::Optimize(TermQuery *&self) {
    for (u32 i = 0; i < children_.size(); ++i) {
        UniquePtr<TermQuery> &child = children_[i];
        auto *child_ptr = child.release();
        child_ptr->Optimize(child_ptr);
        child.reset(child_ptr);
    }
    OptimizeSelf();
}

UniquePtr<DocIterator> MultiQuery::CreateSearch(IndexReader &index_reader, Scorer *scorer) {
    Vector<UniquePtr<DocIterator>> sub_doc_iters;
    sub_doc_iters.reserve(children_.size());
    for (u32 i = 0; i < children_.size(); ++i) {
        auto iter = children_[i]->CreateSearch(index_reader, scorer);
        if (iter)
            sub_doc_iters.push_back(std::move(iter));
    }
    return CreateMultiSearch(std::move(sub_doc_iters));
}

void AndQuery::OptimizeSelf() {
    for (u32 i = 0; i < GetChildrenCount(); ++i) {
        TermQuery *child = GetChild(i);
        if (child->IsAnd()) {
            auto c = reinterpret_cast<MultiQuery *>(child);
            while (c->GetChildrenCount() > 0) {
                AddChild(c->RemoveLastChild());
            }
            RemoveChild(i--);
        }
    }
}

UniquePtr<DocIterator> AndQuery::CreateMultiSearch(Vector<UniquePtr<DocIterator>> sub_doc_iters) {
    UniquePtr<AndIterator> search = MakeUnique<AndIterator>(std::move(sub_doc_iters));

    return search;
}

void AndNotQuery::OptimizeSelf() {
    if (children_.empty())
        return;

    TermQuery *child = GetChild(0);
    if (child->IsAndNot()) {
        auto c = reinterpret_cast<MultiQuery *>(child);
        while (c->GetChildrenCount() > 1) {
            AddChild(c->RemoveLastChild());
        }
        InsertChild(1, c->RemoveChild(0));
        RemoveChild(0);
    }

    child = GetChild(0);
    if (child->IsAnd()) {
        auto c = reinterpret_cast<MultiQuery *>(child);
        for (u32 i = 0; i < c->GetChildrenCount(); ++i) {
            auto *grandchild = c->GetChild(i);
            if (grandchild->IsAndNot()) {
                auto gc = reinterpret_cast<MultiQuery *>(grandchild);
                while (gc->GetChildrenCount() > 1) {
                    AddChild(gc->RemoveLastChild());
                }
                UniquePtr<TermQuery> gc0 = gc->RemoveChild(0);
                c->RemoveChild(i--);
                if (gc0->IsAnd()) {
                    // c is "and", gc0 is also "and", flatten gc0 into c
                    auto gc0_ptr = reinterpret_cast<MultiQuery *>(gc0.get());
                    while (gc0_ptr->GetChildrenCount() > 0) {
                        c->AddChild(gc0_ptr->RemoveLastChild());
                    }
                } else {
                    c->AddChild(std::move(gc0));
                }
            }
        }
    }
}

UniquePtr<DocIterator> AndNotQuery::CreateMultiSearch(Vector<UniquePtr<DocIterator>> sub_doc_iters) {
    UniquePtr<AndNotIterator> search = MakeUnique<AndNotIterator>(std::move(sub_doc_iters));

    return search;
}

void OrQuery::OptimizeSelf() {
    for (u32 i = 0; (GetChildrenCount() > 1) && (i < GetChildrenCount()); ++i) {
        TermQuery *child = GetChild(i);
        if (child->IsOr()) {
            auto c = reinterpret_cast<MultiQuery *>(child);
            while (c->GetChildrenCount() > 0) {
                AddChild(c->RemoveLastChild());
            }
            RemoveChild(i--);
        }
    }
}

UniquePtr<DocIterator> OrQuery::CreateMultiSearch(Vector<UniquePtr<DocIterator>> sub_doc_iters) {
    UniquePtr<OrIterator> search = MakeUnique<OrIterator>(std::move(sub_doc_iters));

    return search;
}

UniquePtr<DocIterator> WandQuery::CreateMultiSearch(Vector<UniquePtr<DocIterator>> sub_doc_iters) { return nullptr; }
} // namespace infinity