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

export module infinity_core:batch_or_iterator;

import :stl;
import :index_defines;
import :doc_iterator;
import :multi_doc_iterator;
import internal_types;

namespace infinity {

export class BatchOrIterator : public MultiDocIterator {
public:
    explicit BatchOrIterator(Vector<UniquePtr<DocIterator>> &&iterators);

    ~BatchOrIterator() override;

    DocIteratorType GetType() const override { return DocIteratorType::kBatchOrIterator; }

    String Name() const override { return "BatchOrIterator"; }

    void UpdateScoreThreshold(float threshold) override {}

    bool Next(RowID doc_id) override;

    float BM25Score() const;

    float Score() override { return BM25Score(); }

    u32 MatchCount() const override;

    void DecodeFrom(RowID buffer_start_doc_id);

private:
    // Dynamic batch sizing based on keyword count
    u32 CalculateOptimalBatchSize(u32 num_keywords) const;

    Vector<f32> f1_;
    Vector<f32> f2_;
    Vector<f32> bm25_common_score_;
    RowID buffer_start_doc_id_ = INVALID_ROWID;
    u32 batch_size_ = 128; // Dynamic batch size instead of fixed BATCH_OR_LEN
    u32 memset_bytes_ = 0;
    void *aligned_buffer_ = nullptr;
    u32 *tf_ptr_ = nullptr;
    u32 *doc_len_ptr_ = nullptr;
    u32 *match_cnt_ptr_ = nullptr;
    f32 *bm25_score_sum_ptr_ = nullptr;
};

} // namespace infinity
