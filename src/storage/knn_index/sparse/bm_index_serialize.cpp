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
#include <xmmintrin.h>

module bm_index;

import infinity_exception;
import third_party;
import knn_result_handler;
import serialize;

namespace infinity {

// --------------------------PostingList--------------------------

SizeT PostingList::GetSizeInBytes() const {
    SizeT size = sizeof(kth_) + sizeof(kth_score_);
    size += sizeof(SizeT) + max_scores_.size() * (sizeof(i32) + sizeof(f32));
    return size;
}

void PostingList::WriteAdv(char *&p) const {
    WriteBufAdv<i32>(p, kth_);
    WriteBufAdv<f32>(p, kth_score_);
    SizeT max_score_size = max_scores_.size();
    WriteBufAdv<SizeT>(p, max_score_size);
    WriteBufCharsAdv(p, reinterpret_cast<const char *>(block_ids_.data()), sizeof(i32) * block_ids_.size());
    WriteBufCharsAdv(p, reinterpret_cast<const char *>(max_scores_.data()), sizeof(f32) * max_scores_.size());
}

PostingList PostingList::ReadAdv(char *&p) {
    PostingList res;
    res.kth_ = ReadBufAdv<i32>(p);
    res.kth_score_ = ReadBufAdv<f32>(p);
    SizeT max_score_size = ReadBufAdv<SizeT>(p);
    res.block_ids_.resize(max_score_size);
    res.max_scores_.resize(max_score_size);
    for (SizeT i = 0; i < max_score_size; ++i) {
        res.block_ids_[i] = ReadBufAdv<i32>(p);
    }
    for (SizeT i = 0; i < max_score_size; ++i) {
        res.max_scores_[i] = ReadBufAdv<f32>(p);
    }
    return res;
}

// --------------------------BMIvt--------------------------

SizeT BMIvt::GetSizeInBytes() const {
    SizeT size = sizeof(SizeT);
    for (const auto &posting : postings_) {
        size += posting.GetSizeInBytes();
    }
    return size;
}

void BMIvt::WriteAdv(char *&p) const {
    SizeT posting_size = postings_.size();
    WriteBufAdv<SizeT>(p, posting_size);
    for (const auto &posting : postings_) {
        posting.WriteAdv(p);
    }
}

BMIvt BMIvt::ReadAdv(char *&p) {
    SizeT posting_size = ReadBufAdv<SizeT>(p);
    Vector<PostingList> postings(posting_size);
    for (SizeT i = 0; i < posting_size; ++i) {
        postings[i] = PostingList::ReadAdv(p);
    }
    return BMIvt(std::move(postings));
}

// --------------------------TailFwd--------------------------

SizeT TailFwd::GetSizeInBytes() const {
    SizeT size = sizeof(SizeT);
    for (const auto &tail_terms : tail_terms_) {
        size += sizeof(SizeT) + tail_terms.size() * (sizeof(i32) + sizeof(f32));
    }
    return size;
}

void TailFwd::WriteAdv(char *&p) const {
    SizeT tail_num = tail_terms_.size();
    WriteBufAdv<SizeT>(p, tail_num);
    for (const auto &tail_terms : tail_terms_) {
        SizeT term_num = tail_terms.size();
        WriteBufAdv<SizeT>(p, term_num);
        for (const auto &[term_id, score] : tail_terms) {
            WriteBufAdv<i32>(p, term_id);
            WriteBufAdv<f32>(p, score);
        }
    }
}

TailFwd TailFwd::ReadAdv(char *&p) {
    SizeT tail_num = ReadBufAdv<SizeT>(p);
    Vector<Vector<Pair<i32, f32>>> tail_terms(tail_num);
    for (SizeT i = 0; i < tail_num; ++i) {
        SizeT term_num = ReadBufAdv<SizeT>(p);
        tail_terms[i].resize(term_num);
        for (SizeT j = 0; j < term_num; ++j) {
            tail_terms[i][j].first = ReadBufAdv<i32>(p);
            tail_terms[i][j].second = ReadBufAdv<f32>(p);
        }
    }
    return TailFwd(std::move(tail_terms));
}

// --------------------------BlockFwd--------------------------

SizeT BlockFwd::GetSizeInBytes() const {
    SizeT size = sizeof(block_size_) + sizeof(SizeT);
    for (const auto &block_terms : block_terms_) {
        size += sizeof(SizeT);
        for (const auto &[term_id, block_offsets, scores] : block_terms) {
            size += sizeof(i32) + sizeof(SizeT) + block_offsets.size() * sizeof(i8) + scores.size() * sizeof(f32);
        }
    }
    size += tail_fwd_.GetSizeInBytes();
    return size;
}

void BlockFwd::WriteAdv(char *&p) const {
    WriteBufAdv<i8>(p, block_size_);
    SizeT block_num = block_terms_.size();
    WriteBufAdv<SizeT>(p, block_num);
    for (const auto &block_terms : block_terms_) {
        SizeT term_num = block_terms.size();
        WriteBufAdv<SizeT>(p, term_num);
        for (const auto &[term_id, block_offsets, scores] : block_terms) {
            WriteBufAdv<i32>(p, term_id);
            SizeT inner_num = block_offsets.size();
            WriteBufAdv<SizeT>(p, inner_num);
            WriteBufCharsAdv(p, reinterpret_cast<const char *>(block_offsets.data()), sizeof(i8) * block_offsets.size());
            WriteBufCharsAdv(p, reinterpret_cast<const char *>(scores.data()), sizeof(f32) * scores.size());
        }
    }
    tail_fwd_.WriteAdv(p);
}

BlockFwd BlockFwd::ReadAdv(char *&p) {
    i32 block_size = ReadBufAdv<i8>(p);
    SizeT block_num = ReadBufAdv<SizeT>(p);
    Vector<Vector<Tuple<i32, Vector<i8>, Vector<f32>>>> block_terms(block_num);
    for (SizeT i = 0; i < block_num; ++i) {
        SizeT term_num = ReadBufAdv<SizeT>(p);
        block_terms[i].resize(term_num);
        for (SizeT j = 0; j < term_num; ++j) {
            i32 term_id = ReadBufAdv<i32>(p);
            SizeT inner_num = ReadBufAdv<SizeT>(p);
            Vector<i8> block_offsets(inner_num);
            Vector<f32> scores(inner_num);
            for (SizeT k = 0; k < inner_num; ++k) {
                block_offsets[k] = ReadBufAdv<i8>(p);
            }
            for (SizeT k = 0; k < inner_num; ++k) {
                scores[k] = ReadBufAdv<f32>(p);
            }
            block_terms[i][j] = {term_id, std::move(block_offsets), std::move(scores)};
        }
    }
    TailFwd tail_fwd = TailFwd::ReadAdv(p);
    return BlockFwd(block_size, std::move(block_terms), std::move(tail_fwd));
}

// --------------------------BMIndex--------------------------

void BMIndex::Save(FileHandler &file_handler) const {
    SizeT size = GetSizeInBytes();
    auto buffer = MakeUnique<char[]>(sizeof(size) + size);
    char *p = buffer.get();
    WriteBufAdv<SizeT>(p, size);
    WriteAdv(p);
    if (SizeT write_n = p - buffer.get(); write_n != sizeof(size) + size) {
        UnrecoverableError(fmt::format("BMIndex::Save: write_n != sizeof(size) + size: {} != {}", write_n, sizeof(size) + size));
    }
    file_handler.Write(buffer.get(), sizeof(size) + size);
}

BMIndex BMIndex::Load(FileHandler &file_handler) {
    SizeT size;
    file_handler.Read(&size, sizeof(size));
    auto buffer = MakeUnique<char[]>(size);
    file_handler.Read(buffer.get(), size);
    char *p = buffer.get();
    return ReadAdv(p);
}

SizeT BMIndex::GetSizeInBytes() const {
    SizeT size = 0;
    size += bm_ivt_.GetSizeInBytes();
    size += block_fwd_.GetSizeInBytes();
    return size;
}

void BMIndex::WriteAdv(char *&p) const {
    bm_ivt_.WriteAdv(p);
    block_fwd_.WriteAdv(p);
}

BMIndex BMIndex::ReadAdv(char *&p) {
    BMIvt postings = BMIvt::ReadAdv(p);
    BlockFwd block_fwd = BlockFwd::ReadAdv(p);
    return BMIndex(std::move(postings), std::move(block_fwd));
}

} // namespace infinity