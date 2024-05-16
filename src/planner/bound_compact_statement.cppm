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

export module bound_compact_statement;

import stl;
import logical_compact;
import logical_compact_finish;
import logical_compact_index;
import base_table_ref;
import block_index;
import logical_node;
import query_context;
import bind_context;
import logger;
import third_party;
import table_entry;
import compact_statement;

namespace infinity {

export struct BoundCompactStatement final {
public:
    BoundCompactStatement(SharedPtr<BindContext> bind_context, SharedPtr<BaseTableRef> base_table_ref, CompactStatementType compact_type)
        : bind_context_(std::move(bind_context)), base_table_ref_(base_table_ref), compact_type_(compact_type) {}

    Vector<SharedPtr<LogicalNode>> BuildPlans(QueryContext *query_context) {
        Vector<SharedPtr<LogicalNode>> res;
        const SharedPtr<BindContext> &bind_context = this->bind_context_;

        auto compact_node = MakeShared<LogicalCompact>(bind_context->GetNewLogicalNodeId(), base_table_ref_, compact_type_);
        auto &index_index = base_table_ref_->index_index_;
        if (!index_index->IsEmpty()) {
            if (index_index->index_snapshots_.size() == 1) {
                auto compact_index_node = MakeShared<LogicalCompactIndex>(bind_context->GetNewLogicalNodeId(), base_table_ref_);
                compact_index_node->set_left_node(compact_node);
                auto compact_finish_node = MakeShared<LogicalCompactFinish>(bind_context->GetNewLogicalNodeId(), base_table_ref_, compact_type_);
                compact_finish_node->set_left_node(compact_index_node);
                res.emplace_back(compact_finish_node);
            } else {
                res.emplace_back(compact_node);
                if (index_index->index_snapshots_.size() > 2) {
                    LOG_WARN(fmt::format("Table {} has more than 2 index, but logical plan is binary tree now", *base_table_ref_->table_name()));
                }
                auto index_index1 = MakeShared<IndexIndex>();
                auto index_index2 = MakeShared<IndexIndex>();
                SizeT idx = 0;
                for (const auto &[index_name, index_snapshot] : index_index->index_snapshots_) {
                    if (idx == 0) {
                        index_index1->Insert(index_name, index_snapshot);
                    } else {
                        index_index2->Insert(index_name, index_snapshot);
                    }
                    ++idx;
                }
                auto base_table_ref1 = MakeShared<BaseTableRef>(base_table_ref_->table_entry_ptr_, base_table_ref_->block_index_, index_index1);
                auto base_table_ref2 = MakeShared<BaseTableRef>(base_table_ref_->table_entry_ptr_, base_table_ref_->block_index_, index_index2);
                auto compact_index_node1 = MakeShared<LogicalCompactIndex>(bind_context->GetNewLogicalNodeId(), base_table_ref1);
                auto compact_index_node2 = MakeShared<LogicalCompactIndex>(bind_context->GetNewLogicalNodeId(), base_table_ref2);
                auto compact_finish = MakeShared<LogicalCompactFinish>(bind_context->GetNewLogicalNodeId(), base_table_ref_, compact_type_);
                compact_finish->set_left_node(compact_index_node1);
                compact_finish->set_right_node(compact_index_node2);
                res.emplace_back(compact_finish);
            }
        } else {
            auto compact_finish = MakeShared<LogicalCompactFinish>(bind_context->GetNewLogicalNodeId(), base_table_ref_, compact_type_);
            compact_finish->set_left_node(compact_node);
            res.emplace_back(compact_finish);
        }
        return res;
    }

private:
    SharedPtr<BindContext> bind_context_{};

    SharedPtr<BaseTableRef> base_table_ref_{};
    CompactStatementType compact_type_;
};

} // namespace infinity