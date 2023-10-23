#include "merge_knn.h"
#include "common/utility/infinity_assert.h"

namespace infinity {
template <typename DistType, template <typename, typename> typename C>
void MergeKnn<DistType, C>::Search(const DistType *dist, const RowID *row_ids, int count) {
    this->total_count_ += count;
    for (int64_t i = 0; i < this->query_count_; i++) {
        const DistType *d = dist + i * count;
        const RowID *r = row_ids + i * count;
        for (i16 j = 0; j < count; j++) {
            single_result_handler_->add_result(d[j], r[j], i);
        }
    }
}

template <typename DistType, template <typename, typename> typename C>
void MergeKnn<DistType, C>::Begin() {
    if (this->begin_ || this->query_count_ == 0) {
        return;
    }
    for (SizeT i = 0; i < this->query_count_; i++) {
        single_result_handler_->begin(i);
    }
    this->begin_ = true;
}

template <typename DistType, template <typename, typename> typename C>
void MergeKnn<DistType, C>::End() {
    if (!this->begin_)
        return;

    for (i32 i = 0; i < this->query_count_; ++i) {
        single_result_handler_->end(i);
    }

    this->begin_ = false;
}

template <typename DistType, template <typename, typename> typename C>
DistType *MergeKnn<DistType, C>::GetDistances() const {
    return heap_result_handler_->heap_dis_tab;
}

template <typename DistType, template <typename, typename> typename C>
RowID *MergeKnn<DistType, C>::GetIDs() const {
    return heap_result_handler_->heap_ids_tab;
}

template <typename DistType, template <typename, typename> typename C>
DistType *MergeKnn<DistType, C>::GetDistancesByIdx(i64 idx) const {
    if (idx >= this->query_count_) {
        ExecutorError("Query index exceeds the limit")
    }
    return heap_result_handler_->heap_dis_tab + idx * this->topk_;
}

template <typename DistType, template <typename, typename> typename C>
RowID *MergeKnn<DistType, C>::GetIDsByIdx(i64 idx) const {
    if (idx >= this->query_count_) {
        ExecutorError("Query index exceeds the limit")
    }
    return heap_result_handler_->heap_ids_tab + idx * this->topk_;
}

} // namespace infinity