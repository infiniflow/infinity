
module;

import stl;
import parser;
import knn_result_handler;

import infinity_exception;
import faiss;

export module merge_knn;

namespace infinity {

export class MergeKnnBase {};

export template <typename DataType, template <typename, typename> typename C>
class MergeKnn : public MergeKnnBase {
    using HeapResultHandler = NewHeapResultHandler<C<DataType, RowID>>;
    using SingleResultHandler = HeapResultHandler::HeapSingleResultHandler;

public:
    explicit MergeKnn(u64 query_count, u64 topk)
        : total_count_(0), query_count_(query_count), topk_(topk), idx_array_(MakeUnique<RowID[]>(topk * query_count)),
          distance_array_(MakeUnique<DataType[]>(topk * query_count)) {
        heap_result_handler_ = MakeUnique<HeapResultHandler>(query_count, this->distance_array_.get(), this->idx_array_.get(), topk);
        single_result_handler_ = MakeUnique<SingleResultHandler>(*heap_result_handler_, query_count);
    }

public:
    void Search(const DataType *dist, const RowID *row_ids, u16 count);

    void Begin();

    void End();

    DataType *GetDistances() const;

    RowID *GetIDs() const;

    DataType *GetDistancesByIdx(i64 idx) const;

    RowID *GetIDsByIdx(i64 idx) const;

    i64 total_count() const { return total_count_; }

private:
    i64 total_count_{};
    bool begin_{false};
    i64 query_count_{};
    i64 topk_{};
    UniquePtr<RowID[]> idx_array_{};
    UniquePtr<DataType[]> distance_array_{};

private:
    UniquePtr<HeapResultHandler> heap_result_handler_{};
    UniquePtr<SingleResultHandler> single_result_handler_{};
};

template <typename DataType, template <typename, typename> typename C>
void MergeKnn<DataType, C>::Search(const DataType *dist, const RowID *row_ids, u16 count) {
    this->total_count_ += count;
    for (u64 i = 0; i < this->query_count_; ++i) {
        const DataType *d = dist + i * count;
        const RowID *r = row_ids + i * count;
        for (u16 j = 0; j < count; j++) {
            single_result_handler_->add_result(d[j], r[j], i);
        }
    }
}

template <typename DataType, template <typename, typename> typename C>
void MergeKnn<DataType, C>::Begin() {
    if (this->begin_ || this->query_count_ == 0) {
        return;
    }
    for (SizeT i = 0; i < this->query_count_; i++) {
        single_result_handler_->begin(i);
    }
    this->begin_ = true;
}

template <typename DataType, template <typename, typename> typename C>
void MergeKnn<DataType, C>::End() {
    if (!this->begin_)
        return;

    for (u64 i = 0; i < this->query_count_; ++i) {
        single_result_handler_->end(i);
    }

    this->begin_ = false;
}

template <typename DataType, template <typename, typename> typename C>
DataType *MergeKnn<DataType, C>::GetDistances() const {
    return heap_result_handler_->heap_dis_tab;
}

template <typename DataType, template <typename, typename> typename C>
RowID *MergeKnn<DataType, C>::GetIDs() const {
    return heap_result_handler_->heap_ids_tab;
}

template <typename DataType, template <typename, typename> typename C>
DataType *MergeKnn<DataType, C>::GetDistancesByIdx(i64 idx) const {
    if (idx >= this->query_count_) {
        Error<ExecutorException>("Query index exceeds the limit");
    }
    return heap_result_handler_->heap_dis_tab + idx * this->topk_;
}

template <typename DataType, template <typename, typename> typename C>
RowID *MergeKnn<DataType, C>::GetIDsByIdx(i64 idx) const {
    if (idx >= this->query_count_) {
        Error<ExecutorException>("Query index exceeds the limit");
    }
    return heap_result_handler_->heap_ids_tab + idx * this->topk_;
}

template class MergeKnn<f32, FaissCMax>;
template class MergeKnn<f32, FaissCMin>;

} // namespace infinity