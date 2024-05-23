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
#include <cstring>
#include <sys/mman.h>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>

module external_sort_merger;

import stl;
import mmap;
import third_party;
import file_writer;
import local_file_system;
import profiler;
import logger;

namespace infinity {

#define IASSERT(exp)                                                                                                                                 \
    {                                                                                                                                                \
        if (!(exp)) {                                                                                                                                \
            assert(false);                                                                                                                           \
        }                                                                                                                                            \
    }

template <typename KeyType, typename LenType>
SortMerger<KeyType, LenType>::SortMerger(const char *filenm, u32 group_size, u32 bs, u32 output_num)
    : filenm_(filenm), MAX_GROUP_SIZE_(group_size), BS_SIZE_(bs), PRE_BUF_SIZE_((u32)(1. * bs * 0.8 / (group_size + 1))),
      RUN_BUF_SIZE_(PRE_BUF_SIZE_ * group_size), OUT_BUF_SIZE_(bs - RUN_BUF_SIZE_ - PRE_BUF_SIZE_), OUT_BUF_NUM_(output_num) {
    run_buf_ = out_buf_ = nullptr;
    count_ = 0;

    micro_run_idx_ = new u32[MAX_GROUP_SIZE_];
    micro_run_pos_ = new u32[MAX_GROUP_SIZE_];
    num_micro_run_ = new u32[MAX_GROUP_SIZE_];
    size_micro_run_ = new u32[MAX_GROUP_SIZE_];
    size_run_ = new u32[MAX_GROUP_SIZE_];
    run_addr_ = new u64[MAX_GROUP_SIZE_];

    micro_buf_ = new char *[MAX_GROUP_SIZE_];
    sub_out_buf_ = new char *[OUT_BUF_NUM_];

    in_out_mtx_ = new std::mutex[OUT_BUF_NUM_];
    in_out_con_ = new std::condition_variable[OUT_BUF_NUM_];

    out_buf_size_ = new u32[OUT_BUF_NUM_];
    out_buf_full_ = new bool[OUT_BUF_NUM_];


    CYCLE_BUF_SIZE_ = MAX_GROUP_SIZE_ * 4;
    CYCLE_BUF_THRESHOLD_ = MAX_GROUP_SIZE_ * 3;
    OUT_BATCH_SIZE_ = 10240;
    assert(CYCLE_BUF_THRESHOLD_ <= CYCLE_BUF_SIZE_);
    cycle_buffer_ = MakeUnique<CycleBuffer>(CYCLE_BUF_SIZE_, PRE_BUF_SIZE_);

    merge_loser_tree_ = MakeShared<LoserTree<KeyAddr>>(MAX_GROUP_SIZE_);
}

template <typename KeyType, typename LenType>
SortMerger<KeyType, LenType>::~SortMerger() {
    if (run_buf_) {
        free(run_buf_);
    }

    if (out_buf_) {
        free(out_buf_);
    }

    delete[] micro_run_idx_;
    delete[] micro_run_pos_;
    delete[] num_micro_run_;
    delete[] size_micro_run_;
    delete[] size_run_;
    delete[] run_addr_;

    delete[] micro_buf_;
    delete[] sub_out_buf_;

    delete[] in_out_mtx_;
    delete[] in_out_con_;

    delete[] out_buf_size_;
    delete[] out_buf_full_;
}

template <typename KeyType, typename LenType>
void SortMerger<KeyType, LenType>::NewBuffer() {
    if (!run_buf_) {
        run_buf_ = (char *)malloc(RUN_BUF_SIZE_);
    }

    if (!out_buf_) {
        out_buf_ = (char *)malloc(OUT_BUF_SIZE_);
    }
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
    for (u32 i = 1; i < MAX_GROUP_SIZE_; ++i) {
        micro_buf_[i] = micro_buf_[i - 1] + PRE_BUF_SIZE_;
    }

    //
    group_size_ = 0;
    u64 next_run_pos = 0;
    u32 num_run = 0;
    for (u32 i = 0; i < MAX_GROUP_SIZE_ && (u64)io_stream.Tell() < FILE_LEN_; ++i, ++group_size_) {
        // get the size of run
        io_stream.Read((char *)(size_run_ + i), sizeof(u32));
        // get the records number of a run
        io_stream.Read((char *)(&num_run), sizeof(u32));
        io_stream.Read((char *)(&next_run_pos), sizeof(u64));

        run_addr_[i] = io_stream.Tell(); // ftell(f);

        // loading size of a microrun
        u32 s = size_run_[i] > PRE_BUF_SIZE_ ? PRE_BUF_SIZE_ : size_run_[i];
        size_t ret = io_stream.Read(micro_buf_[i], s);
        size_micro_run_[i] = ret;

        /// it is not needed for compression, validation will be made within IOStream in that case
        // if a record can fit in microrun buffer
        bool flag = false;
        while (*(LenType *)(micro_buf_[i]) + sizeof(LenType) > s) {
            size_micro_run_[i] = 0;
            --count_;

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

        if (flag) {
            merge_loser_tree_->InsertStart(nullptr, static_cast<LoserTree<u64>::Source>(i), true);
            continue;
        }

        auto key = KeyAddr(micro_buf_[i], -1, i);
        merge_loser_tree_->InsertStart(&key, static_cast<LoserTree<u64>::Source>(i), false);

        micro_run_idx_[i] = 1;
        micro_run_pos_[i] = KeyAddr(micro_buf_[i], -1, i).LEN() + sizeof(LenType);
        num_micro_run_[i] = 0;

        io_stream.Seek(next_run_pos);
    }

    merge_loser_tree_->Init();

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

template <typename KeyType, typename LenType>
void SortMerger<KeyType, LenType>::Predict(DirectIO &io_stream) {
    UniquePtr<char[]> data_buf = MakeUnique<char[]>(PRE_BUF_SIZE_);
    while (pre_heap_.size() > 0) {
        KeyAddr top = pre_heap_.top();
        pre_heap_.pop();
        u64 addr = top.ADDR();
        u32 idx = top.IDX();
        free(top.data);
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
        io_stream.Read(data_buf.get(), s);
        auto data_ptr = data_buf.get();

        u32 pos = 0;
        u32 last_pos = -1;
        u32 pre_buf_num = 0;
        u32 pre_buf_size = 0;
        while (1) {
            if (pos + sizeof(LenType) > s) {
                // the last record of this microrun
                IASSERT(last_pos != (u32)-1); // buffer too small that can't hold one record
                LenType len = *(LenType *)(data_ptr + last_pos) + sizeof(LenType);
                char *tmp = (char *)malloc(len);
                memcpy(tmp, data_ptr + last_pos, len);
                pre_heap_.push(KeyAddr(tmp, addr + (u64)pos, idx));
                break;
            }
            LenType len = *(LenType *)(data_ptr + pos);
            if (pos + sizeof(LenType) + len > s) {
                IASSERT(last_pos != (u32)-1); // buffer too small that can't hold one record
                len = *(LenType *)(data_ptr + last_pos) + sizeof(LenType);
                char *tmp = (char *)malloc(len);
                memcpy(tmp, data_ptr + last_pos, len);
                pre_heap_.push(KeyAddr(tmp, addr + (u64)pos, idx));
                break;
            }

            ++pre_buf_num;
            last_pos = pos;
            pos += sizeof(LenType) + len;
        }
        pre_buf_size = pos;

        std::unique_lock lock(cycle_buf_mtx_);
        cycle_buf_con_.wait(lock, [this]() { return !this->cycle_buffer_->IsFull(); });

        cycle_buffer_->PutReal(data_buf, pre_buf_size, pre_buf_num);
        cycle_buf_con_.notify_one();
    }
    {
        std::unique_lock lock(cycle_buf_mtx_);
        read_finish_ = true;
        cycle_buf_con_.notify_one();
    }
}

template <typename KeyType, typename LenType>
void SortMerger<KeyType, LenType>::Merge() {
    while (merge_loser_tree_->TopSource() != LoserTree<KeyAddr>::invalid_) {
        auto top = merge_loser_tree_->TopKey();
        u32 idx = top.IDX();
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
            std::unique_lock lock(cycle_buf_mtx_);

            cycle_buf_con_.wait(lock, [this]() {
                return !this->cycle_buffer_->IsEmpty() || (this->read_finish_ && this->cycle_buffer_->IsEmpty());
            });

            if (cycle_buffer_->IsEmpty() && read_finish_) {
                merge_loser_tree_->DeleteTopInsert(nullptr, true);
                continue;
            }

            assert(idx < MAX_GROUP_SIZE_);
            auto res = cycle_buffer_->GetTuple();
            auto pre_buf_size = std::get<1>(res);
            auto pre_buf_num = std::get<2>(res);
            memcpy(micro_buf_[idx], std::get<0>(res), pre_buf_size);

            size_micro_run_[idx] = pre_buf_size;
            num_micro_run_[idx] = pre_buf_num;
            micro_run_pos_[idx] = micro_run_idx_[idx] = 0;

            if (cycle_buffer_->Size() < CYCLE_BUF_THRESHOLD_) {
                cycle_buf_con_.notify_one();
            }
        }

        assert(idx < MAX_GROUP_SIZE_);
        auto key = KeyAddr(micro_buf_[idx] + micro_run_pos_[idx], -1, idx);
        merge_loser_tree_->DeleteTopInsert(&key, false);
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
void SortMerger<KeyType, LenType>::OutputByQueue(FILE *f) {
    DirectIO io_stream(f, "w");
    while (count_ > 0) {
        // wait its turn to output
        Queue<UniquePtr<char_t[]>> temp_out_queue;
        Queue<u32> temp_out_size_queue;
        {
            std::unique_lock out_lock(out_queue_mtx_);
            out_queue_con_.wait(out_lock, [this]() { return !this->out_queue_.empty(); });

            if (count_ == 0) {
                break;
            }

            auto write_cnt = OUT_BATCH_SIZE_;

            while (count_ > 0 && write_cnt > 0 && !out_queue_.empty()) {
                temp_out_queue.push(std::move(out_queue_.front()));
                temp_out_size_queue.push(out_size_queue_.front());
                out_queue_.pop();
                out_size_queue_.pop();

                --count_;
                --write_cnt;
            }
        }
        assert(temp_out_queue.size() == temp_out_size_queue.size());
        while(temp_out_queue.size()) {
            auto top_data = std::move(temp_out_queue.front());
            auto data_len = temp_out_size_queue.front();
            temp_out_queue.pop();
            temp_out_size_queue.pop();
            io_stream.Write(top_data.get(), data_len);
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
    FILE *f = fopen(filenm_.c_str(), "r");

    DirectIO io_stream(f);
    FILE_LEN_ = io_stream.Length();

    io_stream.Read((char *)(&count_), sizeof(u64));

    Init(io_stream);

    Thread predict_thread(std::bind(&self_t::Predict, this, io_stream));
    Thread merge_thread(std::bind(&self_t::Merge, this));
    FILE *out_f = fopen((filenm_ + ".out").c_str(), "w+");
    IASSERT(out_f);
    IASSERT(fwrite(&count_, sizeof(u64), 1, out_f) == 1);

    Vector<Thread *> out_thread(OUT_BUF_NUM_);
    for (u32 i = 0; i < OUT_BUF_NUM_; ++i) {
        out_thread[i] = new Thread(std::bind(&self_t::Output, this, out_f, i));
    }

    predict_thread.join();
    merge_thread.join();
    for (u32 i = 0; i < OUT_BUF_NUM_; ++i) {
        out_thread[i]->join();
        delete out_thread[i];
    }
    fclose(f);
    fclose(out_f);

    if (std::filesystem::exists(filenm_)) {
        std::filesystem::remove(filenm_);
    }
    if (std::filesystem::exists(filenm_ + ".out")) {
        std::filesystem::rename(filenm_ + ".out", filenm_);
    }
}

template <typename KeyType, typename LenType>
requires std::same_as<KeyType, TermTuple>
void SortMergerTermTuple<KeyType, LenType>::MergeImpl() {
    UniquePtr<TermTupleList> tuple_list = nullptr;
    u32 last_idx = -1;
    while (merge_loser_tree_->TopSource() != LoserTree<KeyAddr>::invalid_) {
        auto top = merge_loser_tree_->TopKey();
        u32 idx = top.IDX();
        auto out_key = top.KEY();
        if (tuple_list == nullptr) {
            tuple_list = MakeUnique<TermTupleList>(out_key.term_);
            tuple_list->Add(out_key.doc_id_, out_key.term_pos_);
        } else if (idx != last_idx) {
            if (tuple_list->IsFull() || out_key.term_ != tuple_list->term_) {
                // output
                {
                    std::unique_lock lock(out_queue_mtx_);
                    term_tuple_list_queue_.push(std::move(tuple_list));
                    out_queue_con_.notify_one();
                }
                tuple_list = MakeUnique<TermTupleList>(out_key.term_);
                tuple_list->Add(out_key.doc_id_, out_key.term_pos_);
            } else {
                tuple_list->Add(out_key.doc_id_, out_key.term_pos_);
            }
        }

        assert(idx < MAX_GROUP_SIZE_);

        if (micro_run_idx_[idx] == num_micro_run_[idx]) {
            IASSERT(micro_run_pos_[idx] <= size_micro_run_[idx]);
            std::unique_lock lock(cycle_buf_mtx_);

            cycle_buf_con_.wait(lock, [this]() {
                return !this->cycle_buffer_->IsEmpty() || (this->read_finish_ && this->cycle_buffer_->IsEmpty());
            });

            if (cycle_buffer_->IsEmpty() && read_finish_) {
                merge_loser_tree_->DeleteTopInsert(nullptr, true);
                continue;
            }

            assert(idx < MAX_GROUP_SIZE_);
            auto res = cycle_buffer_->GetTuple();
            auto pre_buf_size = std::get<1>(res);
            auto pre_buf_num = std::get<2>(res);
            memcpy(micro_buf_[idx], std::get<0>(res), pre_buf_size);

            size_micro_run_[idx] = pre_buf_size;
            num_micro_run_[idx] = pre_buf_num;
            micro_run_pos_[idx] = micro_run_idx_[idx] = 0;

            if (cycle_buffer_->Size() < CYCLE_BUF_THRESHOLD_) {
                cycle_buf_con_.notify_one();
            }
        }

        assert(idx < MAX_GROUP_SIZE_);
        auto key = KeyAddr(micro_buf_[idx] + micro_run_pos_[idx], -1, idx);
        merge_loser_tree_->DeleteTopInsert(&key, false);

        ++micro_run_idx_[idx];
        micro_run_pos_[idx] += KeyAddr(micro_buf_[idx] + micro_run_pos_[idx], -1, idx).LEN() + sizeof(LenType);
    }
    {
        std::unique_lock lock(out_queue_mtx_);
        if (tuple_list != nullptr) {
            term_tuple_list_queue_.push(std::move(tuple_list));
        }
        out_queue_con_.notify_one();
    }
}

template <typename KeyType, typename LenType>
requires std::same_as<KeyType, TermTuple>
void SortMergerTermTuple<KeyType, LenType>::OutputImpl(FILE *f) {
    DirectIO io_stream(f, "w");
    while (count_ > 0) {
        UniquePtr<TermTupleList> temp_term_tuple;
        {
            std::unique_lock out_lock(out_queue_mtx_);
            out_queue_con_.wait(out_lock, [this]() { return !this->term_tuple_list_queue_.empty(); });

            if (count_ == 0) {
                break;
            }

            temp_term_tuple = std::move(term_tuple_list_queue_.front());
            ++term_list_count_;
            term_tuple_list_queue_.pop();
        }
        count_ -= temp_term_tuple->Size();

        // output format
        // |   u32    |    u32   |     u32       |  char [term_len]  | pair<u32, u32> [doc_list_size]
        // | data_len | term_len | doc_list_size |       term        |      [doc_id, term_pos]...
        u32 term_len = temp_term_tuple->term_.size();
        u32 doc_list_size = temp_term_tuple->Size();
        u32 data_len = sizeof(u32) + sizeof(u32) + term_len + 2 * sizeof(u32) * doc_list_size;

        char buf[20];
        auto SIZE_U32 = sizeof(u32);
        memcpy(buf, &data_len, SIZE_U32);
        memcpy(buf + SIZE_U32, &term_len, SIZE_U32);
        memcpy(buf + SIZE_U32 + SIZE_U32, &doc_list_size, SIZE_U32);
        io_stream.Write(buf, SIZE_U32 * 3);
        io_stream.Write(temp_term_tuple->term_.data(), term_len);
        io_stream.Write((char*)temp_term_tuple->doc_pos_list_.data(), SIZE_U32 * 2 * doc_list_size);
        if (count_ == 0) {
            io_stream.Seek(0, SEEK_SET);
            io_stream.Write((char*)(&term_list_count_), sizeof(u64));
            term_list_count_ = 0;
            break;
        }
    }
}

template <typename KeyType, typename LenType>
requires std::same_as<KeyType, TermTuple>
void SortMergerTermTuple<KeyType, LenType>::PredictImpl(DirectIO &io_stream) {
    this->Predict(io_stream);
}

template <typename KeyType, typename LenType>
requires std::same_as<KeyType, TermTuple>
void SortMergerTermTuple<KeyType, LenType>::Run() {
    FILE *f = fopen(filenm_.c_str(), "r");

    DirectIO io_stream(f);
    FILE_LEN_ = io_stream.Length();

    term_list_count_ = 0;
    io_stream.Read((char *)(&count_), sizeof(u64));

    Super::Init(io_stream);

    Thread predict_thread(std::bind(&self_t::PredictImpl, this, io_stream));
    Thread merge_thread(std::bind(&self_t::MergeImpl, this));
    FILE *out_f = fopen((filenm_ + ".out").c_str(), "w+");
    IASSERT(out_f);
    IASSERT(fwrite(&count_, sizeof(u64), 1, out_f) == 1);

    Thread out_thread(std::bind(&self_t::OutputImpl, this, out_f));

    predict_thread.join();
    merge_thread.join();
    out_thread.join();

    fclose(f);
    fclose(out_f);

    if (std::filesystem::exists(filenm_)) {
        std::filesystem::remove(filenm_);
    }
    if (std::filesystem::exists(filenm_ + ".out")) {
        std::filesystem::rename(filenm_ + ".out", filenm_);
    }
}


template class SortMerger<u32, u8>;
template class SortMerger<TermTuple, u32>;
template class SortMergerTermTuple<TermTuple, u32>;
} // namespace infinity