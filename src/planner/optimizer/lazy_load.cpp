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
import logical_match_scan_base;
import logical_fusion;
import base_table_ref;
import load_meta;
import special_function;
import infinity_exception;
import third_party;

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
        case LogicalNodeType::kMatchTensorScan:
        case LogicalNodeType::kMatchSparseScan:
        case LogicalNodeType::kKnnScan: {
            auto &match_base = static_cast<LogicalMatchScanBase &>(op);

            return match_base.base_table_ref_.get();
        }
        case LogicalNodeType::kMatch: {
            auto &match = static_cast<LogicalMatch &>(op);

            return match.base_table_ref_.get();
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
    auto special_type = expression->special();
    if (special_type.has_value()) {
        switch (*special_type) {
            case SpecialType::kRowID:
            case SpecialType::kDistance:
            case SpecialType::kSimilarity:
            case SpecialType::kScore: {
                return expression;
            }
            default: {
                break;
            }
        }
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
        case LogicalNodeType::kKnnScan:
        case LogicalNodeType::kMatchSparseScan:
        case LogicalNodeType::kMatchTensorScan: {
            CleanScanVisitBaseTableRefNode<LogicalMatchScanBase>(op, last_op_load_metas_, scan_table_indexes_);
            break;
        }
        case LogicalNodeType::kMatch: {
            CleanScanVisitBaseTableRefNode<LogicalMatch>(op, last_op_load_metas_, scan_table_indexes_);
            break;
        }
        case LogicalNodeType::kLimit: {
            // Skip
            VisitNodeChildren(op);
            VisitNodeExpression(op);
            break;
        }
        case LogicalNodeType::kFusion: {
            last_op_load_metas_ = op.load_metas();
            last_op_node_id_ = op.node_id();
            const auto &fusion = static_cast<const LogicalFusion &>(op);
            if (!op.left_node()) {
                UnrecoverableError("Internal error: Fusion has no left node.");
            }
            if (op.left_node()->operator_type() == LogicalNodeType::kFusion) {
                // check fusion child
                if (op.right_node() || !fusion.other_children_.empty()) {
                    UnrecoverableError("Internal error: Fusion with fusion child has right node or other children.");
                }
                // Skip
                VisitNodeChildren(op);
                VisitNodeExpression(op);
                break;
            }
            // now fusion has only search node children
            auto apply_to_fusion_children = [&fusion](auto &&apply_func) {
                if (fusion.left_node()) {
                    apply_func(*fusion.left_node());
                }
                if (fusion.right_node()) {
                    apply_func(*fusion.right_node());
                }
                for (auto &child : fusion.other_children_) {
                    apply_func(*child);
                }
            };
            // make sure that children can only be search nodes
            apply_to_fusion_children([](const LogicalNode &node) {
                switch (node.operator_type()) {
                    case LogicalNodeType::kMatch:
                    case LogicalNodeType::kKnnScan:
                    case LogicalNodeType::kMatchSparseScan:
                    case LogicalNodeType::kMatchTensorScan: {
                        break;
                    }
                    default: {
                        UnrecoverableError("Internal error: Fusion children are not search nodes.");
                    }
                }
            });
            // make sure that children share same base_table_ref_
            BaseTableRef *common_base_table_ref = nullptr;
            apply_to_fusion_children([&common_base_table_ref](LogicalNode &node) {
                if (const auto base_table_ref = GetScanTableRef(node); base_table_ref.has_value()) {
                    if (common_base_table_ref == nullptr) {
                        common_base_table_ref = base_table_ref.value();
                    } else if (common_base_table_ref != base_table_ref.value()) {
                        UnrecoverableError("Internal error: Fusion children have different base_table_ref_.");
                    }
                } else {
                    UnrecoverableError(fmt::format("Internal error: Fusion child {} has no base_table_ref_.", node.name()));
                }
            });
            // get all children's load_metas
            if (op.load_metas() && !op.load_metas()->empty()) {
                UnrecoverableError("Internal error: Fusion has load_metas");
            }
            Vector<LoadMeta> children_columns;
            apply_to_fusion_children([&children_columns](LogicalNode &node) {
                auto &node_load_metas = *node.load_metas();
                children_columns.insert(children_columns.end(),
                                        std::make_move_iterator(node_load_metas.begin()),
                                        std::make_move_iterator(node_load_metas.end()));
                node_load_metas.clear();
            });
            // sort and unique children_columns
            std::sort(children_columns.begin(), children_columns.end(), [](const LoadMeta &a, const LoadMeta &b) { return a.binding_ < b.binding_; });
            children_columns.erase(std::unique(children_columns.begin(),
                                               children_columns.end(),
                                               [](const LoadMeta &a, const LoadMeta &b) { return a.binding_ == b.binding_; }),
                                   children_columns.end());
            // edit children's base_table_ref_
            scan_table_indexes_.push_back(common_base_table_ref->table_index_);
            common_base_table_ref->RetainColumnByIndices(LoadedColumn(&children_columns, common_base_table_ref));
            break;
        }
        default: {
            last_op_load_metas_ = op.load_metas();
            last_op_node_id_ = op.node_id();
            VisitNodeChildren(op);
            VisitNodeExpression(op);
            if (last_op_node_id_ != op.node_id()) {
                // last_op_load_metas_ is not used
                break;
            }
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