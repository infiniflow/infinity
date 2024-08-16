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

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
module emvb_index;
import stl;
import mlas_matrix_multiply;
import vector_distance;
import emvb_product_quantization;
import emvb_search;
import kmeans_partition;
import index_base;
import status;
import logger;
import third_party;
import infinity_exception;
import file_system;
import internal_types;
import segment_entry;
import block_entry;
import block_column_entry;
import column_vector;
import column_def;
import buffer_manager;
import default_values;
import block_index;

namespace infinity {

extern template class EMVBSharedVec<u32>;
extern template class EMVBSearch<32>;
extern template class EMVBSearch<64>;
extern template class EMVBSearch<96>;
extern template class EMVBSearch<128>;
extern template class EMVBSearch<160>;
extern template class EMVBSearch<192>;
extern template class EMVBSearch<224>;
extern template class EMVBSearch<256>;

EMVBIndex::EMVBIndex(const u32 start_segment_offset,
                     const u32 embedding_dimension,
                     const u32 residual_pq_subspace_num,
                     const u32 residual_pq_subspace_bits)
    : start_segment_offset_(start_segment_offset), embedding_dimension_(embedding_dimension), residual_pq_subspace_num_(residual_pq_subspace_num),
      residual_pq_subspace_bits_(residual_pq_subspace_bits) {
    // now always use OPQ
    product_quantizer_ = GetEMVBOPQ(residual_pq_subspace_num, residual_pq_subspace_bits, embedding_dimension);
}

u32 EMVBIndex::GetDocNum() const {
    std::shared_lock lock(rw_mutex_);
    return n_docs_;
}

u32 EMVBIndex::GetTotalEmbeddingNum() const {
    std::shared_lock lock(rw_mutex_);
    return n_total_embeddings_;
}

void EMVBIndex::BuildEMVBIndex(const RowID base_rowid,
                               const u32 row_count,
                               const SegmentEntry *segment_entry,
                               const SharedPtr<ColumnDef> &column_def,
                               BufferManager *buffer_mgr) {
    const auto time_0 = std::chrono::high_resolution_clock::now();
    {
        const SegmentID expected_segment_id = base_rowid.segment_id_;
        if (const SegmentID segment_id = segment_entry->segment_id(); segment_id != expected_segment_id) {
            const auto error_msg = fmt::format("EMVBIndex::BuildEMVBIndex: segment_id mismatch: expect {}, got {}.", expected_segment_id, segment_id);
            UnrecoverableError(error_msg);
        }
    }
    Vector<SharedPtr<BlockEntry>> block_entries;
    {
        const BlocksGuard blocks_guard = segment_entry->GetBlocksGuard();
        block_entries = blocks_guard.block_entries_;
    }
    const SegmentOffset start_segment_offset = base_rowid.segment_offset_;
    const ColumnID column_id = column_def->id();
    u64 embedding_count = 0;
    Deque<Pair<u32, u32>> all_embedding_pos;
    {
        // read the segment to get total embedding count
        BlockID block_id = start_segment_offset / DEFAULT_BLOCK_CAPACITY;
        BlockOffset block_offset = start_segment_offset % DEFAULT_BLOCK_CAPACITY;
        auto column_vector = MakeUnique<ColumnVector>(block_entries[block_id]->GetColumnBlockEntry(column_id)->GetConstColumnVector(buffer_mgr));
        const TensorT *tensor_ptr = reinterpret_cast<const TensorT *>(column_vector->data());
        for (u32 i = 0; i < row_count; ++i) {
            {
                const SegmentOffset new_segment_offset = start_segment_offset + i;
                block_offset = new_segment_offset % DEFAULT_BLOCK_CAPACITY;
                if (const BlockID new_block_id = new_segment_offset / DEFAULT_BLOCK_CAPACITY; new_block_id != block_id) {
                    block_id = new_block_id;
                    column_vector =
                        MakeUnique<ColumnVector>(block_entries[block_id]->GetColumnBlockEntry(column_id)->GetConstColumnVector(buffer_mgr));
                    tensor_ptr = reinterpret_cast<const TensorT *>(column_vector->data());
                }
            }
            const auto [embedding_num, chunk_offset] = tensor_ptr[block_offset];
            embedding_count += embedding_num;
            for (u32 j = 0; j < embedding_num; ++j) {
                all_embedding_pos.emplace_back(i, j);
            }
        }
    }
    const auto time_1 = std::chrono::high_resolution_clock::now();
    {
        std::ostringstream oss;
        oss << "EMVBIndex::BuildEMVBIndex: Read segment data, total row count: " << row_count << ", total embedding count: " << embedding_count
            << ", total time cost: " << std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(time_1 - time_0);
        LOG_INFO(std::move(oss).str());
    }
    // prepare centroid count
    const u64 sqrt_embedding_num = std::sqrt(embedding_count);
    const u64 centroid_count_before_8 = std::min<u64>(sqrt_embedding_num, embedding_count / 32ul);
    const u64 centroid_count = (centroid_count_before_8 + 7ul) & (~(7ul));
    // prepare training data
    if (centroid_count > std::numeric_limits<u32>::max()) {
        const auto error_msg = "EMVBIndex::BuildEMVBIndex: centroid_count exceeds u32 limit!";
        LOG_ERROR(error_msg);
        UnrecoverableError(error_msg);
    }
    n_centroids_ = static_cast<u32>(centroid_count);
    const auto least_training_data_num = ExpectLeastTrainingDataNum();
    if (embedding_count < least_training_data_num) {
        const auto error_msg =
            fmt::format("EMVBIndex::BuildEMVBIndex: embedding_count must be at least {}, got {} instead.", least_training_data_num, embedding_count);
        UnrecoverableError(error_msg);
    }
    // train the index
    {
        const auto training_embedding_num = std::min<u64>(embedding_count, 8ul * least_training_data_num);
        const auto training_data = MakeUniqueForOverwrite<f32[]>(training_embedding_num * embedding_dimension_);
        // prepare training data
        {
            Vector<Pair<u32, u32>> sample_result;
            sample_result.reserve(training_embedding_num);
            std::random_device rd;
            std::mt19937 gen(rd());
            std::ranges::sample(all_embedding_pos, std::back_inserter(sample_result), training_embedding_num, gen);
            if (sample_result.size() != training_embedding_num) {
                const auto error_msg = fmt::format("EMVBIndex::BuildEMVBIndex: sample failed to get {} samples.", training_embedding_num);
                UnrecoverableError(error_msg);
            }
            for (u64 i = 0; i < training_embedding_num; ++i) {
                const auto [sample_row, sample_id] = sample_result[i];
                const SegmentOffset new_segment_offset = start_segment_offset + sample_row;
                const BlockID block_id = new_segment_offset / DEFAULT_BLOCK_CAPACITY;
                const BlockOffset block_offset = new_segment_offset % DEFAULT_BLOCK_CAPACITY;
                auto column_vector = block_entries[block_id]->GetColumnBlockEntry(column_id)->GetConstColumnVector(buffer_mgr);
                auto [raw_data, embedding_num] = column_vector.GetTensorRaw(block_offset);
                std::copy_n(reinterpret_cast<const f32 *>(raw_data.data()) + sample_id * embedding_dimension_,
                            embedding_dimension_,
                            training_data.get() + i * embedding_dimension_);
            }
        }
        const auto time_2 = std::chrono::high_resolution_clock::now();
        {
            std::ostringstream oss;
            oss << "EMVBIndex::BuildEMVBIndex: Prepare training data, training embedding num: " << training_embedding_num
                << ", time cost: " << std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(time_2 - time_1);
            LOG_INFO(std::move(oss).str());
        }
        // call train
        Train(centroid_count, training_data.get(), training_embedding_num, 20);
        const auto time_3 = std::chrono::high_resolution_clock::now();
        {
            std::ostringstream oss;
            oss << "EMVBIndex::BuildEMVBIndex: Train index, time cost: "
                << std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(time_3 - time_2);
            LOG_INFO(std::move(oss).str());
        }
    }
    // add data
    {
        const auto time_4 = std::chrono::high_resolution_clock::now();
        BlockID block_id = start_segment_offset / DEFAULT_BLOCK_CAPACITY;
        BlockOffset block_offset = start_segment_offset % DEFAULT_BLOCK_CAPACITY;
        auto column_vector = MakeUnique<ColumnVector>(block_entries[block_id]->GetColumnBlockEntry(column_id)->GetConstColumnVector(buffer_mgr));
        for (u32 i = 0; i < row_count; ++i) {
            {
                const SegmentOffset new_segment_offset = start_segment_offset + i;
                block_offset = new_segment_offset % DEFAULT_BLOCK_CAPACITY;
                if (const BlockID new_block_id = new_segment_offset / DEFAULT_BLOCK_CAPACITY; new_block_id != block_id) {
                    block_id = new_block_id;
                    column_vector =
                        MakeUnique<ColumnVector>(block_entries[block_id]->GetColumnBlockEntry(column_id)->GetConstColumnVector(buffer_mgr));
                }
            }
            auto [raw_data, embedding_num] = column_vector->GetTensorRaw(block_offset);
            AddOneDocEmbeddings(reinterpret_cast<const f32 *>(raw_data.data()), embedding_num);
        }
        {
            const auto time_5 = std::chrono::high_resolution_clock::now();
            const auto total_add_time = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(time_5 - time_4);
            const auto avg_add_time = total_add_time / row_count;
            std::ostringstream oss;
            oss << "EMVBIndex::BuildEMVBIndex: Add data, total row count: " << row_count << ", total time cost: " << total_add_time
                << ", avg time cost: " << avg_add_time;
            LOG_INFO(std::move(oss).str());
        }
    }
}

// need embedding num:
// 1. (32 ~ 256) * n_centroids_ for centroids
// 2. (32 ~ 256) * (1 << residual_pq_subspace_bits) for residual product quantizer
u64 EMVBIndex::ExpectLeastTrainingDataNum() const { return std::max<u64>(32ul * n_centroids_, 32ul * (1ul << residual_pq_subspace_bits_)); }

void EMVBIndex::Train(const u32 centroids_num, const f32 *embedding_data, const u64 embedding_num, const u32 iter_cnt) {
    // set n_centroids_
    n_centroids_ = centroids_num;
    // check n_centroids_
    if (((n_centroids_ % 8) != 0) || (n_centroids_ == 0)) {
        const auto error_msg = fmt::format("EMVBIndex::Train: n_centroids_ must be a multiple of 8, got {} instead.", n_centroids_);
        UnrecoverableError(error_msg);
    }
    // allocate space for centroids
    centroids_data_.resize(static_cast<u64>(n_centroids_) * embedding_dimension_);
    centroid_norms_neg_half_.resize(n_centroids_);
    centroids_to_docid_ = MakeUnique<EMVBSharedVec<u32>[]>(n_centroids_);
    // check training data num
    if (const auto least_num = ExpectLeastTrainingDataNum(); embedding_num < least_num) {
        const auto error_msg = fmt::format("EMVBIndex::Train: embedding_num must be at least {}, got {} instead.", least_num, embedding_num);
        UnrecoverableError(error_msg);
    }
    // train both centroids and residual product quantizer
    // step 1. train centroids
    const auto time_0 = std::chrono::high_resolution_clock::now();
    {
        const auto result_centroid_num = GetKMeansCentroids<f32>(MetricType::kMetricL2,
                                                                 embedding_dimension_,
                                                                 embedding_num,
                                                                 embedding_data,
                                                                 centroids_data_,
                                                                 n_centroids_,
                                                                 iter_cnt);
        if (result_centroid_num != n_centroids_) {
            const auto error_msg =
                fmt::format("EMVBIndex::Train: KMeans failed to get {} centroids, got {} instead.", n_centroids_, result_centroid_num);
            UnrecoverableError(error_msg);
        }
        LOG_TRACE(fmt::format("EMVBIndex::Train: KMeans got {} centroids.", result_centroid_num));
    }
    {
        const f32 *centroid_data = centroids_data_.data();
        for (u32 i = 0; i < n_centroids_; ++i) {
            centroid_norms_neg_half_[i] = -0.5f * L2NormSquare<f32, f32, u32>(centroid_data, embedding_dimension_);
            centroid_data += embedding_dimension_;
        }
    }
    const auto time_1 = std::chrono::high_resolution_clock::now();
    {
        std::ostringstream oss;
        oss << "EMVBIndex::Train: Finish train centroids, time cost: "
            << std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(time_1 - time_0);
        LOG_INFO(std::move(oss).str());
    }
    // step 2. get residuals
    const auto residuals = MakeUniqueForOverwrite<f32[]>(embedding_num * embedding_dimension_);
    {
        // distance: for every embedding, e * c - 0.5 * c^2, find max
        const auto dist_table = MakeUniqueForOverwrite<f32[]>(embedding_num * n_centroids_);
        matrixA_multiply_transpose_matrixB_output_to_C(embedding_data,
                                                       centroids_data_.data(),
                                                       embedding_num,
                                                       n_centroids_,
                                                       embedding_dimension_,
                                                       dist_table.get());
        for (u64 i = 0; i < embedding_num; ++i) {
            const f32 *embedding_data_ptr = embedding_data + i * embedding_dimension_;
            f32 *output_ptr = residuals.get() + i * embedding_dimension_;
            f32 max_neg_distance = std::numeric_limits<f32>::lowest();
            u64 max_id = 0;
            const f32 *dist_ptr = dist_table.get() + i * n_centroids_;
            for (u32 k = 0; k < n_centroids_; ++k) {
                if (const f32 neg_distance = dist_ptr[k] + centroid_norms_neg_half_[k]; neg_distance > max_neg_distance) {
                    max_neg_distance = neg_distance;
                    max_id = k;
                }
            }
            const f32 *centroids_data_ptr = centroids_data_.data() + max_id * embedding_dimension_;
            for (u32 j = 0; j < embedding_dimension_; ++j) {
                output_ptr[j] = embedding_data_ptr[j] - centroids_data_ptr[j];
            }
        }
    }
    const auto time_2 = std::chrono::high_resolution_clock::now();
    {
        std::ostringstream oss;
        oss << "EMVBIndex::Train: Finish calculate residuals, time cost: "
            << std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(time_2 - time_1);
        LOG_INFO(std::move(oss).str());
    }
    LOG_TRACE("EMVBIndex::Train: Finish calculate residuals.");
    // step 3. train residuals
    product_quantizer_->Train(residuals.get(), embedding_num, iter_cnt);
    const auto time_3 = std::chrono::high_resolution_clock::now();
    {
        std::ostringstream oss;
        oss << "EMVBIndex::Train: Finish train pq for residuals, time cost: "
            << std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(time_3 - time_2);
        LOG_INFO(std::move(oss).str());
    }
    LOG_TRACE("EMVBIndex::Train: Finish train pq for residuals.");
}

void EMVBIndex::AddOneDocEmbeddings(const f32 *embedding_data, const u32 embedding_num) {
    if (embedding_num == 0) {
        const auto error_msg = "EMVBIndex::AddOneDocEmbeddings: embedding_num must be greater than 0.";
        UnrecoverableError(error_msg);
    }
    std::unique_lock lock(rw_mutex_);
    // only one thread can add doc to the index at the same time
    // step 1. doc - embedding info
    const u32 old_doc_num = n_docs_;
    const u32 old_total_embeddings = n_total_embeddings_;
    doc_lens_.PushBack(embedding_num);
    doc_offsets_.PushBack(old_total_embeddings);
    // step 2. assign to centroids
    const auto centroid_id_assignments = MakeUniqueForOverwrite<u32[]>(embedding_num);
    const auto residuals = MakeUniqueForOverwrite<f32[]>(embedding_num * embedding_dimension_);
    {
        // distance: for every embedding, e * c - 0.5 * c^2, find max
        const auto dist_table = MakeUniqueForOverwrite<f32[]>(embedding_num * n_centroids_);
        matrixA_multiply_transpose_matrixB_output_to_C(embedding_data,
                                                       centroids_data_.data(),
                                                       embedding_num,
                                                       n_centroids_,
                                                       embedding_dimension_,
                                                       dist_table.get());
        for (u32 i = 0; i < embedding_num; ++i) {
            const f32 *embedding_data_ptr = embedding_data + i * embedding_dimension_;
            f32 *output_ptr = residuals.get() + i * embedding_dimension_;
            f32 max_neg_distance = std::numeric_limits<f32>::lowest();
            u32 max_id = 0;
            const f32 *dist_ptr = dist_table.get() + i * n_centroids_;
            for (u32 k = 0; k < n_centroids_; ++k) {
                if (const f32 neg_distance = dist_ptr[k] + centroid_norms_neg_half_[k]; neg_distance > max_neg_distance) {
                    max_neg_distance = neg_distance;
                    max_id = k;
                }
            }
            centroid_id_assignments[i] = max_id;
            const f32 *centroids_data_ptr = centroids_data_.data() + max_id * embedding_dimension_;
            for (u32 j = 0; j < embedding_dimension_; ++j) {
                output_ptr[j] = embedding_data_ptr[j] - centroids_data_ptr[j];
            }
        }
    }
    centroid_id_assignments_.PushBack(centroid_id_assignments.get(), centroid_id_assignments.get() + embedding_num);
    for (u32 i = 0; i < embedding_num; ++i) {
        const u32 centroid_id = centroid_id_assignments[i];
        centroids_to_docid_[centroid_id].PushBackIfDifferentFromLast(old_doc_num);
    }
    // step 3. add residuals to product quantizer
    product_quantizer_->AddEmbeddings(residuals.get(), embedding_num);
    // finally, update count
    n_total_embeddings_ += embedding_num;
    ++n_docs_;
}

// return id: offset in the segment
EMVBQueryResultType EMVBIndex::SearchWithBitmask(const f32 *query_ptr,
                                                 const u32 query_embedding_num,
                                                 const u32 top_n,
                                                 Bitmask &bitmask,
                                                 const SegmentEntry *segment_entry,
                                                 const BlockIndex *block_index,
                                                 const TxnTimeStamp begin_ts,
                                                 const u32 centroid_nprobe,
                                                 const f32 threshold_first,
                                                 const u32 n_doc_to_score,
                                                 const u32 out_second_stage,
                                                 const f32 threshold_final) const {
    // template argument should be in ascending order
    // keep consistent with emvb_search.cpp
    return query_token_num_helper<32, 64, 96, 128, 160, 192, 224, 256>(query_ptr,
                                                                       query_embedding_num,
                                                                       centroid_nprobe,
                                                                       threshold_first,
                                                                       n_doc_to_score,
                                                                       out_second_stage,
                                                                       top_n,
                                                                       threshold_final,
                                                                       bitmask,
                                                                       start_segment_offset_,
                                                                       segment_entry,
                                                                       block_index,
                                                                       begin_ts);
}

// the two thresholds are for every (query embedding, candidate embedding) pair
// candidate embeddings are centroids
// unqualified pairs will not be scored
// but if nothing is left, an exhaustive search will be performed

constexpr u32 current_max_query_token_num = 256;

// return id: offset from start_segment_offset_
EMVBQueryResultType EMVBIndex::GetQueryResult(const f32 *query_ptr,
                                              const u32 query_embedding_num,
                                              const u32 centroid_nprobe,  // step 1, centroid candidates for every query embedding
                                              const f32 threshold_first,  // step 1, threshold for query - centroid score
                                              const u32 n_doc_to_score,   // topn by centroids hit count
                                              const u32 out_second_stage, // step 2, topn, use nearest centroid score as embedding score
                                              const u32 top_k,            // step 3, final topk, refine score by residual pq
                                              const f32 threshold_final   // step 3, threshold to reduce maxsim calculation
) const {
    // template argument should be in ascending order
    // keep consistent with emvb_search.cpp
    return query_token_num_helper<32, 64, 96, 128, 160, 192, 224, 256>(query_ptr,
                                                                       query_embedding_num,
                                                                       centroid_nprobe,
                                                                       threshold_first,
                                                                       n_doc_to_score,
                                                                       out_second_stage,
                                                                       top_k,
                                                                       threshold_final);
}

template <u32 I, u32... J>
EMVBQueryResultType EMVBIndex::query_token_num_helper(const f32 *query_ptr, u32 query_embedding_num, auto &&...query_args) const {
    if (query_embedding_num <= I) {
        return GetQueryResultT<I>(query_ptr, query_embedding_num, std::forward<decltype(query_args)>(query_args)...);
    }
    return query_token_num_helper<J...>(query_ptr, query_embedding_num, std::forward<decltype(query_args)>(query_args)...);
}

template <>
EMVBQueryResultType EMVBIndex::query_token_num_helper(const f32 *query_ptr, u32 query_embedding_num, auto &&...query_args) const {
    auto error_msg = fmt::format("EMVBIndex::GetQueryResult: query_embedding_num max value: {}, got {} instead.",
                                 current_max_query_token_num,
                                 query_embedding_num);
    error_msg += fmt::format(" Embeddings after {} will not be used for search.", current_max_query_token_num);
    error_msg += " Please Add instantiation of EMVBSearch with a bigger FIXED_QUERY_TOKEN_NUM value.";
    LOG_ERROR(error_msg);
    return GetQueryResultT<current_max_query_token_num>(query_ptr, query_embedding_num, std::forward<decltype(query_args)>(query_args)...);
}

template <u32 FIXED_QUERY_TOKEN_NUM>
EMVBQueryResultType EMVBIndex::GetQueryResultT(const f32 *query_ptr, const u32 query_embedding_num, auto &&...query_args) const {
    UniquePtr<f32[]> extended_query_ptr;
    const f32 *query_ptr_to_use = query_ptr;
    // extend query to FIXED_QUERY_TOKEN_NUM
    if (query_embedding_num < FIXED_QUERY_TOKEN_NUM) {
        extended_query_ptr = MakeUniqueForOverwrite<f32[]>(FIXED_QUERY_TOKEN_NUM * embedding_dimension_);
        std::copy_n(query_ptr, query_embedding_num * embedding_dimension_, extended_query_ptr.get());
        std::fill_n(extended_query_ptr.get() + query_embedding_num * embedding_dimension_,
                    (FIXED_QUERY_TOKEN_NUM - query_embedding_num) * embedding_dimension_,
                    0.0f);
        query_ptr_to_use = extended_query_ptr.get();
    }
    // access snapshot of index data
    const u32 n_docs = n_docs_.load(std::memory_order_acquire);
    const auto doc_lens_snapshot = doc_lens_.GetData();
    const auto doc_offsets_snapshot = doc_offsets_.GetData();
    const auto centroid_id_assignments_snapshot = centroid_id_assignments_.GetData();
    // execute search
    EMVBSearch<FIXED_QUERY_TOKEN_NUM> search_helper(embedding_dimension_,
                                                    n_docs,
                                                    n_centroids_,
                                                    doc_lens_snapshot.first.get(),
                                                    doc_offsets_snapshot.first.get(),
                                                    centroid_id_assignments_snapshot.first.get(),
                                                    centroids_data_.data(),
                                                    centroids_to_docid_.get(),
                                                    product_quantizer_.get());
    return search_helper.GetQueryResult(query_ptr_to_use, std::forward<decltype(query_args)>(query_args)...);
}

template <typename T>
void Serialize(FileHandler &file_handler, const Vector<T> &val) {
    const u32 size = val.size();
    file_handler.Write(&size, sizeof(size));
    file_handler.Write(val.data(), size * sizeof(T));
}

template <typename T>
void DeSerialize(FileHandler &file_handler, Vector<T> &val) {
    u32 size = 0;
    file_handler.Read(&size, sizeof(size));
    val.resize(size);
    file_handler.Read(val.data(), size * sizeof(T));
}

void Serialize(FileHandler &file_handler, const EMVBSharedVec<u32> &val, const u32 expect_element_num) {
    const auto [shared_u32_ptr, size] = val.GetData();
    if (size != expect_element_num) {
        const auto error_msg = fmt::format("EMVBSharedVec size mismatch: expect {}, got {}.", expect_element_num, size);
        UnrecoverableError(error_msg);
    }
    file_handler.Write(&expect_element_num, sizeof(expect_element_num));
    file_handler.Write(shared_u32_ptr.get(), expect_element_num * sizeof(u32));
}

void DeSerialize(FileHandler &file_handler, EMVBSharedVec<u32> &val, const u32 expect_element_num) {
    if (const auto [_, old_size] = val.GetData(); old_size > 0) {
        const auto error_msg = fmt::format("EMVBSharedVec size mismatch: expect 0, got {}.", old_size);
        UnrecoverableError(error_msg);
    }
    u32 size = 0;
    file_handler.Read(&size, sizeof(size));
    if (size != expect_element_num) {
        const auto error_msg = fmt::format("EMVBSharedVec size mismatch: expect {}, got {}.", expect_element_num, size);
        UnrecoverableError(error_msg);
    }
    const auto tmp_buffer = MakeUniqueForOverwrite<u32[]>(expect_element_num);
    file_handler.Read(tmp_buffer.get(), expect_element_num * sizeof(u32));
    val.PushBack(tmp_buffer.get(), tmp_buffer.get() + expect_element_num);
}

void Serialize(FileHandler &file_handler, const EMVBSharedVec<u32> &val) {
    const auto [shared_u32_ptr, size] = val.GetData();
    const u32 element_num = size;
    file_handler.Write(&element_num, sizeof(element_num));
    file_handler.Write(shared_u32_ptr.get(), size * sizeof(u32));
}

void DeSerialize(FileHandler &file_handler, EMVBSharedVec<u32> &val) {
    if (const auto [_, old_size] = val.GetData(); old_size > 0) {
        const auto error_msg = fmt::format("EMVBSharedVec size mismatch: expect 0, got {}.", old_size);
        UnrecoverableError(error_msg);
    }
    u32 element_num = 0;
    file_handler.Read(&element_num, sizeof(element_num));
    const auto tmp_buffer = MakeUniqueForOverwrite<u32[]>(element_num);
    file_handler.Read(tmp_buffer.get(), element_num * sizeof(u32));
    val.PushBack(tmp_buffer.get(), tmp_buffer.get() + element_num);
}

void EMVBIndex::SaveIndexInner(FileHandler &file_handler) {
    std::unique_lock lock(rw_mutex_);
    // write index data
    file_handler.Write(&start_segment_offset_, sizeof(start_segment_offset_));
    file_handler.Write(&embedding_dimension_, sizeof(embedding_dimension_));
    file_handler.Write(&residual_pq_subspace_num_, sizeof(residual_pq_subspace_num_));
    file_handler.Write(&residual_pq_subspace_bits_, sizeof(residual_pq_subspace_bits_));
    file_handler.Write(&n_centroids_, sizeof(n_centroids_));
    Serialize(file_handler, centroids_data_);
    Serialize(file_handler, centroid_norms_neg_half_);
    const u32 n_docs = n_docs_;
    file_handler.Write(&n_docs, sizeof(n_docs));
    file_handler.Write(&n_total_embeddings_, sizeof(n_total_embeddings_));
    Serialize(file_handler, doc_lens_, n_docs);
    Serialize(file_handler, doc_offsets_, n_docs);
    Serialize(file_handler, centroid_id_assignments_, n_total_embeddings_);
    for (u32 i = 0; i < n_centroids_; ++i) {
        Serialize(file_handler, centroids_to_docid_[i]);
    }
    // write product quantizer
    product_quantizer_->Save(file_handler);
}

void EMVBIndex::ReadIndexInner(FileHandler &file_handler) {
    std::unique_lock lock(rw_mutex_);
    // read index data
    {
        // check start_segment_offset_, embedding_dimension_, residual_pq_subspace_num_, residual_pq_subspace_bits_
        u32 tmp_u32 = 0;
        file_handler.Read(&tmp_u32, sizeof(tmp_u32));
        if (tmp_u32 != start_segment_offset_) {
            const auto error_msg =
                fmt::format("EMVBIndex::ReadIndexInner: start_segment_offset_ mismatch: expect {}, got {}.", start_segment_offset_, tmp_u32);
            UnrecoverableError(error_msg);
        }
        file_handler.Read(&tmp_u32, sizeof(tmp_u32));
        if (tmp_u32 != embedding_dimension_) {
            const auto error_msg =
                fmt::format("EMVBIndex::ReadIndexInner: embedding_dimension_ mismatch: expect {}, got {}.", embedding_dimension_, tmp_u32);
            UnrecoverableError(error_msg);
        }
        file_handler.Read(&tmp_u32, sizeof(tmp_u32));
        if (tmp_u32 != residual_pq_subspace_num_) {
            const auto error_msg =
                fmt::format("EMVBIndex::ReadIndexInner: residual_pq_subspace_num_ mismatch: expect {}, got {}.", residual_pq_subspace_num_, tmp_u32);
            UnrecoverableError(error_msg);
        }
        file_handler.Read(&tmp_u32, sizeof(tmp_u32));
        if (tmp_u32 != residual_pq_subspace_bits_) {
            const auto error_msg = fmt::format("EMVBIndex::ReadIndexInner: residual_pq_subspace_bits_ mismatch: expect {}, got {}.",
                                               residual_pq_subspace_bits_,
                                               tmp_u32);
            UnrecoverableError(error_msg);
        }
    }
    file_handler.Read(&n_centroids_, sizeof(n_centroids_));
    DeSerialize(file_handler, centroids_data_);
    DeSerialize(file_handler, centroid_norms_neg_half_);
    u32 n_docs = 0;
    file_handler.Read(&n_docs, sizeof(n_docs));
    n_docs_ = n_docs;
    file_handler.Read(&n_total_embeddings_, sizeof(n_total_embeddings_));
    DeSerialize(file_handler, doc_lens_, n_docs);
    DeSerialize(file_handler, doc_offsets_, n_docs);
    DeSerialize(file_handler, centroid_id_assignments_, n_total_embeddings_);
    centroids_to_docid_ = MakeUnique<EMVBSharedVec<u32>[]>(n_centroids_);
    for (u32 i = 0; i < n_centroids_; ++i) {
        DeSerialize(file_handler, centroids_to_docid_[i]);
    }
    // read product quantizer
    product_quantizer_->Load(file_handler);
}

EMVBIndex &EMVBIndex::operator=(EMVBIndex &&other) {
    std::unique_lock lock(rw_mutex_);
    // check const vals
    if (start_segment_offset_ != other.start_segment_offset_ || embedding_dimension_ != other.embedding_dimension_ ||
        residual_pq_subspace_num_ != other.residual_pq_subspace_num_ || residual_pq_subspace_bits_ != other.residual_pq_subspace_bits_) {
        const auto error_msg = "EMVBIndex::move assignment: const vals mismatch.";
        UnrecoverableError(error_msg);
    }
    // move data
#define MOVE_MEMBER(member) member = std::move(other.member)
    MOVE_MEMBER(n_centroids_);
    MOVE_MEMBER(centroids_data_);
    MOVE_MEMBER(centroid_norms_neg_half_);
    const u32 other_n_docs = other.n_docs_;
    n_docs_ = other_n_docs;
    MOVE_MEMBER(n_total_embeddings_);
    MOVE_MEMBER(doc_lens_);
    MOVE_MEMBER(doc_offsets_);
    MOVE_MEMBER(centroid_id_assignments_);
    MOVE_MEMBER(centroids_to_docid_);
    MOVE_MEMBER(product_quantizer_);
    return *this;
}

} // namespace infinity
