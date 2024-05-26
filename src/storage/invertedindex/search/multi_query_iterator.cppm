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

export module multi_query_iterator;

import stl;

import index_defines;
import doc_iterator;
import term_doc_iterator;

namespace infinity {
export class MultiQueryDocIterator : public DocIterator {
public:
    explicit MultiQueryDocIterator(Vector<UniquePtr<DocIterator>> &&children) : children_(std::move(children)) {
        for (u32 i = 0; i < children_.size(); ++i) {
            if (auto term_doc_iter = dynamic_cast<TermDocIterator *>(children_[i].get()); term_doc_iter) {
                term_doc_iter->DoSeek(0);
            }
        }
    }

    virtual bool IsAnd() const { return false; }

    virtual bool IsAndNot() const { return false; }

    virtual bool IsOr() const { return false; }

    const Vector<UniquePtr<DocIterator>> &GetChildren() { return children_; }

    void PrintTree(std::ostream &os, const String &prefix, bool is_final) const override;

protected:
    Vector<UniquePtr<DocIterator>> children_;
};
} // namespace infinity