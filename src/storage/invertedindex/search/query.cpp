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

namespace infinity {

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
                c->AddChild(gc->RemoveChild(0));
                c->RemoveChild(i--);
            }
        }
    }
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

} // namespace infinity