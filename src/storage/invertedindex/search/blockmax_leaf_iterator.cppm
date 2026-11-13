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

export module infinity_core:blockmax_leaf_iterator;

import :doc_iterator;
import :column_length_io;

import internal_types;

namespace infinity {

export class BlockMaxLeafIterator : public DocIterator {
public:
    // ref: https://en.wikipedia.org/wiki/Okapi_BM25
    virtual void InitBM25Info(std::unique_ptr<FullTextColumnLengthReader> &&column_length_reader, float delta, float k1, float b) = 0;

    virtual RowID BlockMinPossibleDocID() const = 0;

    virtual RowID BlockLastDocID() const = 0;

    virtual float BlockMaxBM25Score() = 0;

    // Move block cursor to ensure its last_doc_id is no less than given doc_id.
    // Returns false and update doc_id_ to INVALID_ROWID if the iterator is exhausted.
    // Note that this routine decode skip_list only, and doesn't update doc_id_ when returns true.
    // Caller may invoke BlockMaxBM25Score() after this routine.
    virtual bool NextShallow(RowID doc_id) = 0;

    virtual float BM25Score() = 0;

    // True when the posting read position is decoded on DocID(), i.e. score() / BM25Score() may be
    // called. NextShallow() moves the block cursor without repositioning the posting reader, so after
    // it returns the iterator becomes "stale": doc_id_ still holds the old doc while the block cursor
    // (and its skip-list entry) already points at a later block. Scoring such an iterator decodes TF
    // from a mismatched block, which is the failure class fixed by 473f4fa0 ("number of bits is
    // unsupported" / "Decode posting FAILED"). Upper-bound-only calls (BlockMaxBM25Score) are still
    // fine in that state.
    virtual bool IsCurrentDocDecoded() const { return true; }

    // Positioning only (SeekDoc): skips the "walk docs by local threshold" loop. When the caller
    // (e.g. block-max WAND) re-checks competitiveness with its own block bounds afterwards,
    // decoding tf / doc length for every candidate is wasted work. The default implementation
    // forwards to Next(); non-term iterators can keep it.
    virtual bool NextNoSkip(RowID doc_id) { return Next(doc_id); }
};

} // namespace infinity
