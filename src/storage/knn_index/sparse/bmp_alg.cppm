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

export module bmp_alg;

import stl;
import sparse_util;
import file_system;
import bm_posting;
import bmp_util;
import hnsw_common;

namespace infinity {

template <typename DataType, BMPCompressType CompressType>
class BMPIvt {
private:
    BMPIvt(Vector<BlockPostings<DataType, CompressType>> postings) : postings_(std::move(postings)) {}

public:
    BMPIvt(SizeT term_num) : postings_(term_num) {}

    template <typename IdxType>
    void AddBlock(BMPBlockID block_id, const Vector<Vector<Pair<IdxType, DataType>>> &tail_terms);

    void Optimize(i32 topk, Vector<Vector<DataType>> ivt_scores);

    const Vector<BlockPostings<DataType, CompressType>> &GetPostings() const { return postings_; }

    SizeT term_num() const { return postings_.size(); }

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static BMPIvt ReadAdv(char *&p);

private:
    Vector<BlockPostings<DataType, CompressType>> postings_;
};

template <typename DataType, typename IdxType>
class TailFwd {
private:
    TailFwd(Vector<Vector<Pair<IdxType, DataType>>> tail_terms) : tail_terms_(std::move(tail_terms)) {}

public:
    TailFwd() = default;

    SizeT AddDoc(const SparseVecRef<DataType, IdxType> &doc);

    const Vector<Vector<Pair<IdxType, DataType>>> &GetTailTerms() const { return tail_terms_; }

    Vector<Tuple<IdxType, Vector<BMPBlockOffset>, Vector<DataType>>> ToBlockFwd() const;

    Vector<DataType> GetScores(const SparseVecRef<DataType, IdxType> &query) const;

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static TailFwd<DataType, IdxType> ReadAdv(char *&p);

private:
    Vector<Vector<Pair<IdxType, DataType>>> tail_terms_;
};

template <typename DataType, typename IdxType>
class BlockFwd {
private:
    BlockFwd(SizeT block_size,
             Vector<Vector<Tuple<IdxType, Vector<BMPBlockOffset>, Vector<DataType>>>> block_terms,
             TailFwd<DataType, IdxType> tail_fwd)
        : block_size_(block_size), block_terms_(std::move(block_terms)), tail_fwd_(std::move(tail_fwd)) {}

public:
    BlockFwd(SizeT block_size) : block_size_(block_size) {}

    Optional<TailFwd<DataType, IdxType>> AddDoc(const SparseVecRef<DataType, IdxType> &doc);

    Vector<Vector<DataType>> GetIvtScores(SizeT term_num) const;

    Vector<DataType> GetScores(BMPBlockID block_id, const SparseVecRef<DataType, IdxType> &query) const;

    Vector<DataType> GetScoresTail(const SparseVecRef<DataType, IdxType> &query) const { return tail_fwd_.GetScores(query); }

    void Prefetch(BMPBlockID block_id) const;

    SizeT block_size() const { return block_size_; }

    SizeT block_num() const { return block_terms_.size(); }

    SizeT GetSizeInBytes() const;
    void WriteAdv(char *&p) const;
    static BlockFwd ReadAdv(char *&p);

private:
    static void Calculate(const Vector<BMPBlockOffset> &block_offsets, const Vector<DataType> scores, Vector<DataType> &res, DataType query_score);

private:
    SizeT block_size_;
    Vector<Vector<Tuple<IdxType, Vector<BMPBlockOffset>, Vector<DataType>>>> block_terms_;
    TailFwd<DataType, IdxType> tail_fwd_;
};

export template <typename DataType, typename IdxType, BMPCompressType CompressType>
class BMPAlg {
public:
    using DataT = DataType;
    using IdxT = IdxType;

private:
    BMPAlg(BMPIvt<DataType, CompressType> bm_ivt, BlockFwd<DataType, IdxType> block_fwd, Vector<BMPDocID> doc_ids)
        : bm_ivt_(std::move(bm_ivt)), block_fwd_(std::move(block_fwd)), doc_ids_(std::move(doc_ids)) {}

public:
    BMPAlg(SizeT term_num, SizeT block_size) : bm_ivt_(term_num), block_fwd_(block_size) {}

    void AddDoc(const SparseVecRef<DataType, IdxType> &doc, BMPDocID doc_id);

    template <DataIteratorConcept<SparseVecRef<DataType, IdxType>, BMPDocID> Iterator>
    SizeT AddDocs(Iterator iter);

    void Optimize(i32 topk);

    Pair<Vector<BMPDocID>, Vector<DataType>>
    SearchKnn(const SparseVecRef<DataType, IdxType> &query, i32 topk, const BmpSearchOptions &options) const;

    void Save(FileHandler &file_handler) const;

    static BMPAlg<DataType, IdxType, CompressType> Load(FileHandler &file_handler);

private:
    SizeT GetSizeInBytes() const;

    void WriteAdv(char *&p) const;

    static BMPAlg<DataType, IdxType, CompressType> ReadAdv(char *&p);

private:
    BMPIvt<DataType, CompressType> bm_ivt_;
    BlockFwd<DataType, IdxType> block_fwd_;
    Vector<BMPDocID> doc_ids_;

    mutable std::shared_mutex mtx_;
};

template <typename DataType, typename IdxType, BMPCompressType CompressType>
template <DataIteratorConcept<SparseVecRef<DataType, IdxType>, BMPDocID> Iterator>
SizeT BMPAlg<DataType, IdxType, CompressType>::AddDocs(Iterator iter) {
    SizeT cnt = 0;
    while (true) {
        auto ret = iter.Next();
        if (!ret.has_value()) {
            break;
        }
        const auto &[sparse_ref, doc_id] = *ret;
        AddDoc(sparse_ref, doc_id);
        ++cnt;
    }
    return cnt;
}

} // namespace infinity
