//
// Created by yzq on 11/6/23.
//
module;
#include "faiss/impl/ResultHandler.h"
#include "faiss/utils/Heap.h"
import faiss;
export module extra_faiss_part;
namespace infinity {
export template <typename T>
using FaissSingleBestResultHandler = faiss::SingleBestResultHandler<T>;

export template <typename T = int64_t>
using float_maxheap_array_t = faiss::HeapArray<FaissCMax<float, T>>;
} // namespace infinity