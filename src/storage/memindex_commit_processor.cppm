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

export module memindex_commit_process;

import stl;

namespace infinity {

struct Catalog;
class MemoryIndexer;
class PhysicalMemIndexCommit;
class TxnManager;

struct MemIdxPtrHasher {
    using is_transparent = void;

    SizeT operator()(const SharedPtr<MemoryIndexer> &memory_indexer) const;

    SizeT operator()(MemoryIndexer *memory_indexer) const;
};

struct MemIdxPtrEqualer {
    using is_transparent = void;

    template <typename LHS, typename RHS>
    bool operator()(const LHS &lhs, const RHS &rhs) const {
        return ToPtr(lhs) == ToPtr(rhs);
    }

private:
    static MemoryIndexer *ToPtr(const SharedPtr<MemoryIndexer> &memory_indexer);
    static MemoryIndexer *ToPtr(MemoryIndexer *memory_indexer);
};

export class MemIndexCommitProcessor {
public:
    MemIndexCommitProcessor();

    void Start();

    void Stop();

    ~MemIndexCommitProcessor();

    void AddMemoryIndex(SharedPtr<MemoryIndexer> memory_indexer);

    void RemoveMemoryIndex(MemoryIndexer *memory_indexer);

private:
    void MemIndexCommitLoop();

    // User shall invoke this reguarly to populate recently inserted rows into the fulltext index. Noop for other types of index.
    void MemIndexCommit();

    UniquePtr<PhysicalMemIndexCommit> MakeCommitOperator(MemoryIndexer *memory_indexer);

private:
    Thread mem_index_commit_thread_{};

    Atomic<bool> running_{};

    std::mutex mtx_;
    HashSet<SharedPtr<MemoryIndexer>, MemIdxPtrHasher, MemIdxPtrEqualer> memory_indexers_;
};

} // namespace infinity