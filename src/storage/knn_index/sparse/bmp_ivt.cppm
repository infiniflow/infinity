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

#include "common/simd/simd_common_intrin_include.h"

export module infinity_core:bmp_ivt;

import :stl;
import :sparse_util;
import :local_file_handle;
import :bmp_util;
import :knn_result_handler;

import std;

import serialize;

namespace infinity {

export template <typename DataType, BMPCompressType CompressType, BMPOwnMem OwnMem>
struct BlockData {};

export template <typename DataType>
struct BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue> {
private:
    BlockData(std::vector<BMPBlockID> block_ids, std::vector<DataType> max_scores) : block_ids_(std::move(block_ids)), max_scores_(std::move(max_scores)) {}

public:
    BlockData() = default;

    void AddBlock(BMPBlockID block_id, DataType max_score, size_t &mem_usage) {
        block_ids_.push_back(block_id);
        max_scores_.push_back(max_score);
        mem_usage += (sizeof(BMPBlockID) + sizeof(DataType));
    }

    void Prefetch() const {
        _mm_prefetch((const char *)block_ids_.data(), _MM_HINT_T0);
        _mm_prefetch((const char *)max_scores_.data(), _MM_HINT_T0);
    }

    size_t GetSizeInBytes() const { return sizeof(size_t) + block_ids_.size() * sizeof(BMPBlockID) + max_scores_.size() * sizeof(DataType); }

    void WriteAdv(char *&p) const {
        size_t max_score_size = max_scores_.size();
        WriteBufAdv<size_t>(p, max_score_size);
        WriteBufVecAdv(p, block_ids_.data(), block_ids_.size());
        WriteBufVecAdv(p, max_scores_.data(), max_scores_.size());
    }

    static BlockData ReadAdv(const char *&p) {
        BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue> res;
        size_t max_score_size = ReadBufAdv<size_t>(p);
        res.block_ids_.resize(max_score_size);
        res.max_scores_.resize(max_score_size);
        for (size_t i = 0; i < max_score_size; ++i) {
            res.block_ids_[i] = ReadBufAdv<BMPBlockID>(p);
        }
        for (size_t i = 0; i < max_score_size; ++i) {
            res.max_scores_[i] = ReadBufAdv<DataType>(p);
        }
        return res;
    }

    static void GetSizeToPtr(char *&p, const std::vector<const BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue> *> &block_data_list) {
        GetSizeInBytesVecAligned<size_t>(p, block_data_list.size() + 1);
        size_t num_sum = 0;
        for (const auto *block_data : block_data_list) {
            num_sum += block_data->block_ids_.size();
        }
        GetSizeInBytesVecAligned<BMPBlockID>(p, num_sum);
        GetSizeInBytesVecAligned<DataType>(p, num_sum);
    }

    static void WriteToPtr(char *&p, const std::vector<const BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kTrue> *> &block_data_list) {
        size_t num_sum = 0;
        std::vector<size_t> block_data_prefix_sum;
        block_data_prefix_sum.push_back(0);
        for (const auto *block_data : block_data_list) {
            num_sum += block_data->block_ids_.size();
            block_data_prefix_sum.push_back(num_sum);
        }
        WriteBufVecAdvAligned<size_t>(p, block_data_prefix_sum.data(), block_data_prefix_sum.size());
        for (const auto *block_data : block_data_list) {
            WriteBufVecAdvAligned<BMPBlockID>(p, block_data->block_ids_.data(), block_data->block_ids_.size());
        }
        for (const auto *block_data : block_data_list) {
            WriteBufVecAdvAligned<DataType>(p, block_data->max_scores_.data(), block_data->max_scores_.size());
        }
    }

    static std::vector<BlockData> ReadFromPtr(const char *&p, size_t posting_num) {
        std::vector<BlockData> res;
        const size_t *block_data_prefix_sum = ReadBufVecAdvAligned<size_t>(p, posting_num + 1);
        std::vector<const BMPBlockID *> block_ids_ptrs;
        for (size_t i = 0; i < posting_num; ++i) {
            size_t block_data_size = block_data_prefix_sum[i + 1] - block_data_prefix_sum[i];
            const BMPBlockID *block_ids_ptr = ReadBufVecAdvAligned<BMPBlockID>(p, block_data_size);
            block_ids_ptrs.push_back(block_ids_ptr);
        }
        for (size_t i = 0; i < posting_num; ++i) {
            size_t block_data_size = block_data_prefix_sum[i + 1] - block_data_prefix_sum[i];
            const DataType *max_scores_ptr = ReadBufVecAdvAligned<DataType>(p, block_data_size);
            std::vector<BMPBlockID> block_ids(block_ids_ptrs[i], block_ids_ptrs[i] + block_data_size);
            std::vector<DataType> max_scores(max_scores_ptr, max_scores_ptr + block_data_size);
            res.push_back(BlockData(std::move(block_ids), std::move(max_scores)));
        }
        return res;
    }

    size_t block_num() const { return block_ids_.size(); }
    const BMPBlockID *block_ids() const { return block_ids_.data(); }
    const DataType *max_scores() const { return max_scores_.data(); }

private:
    std::vector<BMPBlockID> block_ids_;
    std::vector<DataType> max_scores_;
};

export template <typename DataType>
struct BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kFalse> {
public:
    BlockData(size_t block_size, const BMPBlockID *block_ids, const DataType *max_scores)
        : block_size_(block_size), block_ids_(block_ids), max_scores_(max_scores) {}

    void Prefetch() const {
        _mm_prefetch((const char *)block_ids_, _MM_HINT_T0);
        _mm_prefetch((const char *)max_scores_, _MM_HINT_T0);
    }

    static std::tuple<const size_t *, const BMPBlockID *, const DataType *> ReadFromPtr(const char *&p, size_t posting_num) {
        const size_t *block_data_prefix_sum = ReadBufVecAdvAligned<size_t>(p, posting_num + 1);
        size_t block_data_num = block_data_prefix_sum[posting_num];
        const BMPBlockID *block_ids = ReadBufVecAdvAligned<BMPBlockID>(p, block_data_num);
        const DataType *max_scores = ReadBufVecAdvAligned<DataType>(p, block_data_num);
        return {block_data_prefix_sum, block_ids, max_scores};
    }

    size_t block_num() const { return block_size_; }
    const BMPBlockID *block_ids() const { return block_ids_; }
    const DataType *max_scores() const { return max_scores_; }

private:
    size_t block_size_;
    const BMPBlockID *block_ids_;
    const DataType *max_scores_;
};

export template <typename DataType>
struct BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kTrue> {
private:
    explicit BlockData(std::vector<DataType> max_scores) : max_scores_(std::move(max_scores)) {}

public:
    BlockData() = default;

    void AddBlock(BMPBlockID block_id, DataType max_score, size_t &mem_usage) {
        if (block_id >= (BMPBlockID)max_scores_.size()) {
            mem_usage += sizeof(BMPBlockID) * (block_id + 1 - max_scores_.size());
            max_scores_.resize(block_id + 1, 0.0);
        }
        max_scores_[block_id] = max_score;
    }

    void Prefetch() const { _mm_prefetch((const char *)max_scores_.data(), _MM_HINT_T0); }

    size_t GetSizeInBytes() const { return sizeof(size_t) + max_scores_.size() * sizeof(DataType); }

    void WriteAdv(char *&p) const {
        size_t max_score_size = max_scores_.size();
        WriteBufAdv<size_t>(p, max_score_size);
        WriteBufVecAdv(p, max_scores_.data(), max_scores_.size());
    }

    static BlockData ReadAdv(const char *&p) {
        BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kTrue> res;
        size_t max_score_size = ReadBufAdv<size_t>(p);
        res.max_scores_.resize(max_score_size);
        for (size_t i = 0; i < max_score_size; ++i) {
            res.max_scores_[i] = ReadBufAdv<DataType>(p);
        }
        return res;
    }

    static void GetSizeToPtr(char *&p, const std::vector<const BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kTrue> *> &block_data_list) {
        GetSizeInBytesVecAligned<size_t>(p, block_data_list.size() + 1);
        size_t num_sum = 0;
        for (const auto *block_data : block_data_list) {
            num_sum += block_data->max_scores_.size();
        }
        GetSizeInBytesVecAligned<DataType>(p, num_sum);
    }

    static void WriteToPtr(char *&p, const std::vector<const BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kTrue> *> &block_data_list) {
        size_t num_sum = 0;
        std::vector<size_t> block_data_prefix_sum;
        block_data_prefix_sum.push_back(0);
        for (const auto *block_data : block_data_list) {
            num_sum += block_data->max_scores_.size();
            block_data_prefix_sum.push_back(num_sum);
        }
        WriteBufVecAdvAligned<size_t>(p, block_data_prefix_sum.data(), block_data_prefix_sum.size());
        for (const auto *block_data : block_data_list) {
            WriteBufVecAdvAligned<DataType>(p, block_data->max_scores_.data(), block_data->max_scores_.size());
        }
    }

    static std::vector<BlockData> ReadFromPtr(const char *&p, size_t posting_num) {
        std::vector<BlockData> res;
        const size_t *block_data_prefix_sum = ReadBufVecAdvAligned<size_t>(p, posting_num + 1);
        for (size_t i = 0; i < posting_num; ++i) {
            size_t block_data_size = block_data_prefix_sum[i + 1] - block_data_prefix_sum[i];
            const DataType *max_scores_ptr = ReadBufVecAdvAligned<DataType>(p, block_data_size);
            std::vector<DataType> max_scores(max_scores_ptr, max_scores_ptr + block_data_size);
            res.push_back(BlockData(std::move(max_scores)));
        }
        return res;
    }

    size_t block_num() const { return max_scores_.size(); }
    const DataType *max_scores() const { return max_scores_.data(); }

public:
    std::vector<DataType> max_scores_;
};

export template <typename DataType>
struct BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kFalse> {
public:
    BlockData(size_t block_size, const DataType *max_scores) : block_size_(block_size), max_scores_(max_scores) {}

    void Prefetch() const { _mm_prefetch((const char *)max_scores_, _MM_HINT_T0); }

    static std::tuple<const size_t *, const DataType *> ReadFromPtr(const char *&p, size_t posting_num) {
        const size_t *block_data_prefix_sum = ReadBufVecAdvAligned<size_t>(p, posting_num + 1);
        size_t block_data_num = block_data_prefix_sum[posting_num];
        const DataType *max_scores = ReadBufVecAdvAligned<DataType>(p, block_data_num);
        return {block_data_prefix_sum, max_scores};
    }

    size_t block_num() const { return block_size_; }
    const DataType *max_scores() const { return max_scores_; }

private:
    size_t block_size_;
    const DataType *max_scores_;
};

template <typename DataType, BMPCompressType CompressType, BMPOwnMem OwnMem>
struct BlockPostingsBase {
public:
    BlockPostingsBase() = default;
    BlockPostingsBase(i32 kth, DataType kth_score, BlockData<DataType, CompressType, OwnMem> data)
        : kth_(kth), kth_score_(kth_score), data_(std::move(data)) {}

    DataType kth(i32 topk) const { return topk == kth_ ? kth_score_ : 0.0; }

    const BlockData<DataType, CompressType, OwnMem> &data() const { return data_; }
    BlockData<DataType, CompressType, OwnMem> &data() { return data_; }

    void SetKth(i32 kth, DataType kth_score) {
        kth_ = kth;
        kth_score_ = kth_score;
    }

    i32 kth_ = -1;
    DataType kth_score_ = 0.0;

protected:
    BlockData<DataType, CompressType, OwnMem> data_;
};

template <typename DataType, BMPCompressType CompressType, BMPOwnMem OwnMem>
struct BlockPostings {};

template <typename DataType, BMPCompressType CompressType>
struct BlockPostings<DataType, CompressType, BMPOwnMem::kTrue> : public BlockPostingsBase<DataType, CompressType, BMPOwnMem::kTrue> {
    using BlockData = BlockData<DataType, CompressType, BMPOwnMem::kTrue>;

    BlockPostings(i32 kth, DataType kth_score, BlockData data)
        : BlockPostingsBase<DataType, CompressType, BMPOwnMem::kTrue>(kth, kth_score, std::move(data)) {}

public:
    BlockPostings() = default;

    size_t GetSizeInBytes() const { return sizeof(this->kth_) + sizeof(this->kth_score_) + this->data_.GetSizeInBytes(); }

    void WriteAdv(char *&p) const {
        WriteBufAdv<i32>(p, this->kth_);
        WriteBufAdv<DataType>(p, this->kth_score_);
        this->data_.WriteAdv(p);
    }

    static BlockPostings ReadAdv(const char *&p) {
        BlockPostings res;
        res.kth_ = ReadBufAdv<i32>(p);
        res.kth_score_ = ReadBufAdv<DataType>(p);
        res.data_ = BlockData::ReadAdv(p);
        return res;
    }

    static void GetSizeToPtr(char *&p, const std::vector<BlockPostings> &postings) {
        GetSizeInBytesAligned<size_t>(p);
        GetSizeInBytesVecAligned<i32>(p, postings.size());
        GetSizeInBytesVecAligned<DataType>(p, postings.size());
        std::vector<const BlockData *> block_data_list;
        for (const auto &posting : postings) {
            block_data_list.push_back(&posting.data_);
        }
        BlockData::GetSizeToPtr(p, block_data_list);
    }

    static void WriteToPtr(char *&p, const std::vector<BlockPostings> &postings) {
        WriteBufAdvAligned<size_t>(p, postings.size());
        std::vector<i32> kths;
        std::vector<DataType> kth_scores;
        for (const auto &posting : postings) {
            kths.push_back(posting.kth_);
            kth_scores.push_back(posting.kth_score_);
        }
        WriteBufVecAdvAligned<i32>(p, kths.data(), kths.size());
        WriteBufVecAdvAligned<DataType>(p, kth_scores.data(), kth_scores.size());
        std::vector<const BlockData *> block_data_list;
        for (const auto &posting : postings) {
            block_data_list.push_back(&posting.data_);
        }
        BlockData::WriteToPtr(p, block_data_list);
    }

    static std::vector<BlockPostings> ReadFromPtr(const char *&p) {
        std::vector<BlockPostings> res;
        size_t posting_num = ReadBufAdvAligned<size_t>(p);
        const i32 *kths = ReadBufVecAdvAligned<i32>(p, posting_num);
        const DataType *kth_scores = ReadBufVecAdvAligned<DataType>(p, posting_num);
        std::vector<BlockData> block_data_list = BlockData::ReadFromPtr(p, posting_num);
        for (size_t i = 0; i < posting_num; ++i) {
            res.emplace_back(kths[i], kth_scores[i], std::move(block_data_list[i]));
        }
        return res;
    }
};

template <typename DataType, BMPCompressType CompressType>
struct BlockPostings<DataType, CompressType, BMPOwnMem::kFalse> : public BlockPostingsBase<DataType, CompressType, BMPOwnMem::kFalse> {
public:
    static std::tuple<size_t, const i32 *, const DataType *> ReadFromPtr(const char *&p) {
        size_t posting_num = ReadBufAdvAligned<size_t>(p);
        const i32 *kths = ReadBufVecAdvAligned<i32>(p, posting_num);
        const DataType *kth_scores = ReadBufVecAdvAligned<DataType>(p, posting_num);
        return {posting_num, kths, kth_scores};
    }

    BlockPostings(i32 kth, DataType kth_score, BlockData<DataType, CompressType, BMPOwnMem::kFalse> data)
        : BlockPostingsBase<DataType, CompressType, BMPOwnMem::kFalse>(kth, kth_score, std::move(data)) {}
};

export template <typename DataType, BMPCompressType CompressType, BMPOwnMem OwnMem>
class BMPIvt {};

export template <typename DataType, BMPCompressType CompressType>
class BMPIvt<DataType, CompressType, BMPOwnMem::kTrue> {
private:
    using BlockPostings = BlockPostings<DataType, CompressType, BMPOwnMem::kTrue>;

    BMPIvt(std::vector<BlockPostings> postings) : postings_(std::move(postings)) {}

public:
    BMPIvt() {}
    BMPIvt(size_t term_num) : postings_(term_num) {}

    template <typename IdxType>
    void AddBlock(BMPBlockID block_id, const std::vector<std::pair<std::vector<IdxType>, std::vector<DataType>>> &tail_terms, size_t &mem_usage) {
        std::unordered_map<IdxType, DataType> max_scores;
        for (const auto &[indices, data] : tail_terms) {
            size_t nnz = indices.size();
            for (size_t i = 0; i < nnz; ++i) {
                IdxType term_id = indices[i];
                DataType score = data[i];
                max_scores[term_id] = std::max(max_scores[term_id], score);
            }
        }
        for (const auto &[term_id, score] : max_scores) {
            postings_[term_id].data().AddBlock(block_id, score, mem_usage);
        }
    }

    void Optimize(i32 topk, std::vector<std::vector<DataType>> ivt_scores) {
        for (size_t term_id = 0; term_id < ivt_scores.size(); ++term_id) {
            auto &posting = postings_[term_id];
            auto &term_scores = ivt_scores[term_id];
            if ((i32)term_scores.size() < topk) {
                posting.SetKth(topk, 0.0);
                continue;
            }
            std::nth_element(term_scores.begin(), term_scores.begin() + topk - 1, term_scores.end(), std::greater<>());
            posting.SetKth(topk, term_scores[topk - 1]);
        }
    }

    const BlockPostings &GetPostings(size_t term_id) const { return postings_[term_id]; }

    void Prefetch(size_t term_id) const { postings_[term_id].data().Prefetch(); }

    size_t term_num() const { return postings_.size(); }

    size_t GetSizeInBytes() const {
        size_t size = sizeof(size_t);
        for (const auto &posting : postings_) {
            size += posting.GetSizeInBytes();
        }
        return size;
    }

    void WriteAdv(char *&p) const {
        size_t posting_size = postings_.size();
        WriteBufAdv<size_t>(p, posting_size);
        for (const auto &posting : postings_) {
            posting.WriteAdv(p);
        }
    }

    static BMPIvt ReadAdv(const char *&p) {
        size_t posting_size = ReadBufAdv<size_t>(p);
        std::vector<BlockPostings> postings(posting_size);
        for (size_t i = 0; i < posting_size; ++i) {
            postings[i] = BlockPostings::ReadAdv(p);
        }
        return BMPIvt(std::move(postings));
    }

    void GetSizeToPtr(char *&p) const { BlockPostings::GetSizeToPtr(p, postings_); }

    void WriteToPtr(char *&p) const { BlockPostings::WriteToPtr(p, postings_); }

    static BMPIvt ReadFromPtr(const char *&p) { return BMPIvt(BlockPostings::ReadFromPtr(p)); }

private:
    std::vector<BlockPostings> postings_;
};

template <typename DataType>
class BMPIvtBase {
protected:
    size_t posting_size_ = 0;
    const i32 *kth_ = nullptr;
    const DataType *kth_score_ = nullptr;
    const size_t *block_data_prefix_sum_ = nullptr;
};

export template <typename DataType>
class BMPIvt<DataType, BMPCompressType::kCompressed, BMPOwnMem::kFalse> : public BMPIvtBase<DataType> {
    using BlockPostings = BlockPostings<DataType, BMPCompressType::kCompressed, BMPOwnMem::kFalse>;
    using BlockData = BlockData<DataType, BMPCompressType::kCompressed, BMPOwnMem::kFalse>;

public:
    BMPIvt() {}

    BlockPostings GetPostings(size_t term_id) const {
        i32 kth = this->kth_[term_id];
        DataType kth_score = this->kth_score_[term_id];
        size_t block_data_size = this->block_data_prefix_sum_[term_id + 1] - this->block_data_prefix_sum_[term_id];
        const BMPBlockID *block_ids = block_ids_ + this->block_data_prefix_sum_[term_id];
        const DataType *max_scores = max_scores_ + this->block_data_prefix_sum_[term_id];
        return BlockPostings(kth, kth_score, BlockData(block_data_size, block_ids, max_scores));
    }

    void Prefetch(size_t term_id) const {
        _mm_prefetch((const char *)(block_ids_ + this->block_data_prefix_sum_[term_id]), _MM_HINT_T0);
        _mm_prefetch((const char *)(max_scores_ + this->block_data_prefix_sum_[term_id]), _MM_HINT_T0);
    }

    static BMPIvt ReadFromPtr(const char *&p) {
        BMPIvt ret{};
        std::tie(ret.posting_size_, ret.kth_, ret.kth_score_) = BlockPostings::ReadFromPtr(p);
        std::tie(ret.block_data_prefix_sum_, ret.block_ids_, ret.max_scores_) = BlockData::ReadFromPtr(p, ret.posting_size_);
        return ret;
    }

private:
    const BMPBlockID *block_ids_ = nullptr;
    const DataType *max_scores_ = nullptr;
};

export template <typename DataType>
class BMPIvt<DataType, BMPCompressType::kRaw, BMPOwnMem::kFalse> : public BMPIvtBase<DataType> {
public:
    BMPIvt() {}

    BlockPostings<DataType, BMPCompressType::kRaw, BMPOwnMem::kFalse> GetPostings(size_t term_id) const {
        i32 kth = this->kth_[term_id];
        DataType kth_score = this->kth_score_[term_id];
        using BlockData = BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kFalse>;
        size_t block_data_size = this->block_data_prefix_sum_[term_id + 1] - this->block_data_prefix_sum_[term_id];
        const DataType *max_scores = max_scores_ + this->block_data_prefix_sum_[term_id];
        return BlockPostings<DataType, BMPCompressType::kRaw, BMPOwnMem::kFalse>(kth, kth_score, BlockData(block_data_size, max_scores));
    }

    void Prefetch(size_t term_id) const { _mm_prefetch((const char *)(max_scores_ + this->block_data_prefix_sum_[term_id]), _MM_HINT_T0); }

    static BMPIvt ReadFromPtr(const char *&p) {
        BMPIvt ret{};
        std::tie(ret.posting_size_, ret.kth_, ret.kth_score_) = BlockPostings<DataType, BMPCompressType::kRaw, BMPOwnMem::kFalse>::ReadFromPtr(p);
        std::tie(ret.block_data_prefix_sum_, ret.max_scores_) =
            BlockData<DataType, BMPCompressType::kRaw, BMPOwnMem::kFalse>::ReadFromPtr(p, ret.posting_size_);
        return ret;
    }

private:
    const DataType *max_scores_ = nullptr;
};

} // namespace infinity