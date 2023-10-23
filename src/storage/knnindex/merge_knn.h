#pragma once

#include "common/types/alias/smart_ptr.h"
#include "common/types/complex/row_id.h"
#include "faiss/utils/ordered_key_value.h"
#include "storage/knnindex/knn_flat/new_result_handler.h"

namespace infinity {

class MergeKnnBase {};

template <typename DataType, template <typename, typename> typename C>
class MergeKnn : public MergeKnnBase {
    using HeapResultHandler = NewHeapResultHandler<C<DataType, RowID>>;
    using SingleResultHandler = HeapResultHandler::HeapSingleResultHandler;

public:
    explicit MergeKnn(i64 query_count, i64 topk)
        : total_count_(0), query_count_(query_count), topk_(topk), idx_array_(MakeUnique<RowID[]>(topk * query_count)),
          distance_array_(MakeUnique<DataType[]>(topk * query_count)) {
        heap_result_handler_ = MakeUnique<HeapResultHandler>(query_count, this->distance_array_.get(), this->idx_array_.get(), topk);
        single_result_handler_ = MakeUnique<SingleResultHandler>(*heap_result_handler_, query_count);
    }

public:
    void Search(const DataType *dist, const RowID *row_ids, int count);

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

template class MergeKnn<f32, faiss::CMax>;
template class MergeKnn<f32, faiss::CMin>;

} // namespace infinity