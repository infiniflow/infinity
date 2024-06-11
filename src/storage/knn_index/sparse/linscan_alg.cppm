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
#include <vector>

export module linscan_alg;

import stl;
import sparse_util;
import file_system;
import knn_result_handler;
import serialize;
import infinity_exception;
import third_party;

namespace infinity {

template <typename DataType>
struct Posting {
    u32 doc_id_;
    DataType val_;

    void WriteAdv(char *&p) const {
        WriteBufAdv<u32>(p, doc_id_);
        WriteBufAdv<DataType>(p, val_);
    }

    static Posting<DataType> ReadAdv(char *&p) {
        Posting<DataType> res;
        res.doc_id_ = ReadBufAdv<u32>(p);
        res.val_ = ReadBufAdv<DataType>(p);
        return res;
    }

    static SizeT GetSizeInBytes() { return sizeof(doc_id_) + sizeof(val_); }
};

export template <typename DataType, typename IdxType>
class LinScan {
public:
    void Insert(const SparseVecRef<DataType, IdxType> &vec, u32 doc_id) {
        for (i32 i = 0; i < vec.nnz_; ++i) {
            u32 indice = vec.indices_[i];
            DataType val = vec.data_[i];
            if (val == 0.0) {
                continue;
            }
            Posting<DataType> posting{doc_id, val};
            inverted_idx_[indice].push_back(posting);
        }
        ++row_num_;
    }

    Pair<Vector<IdxType>, Vector<DataType>> SearchBF(const SparseVecRef<DataType, IdxType> &query, u32 top_k) const {
        u32 result_n = std::min(top_k, row_num_);

        HashMap<u32, DataType> scores;
        for (i32 i = 0; i < query.nnz_; ++i) {
            u32 indice = query.indices_[i];
            DataType val = query.data_[i];

            auto it = inverted_idx_.find(indice);
            if (it == inverted_idx_.end()) {
                continue;
            }
            const Vector<Posting<DataType>> &posting_list = it->second;
            for (const auto &posting : posting_list) {
                scores[posting.doc_id_] += val * posting.val_;
            }
        }

        Vector<IdxType> res(result_n);
        Vector<DataType> res_score(result_n);
        HeapResultHandler<CompareMin<DataType, IdxType>> result_handler(1 /*query_n*/, result_n, res_score.data(), res.data());
        for (const auto &[row_id, score] : scores) {
            if (score < 0) {
                continue;
            }
            result_handler.AddResult(0 /*query_id*/, score, row_id);
        }
        result_handler.End(0 /*query_id*/);
        return {res, res_score};
    }

    Tuple<Vector<IdxType>, Vector<DataType>, i32> SearchKnn(const SparseVecRef<DataType, IdxType> &query, u32 top_k, i32 budget) const {
        if (budget <= 0) {
            return {};
        }
        u32 result_n = std::min(top_k, row_num_);

        Vector<u32> query_vec_idx(query.nnz_);
        std::iota(query_vec_idx.begin(), query_vec_idx.end(), 0);
        std::sort(query_vec_idx.begin(), query_vec_idx.end(), [&query](u32 i1, u32 i2) {
            return std::abs(query.data_[i1]) > std::abs(query.data_[i2]);
        });

        HashMap<u32, DataType> scores;
        i32 cur_budget = 0;
        for (i32 budget_i = 0; cur_budget < budget && budget_i < query.nnz_; ++budget_i) {
            u32 indice = query.indices_[query_vec_idx[budget_i]];
            DataType val = query.data_[query_vec_idx[budget_i]];

            auto it = inverted_idx_.find(indice);
            if (it == inverted_idx_.end()) {
                continue;
            }
            const Vector<Posting<DataType>> &posting_list = it->second;
            cur_budget += posting_list.size();
            for (const auto &posting : posting_list) {
                scores[posting.doc_id_] += val * posting.val_;
            }
        }

        Vector<IdxType> result(result_n);
        Vector<DataType> result_score(result_n);
        HeapResultHandler<CompareMin<DataType, IdxType>> result_handler(1 /*query_n*/, result_n, result_score.data(), result.data());
        for (const auto &[row_id, score] : scores) {
            if (score < 0) {
                continue;
            }
            result_handler.AddResult(0 /*query_id*/, score, row_id);
        }
        result_handler.EndWithoutSort(0 /*query_id*/);
        return {result, result_score, cur_budget};
    }

    u32 row_num() const { return row_num_; }

    void Save(FileHandler &file_handler) const {
        SizeT bytes = GetSizeInBytes();
        auto buffer = MakeUnique<char[]>(sizeof(bytes) + bytes);
        char *p = buffer.get();
        WriteBufAdv<SizeT>(p, bytes);
        WriteAdv(p);
        file_handler.Write(buffer.get(), sizeof(bytes) + bytes);
    }

    static LinScan Load(FileHandler &file_handler) {
        LinScan linscan;
        SizeT bytes;
        file_handler.Read(&bytes, sizeof(bytes));
        auto buffer = MakeUnique<char[]>(bytes);
        file_handler.Read(buffer.get(), bytes);
        char *buffer_ptr = buffer.get();
        return ReadAdv(buffer_ptr);
    }

private:
    void WriteAdv(char *&p) const {
        WriteBufAdv<u32>(p, row_num_);
        SizeT inverted_idx_size = inverted_idx_.size();
        WriteBufAdv<SizeT>(p, inverted_idx_size);
        for (const auto &[indice, posting_list] : inverted_idx_) {
            WriteBufAdv<u32>(p, indice);
            SizeT posting_size = posting_list.size();
            WriteBufAdv<SizeT>(p, posting_size);
            for (const auto &posting : posting_list) {
                posting.WriteAdv(p);
            }
        }
    }

    static LinScan ReadAdv(char *&p) {
        LinScan res;
        res.row_num_ = ReadBufAdv<u32>(p);
        SizeT inverted_idx_size = ReadBufAdv<SizeT>(p);
        for (SizeT i = 0; i < inverted_idx_size; ++i) {
            u32 indice = ReadBufAdv<u32>(p);
            SizeT posting_size = ReadBufAdv<SizeT>(p);
            Vector<Posting<DataType>> posting_list(posting_size);
            for (SizeT j = 0; j < posting_size; ++j) {
                posting_list[j] = Posting<DataType>::ReadAdv(p);
                if (j > 0 && posting_list[j].doc_id_ <= posting_list[j - 1].doc_id_) {
                    UnrecoverableError("Duplicate doc_id in posting list");
                }
            }
            auto [iter, insert_ok] = res.inverted_idx_.emplace(indice, std::move(posting_list));
            if (!insert_ok) {
                UnrecoverableError("Duplicate indice in inverted index");
            }
        }
        return res;
    }

    SizeT GetSizeInBytes() const {
        SizeT bytes = 0;
        bytes += sizeof(row_num_);
        bytes += sizeof(SizeT);
        for (const auto &[indice, posting_list] : inverted_idx_) {
            bytes += sizeof(u32);
            bytes += sizeof(SizeT);
            bytes += posting_list.size() * Posting<DataType>::GetSizeInBytes();
        }
        return bytes;
    }

private:
    HashMap<IdxType, Vector<Posting<DataType>>> inverted_idx_;
    u32 row_num_{};
};

} // namespace infinity