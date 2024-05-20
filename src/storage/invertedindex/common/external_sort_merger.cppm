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

#include <filesystem>
#include <queue>
#include <cstring>

export module external_sort_merger;

import stl;
import loser_tree;
import mmap;
import infinity_exception;
import file_writer;

namespace infinity {

export struct DirectIO {
    DirectIO(FILE *fd, const String &mode = "r") : fd_(fd), length_(0) {
        if (mode.compare("r") == 0) {
            fseek(fd_, 0, SEEK_END);
            length_ = ftell(fd_);
            fseek(fd_, 0, SEEK_SET);
        }
    }

    SizeT Write(char *const data, SizeT length) { return fwrite(data, length, 1, fd_); }

    SizeT Read(char *data, SizeT length) { return fread(data, 1, length, fd_); }

    SizeT Tell() { return ftell(fd_); }

    void Seek(SizeT pos, int origin = SEEK_SET) { fseek(fd_, pos, origin); }

    SizeT Length() { return length_; }

    FILE *fd_;

    SizeT length_;
};

export struct TermTuple {
    std::string_view term_;
    u32 doc_id_;
    u32 term_pos_;
    int Compare(const TermTuple &rhs) const {
        int ret = term_.compare(rhs.term_);

        if (ret == 0) {
            if (doc_id_ != rhs.doc_id_) {
                if (doc_id_ < rhs.doc_id_)
                    return -1;
                else
                    return 1;
            } else {
                if (term_pos_ != rhs.term_pos_) {
                    if (term_pos_ < rhs.term_pos_)
                        return -1;
                    else
                        return 1;
                }
                return 0;
            }
        } else
            return ret < 0 ? -1 : 1;
    }

    bool operator==(const TermTuple &other) const { return Compare(other) == 0; }

    bool operator>(const TermTuple &other) const { return Compare(other) > 0; }

    bool operator<(const TermTuple &other) const { return Compare(other) < 0; }

    TermTuple(char *p, u32 len) : term_(p, len - sizeof(doc_id_) - sizeof(term_pos_) - 1) {
        doc_id_ = *((u32 *)(p + term_.size() + 1));
        term_pos_ = *((u32 *)(p + term_.size() + 1 + sizeof(doc_id_)));
    }
};

template <typename KeyType, typename LenType, typename = void>
struct KeyAddress {
    char *data{nullptr};
    u64 addr;
    u32 idx;

    KeyAddress(char *p, u64 ad, u32 i) {
        data = p;
        addr = ad;
        idx = i;
    }

    KeyAddress() {
        data = nullptr;
        addr = -1;
        idx = -1;
    }

    KeyType KEY() { return KeyType(data + sizeof(LenType), LEN()); }
    KeyType KEY() const { return KeyType(data + sizeof(LenType), LEN()); }
    LenType LEN() const { return *(LenType *)data; }
    u64 &ADDR() { return addr; }
    u64 ADDR() const { return addr; }
    u32 IDX() const { return idx; }
    u32 &IDX() { return idx; }

    int Compare(const KeyAddress &p) const {
        if (KEY() == p.KEY())
            return 0;
        else if (KEY() > p.KEY())
            return 1;
        else
            return -1;
    }

    bool operator==(const KeyAddress &other) const { return Compare(other) == 0; }

    bool operator>(const KeyAddress &other) const { return Compare(other) < 0; }

    bool operator<(const KeyAddress &other) const { return Compare(other) > 0; }
};

template <typename KeyType, typename LenType>
struct KeyAddress<KeyType, LenType, typename std::enable_if<std::is_scalar<KeyType>::value>::type> {
    char *data{nullptr};
    u64 addr;
    u32 idx;

    KeyAddress(char *p, u64 ad, u32 i) {
        data = p;
        addr = ad;
        idx = i;
    }

    KeyAddress() {
        data = nullptr;
        addr = -1;
        idx = -1;
    }

    KeyType &KEY() { return *(KeyType *)(data + sizeof(LenType)); }
    KeyType KEY() const { return *(KeyType *)(data + sizeof(LenType)); }

    LenType LEN() const { return *(LenType *)data; }
    u64 &ADDR() { return addr; }
    u64 ADDR() const { return addr; }
    u32 IDX() const { return idx; }
    u32 &IDX() { return idx; }

    int Compare(const KeyAddress &p) const {
        if (KEY() == p.KEY())
            return 0;
        else if (KEY() > p.KEY())
            return 1;
        else
            return -1;
    }

    bool operator==(const KeyAddress &other) const { return Compare(other) == 0; }

    bool operator>(const KeyAddress &other) const { return Compare(other) < 0; }

    bool operator<(const KeyAddress &other) const { return Compare(other) > 0; }
};

template <typename LenType>
struct KeyAddress<TermTuple, LenType> {
    char *data{nullptr};
    u64 addr;
    u32 idx;

    KeyAddress(char *p, u64 ad, u32 i) {
        data = p;
        addr = ad;
        idx = i;
    }

    KeyAddress() {
        data = nullptr;
        addr = -1;
        idx = -1;
    }

    TermTuple KEY() { return TermTuple(data + sizeof(LenType), LEN()); }
    TermTuple KEY() const { return TermTuple(data + sizeof(LenType), LEN()); }
    LenType LEN() const { return *(LenType *)data; }
    u64 &ADDR() { return addr; }
    u64 ADDR() const { return addr; }
    u32 IDX() const { return idx; }
    u32 &IDX() { return idx; }

    int Compare(const KeyAddress &p) const {
        return KEY().Compare(p.KEY());
    }

    bool operator==(const KeyAddress &other) const { return Compare(other) == 0; }

    bool operator>(const KeyAddress &other) const { return Compare(other) < 0; }

    bool operator<(const KeyAddress &other) const { return Compare(other) > 0; }
};

class CycleBuffer {
public:
    CycleBuffer(SizeT total_buffers, SizeT buffer_size)
        : total_buffers_(total_buffers), buffer_size_(buffer_size), head_(0), tail_(0), full_(false) {
        buffer_array_.resize(total_buffers);
        buffer_real_size_.resize(total_buffers);
        buffer_real_num_.resize(total_buffers);
        for (auto& buf : buffer_array_) {
            buf = MakeUnique<char[]>(buffer_size);
        }
    }

    void Put(const char* data, SizeT length) {
        if (length > buffer_size_) {
            throw std::runtime_error("Data length exceeds buffer capacity");
        }
        if (IsFull()) {
            throw std::runtime_error("Buffer is full");
        }

        // Copy data into the current buffer
        std::memcpy(buffer_array_[head_].get(), data, length);
        head_ = (head_ + 1) % total_buffers_;

        if (head_ == tail_) {
            full_ = true;
        }
    }

    void PutReal(const u32& real_size, const u32& real_num) {
        buffer_real_size_[head_] = real_size;
        buffer_real_num_[head_] = real_num;

        head_ = (head_ + 1) % total_buffers_;

        if (head_ == tail_) {
            full_ = true;
        }
    }

    char* PutByRead(DirectIO &io_stream, u32& length) {
        if (length > buffer_size_) {
            throw std::runtime_error("Data length exceeds buffer capacity");
        }
        if (IsFull()) {
            throw std::runtime_error("Buffer is full");
        }

        // Read data into the current buffer
        length = io_stream.Read(buffer_array_[head_].get(), length);
        return buffer_array_[head_].get();
//        auto res_ptr = buffer_array_[head_].get();
//        head_ = (head_ + 1) % total_buffers_;
//
//        if (head_ == tail_) {
//            full_ = true;
//        }
//        return res_ptr;
    }

    Tuple<const char*, u32, u32> Get() {
        // std::cout << "CycleBuffer::Get" << std::endl;
        if (IsEmpty()) {
            throw std::runtime_error("Buffer is empty");
        }

        const char* result_data = buffer_array_[tail_].get();
        auto result_real_size = buffer_real_size_[tail_];
        auto result_real_num = buffer_real_num_[tail_];
        tail_ = (tail_ + 1) % total_buffers_;
        full_ = false;

        return std::make_tuple(result_data, result_real_size, result_real_num);
    }

    void Reset() {
        head_ = tail_ = 0;
        full_ = false;
    }

    bool IsEmpty() const {
        return (!full_ && (head_ == tail_));
    }

    bool IsFull() const {
        return full_;
    }

    SizeT Size() const {
        if (full_) {
            return total_buffers_;
        }
        if (head_ >= tail_) {
            return head_ - tail_;
        }
        return total_buffers_ + head_ - tail_;
    }

private:
    Vector<UniquePtr<char[]>> buffer_array_;
    Vector<u32> buffer_real_size_;
    Vector<u32> buffer_real_num_;
    SizeT total_buffers_;
    SizeT buffer_size_;
    SizeT head_;
    SizeT tail_;
    bool full_;
};

export template <typename KeyType, typename LenType>
class SortMerger {
    typedef SortMerger<KeyType, LenType> self_t;
    typedef KeyAddress<KeyType, LenType> KeyAddr;
    static constexpr SizeT MAX_TUPLE_LENGTH = 1024;
    String filenm_;
    const u32 MAX_GROUP_SIZE_; //!< max group size
    const u32 BS_SIZE_;        //!< in fact it equals to memory size
    u32 PRE_BUF_SIZE_;         //!< max predict buffer size
    u32 RUN_BUF_SIZE_;         //!< max run buffer size
    u32 OUT_BUF_SIZE_;         //!< max size of output buffer
    const u32 OUT_BUF_NUM_;    //!< output threads number

    std::priority_queue<KeyAddr> pre_heap_;   //!< predict heap
    std::priority_queue<KeyAddr> merge_heap_; //!< merge heap
    SharedPtr<LoserTree<KeyAddr>> merge_loser_tree_;

    u32 *micro_run_idx_{nullptr};   //!< the access index of each microruns
    u32 *micro_run_pos_{nullptr};   //!< the access position within each microruns
    u32 *num_micro_run_{nullptr};   //!< the records number of each microruns
    u32 *size_micro_run_{nullptr};  //!< the size of entire microrun
    u32 *num_run_{nullptr};         //!< records number of each runs
    u32 *size_run_{nullptr};        //!< size of each entire runs
    u32 *size_loaded_run_{nullptr}; //!< size of data that have been read within each entire runs
    u64 *run_addr_{nullptr};        //!< start file address of each runs
    u64 *run_curr_addr_{nullptr};   //!< current file address of each runs

    char **micro_buf_{nullptr};   //!< address of every microrun channel buffer
    char **sub_out_buf_{nullptr}; //!< addresses of each output buffer

    char *pre_buf_{nullptr}; //!< predict buffer
    char *run_buf_{nullptr}; //!< entire run buffer
    char *out_buf_{nullptr}; //!< the entire output buffer

    std::mutex pre_buf_mtx_; //!< mutex and condition for predict buffer accessed by merge and predict thread
    std::condition_variable pre_buf_con_;

    std::mutex *in_out_mtx_{nullptr};              //!< mutex and condition for output buffers accessed by output threads and merge threads
    std::condition_variable *in_out_con_{nullptr}; //!<

    std::mutex out_out_mtx_; //!< mutex and condition to ensure the seqence of file writing of all the output threads
    std::condition_variable out_out_con_;

    u32 pre_buf_size_; //!< the current size of microrun that has been loaded onto prediect buffer
    u32 pre_buf_num_;  //!< the current records number of microrun that has been loaded onto prediect buffer
    //u32 pre_idx_;    //!< the index of microrun channel right in the predict buffer
    u32 out_buf_in_idx_;          //!< used by merge to get the current available output buffer
    u32 out_buf_out_idx_;         //!< used by output threads to get the index of the turn of outputting
    u32 *out_buf_size_{nullptr};  //!< data size of each output buffer
    bool *out_buf_full_{nullptr}; //!< a flag to ensure if the output buffer is full or not

    Vector<u64> curr_addr_;
    Vector<u64> end_addr_;
    Vector<UniquePtr<char_t[]>> key_buf_;
    Vector<char*> key_buf_ptr_;
    Vector<SharedPtr<MmapReader>> mmap_io_streams_;
    UniquePtr<CycleBuffer> cycle_buffer_;
    std::mutex cycle_buf_mtx_;
    std::condition_variable cycle_buf_con_;
    bool read_finish_{false};
    u32 CYCLE_BUF_SIZE_;
    u32 CYCLE_BUF_THRESHOLD_;
    u64 count_;      //!< records number
    u32 group_size_; //!< the real run number that can get from the input file.

    u64 FILE_LEN_;

    void NewBuffer();

    void Init(DirectIO &io_stream);

    void Predict(DirectIO &io_stream);

    void PredictByQueue(DirectIO &io_stream);

    void Merge();

    void MergeMmap(MmapReader &io_stream, SharedPtr<FileWriter> out_file_writer);

    void MergeByQueue();

    void Output(FILE *f, u32 idx);

    void Init(MmapReader &io_stream);

    void ReadKeyAt(MmapReader &io_stream, u64 pos);

    void ReadKeyAtNonCopy(MmapReader &io_stream, u64 pos);
public:
    SortMerger(const char *filenm, u32 group_size = 4, u32 bs = 100000000, u32 output_num = 2);

    ~SortMerger();

    void SetParams(u32 max_record_len) {
        if (max_record_len > PRE_BUF_SIZE_) {
            PRE_BUF_SIZE_ = max_record_len;
            RUN_BUF_SIZE_ = PRE_BUF_SIZE_ * MAX_GROUP_SIZE_;
            // OUT_BUF_SIZE_ = BS_SIZE_ - RUN_BUF_SIZE_ - PRE_BUF_SIZE_; ///we do not change OUT_BUF_SIZE_
        }
    }

    void SetParams(u32 max_record_len, u32 min_buff_size_required) {
        if (max_record_len > PRE_BUF_SIZE_)
            PRE_BUF_SIZE_ = max_record_len;
        if (RUN_BUF_SIZE_ < min_buff_size_required)
            RUN_BUF_SIZE_ = min_buff_size_required;
        if (RUN_BUF_SIZE_ < PRE_BUF_SIZE_ * MAX_GROUP_SIZE_)
            RUN_BUF_SIZE_ = PRE_BUF_SIZE_ * MAX_GROUP_SIZE_;
        if (OUT_BUF_SIZE_ < min_buff_size_required)
            OUT_BUF_SIZE_ = min_buff_size_required;
    }

    void Run();
};

} // namespace infinity
