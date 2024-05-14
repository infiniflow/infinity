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
#include <filesystem>
#include <functional>
#include <queue>
#include <string.h>

module external_sort_merger;

import stl;

namespace infinity {

#define IASSERT(exp)                                                                                                                                 \
    {                                                                                                                                                \
        if (!(exp)) {                                                                                                                                \
            assert(false);                                                                                                                           \
        }                                                                                                                                            \
    }

#define USE_LOSER_TREE
// #define USE_MMAP_IO

template <typename KeyType, typename LenType>
SortMerger<KeyType, LenType>::SortMerger(const char *filenm, u32 group_size, u32 bs, u32 output_num)
    : filenm_(filenm), MAX_GROUP_SIZE_(group_size), BS_SIZE_(bs), PRE_BUF_SIZE_((u32)(1. * bs * 0.8 / (group_size + 1))),
      RUN_BUF_SIZE_(PRE_BUF_SIZE_ * group_size), OUT_BUF_SIZE_(bs - RUN_BUF_SIZE_ - PRE_BUF_SIZE_), OUT_BUF_NUM_(output_num) {
    pre_buf_ = run_buf_ = out_buf_ = nullptr;

    pre_buf_size_ = pre_buf_num_ = count_ = 0;

    micro_run_idx_ = new u32[MAX_GROUP_SIZE_];
    micro_run_pos_ = new u32[MAX_GROUP_SIZE_];
    num_micro_run_ = new u32[MAX_GROUP_SIZE_];
    size_micro_run_ = new u32[MAX_GROUP_SIZE_];
    num_run_ = new u32[MAX_GROUP_SIZE_];
    size_run_ = new u32[MAX_GROUP_SIZE_];
    size_loaded_run_ = new u32[MAX_GROUP_SIZE_];
    run_addr_ = new u64[MAX_GROUP_SIZE_];
    run_curr_addr_ = new u64[MAX_GROUP_SIZE_];

    micro_buf_ = new char *[MAX_GROUP_SIZE_];
    sub_out_buf_ = new char *[OUT_BUF_NUM_];

    in_out_mtx_ = new std::mutex[OUT_BUF_NUM_];
    in_out_con_ = new std::condition_variable[OUT_BUF_NUM_];

    out_buf_size_ = new u32[OUT_BUF_NUM_];
    out_buf_full_ = new bool[OUT_BUF_NUM_];
#ifdef USE_LOSER_TREE
    merge_loser_tree_ = MakeShared<LoserTree<KeyAddr>>(MAX_GROUP_SIZE_);
#endif
}

template <typename KeyType, typename LenType>
SortMerger<KeyType, LenType>::~SortMerger() {
    if (pre_buf_)
        free(pre_buf_);

    if (run_buf_)
        free(run_buf_);

    if (out_buf_)
        free(out_buf_);

    delete[] micro_run_idx_;
    delete[] micro_run_pos_;
    delete[] num_micro_run_;
    delete[] size_micro_run_;
    delete[] num_run_;
    delete[] size_run_;
    delete[] size_loaded_run_;
    delete[] run_addr_;
    delete[] run_curr_addr_;

    delete[] micro_buf_;
    delete[] sub_out_buf_;

    delete[] in_out_mtx_;
    delete[] in_out_con_;

    delete[] out_buf_size_;
    delete[] out_buf_full_;
}

template <typename KeyType, typename LenType>
void SortMerger<KeyType, LenType>::NewBuffer() {
    if (!pre_buf_)
        pre_buf_ = (char *)malloc(PRE_BUF_SIZE_);

    if (!run_buf_)
        run_buf_ = (char *)malloc(RUN_BUF_SIZE_);

    if (!out_buf_)
        out_buf_ = (char *)malloc(OUT_BUF_SIZE_);
}

template <typename KeyType, typename LenType>
void SortMerger<KeyType, LenType>::Init(DirectIO &io_stream) {
    // initialize three buffers
    NewBuffer();

    // initiate output buffers
    out_buf_size_[0] = 0;
    sub_out_buf_[0] = out_buf_;
    out_buf_full_[0] = false;
    for (u32 i = 1; i < OUT_BUF_NUM_; ++i) {
        sub_out_buf_[i] = sub_out_buf_[i - 1] + OUT_BUF_SIZE_ / OUT_BUF_NUM_;
        out_buf_size_[i] = 0;
        out_buf_full_[i] = false;
    }
    out_buf_in_idx_ = 0;
    out_buf_out_idx_ = 0;

    // initiate the microrun buffer
    micro_buf_[0] = run_buf_;
    for (u32 i = 1; i < MAX_GROUP_SIZE_; ++i)
        micro_buf_[i] = micro_buf_[i - 1] + PRE_BUF_SIZE_;

    //
    group_size_ = 0;
    u64 next_run_pos = 0;
    for (u32 i = 0; i < MAX_GROUP_SIZE_ && (u64)io_stream.Tell() < FILE_LEN_; ++i, ++group_size_) {
        // get the size of run
        io_stream.Read((char *)(size_run_ + i), sizeof(u32));
        // get the records number of a run
        io_stream.Read((char *)(num_run_ + i), sizeof(u32));
        io_stream.Read((char *)(&next_run_pos), sizeof(u64));

        run_addr_[i] = io_stream.Tell(); // ftell(f);

        // loading size of a microrun
        u32 s = size_run_[i] > PRE_BUF_SIZE_ ? PRE_BUF_SIZE_ : size_run_[i];
        size_t ret = io_stream.Read(micro_buf_[i], s);
        size_micro_run_[i] = ret;
        size_loaded_run_[i] = ret;
        run_curr_addr_[i] = io_stream.Tell();
        // std::cout << "num_run_[" << i << "] " << num_run_[i] << " size_run_ " << size_run_[i] << " size_micro_run " << size_micro_run_[i]
        //           << std::endl;

        /// it is not needed for compression, validation will be made within IOStream in that case
        // if a record can fit in microrun buffer
        bool flag = false;
        while (*(LenType *)(micro_buf_[i]) + sizeof(LenType) > s) {
            size_micro_run_[i] = 0;
            --count_;
            // LOG_WARN("[Warning]: A record is too long, it will be ignored");

            io_stream.Seek(*(LenType *)(micro_buf_[i]) + sizeof(LenType) - s, SEEK_CUR);

            if (io_stream.Tell() - run_addr_[i] >= (u64)size_run_[i]) {
                flag = true;
                break;
            }

            s = (u32)((u64)size_run_[i] - (io_stream.Tell() - run_addr_[i]) > PRE_BUF_SIZE_ ? PRE_BUF_SIZE_
                                                                                            : (u64)size_run_[i] - (io_stream.Tell() - run_addr_[i]));
            size_micro_run_[i] = s;
            io_stream.Read(micro_buf_[i], s);
        }

#ifdef USE_LOSER_TREE
        if (flag) {
            merge_loser_tree_->InsertStart(nullptr, static_cast<LoserTree<u64>::Source>(i), true);
            continue;
        }
        auto key = KeyAddr(micro_buf_[i], -1, i);
        merge_loser_tree_->InsertStart(&key, static_cast<LoserTree<u64>::Source>(i), false);
#else
        if (flag) {
            continue;
        }
        merge_heap_.push(KeyAddr(micro_buf_[i], -1, i));
#endif
        micro_run_idx_[i] = 1;
        micro_run_pos_[i] = KeyAddr(micro_buf_[i], -1, i).LEN() + sizeof(LenType);
        num_micro_run_[i] = 0;

        io_stream.Seek(next_run_pos);
    }
#ifdef USE_LOSER_TREE
    merge_loser_tree_->Init();
#endif
    // initialize predict heap and records number of every microrun
    for (u32 i = 0; i < group_size_; ++i) {
        u32 pos = 0;
        u32 last_pos = -1;
        assert(i < MAX_GROUP_SIZE_);
        if (size_micro_run_[i] <= 0)
            continue;
        while (pos + sizeof(LenType) <= size_micro_run_[i]) {
            LenType len = *(LenType *)(micro_buf_[i] + pos);
            if (pos + sizeof(LenType) + len <= size_micro_run_[i]) {
                num_micro_run_[i]++;
                last_pos = pos;
                pos += sizeof(LenType) + len;
            } else {
                break;
            }
        }
        // std::cout << "len " << len << " size_micro_run_[" << i << "] " << size_micro_run_[i] << std::endl;
        assert(last_pos != (u32)-1); // buffer too small that can't hold one record
        assert(last_pos + sizeof(LenType) <= size_micro_run_[i]);
        assert(pos <= size_micro_run_[i]);
        LenType len = (LenType)(pos - last_pos);
        char *tmp = (char *)malloc(len);
        memcpy(tmp, micro_buf_[i] + last_pos, len);
        pre_heap_.push(KeyAddr(tmp, run_addr_[i] + pos, i));
        size_micro_run_[i] = pos;
    }
}
#ifdef USE_MMAP_IO
template <typename KeyType, typename LenType>
void SortMerger<KeyType, LenType>::Init(MmapIO &io_stream) {
    // initialize three buffers
    NewBuffer();

    // initiate output buffers
    out_buf_size_[0] = 0;
    sub_out_buf_[0] = out_buf_;
    out_buf_full_[0] = false;
    for (u32 i = 1; i < OUT_BUF_NUM_; ++i) {
        sub_out_buf_[i] = sub_out_buf_[i - 1] + OUT_BUF_SIZE_ / OUT_BUF_NUM_;
        out_buf_size_[i] = 0;
        out_buf_full_[i] = false;
    }
    out_buf_in_idx_ = 0;
    out_buf_out_idx_ = 0;

    // initiate the microrun buffer
    micro_buf_[0] = run_buf_;
    for (u32 i = 1; i < MAX_GROUP_SIZE_; ++i) {
        micro_buf_[i] = micro_buf_[i - 1] + PRE_BUF_SIZE_;
    }

    group_size_ = 0;
    u64 next_run_pos = 0;
    for (u32 i = 0; i < MAX_GROUP_SIZE_ && (u64)io_stream.Tell() < FILE_LEN_; ++i, ++group_size_) {
        // get the size of run
        io_stream.ReadU32(size_run_[i]);
        // get the records number of a run
        io_stream.ReadU32(num_run_[i]);
        io_stream.ReadU64(next_run_pos);

        run_addr_[i] = io_stream.Tell();

        // loading size of a microrun
        u32 s = size_run_[i] > PRE_BUF_SIZE_ ? PRE_BUF_SIZE_ : size_run_[i];
        size_t ret = io_stream.ReadBuf(micro_buf_[i], s);
        size_micro_run_[i] = ret;
        size_loaded_run_[i] = ret;
        run_curr_addr_[i] = io_stream.Tell();
        // std::cout << "num_run_[" << i << "] " << num_run_[i] << " size_run_ " << size_run_[i] << " size_micro_run " << size_micro_run_[i]
        //           << std::endl;

        /// it is not needed for compression, validation will be made within IOStream in that case
        // if a record can fit in microrun buffer
        bool flag = false;
        while (*(LenType *)(micro_buf_[i]) + sizeof(LenType) > s) {
            size_micro_run_[i] = 0;
            --count_;
            // LOG_WARN("[Warning]: A record is too long, it will be ignored");

            io_stream.Seek(*(LenType *)(micro_buf_[i]) + sizeof(LenType) - s);
            // io_stream.Seek(*(LenType *)(micro_buf_[i]) + sizeof(LenType) - s, SEEK_CUR);

            if (io_stream.Tell() - run_addr_[i] >= (u64)size_run_[i]) {
                flag = true;
                break;
            }

            s = (u32)((u64)size_run_[i] - (io_stream.Tell() - run_addr_[i]) > PRE_BUF_SIZE_ ? PRE_BUF_SIZE_
                                                                                            : (u64)size_run_[i] - (io_stream.Tell() - run_addr_[i]));
            size_micro_run_[i] = s;
            io_stream.Read(micro_buf_[i], s);
        }

#ifdef USE_LOSER_TREE
        if (flag) {
            merge_loser_tree_->InsertStart(nullptr, static_cast<LoserTree<u64>::Source>(i), true);
            continue;
        }
        auto key = KeyAddr(micro_buf_[i], -1, i);
        merge_loser_tree_->InsertStart(&key, static_cast<LoserTree<u64>::Source>(i), false);
#else
        if (flag) {
            continue;
        }
        merge_heap_.push(KeyAddr(micro_buf_[i], -1, i));
#endif
        micro_run_idx_[i] = 1;
        micro_run_pos_[i] = KeyAddr(micro_buf_[i], -1, i).LEN() + sizeof(LenType);
        num_micro_run_[i] = 0;

        io_stream.Seek(next_run_pos);
    }
#ifdef USE_LOSER_TREE
    merge_loser_tree_->Init();
#endif
    // initialize predict heap and records number of every microrun
    for (u32 i = 0; i < group_size_; ++i) {
        u32 pos = 0;
        u32 last_pos = -1;
        assert(i < MAX_GROUP_SIZE_);
        if (size_micro_run_[i] <= 0)
            continue;
        while (pos + sizeof(LenType) <= size_micro_run_[i]) {
            LenType len = *(LenType *)(micro_buf_[i] + pos);
            if (pos + sizeof(LenType) + len <= size_micro_run_[i]) {
                num_micro_run_[i]++;
                last_pos = pos;
                pos += sizeof(LenType) + len;
            } else {
                break;
            }
        }
        // std::cout << "len " << len << " size_micro_run_[" << i << "] " << size_micro_run_[i] << std::endl;
        assert(last_pos != (u32)-1); // buffer too small that can't hold one record
        assert(last_pos + sizeof(LenType) <= size_micro_run_[i]);
        assert(pos <= size_micro_run_[i]);
        LenType len = (LenType)(pos - last_pos);
        char *tmp = (char *)malloc(len);
        memcpy(tmp, micro_buf_[i] + last_pos, len);
        pre_heap_.push(KeyAddr(tmp, run_addr_[i] + pos, i));
        size_micro_run_[i] = pos;
    }
}
#endif

template <typename KeyType, typename LenType>
void SortMerger<KeyType, LenType>::Predict(DirectIO &io_stream) {
    while (pre_heap_.size() > 0) {
        KeyAddr top = pre_heap_.top();
        pre_heap_.pop();
        u64 addr = top.ADDR();
        u32 idx = top.IDX();
        free(top.data);

        std::unique_lock lock(pre_buf_mtx_);

        while (pre_buf_size_ != 0)
            pre_buf_con_.wait(lock);

        assert(idx < MAX_GROUP_SIZE_);
        // get loading size of a microrun
        u32 s;
        s = (u32)((u64)size_run_[idx] - (addr - run_addr_[idx]));

        if (s == 0) {
            continue;
        }
        s = s > PRE_BUF_SIZE_ ? PRE_BUF_SIZE_ : s;

        // load microrun
        io_stream.Seek(addr);
        s = io_stream.Read(pre_buf_, s);
        size_loaded_run_[idx] += s;
        run_curr_addr_[idx] = io_stream.Tell();

        u32 pos = 0;
        u32 last_pos = -1;
        pre_buf_num_ = 0;
        while (1) {
            if (pos + sizeof(LenType) > s) {
                // the last record of this microrun
                IASSERT(last_pos != (u32)-1); // buffer too small that can't hold one record
                LenType len = *(LenType *)(pre_buf_ + last_pos) + sizeof(LenType);
                char *tmp = (char *)malloc(len);
                memcpy(tmp, pre_buf_ + last_pos, len);
                pre_heap_.push(KeyAddr(tmp, addr + (u64)pos, idx));
                break;
            }
            LenType len = *(LenType *)(pre_buf_ + pos);
            if (pos + sizeof(LenType) + len > s) {
                // the last record of this microrun
                IASSERT(last_pos != (u32)-1); // buffer too small that can't hold one record
                len = *(LenType *)(pre_buf_ + last_pos) + sizeof(LenType);
                char *tmp = (char *)malloc(len);
                memcpy(tmp, pre_buf_ + last_pos, len);
                pre_heap_.push(KeyAddr(tmp, addr + (u64)pos, idx));
                break;
            }

            ++pre_buf_num_;
            last_pos = pos;
            pos += sizeof(LenType) + len;
        }
        pre_buf_size_ = pos;
        pre_buf_con_.notify_one();
    }
    {
        std::unique_lock lock(pre_buf_mtx_);
        pre_buf_size_ = -1;
        pre_buf_con_.notify_one();
    }

    // LOG_INFO("Predicting is over...");
}

template <typename KeyType, typename LenType>
void SortMerger<KeyType, LenType>::Merge() {
#ifdef USE_LOSER_TREE
    while (merge_loser_tree_->TopSource() != LoserTree<KeyAddr>::invalid_) {
        auto top = merge_loser_tree_->TopKey();
#else
    while (merge_heap_.size() > 0) {
        KeyAddr top = merge_heap_.top();
        merge_heap_.pop();
#endif
        u32 idx = top.IDX();

        // output
        while (1) {
            assert(out_buf_in_idx_ < OUT_BUF_NUM_);
            std::unique_lock lock(in_out_mtx_[out_buf_in_idx_]);
            while (out_buf_full_[out_buf_in_idx_])
                in_out_con_[out_buf_in_idx_].wait(lock);

            // if buffer is full
            if (top.LEN() + sizeof(LenType) + out_buf_size_[out_buf_in_idx_] > OUT_BUF_SIZE_ / OUT_BUF_NUM_) {
                IASSERT(out_buf_size_[out_buf_in_idx_] != 0); // output buffer chanel is smaller than size of a record
                out_buf_full_[out_buf_in_idx_] = true;
                u32 tmp = out_buf_in_idx_;
                ++out_buf_in_idx_;
                out_buf_in_idx_ %= OUT_BUF_NUM_;
                in_out_con_[tmp].notify_one();
                continue;
            }

            assert(out_buf_in_idx_ < OUT_BUF_NUM_);
            memcpy(sub_out_buf_[out_buf_in_idx_] + out_buf_size_[out_buf_in_idx_], top.data, top.LEN() + sizeof(LenType));
            out_buf_size_[out_buf_in_idx_] += top.LEN() + sizeof(LenType);

            break;
        }

        assert(idx < MAX_GROUP_SIZE_);
        // reach the end of a microrun
        if (micro_run_idx_[idx] == num_micro_run_[idx]) {
            IASSERT(micro_run_pos_[idx] <= size_micro_run_[idx]);
            std::unique_lock lock(pre_buf_mtx_);
            while (pre_buf_size_ == 0)
                pre_buf_con_.wait(lock);

            if (pre_buf_size_ == (u32)-1) {
#ifdef USE_LOSER_TREE
                merge_loser_tree_->DeleteTopInsert(nullptr, true);
#endif
                continue;
            }

            assert(idx < MAX_GROUP_SIZE_);
            memcpy(micro_buf_[idx], pre_buf_, pre_buf_size_);
            size_micro_run_[idx] = pre_buf_size_;
            num_micro_run_[idx] = pre_buf_num_;
            micro_run_pos_[idx] = micro_run_idx_[idx] = pre_buf_num_ = pre_buf_size_ = 0;
            pre_buf_con_.notify_one();
        }

        assert(idx < MAX_GROUP_SIZE_);
#ifdef USE_LOSER_TREE
        auto key = KeyAddr(micro_buf_[idx] + micro_run_pos_[idx], -1, idx);
        merge_loser_tree_->DeleteTopInsert(&key, false);
#else
        merge_heap_.push(KeyAddr(micro_buf_[idx] + micro_run_pos_[idx], -1, idx));
#endif
        ++micro_run_idx_[idx];
        micro_run_pos_[idx] += KeyAddr(micro_buf_[idx] + micro_run_pos_[idx], -1, idx).LEN() + sizeof(LenType);
    }
    {
        assert(out_buf_in_idx_ < OUT_BUF_NUM_);
        std::unique_lock lock(in_out_mtx_[out_buf_in_idx_]);
        if (!out_buf_full_[out_buf_in_idx_] && out_buf_size_[out_buf_in_idx_] > 0) {
            out_buf_full_[out_buf_in_idx_] = true;
            in_out_con_[out_buf_in_idx_].notify_one();
        }
    }
}

template <typename KeyType, typename LenType>
void SortMerger<KeyType, LenType>::Output(FILE *f, u32 idx) {
    DirectIO io_stream(f, "w");

    while (count_ > 0) {
        // wait its turn to output
        std::unique_lock out_lock(out_out_mtx_);
        while (out_buf_out_idx_ != idx)
            out_out_con_.wait(out_lock);

        if (count_ == 0) {
            ++out_buf_out_idx_;
            out_buf_out_idx_ %= OUT_BUF_NUM_;
            out_out_con_.notify_all();
            break;
        }

        assert(idx < OUT_BUF_NUM_);
        std::unique_lock in_lock(in_out_mtx_[idx]);
        while (!out_buf_full_[idx])
            in_out_con_[idx].wait(in_lock);

        IASSERT(out_buf_size_[idx] != 0);
        assert(idx < OUT_BUF_NUM_);
        for (u32 pos = 0; pos < out_buf_size_[idx]; --count_, pos += *(LenType *)(sub_out_buf_[idx] + pos) + sizeof(LenType))
            ;

        IASSERT(out_buf_size_[idx] <= OUT_BUF_SIZE_ / OUT_BUF_NUM_);
        io_stream.Write(sub_out_buf_[idx], out_buf_size_[idx]);
        out_buf_full_[idx] = false;
        out_buf_size_[idx] = 0;
        ++out_buf_out_idx_;
        out_buf_out_idx_ %= OUT_BUF_NUM_;

        out_out_con_.notify_all();
        in_out_con_[idx].notify_one();
    }
}

template <typename KeyType, typename LenType>
void SortMerger<KeyType, LenType>::Run() {
#ifdef USE_MMAP_IO
    MMappedIO io_stream(filenm_);
    FILE_LEN_ = io_stream.DataLen();
    io_stream.ReadU64(count_);
    Init(io_stream);
#else
    FILE *f = fopen(filenm_.c_str(), "r");

    DirectIO io_stream(f);
    FILE_LEN_ = io_stream.Length();

    io_stream.Read((char *)(&count_), sizeof(u64));

    Init(io_stream);

    Thread predict_thread(std::bind(&self_t::Predict, this, io_stream));
#endif

    Thread merge_thread(std::bind(&self_t::Merge, this));

    FILE *out_f = fopen((filenm_ + ".out").c_str(), "w+");
    IASSERT(out_f);
    IASSERT(fwrite(&count_, sizeof(u64), 1, out_f) == 1);

    Vector<Thread *> out_thread(OUT_BUF_NUM_);
    for (u32 i = 0; i < OUT_BUF_NUM_; ++i) {
        out_thread[i] = new Thread(std::bind(&self_t::Output, this, out_f, i));
    }
#ifndef USE_MMAP_IO
    predict_thread.join();
#endif
    merge_thread.join();
    for (u32 i = 0; i < OUT_BUF_NUM_; ++i) {
        out_thread[i]->join();
        delete out_thread[i];
    }
#ifndef USE_MMAP_IO
    fclose(f);
#endif
    fclose(out_f);

    if (std::filesystem::exists(filenm_))
        std::filesystem::remove(filenm_);
    if (std::filesystem::exists(filenm_ + ".out"))
        std::filesystem::rename(filenm_ + ".out", filenm_);
}

template class SortMerger<u32, u8>;
template class SortMerger<TermTuple, u32>;
} // namespace infinity