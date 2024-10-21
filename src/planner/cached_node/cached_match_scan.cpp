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

module cached_match_scan;

import logical_match_sparse_scan;
import logical_match_tensor_scan;
import logical_node_type;
import physical_scan_base;
import physical_knn_scan;
import physical_match_sparse_scan;
import physical_match_tensor_scan;
import physical_merge_knn;
import physical_merge_match_sparse;
import physical_merge_match_tensor;

namespace infinity {

CachedMatchScanBase::CachedMatchScanBase(TxnTimeStamp query_ts, const LogicalMatchScanBase *logical_match_scan)
    : CachedScanBase(logical_match_scan->operator_type(), logical_match_scan->base_table_ref_.get(), query_ts, logical_match_scan->GetOutputNames()),
      query_expression_(logical_match_scan->query_expression_), filter_expression_(logical_match_scan->filter_expression_) {}

CachedMatchScanBase::CachedMatchScanBase(TxnTimeStamp query_ts, const PhysicalKnnScan *physical_knn_scan)
    : CachedScanBase(LogicalNodeType::kKnnScan, physical_knn_scan, query_ts), query_expression_(physical_knn_scan->knn_expression_),
      filter_expression_(physical_knn_scan->common_query_filter_->original_filter_) {}

CachedMatchScanBase::CachedMatchScanBase(TxnTimeStamp query_ts, const PhysicalMatchSparseScan *physical_sparse_scan)
    : CachedScanBase(LogicalNodeType::kMatchSparseScan, physical_sparse_scan, query_ts), query_expression_(physical_sparse_scan->match_sparse_expr()),
      filter_expression_(physical_sparse_scan->common_query_filter_->original_filter_) {}

CachedMatchScanBase::CachedMatchScanBase(TxnTimeStamp query_ts, const PhysicalMatchTensorScan *physical_tensor_scan)
    : CachedScanBase(LogicalNodeType::kMatchTensorScan, physical_tensor_scan, query_ts), query_expression_(physical_tensor_scan->match_tensor_expr()),
      filter_expression_(physical_tensor_scan->common_query_filter()->original_filter_) {}

CachedMatchScanBase::CachedMatchScanBase(TxnTimeStamp query_ts, const PhysicalMergeKnn *physical_merge_knn)
    : CachedScanBase(LogicalNodeType::kKnnScan, physical_merge_knn, query_ts), query_expression_(physical_merge_knn->knn_expression_),
      filter_expression_(physical_merge_knn->filter_expression_) {}

CachedMatchScanBase::CachedMatchScanBase(TxnTimeStamp query_ts, const PhysicalMergeMatchSparse *physical_merge_match_sparse)
    : CachedScanBase(LogicalNodeType::kMatchSparseScan, physical_merge_match_sparse, query_ts),
      query_expression_(physical_merge_match_sparse->match_sparse_expr()), filter_expression_(physical_merge_match_sparse->filter_expression()) {}

CachedMatchScanBase::CachedMatchScanBase(TxnTimeStamp query_ts, const PhysicalMergeMatchTensor *physical_merge_match_tensor)
    : CachedScanBase(LogicalNodeType::kMatchTensorScan,
                     physical_merge_match_tensor->base_table_ref(),
                     query_ts,
                     physical_merge_match_tensor->GetOutputNames()),
      query_expression_(physical_merge_match_tensor->match_tensor_expr()), filter_expression_(physical_merge_match_tensor->filter_expression()) {}

u64 CachedMatchScanBase::Hash() const {
    u64 h = CachedScanBase::Hash();
    h ^= query_expression_->Hash();
    if (filter_expression_) {
        h ^= filter_expression_->Hash();
    }
    return h;
}

bool CachedMatchScanBase::Eq(const CachedNodeBase &other_base) const {
    if (type() != other_base.type()) {
        return false;
    }
    const auto &other = static_cast<const CachedMatchScanBase &>(other_base);
    if (!CachedScanBase::Eq(other)) {
        return false;
    }
    if (!query_expression_->Eq(*other.query_expression_)) {
        return false;
    }
    if (filter_expression_ && other.filter_expression_) {
        return filter_expression_->Eq(*other.filter_expression_);
    }
    if (bool(filter_expression_) ^ bool(other.filter_expression_)) {
        return false;
    }
    return true;
}

CachedKnnScan::CachedKnnScan(TxnTimeStamp query_ts, const LogicalKnnScan *logical_knn_scan) : CachedMatchScanBase(query_ts, logical_knn_scan) {}

CachedKnnScan::CachedKnnScan(TxnTimeStamp query_ts, const PhysicalKnnScan *physical_knn_scan) : CachedMatchScanBase(query_ts, physical_knn_scan) {
    auto &expr = physical_knn_scan->knn_expression_;
    expr->query_embedding_.Own(expr->embedding_data_type_, expr->dimension_);
}

CachedKnnScan::CachedKnnScan(TxnTimeStamp query_ts, const PhysicalMergeKnn *physical_merge_knn) : CachedMatchScanBase(query_ts, physical_merge_knn) {
    auto &expr = physical_merge_knn->knn_expression_;
    expr->query_embedding_.Own(expr->embedding_data_type_, expr->dimension_);
}

CachedMatchSparseScan::CachedMatchSparseScan(TxnTimeStamp query_ts, const LogicalMatchSparseScan *logical_sparse_scan)
    : CachedMatchScanBase(query_ts, logical_sparse_scan) {}

CachedMatchSparseScan::CachedMatchSparseScan(TxnTimeStamp query_ts, const PhysicalMatchSparseScan *physical_sparse_scan)
    : CachedMatchScanBase(query_ts, physical_sparse_scan) {}

CachedMatchSparseScan::CachedMatchSparseScan(TxnTimeStamp query_ts, const PhysicalMergeMatchSparse *physical_merge_match_sparse)
    : CachedMatchScanBase(query_ts, physical_merge_match_sparse) {}

CachedMatchTensorScan::CachedMatchTensorScan(TxnTimeStamp query_ts, const LogicalMatchTensorScan *logical_tensor_scan)
    : CachedMatchScanBase(query_ts, logical_tensor_scan), topn_(logical_tensor_scan->topn_), index_options_(logical_tensor_scan->index_options_) {}

CachedMatchTensorScan::CachedMatchTensorScan(TxnTimeStamp query_ts, const PhysicalMatchTensorScan *physical_tensor_scan)
    : CachedMatchScanBase(query_ts, physical_tensor_scan), topn_(physical_tensor_scan->GetTopN()),
      index_options_(physical_tensor_scan->index_options()) {
    auto &expr = physical_tensor_scan->match_tensor_expr();
    expr->query_embedding_.Own(expr->embedding_data_type_, expr->dimension_);
}

CachedMatchTensorScan::CachedMatchTensorScan(TxnTimeStamp query_ts, const PhysicalMergeMatchTensor *physical_merge_match_tensor)
    : CachedMatchScanBase(query_ts, physical_merge_match_tensor), topn_(physical_merge_match_tensor->GetTopN()),
      index_options_(physical_merge_match_tensor->index_options()) {
    auto &expr = physical_merge_match_tensor->match_tensor_expr();
    expr->query_embedding_.Own(expr->embedding_data_type_, expr->dimension_);
}

u64 CachedMatchTensorScan::Hash() const {
    u64 h = 0;
    h ^= CachedMatchScanBase::Hash();
    h ^= std::hash<u32>()(topn_);
    return h;
}

bool CachedMatchTensorScan::Eq(const CachedNodeBase &other) const {
    if (!CachedMatchScanBase::Eq(other)) {
        return false;
    }
    const auto &other_tensor = static_cast<const CachedMatchTensorScan &>(other);
    return topn_ == other_tensor.topn_ && *index_options_ == *other_tensor.index_options_;
}

} // namespace infinity
