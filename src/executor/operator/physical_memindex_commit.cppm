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

export module physical_memindex_commit;

import stl;
import physical_operator;
import physical_operator_type;
import load_meta;
import data_type;
import operator_state;
import query_context;
import memory_indexer;

namespace infinity {

export class PhysicalMemIndexCommit : public PhysicalOperator {
public:
    PhysicalMemIndexCommit(u64 id,
                           MemoryIndexer *memory_indexer,
                           SharedPtr<Vector<LoadMeta>> load_metas,
                           SharedPtr<Vector<String>> output_names,
                           SharedPtr<Vector<SharedPtr<DataType>>> output_types)
        : PhysicalOperator(PhysicalOperatorType::kMemIndexCommit, nullptr, nullptr, id, load_metas), memory_indexer_(memory_indexer),
          output_names_(output_names), output_types_(output_types) {}

    ~PhysicalMemIndexCommit() override = default;

    void Init() override {}

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final {
        auto *memindex_commit_operator_state = static_cast<MemIndexCommitOperatorState *>(operator_state);

        memory_indexer_->Commit1(false /*offline*/);

        memindex_commit_operator_state->SetComplete();
        return true;
    }

    SizeT TaskletCount() override { return 1; }

    SharedPtr<Vector<String>> GetOutputNames() const override { return output_names_; }

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override { return output_types_; }

public:
    MemoryIndexer *memory_indexer_;

private:
    SharedPtr<Vector<String>> output_names_;
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_;
};

} // namespace infinity