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
#include <ostream>
#include <type_traits>

export module infinity_core:data_store;

import :stl;
import :hnsw_common;
import :local_file_handle;
import :vec_store_type;
import :graph_store;
import :infinity_exception;
import serialize;
import :data_store_util;
import :plain_vec_store;

namespace infinity {

template <typename VecStoreT, typename LabelType, bool OwnMem>
class DataStoreInner;

export template <typename VecStoreT, typename LabelType>
class DataStoreChunkIter;

template <typename VecStoreT, typename LabelType>
class DataStoreIter;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"

export template <typename VecStoreT, typename LabelType, bool OwnMem>
class DataStoreBase {
public:
    using This = DataStoreBase<VecStoreT, LabelType, OwnMem>;
    using QueryVecType = typename VecStoreT::QueryVecType;
    using VecStoreMeta = typename VecStoreT::template Meta<OwnMem>;

public:
    template <typename T, typename = void>
    struct has_compress_type : std::false_type {};

    template <typename T>
    struct has_compress_type<T, std::void_t<typename T::CompressType>> : std::true_type {};

    DataStoreBase() = default;
    DataStoreBase(VecStoreMeta &&vec_store_meta, GraphStoreMeta &&graph_store_meta)
        : vec_store_meta_(std::move(vec_store_meta)), graph_store_meta_(std::move(graph_store_meta)) {}
    // DataStoreBase(This &&other) : vec_store_meta_(std::move(other.vec_store_meta_)), graph_store_meta_(std::move(other.graph_store_meta_)) {}
    // DataStoreBase &operator=(This &&other) {
    //     if (this != &other) {
    //         vec_store_meta_ = std::move(other.vec_store_meta_);
    //         graph_store_meta_ = std::move(other.graph_store_meta_);
    //     }
    //     return *this;
    // }
    // ~DataStoreBase() = default;

    typename VecStoreT::QueryType MakeQuery(QueryVecType query) const { return vec_store_meta_.MakeQuery(query); }

    const VecStoreMeta &vec_store_meta() const { return vec_store_meta_; }

    SizeT dim() const { return vec_store_meta_.dim(); }

    // Graph store
    Pair<i32, VertexType> GetEnterPoint() const { return graph_store_meta_.GetEnterPoint(); }

    SizeT Mmax0() const { return graph_store_meta_.Mmax0(); }
    SizeT Mmax() const { return graph_store_meta_.Mmax(); }

protected:
    VecStoreMeta vec_store_meta_;
    GraphStoreMeta graph_store_meta_;
};

export template <typename VecStoreT, typename LabelType, bool OwnMem = true>
class DataStore : public DataStoreBase<VecStoreT, LabelType, OwnMem> {
public:
    using This = DataStore<VecStoreT, LabelType, OwnMem>;
    using Base = DataStoreBase<VecStoreT, LabelType, OwnMem>;
    using DataType = typename VecStoreT::DataType;
    using QueryVecType = typename VecStoreT::QueryVecType;
    using Inner = DataStoreInner<VecStoreT, LabelType, OwnMem>;
    using VecStoreMeta = typename VecStoreT::template Meta<OwnMem>;
    using VecStoreInner = typename VecStoreT::template Inner<OwnMem>;

    friend class DataStoreChunkIter<VecStoreT, LabelType>;
    friend class DataStoreIter<VecStoreT, LabelType>;

private:
    DataStore(SizeT chunk_size, SizeT max_chunk_n, VecStoreMeta &&vec_store_meta, GraphStoreMeta &&graph_store_meta)
        : Base(std::move(vec_store_meta), std::move(graph_store_meta)), chunk_size_(chunk_size), max_chunk_n_(max_chunk_n),
          chunk_shift_(__builtin_ctzll(chunk_size)), inners_(MakeUnique<Inner[]>(max_chunk_n)), mem_usage_(0) {
        assert(chunk_size > 0);
        assert((chunk_size & (chunk_size - 1)) == 0);
        cur_vec_num_ = 0;
    }

public:
    DataStore() = default;
    DataStore(DataStore &&other) : Base(std::move(other)) {
        chunk_size_ = std::exchange(other.chunk_size_, 0);
        max_chunk_n_ = std::exchange(other.max_chunk_n_, 0);
        chunk_shift_ = std::exchange(other.chunk_shift_, 0);
        cur_vec_num_ = other.cur_vec_num_.exchange(0);
        inners_ = std::exchange(other.inners_, nullptr);
        mem_usage_ = other.mem_usage_.exchange(0);
    }
    DataStore &operator=(DataStore &&other) {
        if (this != &other) {
            Base::operator=(std::move(other));
            chunk_size_ = std::exchange(other.chunk_size_, 0);
            max_chunk_n_ = std::exchange(other.max_chunk_n_, 0);
            chunk_shift_ = std::exchange(other.chunk_shift_, 0);
            cur_vec_num_ = other.cur_vec_num_.exchange(0);
            inners_ = std::exchange(other.inners_, nullptr);
            mem_usage_ = other.mem_usage_.exchange(0);
        }
        return *this;
    }
    ~DataStore() {
        if (!inners_) {
            return;
        }
        SizeT cur_vec_num = this->cur_vec_num();
        auto [chunk_num, last_chunk_size] = ChunkInfo(cur_vec_num);
        for (SizeT i = 0; i < chunk_num; ++i) {
            SizeT chunk_size = (i < chunk_num - 1) ? chunk_size_ : last_chunk_size;
            inners_[i].Free(chunk_size, this->graph_store_meta_);
        }
    }

    static This Make(SizeT chunk_size, SizeT max_chunk_n, SizeT dim, SizeT Mmax0, SizeT Mmax) {
        bool normalize = false;
        if constexpr (Base::template has_compress_type<VecStoreT>::value) {
            normalize = std::is_same_v<VecStoreMeta, typename LVQCosVecStoreType<DataType, typename VecStoreT::CompressType>::template Meta<OwnMem>>;
        }
        VecStoreMeta vec_store_meta = VecStoreMeta::Make(dim, normalize);
        GraphStoreMeta graph_store_meta = GraphStoreMeta::Make(Mmax0, Mmax);
        This ret(chunk_size, max_chunk_n, std::move(vec_store_meta), std::move(graph_store_meta));
        ret.cur_vec_num_ = 0;

        SizeT mem_usage = 0;
        ret.inners_[0] = Inner::Make(chunk_size, ret.vec_store_meta_, ret.graph_store_meta_, mem_usage);
        ret.mem_usage_.store(mem_usage);
        return ret;
    }

    void Save(LocalFileHandle &file_handle) const {
        SizeT cur_vec_num = this->cur_vec_num();
        auto [chunk_num, last_chunk_size] = ChunkInfo(cur_vec_num);

        file_handle.Append(&chunk_size_, sizeof(chunk_size_));
        file_handle.Append(&max_chunk_n_, sizeof(max_chunk_n_));

        file_handle.Append(&cur_vec_num, sizeof(cur_vec_num));
        this->vec_store_meta_.Save(file_handle);
        this->graph_store_meta_.Save(file_handle, cur_vec_num);
        for (SizeT i = 0; i < chunk_num; ++i) {
            SizeT chunk_size = (i < chunk_num - 1) ? chunk_size_ : last_chunk_size;
            inners_[i].Save(file_handle, chunk_size, this->vec_store_meta_, this->graph_store_meta_);
        }
    }

    void SaveToPtr(LocalFileHandle &file_handle) const {
        SizeT cur_vec_num = this->cur_vec_num();

        file_handle.Append(&cur_vec_num, sizeof(cur_vec_num));
        this->vec_store_meta_.Save(file_handle);
        this->graph_store_meta_.Save(file_handle, cur_vec_num);

        auto [chunk_num, last_chunk_size] = ChunkInfo(cur_vec_num);
        Inner::SaveToPtr(file_handle, inners_.get(), this->vec_store_meta_, this->graph_store_meta_, chunk_size_, chunk_num, last_chunk_size);
    }

    static This Load(LocalFileHandle &file_handle, SizeT max_chunk_n = 0) {
        SizeT chunk_size;
        file_handle.Read(&chunk_size, sizeof(chunk_size));
        SizeT max_chunk_n1;
        file_handle.Read(&max_chunk_n1, sizeof(max_chunk_n1));
        if (max_chunk_n == 0) {
            max_chunk_n = max_chunk_n1;
        }
        assert(max_chunk_n >= max_chunk_n1);

        SizeT cur_vec_num;
        file_handle.Read(&cur_vec_num, sizeof(cur_vec_num));
        VecStoreMeta vec_store_meta = VecStoreMeta::Load(file_handle);
        GraphStoreMeta graph_store_meta = GraphStoreMeta::Load(file_handle);

        This ret = This(chunk_size, max_chunk_n, std::move(vec_store_meta), std::move(graph_store_meta));
        ret.cur_vec_num_ = cur_vec_num;

        SizeT mem_usage = 0;
        auto [chunk_num, last_chunk_size] = ret.ChunkInfo(cur_vec_num);
        for (SizeT i = 0; i < chunk_num; ++i) {
            SizeT cur_chunk_size = (i < chunk_num - 1) ? chunk_size : last_chunk_size;
            ret.inners_[i] = Inner::Load(file_handle, cur_chunk_size, chunk_size, ret.vec_store_meta_, ret.graph_store_meta_, mem_usage);
        }
        ret.mem_usage_.store(mem_usage);
        return ret;
    }

    static This LoadFromPtr(const char *&ptr) {
        SizeT cur_vec_num = ReadBufAdv<SizeT>(ptr);
        VecStoreMeta vec_store_meta = VecStoreMeta::LoadFromPtr(ptr);
        GraphStoreMeta graph_store_meta = GraphStoreMeta::LoadFromPtr(ptr);

        SizeT chunk_size = 1;
        while (chunk_size < cur_vec_num) {
            chunk_size <<= 1;
        }
        This ret = This(chunk_size, 1 /*max_chunk_n*/, std::move(vec_store_meta), std::move(graph_store_meta));
        ret.cur_vec_num_ = cur_vec_num;

        SizeT mem_usage = 0;
        ret.inners_[0] = Inner::LoadFromPtr(ptr, cur_vec_num, chunk_size, ret.vec_store_meta_, ret.graph_store_meta_, mem_usage);
        ret.mem_usage_.store(mem_usage);
        return ret;
    }

    void SetGraph(GraphStoreMeta &&graph_meta, Vector<GraphStoreInner<OwnMem>> &&graph_inners) {
        this->graph_store_meta_ = std::move(graph_meta);
        for (SizeT i = 0; i < graph_inners.size(); ++i) {
            inners_[i].SetGraphStoreInner(std::move(graph_inners[i]));
        }
    }

    SizeT GetSizeInBytes() const {
        SizeT cur_vec_num = this->cur_vec_num();
        auto [chunk_num, last_chunk_size] = ChunkInfo(cur_vec_num);

        SizeT size = 0;
        size += sizeof(chunk_size_);
        size += sizeof(max_chunk_n_);
        size += sizeof(cur_vec_num_);
        size += this->vec_store_meta_.GetSizeInBytes();
        size += this->graph_store_meta_.GetSizeInBytes();
        for (SizeT i = 0; i < chunk_num; ++i) {
            SizeT chunk_size = (i < chunk_num - 1) ? chunk_size_ : last_chunk_size;
            size += inners_[i].GetSizeInBytes(chunk_size, this->vec_store_meta_, this->graph_store_meta_);
        }
        return size;
    }

    // Vec store
    template <DataIteratorConcept<QueryVecType, LabelType> Iterator>
    Pair<SizeT, SizeT> AddVec(Iterator &&query_iter) {
        SizeT mem_usage = 0;
        SizeT cur_vec_num = this->cur_vec_num();
        SizeT start_idx = cur_vec_num;
        auto [chunk_num, last_chunk_size] = ChunkInfo(cur_vec_num);
        while (true) {
            SizeT remain_size = chunk_size_ - last_chunk_size;
            auto [insert_n, used_up] =
                inners_[chunk_num - 1].AddVec(std::forward<Iterator>(query_iter), last_chunk_size, remain_size, this->vec_store_meta_, mem_usage);
            cur_vec_num += insert_n;
            last_chunk_size += insert_n;
            if (cur_vec_num == max_chunk_n_ * chunk_size_) {
                break;
            }
            if (last_chunk_size == chunk_size_) {
                inners_[chunk_num++] = Inner::Make(chunk_size_, this->vec_store_meta_, this->graph_store_meta_, mem_usage);
                last_chunk_size = 0;
            }
            if (used_up) {
                break;
            }
        }
        cur_vec_num_.store(cur_vec_num);
        mem_usage_.fetch_add(mem_usage);
        return {start_idx, cur_vec_num};
    }

    template <DataIteratorConcept<QueryVecType, LabelType> Iterator>
    Pair<SizeT, SizeT> OptAddVec(Iterator &&query_iter) {
        if constexpr (VecStoreT::HasOptimize) {
            SizeT mem_usage = 0;
            SizeT cur_vec_num = this->cur_vec_num();
            auto [chunk_num, last_chunk_size] = ChunkInfo(cur_vec_num);
            if (chunk_num > 0) {
                Vector<Pair<VecStoreInner *, SizeT>> vec_inners;
                for (SizeT i = 0; i < chunk_num; ++i) {
                    SizeT chunk_size = (i < chunk_num - 1) ? chunk_size_ : last_chunk_size;
                    vec_inners.emplace_back(inners_[i].vec_store_inner(), chunk_size);
                }
                Iterator query_iter_copy = query_iter;
                this->vec_store_meta_.template Optimize<LabelType, Iterator>(std::move(query_iter_copy), vec_inners, mem_usage);
            }
            mem_usage_.fetch_add(mem_usage);
        }
        return AddVec(std::move(query_iter));
    }

    void Optimize() {
        if constexpr (!VecStoreT::HasOptimize) {
            return;
        }
        DenseVectorIter<DataType, LabelType> empty_iter(nullptr, this->dim(), 0);
        AddVec(std::move(empty_iter));
    }

    void PrefetchVec(SizeT vec_i) const {
        const auto &[inner, idx] = GetInner(vec_i);
        inner.PrefetchVec(idx, this->vec_store_meta_);
    }

    typename VecStoreT::StoreType GetVec(SizeT vec_i) const {
        const auto &[inner, idx] = GetInner(vec_i);
        return inner.GetVec(idx, this->vec_store_meta_);
    }

    // Graph store
    void AddVertex(VertexType vec_i, i32 layer_n) {
        auto [inner, idx] = GetInner(vec_i);
        SizeT mem_usage = 0;
        inner.AddVertex(idx, layer_n, this->graph_store_meta_, mem_usage);
        mem_usage_.fetch_add(mem_usage);
    }

    Pair<VertexType *, VertexListSize *> GetNeighborsMut(VertexType vertex_i, i32 layer_i) {
        auto [inner, idx] = GetInner(vertex_i);
        return inner.GetNeighborsMut(idx, layer_i, this->graph_store_meta_);
    }

    Pair<const VertexType *, VertexListSize> GetNeighbors(VertexType vertex_i, i32 layer_i) const {
        const auto &[inner, idx] = GetInner(vertex_i);
        return inner.GetNeighbors(idx, layer_i, this->graph_store_meta_);
    }

    Pair<i32, VertexType> TryUpdateEnterPoint(i32 layer, VertexType vertex_i) { return this->graph_store_meta_.TryUpdateEnterPoint(layer, vertex_i); }

    // other
    LabelType GetLabel(SizeT vec_i) const {
        const auto &[inner, idx] = GetInner(vec_i);
        return inner.GetLabel(idx);
    }

    std::shared_lock<std::shared_mutex> SharedLock(SizeT vec_i) const {
        const auto &[inner, idx] = GetInner(vec_i);
        return inner.SharedLock(idx);
    }

    std::unique_lock<std::shared_mutex> UniqueLock(SizeT vec_i) {
        const auto &[inner, idx] = GetInner(vec_i);
        return inner.UniqueLock(idx);
    }

    SizeT cur_vec_num() const { return cur_vec_num_.load(); }

    SizeT mem_usage() const { return mem_usage_.load(); }

    template <typename CompressVecStoreType>
    DataStore<CompressVecStoreType, LabelType, OwnMem> CompressToLVQ() &&;

private:
    Pair<Inner &, SizeT> GetInner(SizeT vec_i) { return {inners_[vec_i >> chunk_shift_], vec_i & (chunk_size_ - 1)}; }

    Pair<const Inner &, SizeT> GetInner(SizeT vec_i) const { return {inners_[vec_i >> chunk_shift_], vec_i & (chunk_size_ - 1)}; }

    // return chunk_num & last chunk size
    Pair<SizeT, SizeT> ChunkInfo(SizeT cur_vec_num) const {
        SizeT chunk_num = std::min(max_chunk_n_, (cur_vec_num >> chunk_shift_) + 1);
        assert(chunk_num > 0);
        SizeT last_chunk_size = cur_vec_num - ((chunk_num - 1) << chunk_shift_);
        return {chunk_num, last_chunk_size};
    };

private:
    SizeT chunk_size_;
    SizeT max_chunk_n_;
    SizeT chunk_shift_;

    Atomic<SizeT> cur_vec_num_;

    UniquePtr<Inner[]> inners_;
    Atomic<SizeT> mem_usage_ = 0;

public:
    void Check() const {
        i32 max_l = -1;
        SizeT i;
        SizeT cur_vec_num = this->cur_vec_num();
        auto [chunk_num, last_chunk_size] = ChunkInfo(cur_vec_num);
        for (i = 0; i < chunk_num; ++i) {
            i32 max_l1 = -1;
            SizeT chunk_size = i < chunk_num - 1 ? chunk_size_ : last_chunk_size;
            inners_[i].Check(chunk_size, this->graph_store_meta_, i * chunk_size_, cur_vec_num, max_l1);
            max_l = std::max(max_l, max_l1);
        }
        auto [max_layer, ep] = this->GetEnterPoint();
        if (max_l != max_layer) {
            UnrecoverableError("max_l != max_layer");
        }
    }

    void Dump(std::ostream &os) const {
        SizeT cur_vec_num = this->cur_vec_num();
        auto [chunk_num, last_chunk_size] = ChunkInfo(cur_vec_num);

        os << "[CONST] chunk_size: " << chunk_size_ << ", max_chunk_n: " << max_chunk_n_ << ", chunk_shift: " << chunk_shift_ << std::endl;
        os << "cur_vec_num: " << cur_vec_num << std::endl;

        this->vec_store_meta_.Dump(os);
        for (SizeT i = 0; i < chunk_num; ++i) {
            os << "chunk " << i << std::endl;
            SizeT cur_chunk_size = (i < chunk_num - 1) ? chunk_size_ : last_chunk_size;
            inners_[i].DumpVec(os, i * chunk_size_, cur_chunk_size, this->vec_store_meta_);
        }

        this->graph_store_meta_.Dump(os);
        for (SizeT i = 0; i < chunk_num; ++i) {
            os << "chunk " << i << std::endl;
            SizeT cur_chunk_size = (i < chunk_num - 1) ? chunk_size_ : last_chunk_size;
            inners_[i].DumpGraph(os, cur_chunk_size, this->graph_store_meta_);
        }
    }
};

export template <typename VecStoreT, typename LabelType>
class DataStore<VecStoreT, LabelType, false> : public DataStoreBase<VecStoreT, LabelType, false> {
public:
    using This = DataStore<VecStoreT, LabelType, false>;
    using VecStoreMeta = typename VecStoreT::template Meta<false>;
    using Base = DataStoreBase<VecStoreT, LabelType, false>;
    using Inner = DataStoreInner<VecStoreT, LabelType, false>;

private:
    DataStore(SizeT cur_vec_num, VecStoreMeta vec_store_meta, GraphStoreMeta graph_store_meta)
        : Base(std::move(vec_store_meta), std::move(graph_store_meta)), cur_vec_num_(cur_vec_num) {}

public:
    DataStore() = default;
    // DataStore(DataStore &&other) : Base(std::move(other)), inner_(std::move(other.inner_)), cur_vec_num_(other.cur_vec_num_) {}
    // DataStore &operator=(DataStore &&other) {
    //     if (this != &other) {
    //         Base::operator=(std::move(other));
    //         inner_ = std::move(other.inner_);
    //         cur_vec_num_ = other.cur_vec_num_;
    //     }
    //     return *this;
    // }
    // ~DataStore() = default;

    static This LoadFromPtr(const char *&ptr) {
        SizeT cur_vec_num = ReadBufAdv<SizeT>(ptr);
        VecStoreMeta vec_store_meta = VecStoreMeta::LoadFromPtr(ptr);
        GraphStoreMeta graph_store_meta = GraphStoreMeta::LoadFromPtr(ptr);

        This ret = This(cur_vec_num, std::move(vec_store_meta), std::move(graph_store_meta));
        ret.inner_ = Inner::LoadFromPtr(ptr, cur_vec_num, cur_vec_num, ret.vec_store_meta_, ret.graph_store_meta_);
        return ret;
    }

    typename VecStoreT::StoreType GetVec(SizeT vec_i) const { return inner_.GetVec(vec_i, this->vec_store_meta_); }

    void PrefetchVec(SizeT vec_i) const { inner_.PrefetchVec(vec_i, this->vec_store_meta_); }

    Pair<const VertexType *, VertexListSize> GetNeighbors(VertexType vertex_i, i32 layer_i) const {
        return inner_.GetNeighbors(vertex_i, layer_i, this->graph_store_meta_);
    }

    LabelType GetLabel(SizeT vec_i) const { return inner_.GetLabel(vec_i); }

    SizeT cur_vec_num() const { return cur_vec_num_; }

    SizeT mem_usage() const { return 0; }

private:
    Inner inner_;
    SizeT cur_vec_num_ = 0;

public:
    void Check() const {
        i32 max_l = -1;
        inner_.Check(cur_vec_num_, this->graph_store_meta_, 0, cur_vec_num_, max_l);
        auto [max_layer, ep] = this->GetEnterPoint();
        if (max_l != max_layer) {
            UnrecoverableError("max_l != max_layer");
        }
    }

    void Dump() const {
        std::cout << "[CONST] cur_vec_num: " << cur_vec_num_ << std::endl;
        this->vec_store_meta_.Dump();
        inner_.DumpVec(std::cout, 0, cur_vec_num_, this->vec_store_meta_);
        this->graph_store_meta_.Dump();
        inner_.DumpGraph(std::cout, cur_vec_num_, this->graph_store_meta_);
    }
};

#pragma clang diagnostic pop
//----------------------------------------------- Inner -----------------------------------------------

template <typename VecStoreT, typename LabelType, bool OwnMem>
class DataStoreInnerBase {
public:
    using This = DataStoreInner<VecStoreT, LabelType, OwnMem>;
    using DataType = typename VecStoreT::DataType;
    using VecStoreInner = typename VecStoreT::template Inner<OwnMem>;
    using VecStoreMeta = typename VecStoreT::template Meta<OwnMem>;
    using GraphStoreInner = GraphStoreInner<OwnMem>;

    friend class DataStoreIter<VecStoreT, LabelType>;

public:
    DataStoreInnerBase() = default;

    void Save(LocalFileHandle &file_handle, SizeT cur_vec_num, const VecStoreMeta &vec_store_meta, const GraphStoreMeta &graph_store_meta) const {
        this->vec_store_inner_.Save(file_handle, cur_vec_num, vec_store_meta);
        this->graph_store_inner_.Save(file_handle, cur_vec_num, graph_store_meta);
        file_handle.Append(this->labels_.get(), sizeof(LabelType) * cur_vec_num);
    }

    static void SaveToPtr(LocalFileHandle &file_handle,
                          const This *inners,
                          const VecStoreMeta &vec_store_meta,
                          const GraphStoreMeta &graph_store_meta,
                          SizeT ck_size,
                          SizeT chunk_num,
                          SizeT last_chunk_size) {
        Vector<const typename VecStoreInner::Base *> vec_store_inners;
        Vector<const typename GraphStoreInner::Base *> graph_store_inners;
        for (SizeT i = 0; i < chunk_num; ++i) {
            vec_store_inners.emplace_back(&inners[i].vec_store_inner_);
            graph_store_inners.emplace_back(&inners[i].graph_store_inner_);
        }
        VecStoreInner::SaveToPtr(file_handle, vec_store_inners, vec_store_meta, ck_size, chunk_num, last_chunk_size);
        GraphStoreInner::SaveToPtr(file_handle, graph_store_inners, graph_store_meta, ck_size, chunk_num, last_chunk_size);
        for (SizeT i = 0; i < chunk_num; ++i) {
            SizeT chunk_size = (i < chunk_num - 1) ? ck_size : last_chunk_size;
            file_handle.Append(inners[i].labels_.get(), sizeof(LabelType) * chunk_size);
        }
    }

    void Free(SizeT cur_vec_num, const GraphStoreMeta &graph_store_meta) { graph_store_inner_.Free(cur_vec_num, graph_store_meta); }

    SizeT GetSizeInBytes(SizeT chunk_size, const VecStoreMeta &vec_store_meta, const GraphStoreMeta &graph_store_meta) const {
        SizeT size = 0;
        size += vec_store_inner_.GetSizeInBytes(chunk_size, vec_store_meta);
        size += graph_store_inner_.GetSizeInBytes(chunk_size, graph_store_meta);
        size += sizeof(LabelType) * chunk_size;
        return size;
    }

    // vec store
    typename VecStoreT::StoreType GetVec(VertexType vec_i, const VecStoreMeta &meta) const { return vec_store_inner_.GetVec(vec_i, meta); }

    void PrefetchVec(VertexType vec_i, const VecStoreMeta &meta) const { vec_store_inner_.Prefetch(vec_i, meta); }

    // graph store
    Pair<const VertexType *, VertexListSize> GetNeighbors(VertexType vertex_i, i32 layer_i, const GraphStoreMeta &meta) const {
        return graph_store_inner_.GetNeighbors(vertex_i, layer_i, meta);
    }

    LabelType GetLabel(VertexType vec_i) const { return labels_[vec_i]; }

    VecStoreInner *vec_store_inner() { return &vec_store_inner_; }

    GraphStoreInner *graph_store_inner() { return &graph_store_inner_; }
    void SetGraphStoreInner(GraphStoreInner &&graph_store_inner) { graph_store_inner_ = std::move(graph_store_inner); }

protected:
    VecStoreInner vec_store_inner_;
    GraphStoreInner graph_store_inner_;
    ArrayPtr<LabelType, OwnMem> labels_;

public:
    void Check(SizeT chunk_size, const GraphStoreMeta &meta, VertexType vertex_i_offset, SizeT cur_vec_num, i32 &max_l) const {
        graph_store_inner_.Check(chunk_size, meta, vertex_i_offset, cur_vec_num, max_l);
    }

    void DumpVec(std::ostream &os, SizeT offset, SizeT chunk_size, const VecStoreMeta &meta) const {
        vec_store_inner_.Dump(os, offset, chunk_size, meta);
        os << "labels: [";
        for (SizeT i = 0; i < chunk_size; ++i) {
            os << labels_[i] << ", ";
        }
        os << "]" << std::endl;
    }

    void DumpGraph(std::ostream &os, SizeT chunk_size, const GraphStoreMeta &meta) const { graph_store_inner_.Dump(os, chunk_size, meta); }
};

template <typename VecStoreT, typename LabelType, bool OwnMem>
class DataStoreInner : public DataStoreInnerBase<VecStoreT, LabelType, OwnMem> {
private:
    using This = DataStoreInner<VecStoreT, LabelType, OwnMem>;
    using VecStoreInner = typename VecStoreT::template Inner<OwnMem>;
    using VecStoreMeta = typename VecStoreT::template Meta<OwnMem>;
    using GraphStoreInner = GraphStoreInner<OwnMem>;
    using QueryVecType = typename VecStoreT::QueryVecType;

    DataStoreInner(SizeT chunk_size, VecStoreInner vec_store_inner, GraphStoreInner graph_store_inner) {
        this->vec_store_inner_ = std::move(vec_store_inner);
        this->graph_store_inner_ = std::move(graph_store_inner);
        this->labels_ = MakeUnique<LabelType[]>(chunk_size);
        vertex_mutex_ = MakeUnique<std::shared_mutex[]>(chunk_size);
    }

public:
    DataStoreInner() = default;
    static This Make(SizeT chunk_size, VecStoreMeta &vec_store_meta, GraphStoreMeta &graph_store_meta, SizeT &mem_usage) {
        auto vec_store_inner = VecStoreInner::Make(chunk_size, vec_store_meta, mem_usage);
        auto graph_store_inner = GraphStoreInner::Make(chunk_size, graph_store_meta, mem_usage);
        return This(chunk_size, std::move(vec_store_inner), std::move(graph_store_inner));
    }

    static This Load(LocalFileHandle &file_handle,
                     SizeT cur_vec_num,
                     SizeT chunk_size,
                     VecStoreMeta &vec_store_meta,
                     GraphStoreMeta &graph_store_meta,
                     SizeT &mem_usage) {
        auto vec_store_inner = VecStoreInner::Load(file_handle, cur_vec_num, chunk_size, vec_store_meta, mem_usage);
        auto graph_store_iner = GraphStoreInner::Load(file_handle, cur_vec_num, chunk_size, graph_store_meta, mem_usage);
        This ret(chunk_size, std::move(vec_store_inner), std::move(graph_store_iner));
        file_handle.Read(ret.labels_.get(), sizeof(LabelType) * cur_vec_num);
        return ret;
    }

    static This LoadFromPtr(const char *&ptr,
                            SizeT cur_vec_num,
                            SizeT chunk_size,
                            VecStoreMeta &vec_store_meta,
                            GraphStoreMeta &graph_store_meta,
                            SizeT &mem_usage) {
        auto vec_store_inner = VecStoreInner::LoadFromPtr(ptr, cur_vec_num, chunk_size, vec_store_meta, mem_usage);
        auto graph_store_inner = GraphStoreInner::LoadFromPtr(ptr, cur_vec_num, chunk_size, graph_store_meta, mem_usage);
        This ret(chunk_size, std::move(vec_store_inner), std::move(graph_store_inner));
        std::memcpy(ret.labels_.get(), ptr, sizeof(LabelType) * cur_vec_num);
        ptr += sizeof(LabelType) * cur_vec_num;
        return ret;
    }

    // vec store
    template <DataIteratorConcept<QueryVecType, LabelType> Iterator>
    Pair<SizeT, bool> AddVec(Iterator &&query_iter, VertexType start_idx, SizeT remain_num, const VecStoreMeta &meta, SizeT &mem_usage) {
        SizeT insert_n = 0;
        bool used_up = false;
        while (insert_n < remain_num) {
            if (auto ret = query_iter.Next(); ret) {
                auto &[vec, label] = *ret;
                this->vec_store_inner_.SetVec(start_idx + insert_n, vec, meta, mem_usage);
                this->labels_[start_idx + insert_n] = label;
                ++insert_n;
            } else {
                used_up = true;
                break;
            }
        }
        return {insert_n, used_up};
    }

    // graph store
    void AddVertex(VertexType vec_i, i32 layer_n, const GraphStoreMeta &meta, SizeT &mem_usage) {
        this->graph_store_inner_.AddVertex(vec_i, layer_n, meta, mem_usage);
    }
    Pair<VertexType *, VertexListSize *> GetNeighborsMut(VertexType vertex_i, i32 layer_i, const GraphStoreMeta &meta) {
        return this->graph_store_inner_.GetNeighborsMut(vertex_i, layer_i, meta);
    }

    std::shared_lock<std::shared_mutex> SharedLock(VertexType vec_i) const { return std::shared_lock<std::shared_mutex>(vertex_mutex_[vec_i]); }

    std::unique_lock<std::shared_mutex> UniqueLock(VertexType vec_i) { return std::unique_lock<std::shared_mutex>(vertex_mutex_[vec_i]); }

private:
    mutable UniquePtr<std::shared_mutex[]> vertex_mutex_;
};

template <typename VecStoreT, typename LabelType>
class DataStoreInner<VecStoreT, LabelType, false> : public DataStoreInnerBase<VecStoreT, LabelType, false> {
public:
    using This = DataStoreInner<VecStoreT, LabelType, false>;
    using VecStoreInner = typename VecStoreT::template Inner<false>;
    using VecStoreMeta = typename VecStoreT::template Meta<false>;
    using GraphStoreInner = GraphStoreInner<false>;

private:
    DataStoreInner(SizeT chunk_size, VecStoreInner vec_store_inner, GraphStoreInner graph_store_inner, const LabelType *labels) {
        this->vec_store_inner_ = std::move(vec_store_inner);
        this->graph_store_inner_ = std::move(graph_store_inner);
        this->labels_ = labels;
    }

public:
    DataStoreInner() = default;

    static This
    LoadFromPtr(const char *&ptr, SizeT cur_vec_num, SizeT chunk_size, VecStoreMeta &vec_store_meta, const GraphStoreMeta &graph_store_meta) {
        auto vec_store_inner = VecStoreInner::LoadFromPtr(ptr, cur_vec_num, vec_store_meta);
        auto graph_store_inner = GraphStoreInner::LoadFromPtr(ptr, cur_vec_num, chunk_size, graph_store_meta);
        auto *labels = reinterpret_cast<const LabelType *>(ptr);
        ptr += sizeof(LabelType) * cur_vec_num;
        return This(chunk_size, std::move(vec_store_inner), std::move(graph_store_inner), labels);
    }
};

template <typename VecStoreT, typename LabelType>
class DataStoreChunkIter {
public:
    using Inner = typename DataStore<VecStoreT, LabelType, true>::Inner;

    DataStoreChunkIter(const DataStore<VecStoreT, LabelType, true> *data_store) : data_store_(data_store) {
        std::tie(chunk_num_, last_chunk_size_) = data_store_->ChunkInfo(data_store_->cur_vec_num());
    }

    Optional<Pair<const Inner *, SizeT>> Next() {
        if (cur_chunk_i_ >= chunk_num_) {
            return None;
        }
        auto ret = Pair<const Inner *, SizeT>(&data_store_->inners_[cur_chunk_i_],
                                              (cur_chunk_i_ == chunk_num_ - 1) ? last_chunk_size_ : data_store_->chunk_size_);
        ++cur_chunk_i_;
        return ret;
    }

    const DataStore<VecStoreT, LabelType, true> *data_store_;

private:
    SizeT cur_chunk_i_ = 0;
    SizeT chunk_num_;
    SizeT last_chunk_size_;
};

template <typename VecStoreT, typename LabelType>
class DataStoreInnerIter {
public:
    using VecMeta = typename VecStoreT::template Meta<true>;
    using Inner = DataStoreInner<VecStoreT, LabelType, true>;
    using StoreType = typename VecStoreT::StoreType;

    DataStoreInnerIter(const VecMeta *vec_meta, const Inner *inner, SizeT max_vec_num)
        : vec_meta_(vec_meta), inner_(inner), max_vec_num_(max_vec_num), cur_idx_(0) {}

    Optional<Pair<StoreType, LabelType>> Next() {
        if (cur_idx_ >= max_vec_num_) {
            return None;
        }
        auto ret = Pair<StoreType, LabelType>(inner_->GetVec(cur_idx_, *vec_meta_), inner_->GetLabel(cur_idx_));
        ++cur_idx_;
        return ret;
    }

private:
    const VecMeta *vec_meta_;
    const Inner *inner_;
    SizeT max_vec_num_;

    SizeT cur_idx_;
};

template <typename VecStoreT, typename LabelType>
class DataStoreIter {
public:
    using StoreType = typename VecStoreT::StoreType;
    using InnerIter = DataStoreInnerIter<VecStoreT, LabelType>;
    using ValueType = StoreType;

    DataStoreIter(const DataStore<VecStoreT, LabelType, true> *data_store) : data_store_iter_(data_store), inner_iter_(None) {}

    Optional<Pair<StoreType, LabelType>> Next() {
        if (!inner_iter_.has_value()) {
            auto inner_opt = data_store_iter_.Next();
            if (!inner_opt.has_value()) {
                return None;
            }
            const auto &[inner, chunk_size] = inner_opt.value();
            inner_iter_ = InnerIter(&data_store_iter_.data_store_->vec_store_meta_, inner, chunk_size);
        }
        auto &inner = inner_iter_.value();
        auto vec_opt = inner.Next();
        if (!vec_opt.has_value()) {
            inner_iter_ = None;
            return Next();
        }
        return vec_opt.value();
    }

private:
    DataStoreChunkIter<VecStoreT, LabelType> data_store_iter_;
    Optional<InnerIter> inner_iter_;
};

template <typename VecStoreT, typename LabelType, bool OwnMem>
template <typename CompressVecStoreType>
DataStore<CompressVecStoreType, LabelType, OwnMem> DataStore<VecStoreT, LabelType, OwnMem>::CompressToLVQ() && {
    if constexpr (std::is_same_v<CompressVecStoreType, VecStoreT>) {
        return std::move(*this);
    } else {
        const auto [chunk_num, last_chunk_size] = this->ChunkInfo(this->cur_vec_num());
        Vector<GraphStoreInner<OwnMem>> graph_inners;
        for (SizeT i = 0; i < chunk_num; ++i) {
            graph_inners.emplace_back(std::move(*this->inners_[i].graph_store_inner()));
        }
        auto ret = DataStore<CompressVecStoreType, LabelType, OwnMem>::Make(this->chunk_size_,
                                                                            this->max_chunk_n_,
                                                                            this->vec_store_meta_.dim(),
                                                                            this->Mmax0(),
                                                                            this->Mmax());
        ret.OptAddVec(DataStoreIter<VecStoreT, LabelType>(this));
        ret.SetGraph(std::move(this->graph_store_meta_), std::move(graph_inners));
        this->inners_ = nullptr;
        return ret;
    }
}

} // namespace infinity