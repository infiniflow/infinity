////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2016 by EMC Corporation, All Rights Reserved
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is EMC Corporation
///
/// @author Andrey Abramov
////////////////////////////////////////////////////////////////////////////////
#include "dictionary.h"

#include "common/utility/builtin.h"
#include "storage/invertedindex/common/fst/fst_utils.h"

#include <fst/matcher.h>

#include "storage/invertedindex/common/fst/bits_utils.h"
#include "storage/invertedindex/common/fst/fst_builder.h"
#include "storage/invertedindex/common/fst/fst_decl.h"
#include "storage/invertedindex/common/fst/immutable_fst.h"
#include "storage/invertedindex/common/fst/matcher.h"
#include "storage/invertedindex/common/fst/string_types.h"
#include "storage/invertedindex/common/fst/string_weight.h"
#include "storage/invertedindex/common/fst/stringref_weight.h"
#include "storage/invertedindex/format/term_meta.h"

#include <string>
#include <vector>

namespace infinity {

template <typename Char>
class VolatileRef {
public:
    using ref_t = basic_string_view<Char>;
    using str_t = basic_string<Char>;

    VolatileRef() = default;

    VolatileRef(VolatileRef &&rhs) noexcept : str_(std::move(rhs.str_)), ref_(str_.empty() ? rhs.ref_ : ref_t(str_)) { rhs.ref_ = ref_; }

    VolatileRef &operator=(VolatileRef &&rhs) noexcept {
        if (this != &rhs) {
            str_ = std::move(rhs.str_);
            ref_ = (str_.empty() ? rhs.ref_ : ref_t(str_));
            rhs.ref_ = ref_;
        }
        return *this;
    }

    void clear() {
        str_.clear();
        ref_ = {};
    }

    void assign(ref_t str) {
        str_.assign(str.data(), str.size());
        ref_ = str_;
    }

    void assign(ref_t str, Char label) {
        str_.resize(str.size() + 1);
        std::memcpy(str_.data(), str.data(), str.size() * sizeof(Char));
        str_[str.size()] = label;
        ref_ = str_;
    }

    ref_t view() const noexcept { return ref_; }

    operator ref_t() const noexcept { return ref_; }

private:
    str_t str_;
    ref_t ref_{};
};

using BytesRef = VolatileRef<byte_type>;

template <typename T>
struct Node {
    T *next_ = nullptr;
};

template <typename T>
struct IntrusiveList {
public:
    IntrusiveList &operator=(const IntrusiveList &) = delete;
    IntrusiveList(const IntrusiveList &) = delete;
    IntrusiveList() noexcept = default;
    IntrusiveList(IntrusiveList &&other) noexcept : tail_{std::exchange(other.tail_, nullptr)} {}

    IntrusiveList &operator=(IntrusiveList &&other) noexcept {
        std::swap(tail_, other.tail_);
        return *this;
    }

    void Append(IntrusiveList &&rhs) noexcept {
        if (rhs.tail_ == nullptr) {
            return;
        }
        if (tail_ == nullptr) {
            tail_ = rhs.tail_;
            rhs.tail_ = nullptr;
            return;
        }
        auto *head = tail_->next_;
        tail_->next_ = rhs.tail_->next_;
        rhs.tail_->next_ = head;
        tail_ = rhs.tail_;
        rhs.tail_ = nullptr;
    }

    void PushFront(T &front) noexcept {
        assert(front.next_ == &front);
        if (likely(tail_ == nullptr)) {
            tail_ = &front;
            return;
        }
        front.next_ = tail_->next_;
        tail_->next_ = &front;
    }

    template <typename Func>
    void Visit(Func &&func) const {
        if (likely(tail_ == nullptr)) {
            return;
        }
        auto *head = tail_->next_;
        auto *it = head;
        do {
            func(*std::exchange(it, it->next_));
        } while (it != head);
    }

    T *tail_ = nullptr;
};

struct Block {
    struct PrefixedOutput : Node<PrefixedOutput> {
        explicit PrefixedOutput(BytesRef &&prefix) : Node<PrefixedOutput>{this}, prefix_(std::move(prefix)) {}
        void WriteByte(byte_type b) { weight_.PushBack(b); }
        void WriteBytes(const byte_type *b, size_t len) { weight_.PushBack(b, b + len); }

        BytesRef prefix_;
        ByteWeight weight_;
    };

    using BlockIndex = IntrusiveList<PrefixedOutput>;

    static constexpr uint16_t INVALID_LABEL{std::numeric_limits<uint16_t>::max()};

    Block(BlockIndex &&other, uint64_t block_start, uint8_t meta, uint16_t label)
        : index_(std::move(other)), start_(block_start), label_(label), meta_(meta) {}

    Block(Block &&rhs) noexcept : index_(std::move(rhs.index_)), start_(rhs.start_), label_(rhs.label_), meta_(rhs.meta_) {}

    Block &operator=(Block &&rhs) {
        if (this != &rhs) {
            index_ = std::move(rhs.index_);
            start_ = rhs.start_;
            label_ = rhs.label_;
            meta_ = rhs.meta_;
        }
        return *this;
    };
    ~Block() {
        index_.Visit([](PrefixedOutput &output) { output.~PrefixedOutput(); });
    }

    BlockIndex index_; // fst index
    uint16_t start_;   // file pointer
    uint16_t label_;   // block read label
    uint8_t meta_;     // block metada
};

template <typename... Types>
struct template_traits_t;

template <typename First, typename... Second>
struct template_traits_t<First, Second...> {
    static constexpr size_t AlignMax(size_t max = 0) noexcept {
        return template_traits_t<Second...>::AlignMax(std::max(max, std::alignment_of_v<First>));
    }
    static constexpr size_t SizeMax(size_t max = 0) noexcept { return template_traits_t<Second...>::SizeMax(std::max(max, sizeof(First))); }
};

template <>
struct template_traits_t<> {
    static constexpr size_t AlignMax(size_t max = 0) noexcept { return max; }

    static constexpr size_t SizeMax(size_t max = 0) noexcept { return max; }
};

template <typename... Types>
struct AlignedUnion {
    struct type {
        alignas(template_traits_t<Types...>::AlignMax()) char raw[template_traits_t<Types...>::SizeMax()];
    };
    static constexpr size_t alignment_value = alignof(type);
    static constexpr size_t size_value = sizeof(type);
};

// Provides the storage (POD type) that is suitable for use as
// uninitialized storage for an object of  any of the specified Types
template <typename... Types>
struct AlignedType {
    template <typename T>
    T *As() noexcept {
        return reinterpret_cast<T *>(&storage);
    }

    template <typename T>
    const T *As() const noexcept {
        return const_cast<AlignedType &>(*this).As<T>();
    }

    template <typename T, typename... Args>
    void Construct(Args &&...args) {
        new (As<T>()) T(std::forward<Args>(args)...);
    }

    template <typename T>
    void Destroy() noexcept {
        As<T>()->~T();
    }

    typename AlignedUnion<Types...>::type storage;
};

enum EntryType : uint8_t { TERM = 0, BLOCK, INVALID };

class Entry {
public:
    Entry(bytes_view term, TermMeta &&meta) : type_(TERM) { data_.assign(term); }
    Entry(bytes_view prefix, Block::BlockIndex &&index, uint64_t block_start, uint8_t meta, uint16_t label) : type_(BLOCK) {
        if (Block::INVALID_LABEL != label) {
            data_.assign(prefix, static_cast<byte_type>(label & 0xFF));
        } else {
            data_.assign(prefix);
        }
    }
    Entry(Entry &&rhs) noexcept : data_(std::move(rhs.data_)) { MoveUnion(std::move(rhs)); }

    Entry &operator=(Entry &&rhs) noexcept {
        if (this != &rhs) {
            data_ = std::move(rhs.data_);
            Destroy();
            MoveUnion(std::move(rhs));
        }

        return *this;
    }

    const TermMeta &term() const noexcept { return *mem_.As<TermMeta>(); }

    TermMeta &term() noexcept { return *mem_.As<TermMeta>(); }

    const Block &block() const noexcept { return *mem_.As<Block>(); }
    Block &block() noexcept { return *mem_.As<Block>(); }

    const BytesRef &data() const noexcept { return data_; }
    BytesRef &data() noexcept { return data_; }

    EntryType type() const noexcept { return type_; }

    void MoveUnion(Entry &&rhs) noexcept {
        type_ = rhs.type_;
        switch (type_) {
            case TERM:
                mem_.Construct<TermMeta>(std::move(rhs.term()));
                rhs.mem_.Destroy<TermMeta>();
                break;
            case BLOCK:
                mem_.Construct<Block>(std::move(rhs.block()));
                rhs.mem_.Destroy<Block>();
                break;
            default:
                break;
        }
        rhs.type_ = INVALID;
    }

    void Destroy() noexcept {
        switch (type_) {
            case TERM:
                mem_.Destroy<TermMeta>();
                break;
            case BLOCK:
                mem_.Destroy<Block>();
                break;
            default:
                break;
        }
        type_ = INVALID;
    }

    ~Entry() noexcept { Destroy(); }

private:
    BytesRef data_;
    AlignedType<TermMeta, Block> mem_;
    EntryType type_;
};

struct BlockMeta {
    // mask bit layout:
    // 0 - has terms
    // 1 - has sub blocks
    // 2 - is floor block
    // block has terms
    static bool Terms(uint8_t mask) noexcept { return fst::CheckBit<TERM>(mask); }

    // block has sub-blocks
    static bool Blocks(uint8_t mask) noexcept { return fst::CheckBit<BLOCK>(mask); }

    static void Type(uint8_t &mask, EntryType type) noexcept { fst::SetBit(mask, type); }

    // block is floor block
    static bool Floor(uint8_t mask) noexcept { return fst::CheckBit<INVALID>(mask); }
    static void Floor(uint8_t &mask, bool b) noexcept { fst::SetBit<INVALID>(b, mask); }

    // resets block meta
    static void Reset(uint8_t mask) noexcept {
        fst::UnsetBit<TERM>(mask);
        fst::UnsetBit<BLOCK>(mask);
    }
};

using Blocks = std::vector<Entry>;

class FSTBuffer : public VectorByteFST {
public:
    struct FSTStatus : FSTStats {
        size_t total_weight_size{};
        void operator()(const ByteWeight &w) noexcept { total_weight_size += w.Size(); }

        [[maybe_unused]] bool operator==(const FSTStatus &rhs) const noexcept {
            return num_states == rhs.num_states && num_arcs == rhs.num_arcs && total_weight_size == rhs.total_weight_size;
        }
    };

    using FSTByteBuilder = FSTBuilder<byte_type, VectorByteFST, FSTStatus>;

    FSTStatus Reset(const Block::BlockIndex &index) {
        builder_.Reset();
        index.Visit([&](Block::PrefixedOutput &output) { builder_.Add(output.prefix_.view(), output.weight_); });
        return builder_.Finish();
    }

private:
    FSTByteBuilder builder_{*this};
};

class DictionaryWriterImpl {
public:
    static constexpr uint32_t DEFAULT_MIN_BLOCK_SIZE = 25;
    static constexpr uint32_t DEFAULT_MAX_BLOCK_SIZE = 48;

    DictionaryWriterImpl(FileWriter &writer) : dict_writer_(writer) { fst_buffer_ = new FSTBuffer; }
    ~DictionaryWriterImpl() { delete fst_buffer_; }

private:
    void WriteBlock(size_t prefix, size_t begin, size_t end, uint8_t meta, uint16_t label);

    void WriteBlocks(size_t prefix, size_t count);

    void Add(bytes_view term);

private:
    FileWriter &dict_writer_;
    Blocks blocks_;
    FSTBuffer *fst_buffer_;
    std::vector<Entry> stack_;
    std::vector<size_t> prefixes_;
    BytesRef last_term_;
};

DictionaryWriter::DictionaryWriter(FileWriter &writer) { impl_ = new DictionaryWriterImpl(writer); }
DictionaryWriter::~DictionaryWriter() { delete impl_; }

void DictionaryWriterImpl::WriteBlock(size_t prefix, size_t begin, size_t end, uint8_t meta, uint16_t label) {
    for (; begin < end; ++begin) {
        auto &e = stack_[begin];
        const bytes_view data = e.data();
        const EntryType type = e.type();
        const uint32_t suf_size = static_cast<uint32_t>(data.size() - prefix);
        if (TERM == type) {
            //
        } else {
            //
        }
    }
}

void DictionaryWriterImpl::WriteBlocks(size_t prefix, size_t count) {
    // block metadata
    uint8_t meta{};

    const size_t end = stack_.size();
    const size_t begin = end - count;
    size_t block_start = begin; // begin of current block to write

    size_t min_suffix = std::numeric_limits<size_t>::max();
    size_t max_suffix = 0;

    uint16_t last_label{Block::INVALID_LABEL}; // last lead suffix label
    uint16_t next_label{Block::INVALID_LABEL}; // next lead suffix label in current block
    for (size_t i = begin; i < end; ++i) {
        const Entry &e = stack_[i];
        const bytes_view data = e.data();
        const size_t suffix = data.size() - prefix;
        min_suffix = std::min(suffix, min_suffix);
        max_suffix = std::max(suffix, max_suffix);

        const uint16_t label = data.size() == prefix ? Block::INVALID_LABEL : data[prefix];
        if (last_label != label) {
            const size_t block_size = i - block_start;

            if (block_size >= DEFAULT_MIN_BLOCK_SIZE && end - block_start > DEFAULT_MAX_BLOCK_SIZE) {
                BlockMeta::Floor(meta, block_size < count);
                WriteBlock(prefix, block_start, i, meta, next_label);
                next_label = label;
                BlockMeta::Reset(meta);
                block_start = i;
                min_suffix = std::numeric_limits<size_t>::max();
                max_suffix = 0;
            }

            last_label = label;
        }
    }
    // write remaining block
    if (block_start < end) {
        BlockMeta::Floor(meta, end - block_start < count);
        WriteBlock(prefix, block_start, end, meta, next_label);
    }
    // Merge blocks into 1st block

    stack_.erase(stack_.begin() + begin, stack_.end());
    // move root block from temporary storage
    // to the top of the stack
    if (!blocks_.empty()) {
        stack_.emplace_back(std::move(blocks_.front()));
        blocks_.clear();
    }
}

void DictionaryWriterImpl::Add(bytes_view term) {
    const bytes_view last = last_term_;
    const size_t limit = std::min(last.size(), term.size());

    // find common prefix
    size_t pos = 0;
    while (pos < limit && term[pos] == last[pos]) {
        ++pos;
    }

    for (size_t i = last.empty() ? 0 : last.size() - 1; i > pos;) {
        --i; // should use it here as we use size_t
        const size_t top = stack_.size() - prefixes_[i];
        if (top > DEFAULT_MIN_BLOCK_SIZE) {
            WriteBlocks(i + 1, top);
            prefixes_[i] -= (top - 1);
        }
    }

    prefixes_.resize(term.size());
    std::fill(prefixes_.begin() + pos, prefixes_.end(), stack_.size());
    last_term_.assign(term);
}

} // namespace infinity