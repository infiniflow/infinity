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

import stl;
import logical_node;
import column_binding;
import base_expression;
import column_expression;
import logical_node_type;
import logical_table_scan;
import logical_knn_scan;
import logical_match;
import base_table_ref;
import load_meta;

module lazy_load;

namespace infinity {

Optional<BaseTableRef *> GetScanTableRef(LogicalNode &op) {
    switch (op.operator_type()) {
        case LogicalNodeType::kTableScan: {
            auto table_scan = dynamic_cast<LogicalTableScan &>(op);

            return table_scan.base_table_ref_.get();
        }
        case LogicalNodeType::kKnnScan: {
            auto knn_scan = dynamic_cast<LogicalKnnScan &>(op);

            return knn_scan.base_table_ref_.get();
        }
        case LogicalNodeType::kMatch: {
            auto match = dynamic_cast<LogicalMatch &>(op);

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
        auto column_types = base_table_ref.value()->column_types_;
        auto scan_bindings = op.GetColumnBindings();

        column_types_.insert({table_idx, column_types});
        scan_bindings_.insert(scan_bindings_.begin(), scan_bindings.begin(), scan_bindings.end());
        unloaded_bindings_.insert(scan_bindings.begin(), scan_bindings.end());
        return;
    }
    VisitNodeChildren(op);
    VisitNodeExpression(op);

    op.set_load_metas(MakeShared<Vector<LoadMeta>>(load_metas_));
    load_metas_.clear();
}

SharedPtr<BaseExpression> RefencecColumnCollection::VisitReplace(const SharedPtr<ColumnExpression> &expression) {
    if(expression->special()) {
        return expression;
    }

    for (SizeT idx = 0; idx < scan_bindings_.size(); ++idx) {
        auto scan_binding = scan_bindings_[idx];

        if (expression->binding() == scan_binding && unloaded_bindings_.contains(scan_binding)) {
            auto types = column_types_[scan_binding.table_idx].get();
            auto load_meta = LoadMeta(scan_binding, idx, (*types)[idx]);

            load_metas_.push_back(load_meta);
            unloaded_bindings_.erase(scan_binding);
        }
    }
    return expression;
}

Vector<SizeT> LoadedColumn(const Vector<LoadMeta> *load_metas, BaseTableRef *table_ref) {
    Vector<SizeT> column_ids;

    for (SizeT i = 0; i < load_metas->size(); i++) {
        auto binding = (*load_metas)[i].binding_;

        if (binding.table_idx == table_ref->table_index_) {
            column_ids.push_back(binding.column_idx);
        }
    }
    return column_ids;
}

SharedPtr<BaseExpression> CleanScan::VisitReplace(const SharedPtr<ColumnExpression> &expression) {
    return expression;
}

void CleanScan::VisitNode(LogicalNode &op) {
    switch (op.operator_type()) {
        case LogicalNodeType::kTableScan: {
            auto table_scan = dynamic_cast<LogicalTableScan &>(op);
            Vector<SizeT> project_indices = LoadedColumn(last_op_load_metas_.get(), table_scan.base_table_ref_.get());

            scan_table_indexes_.push_back(table_scan.base_table_ref_->table_index_);
            table_scan.base_table_ref_->RetainColumnByIndices(Move(project_indices));
            break;
        }
        case LogicalNodeType::kKnnScan: {
            auto knn_scan = dynamic_cast<LogicalKnnScan &>(op);
            Vector<SizeT> project_indices = LoadedColumn(last_op_load_metas_.get(), knn_scan.base_table_ref_.get());

            scan_table_indexes_.push_back(knn_scan.base_table_ref_->table_index_);
            knn_scan.base_table_ref_->RetainColumnByIndices(Move(project_indices));
            break;
        }
        case LogicalNodeType::kMatch: {
            auto match = dynamic_cast<LogicalMatch &>(op);
            Vector<SizeT> project_indices = LoadedColumn(last_op_load_metas_.get(), match.base_table_ref_.get());

            scan_table_indexes_.push_back(match.base_table_ref_->table_index_);
            match.base_table_ref_->RetainColumnByIndices(Move(project_indices));
            break;
        }
        case LogicalNodeType::kFusion: {
            // Skip
            VisitNodeChildren(op);
            VisitNodeExpression(op);
            break;
        }
        default: {
            last_op_load_metas_= op.load_metas();
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
                op.set_load_metas(MakeShared<Vector<LoadMeta>>(filtered_metas));
                scan_table_indexes_.clear();
            }
            break;
        }
    }
}

}