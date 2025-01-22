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

#include <cassert>
#include <cstdlib>
#include <tuple>
#include <vector>

module rank_features_doc_iterator;

import stl;
import third_party;
import index_defines;
import rank_feature_doc_iterator;
import multi_doc_iterator;
import internal_types;
import logger;
import infinity_exception;
import simd_functions;

namespace infinity {

constexpr u32 BATCH_OR_LEN = 128;

RankFeaturesDocIterator::RankFeaturesDocIterator(Vector<UniquePtr<DocIterator>> &&iterators) : MultiDocIterator(std::move(iterators)) {
    estimate_iterate_cost_ = {};
    const SizeT num_iterators = children_.size();
    for (SizeT i = 0; i < num_iterators; i++) {
        auto it = dynamic_cast<const RankFeatureDocIterator *>(children_[i].get());
        if (it == nullptr) {
            UnrecoverableError("RankFeaturesDocIterator only supports RankFeatureDocIterator");
        }
    }
    static_assert(sizeof(f32) == sizeof(u32));
    memset_bytes_ = sizeof(u32) * BATCH_OR_LEN * 2u * num_iterators;
    const auto alloc_bytes = sizeof(u32) * BATCH_OR_LEN * (2u + 2u * num_iterators);
    aligned_buffer_ = std::aligned_alloc(64, alloc_bytes);
    if (!aligned_buffer_) {
        UnrecoverableError(fmt::format("{}: Out of memory!", __func__));
    }
    match_cnt_ptr_ = static_cast<u32 *>(aligned_buffer_);
    payload_ptr_ = reinterpret_cast<u16 *>(match_cnt_ptr_ + BATCH_OR_LEN * num_iterators);
}

RankFeaturesDocIterator::~RankFeaturesDocIterator() { std::free(aligned_buffer_); }

bool RankFeaturesDocIterator::Next(RowID doc_id) {
    if (doc_id_ != INVALID_ROWID) [[likely]] {
        if (doc_id_ >= doc_id) {
            return true;
        }
        // now buffer_start_doc_id_ <= doc_id_ < doc_id
        if (u32 pos = doc_id - buffer_start_doc_id_; pos < BATCH_OR_LEN) {
            for (; pos < BATCH_OR_LEN; ++pos) {
                if (match_cnt_ptr_[pos]) {
                    doc_id_ = buffer_start_doc_id_ + pos;
                    return true;
                }
            }
            // now need to search from buffer_start_doc_id_ + BATCH_OR_LEN
            doc_id = buffer_start_doc_id_ + BATCH_OR_LEN;
        }
    } else {
        for (const auto &child : children_) {
            child->Next(doc_id);
        }
    }
    RowID next_buffer_start_doc_id = INVALID_ROWID;
    for (const auto &child : children_) {
        if (child->DocID() != INVALID_ROWID) {
            child->Next(doc_id);
            const RowID child_doc_id = child->DocID();
            next_buffer_start_doc_id = std::min(next_buffer_start_doc_id, child_doc_id);
        }
    }
    if (next_buffer_start_doc_id != INVALID_ROWID) [[likely]] {
        DecodeFrom(next_buffer_start_doc_id);
    }
    doc_id_ = next_buffer_start_doc_id;
    return doc_id_ != INVALID_ROWID;
}

u32 RankFeaturesDocIterator::MatchCount() const { return match_cnt_ptr_[doc_id_ - buffer_start_doc_id_]; }

void RankFeaturesDocIterator::DecodeFrom(const RowID buffer_start_doc_id) {
    buffer_start_doc_id_ = buffer_start_doc_id;
    std::memset(aligned_buffer_, 0, memset_bytes_);
    const auto buffer_end_doc_id = buffer_start_doc_id + BATCH_OR_LEN;
    for (u32 i = 0; i < children_.size(); ++i) {
        const auto child = children_[i].get();
        if (const auto child_doc_id = child->DocID(); child_doc_id != INVALID_ROWID) {
            assert(child_doc_id >= buffer_start_doc_id);
            if (child_doc_id >= buffer_end_doc_id) {
                // no need to decode
                continue;
            }
            const auto it = dynamic_cast<RankFeatureDocIterator *>(child);
            it->BatchDecodeTo(buffer_start_doc_id, buffer_end_doc_id, payload_ptr_ + i * BATCH_OR_LEN);
        }
    }
    for (u32 i = 0; i < BATCH_OR_LEN; ++i) {
        u32 match_cnt = 0;
        for (u32 j = 0; j < children_.size(); ++j) {
            const auto payload = payload_ptr_[j * BATCH_OR_LEN + i];
            if (payload == 0) {
                continue;
            }
            ++match_cnt;
        }
        match_cnt_ptr_[i] = match_cnt;
    }
}

} // namespace infinity
