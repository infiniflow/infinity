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

export module doc_iterator;

import stl;

import index_defines;
import internal_types;

namespace infinity {

export enum class DocIteratorType : u8 {
    kInvalidIterator,
    kTermIterator,
    kPhraseIterator,
    kMultiQueryIterator
};

export class DocIterator {
public:
    DocIterator() = default;

    virtual ~DocIterator() = default;

    RowID Doc() { return doc_id_; }

    // Check if the given doc id is a hit. If it is a hit, the
    // current doc id of this iterator is set to the given id,
    // If it is not a hit, find the first doc id which is not less
    // than the give doc id. If there is no such a doc id,
    // INVALID_DOCID will be returned
    bool Seek(RowID doc_id) {
        if (doc_id > doc_id_) {
            DoSeek(doc_id);
        }
        return doc_id == doc_id_;
    }

    // for term iter
    void PrepareFirstDoc();

    RowID Next() {
        DoSeek(doc_id_ + 1);
        return doc_id_;
    }

    virtual void DoSeek(RowID doc_id) = 0;

    virtual u32 GetDF() const = 0;

    // print the query tree, for debugging
    virtual void PrintTree(std::ostream &os, const String &prefix = "", bool is_final = true) const = 0;

    virtual DocIteratorType GetType() const { return DocIteratorType::kInvalidIterator; }
protected:
    RowID doc_id_{INVALID_ROWID};
};
} // namespace infinity
