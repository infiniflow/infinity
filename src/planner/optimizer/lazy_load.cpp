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

#include <vector>
module lazy_load;

import stl;
import logical_node;
import column_binding;
import base_expression;
import column_expression;
import logical_node_type;
import logical_table_scan;
import logical_index_scan;
import logical_knn_scan;
import logical_match;
import logical_match_tensor_scan;
import base_table_ref;
import load_meta;

namespace infinity {

Optional<BaseTableRef *> GetScanTableRef(LogicalNode &op) {
    switch (op.operator_type()) {
        case LogicalNodeType::kTableScan: {
            auto &table_scan = static_cast<LogicalTableScan &>(op);

            return table_scan.base_table_ref_.get();
        }
        case LogicalNodeType::kIndexScan: {
            auto &index_scan = static_cast<LogicalIndexScan &>(op);

            return index_scan.base_table_ref_.get();
        }
        case LogicalNodeType::kKnnScan: {
            auto &knn_scan = static_cast<LogicalKnnScan &>(op);

            return knn_scan.base_table_ref_.get();
        }
        case LogicalNodeType::kMatch: {
            auto &match = static_cast<LogicalMatch &>(op);

            return match.base_table_ref_.get();
        }
        case LogicalNodeType::kMatchTensorScan: {
            auto &match_tensor = static_cast<LogicalMatchTensorScan &>(op);

            return match_tensor.base_table_ref_.get();
        }
        default: {
            return None;
        }
    }
}

void RefencecColumnCollection::VisitNode(LogicalNode &op) {
    auto base_table_ref = GetScanTableRef(op);
    if (base_table_ref.has_value()) {
        auto table_idx = base_table_ref.value()->table_index_;
        auto &column_types = base_table_ref.value()->column_types_;
        auto &column_names = base_table_ref.value()->column_names_;
        auto scan_bindings = op.GetColumnBindings();

        column_types_.insert({table_idx, column_types});
        column_names_.insert({table_idx, column_names});
        scan_bindings_.insert({table_idx, scan_bindings});
        unloaded_bindings_.insert(scan_bindings.begin(), scan_bindings.end());
    }
    VisitNodeChildren(op);
    VisitNodeExpression(op);

    op.set_load_metas(MakeShared<Vector<LoadMeta>>(std::move(load_metas_)));
    load_metas_.clear();
}

SharedPtr<BaseExpression> RefencecColumnCollection::VisitReplace(const SharedPtr<ColumnExpression> &expression) {
    if (expression->special()) {
        return expression;
    }

    for (auto &[_, scan_bindings] : scan_bindings_) {
        for (SizeT idx = 0; idx < scan_bindings.size(); ++idx) {
            if (auto scan_binding = scan_bindings[idx]; expression->binding() == scan_binding) {
                if (unloaded_bindings_.contains(scan_binding)) {
                    auto types = column_types_[scan_binding.table_idx].get();
                    auto names = column_names_[scan_binding.table_idx].get();
                    load_metas_.emplace_back(scan_binding, idx, (*types)[idx], (*names)[idx]);
                    unloaded_bindings_.erase(scan_binding);
                }
                return expression;
            }
        }
    }
    return expression;
}

Vector<SizeT> LoadedColumn(const Vector<LoadMeta> *load_metas, BaseTableRef *table_ref) {
    Vector<SizeT> column_ids;

    for (SizeT i = 0; i < load_metas->size(); i++) {
        auto binding = (*load_metas)[i].binding_;

        if (binding.table_idx == table_ref->table_index_) {
            column_ids.push_back((*load_metas)[i].index_);
        }
    }
    return column_ids;
}

SharedPtr<BaseExpression> CleanScan::VisitReplace(const SharedPtr<ColumnExpression> &expression) { return expression; }

template <typename LogicalNodeSubType>
inline void CleanScanVisitBaseTableRefNode(LogicalNode &op, SharedPtr<Vector<LoadMeta>> &last_op_load_metas_, Vector<SizeT> &scan_table_indexes_) {
    auto &node = static_cast<LogicalNodeSubType &>(op);
    // node base table ref has two parts:
    // 1. the columns used by next operator
    // 2. the columns used by filter expression in node
    auto &node_load_metas = *node.load_metas();
    Vector<LoadMeta> node_columns = std::move(node_load_metas);
    node_load_metas.clear(); // need to set load_metas of node to empty vector
    auto &last_op_load_metas = *last_op_load_metas_;
    node_columns.insert(node_columns.end(), last_op_load_metas.begin(), last_op_load_metas.end());
    Vector<SizeT> project_idxs = LoadedColumn(&node_columns, node.base_table_ref_.get());
    scan_table_indexes_.push_back(node.base_table_ref_->table_index_);
    node.base_table_ref_->RetainColumnByIndices(project_idxs);
}

void CleanScan::VisitNode(LogicalNode &op) {
    switch (op.operator_type()) {
        case LogicalNodeType::kTableScan: {
            auto &table_scan = static_cast<LogicalTableScan &>(op);
            Vector<SizeT> project_idxs = LoadedColumn(last_op_load_metas_.get(), table_scan.base_table_ref_.get());

            scan_table_indexes_.push_back(table_scan.base_table_ref_->table_index_);
            table_scan.base_table_ref_->RetainColumnByIndices(project_idxs);
            table_scan.add_row_id_ = true;
            break;
        }
        case LogicalNodeType::kIndexScan: {
            auto &index_scan = static_cast<LogicalIndexScan &>(op);
            Vector<SizeT> project_idxs; // empty output
            index_scan.base_table_ref_->RetainColumnByIndices(project_idxs);
            break;
        }
        case LogicalNodeType::kKnnScan: {
            CleanScanVisitBaseTableRefNode<LogicalKnnScan>(op, last_op_load_metas_, scan_table_indexes_);
            break;
        }
        case LogicalNodeType::kMatch: {
            CleanScanVisitBaseTableRefNode<LogicalMatch>(op, last_op_load_metas_, scan_table_indexes_);
            break;
        }
        case LogicalNodeType::kMatchTensorScan: {
            CleanScanVisitBaseTableRefNode<LogicalMatchTensorScan>(op, last_op_load_metas_, scan_table_indexes_);
            break;
        }
        case LogicalNodeType::kLimit:
        case LogicalNodeType::kFusion: {
            // Skip
            VisitNodeChildren(op);
            VisitNodeExpression(op);
            break;
        }
        default: {
            last_op_load_metas_ = op.load_metas();
            VisitNodeChildren(op);
            VisitNodeExpression(op);

            auto load_metas = op.load_metas();
            if (!scan_table_indexes_.empty()) {
                Vector<LoadMeta> filtered_metas;

                for (SizeT i = 0; i < scan_table_indexes_.size(); i++) {
                    for (SizeT j = 0; j < load_metas->size(); j++) {
                        if ((*load_metas)[j].binding_.table_idx != scan_table_indexes_[i]) {
                            filtered_metas.push_back((*load_metas)[j]);
                        }
                    }
                }
                op.set_load_metas(MakeShared<Vector<LoadMeta>>(std::move(filtered_metas)));
                scan_table_indexes_.clear();
            }
            break;
        }
    }
}

} // namespace infinity