#include "dictionary.h"
#include "common/utility/builtin.h"

#include <fst/matcher.h>

#include "storage/invertedindex/common/openfst/fst_automaton.h"
#include "storage/invertedindex/common/openfst/fst_bits_utils.h"
#include "storage/invertedindex/common/openfst/fst_builder.h"
#include "storage/invertedindex/common/openfst/fst_decl.h"
#include "storage/invertedindex/common/openfst/fst_immutable.h"
#include "storage/invertedindex/common/openfst/fst_matcher.h"
#include "storage/invertedindex/common/openfst/fst_string_types.h"
#include "storage/invertedindex/common/openfst/fst_string_weight.h"
#include "storage/invertedindex/common/openfst/fst_stringref_weight.h"
#include "storage/invertedindex/common/openfst/fst_table_matcher.h"

import term_meta;
import vbyte_compressor;

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

    Block(BlockIndex &&other, uint64_t BlockStart, uint8_t meta, uint16_t label)
        : index_(std::move(other)), start_(BlockStart), label_(label), meta_(meta) {}

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
    Entry(bytes_view prefix, Block::BlockIndex &&index, uint64_t BlockStart, uint8_t meta, uint16_t label) : type_(BLOCK) {
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
// mininum size of string weight we store in FST
constexpr const size_t MIN_WEIGHT_SIZE = 2;

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
#if 0
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
#endif
}

void DictionaryWriterImpl::WriteBlocks(size_t prefix, size_t count) {
    // block metadata
    uint8_t meta{};

    const size_t end = stack_.size();
    const size_t begin = end - count;
    size_t BlockStart = begin; // begin of current block to write

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
            const size_t block_size = i - BlockStart;

            if (block_size >= DEFAULT_MIN_BLOCK_SIZE && end - BlockStart > DEFAULT_MAX_BLOCK_SIZE) {
                BlockMeta::Floor(meta, block_size < count);
                WriteBlock(prefix, BlockStart, i, meta, next_label);
                next_label = label;
                BlockMeta::Reset(meta);
                BlockStart = i;
                min_suffix = std::numeric_limits<size_t>::max();
                max_suffix = 0;
            }

            last_label = label;
        }
    }
    // write remaining block
    if (BlockStart < end) {
        BlockMeta::Floor(meta, end - BlockStart < count);
        WriteBlock(prefix, BlockStart, end, meta, next_label);
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

// Represents a result of seek operation
enum class SeekResult {
    // Exact value is found
    FOUND = 0,

    // Exact value is not found, an iterator is positioned at the next
    // greater value.
    NOT_FOUND,

    // No value greater than a target found, eof
    END
};

class BlockIterator {
public:
    static constexpr uint32_t UNDEFINED_COUNT{std::numeric_limits<uint32_t>::max()};
    static constexpr uint64_t UNDEFINED_ADDRESS{std::numeric_limits<uint64_t>::max()};

    BlockIterator(ByteWeight &&header, size_t prefix) noexcept;

    BlockIterator(bytes_view header, size_t prefix) : BlockIterator(ByteWeight(header), prefix) {}

    BlockIterator(uint64_t start, size_t prefix) noexcept
        : start_{start}, cur_start_{start}, cur_end_{start}, prefix_{static_cast<uint32_t>(prefix)}, sub_count_{UNDEFINED_COUNT} {}

    template <bool ReadHeader>
    bool NextSubBlock() noexcept {
        if (!sub_count_) {
            return false;
        }
        cur_start_ = cur_end_;
        if (sub_count_ != UNDEFINED_COUNT) {
            --sub_count_;
            if constexpr (ReadHeader) {
                VByteCompressor::VSkip<uint64_t>(header_.begin);
                cur_meta_ = *header_.begin++;
                if (sub_count_) {
                    next_label_ = *header_.begin++;
                }
            }
        }
        dirty_ = true;
        return true;
    }

    template <typename Reader>
    void Next(Reader &&reader) {
        if (leaf_) {
            ReadEntryLeaf(std::forward<Reader>(reader));
        } else {
            ReadEntryNonleaf(std::forward<Reader>(reader));
        }
        ++cur_ent_;
    }

    void Reset();

    const TermMeta &State() const noexcept { return state_; }
    bool Dirty() const noexcept { return dirty_; }
    uint8_t Meta() const noexcept { return cur_meta_; }
    size_t Prefix() const noexcept { return prefix_; }
    EntryType Type() const noexcept { return cur_type_; }
    uint64_t BlockStart() const noexcept { return cur_block_start_; }
    uint16_t NextLabel() const noexcept { return next_label_; }
    uint32_t SubCount() const noexcept { return sub_count_; }
    uint64_t Start() const noexcept { return start_; }
    bool Done() const noexcept { return cur_ent_ == ent_count_; }
    bool NoTerms() const noexcept {
        // FIXME(gnusi): add term mark to block entry?
        //
        // Block was loaded using address and doesn't have metadata,
        // assume such blocks have terms
        return sub_count_ != UNDEFINED_COUNT && !BlockMeta::Terms(Meta());
    }
    uint64_t Size() const noexcept { return ent_count_; }

    template <typename Reader>
    SeekResult ScanToTerm(bytes_view term, Reader &&reader) {
        return leaf_ ? ScanToTermLeaf(term, std::forward<Reader>(reader)) : ScanToTermNonleaf(term, std::forward<Reader>(reader));
    }

    template <typename Reader>
    SeekResult Scan(Reader &&reader) {
        return leaf_ ? scan_leaf(std::forward<Reader>(reader)) : ScanNonleaf(std::forward<Reader>(reader));
    }

    // scan to floor block
    void ScanToSubBlock(byte_type label);

    // scan to entry with the following start address
    void ScanToBlock(uint64_t ptr);

private:
    struct DataBlock {
        using block_type = bstring;

        DataBlock() = default;
        DataBlock(block_type &&block) noexcept : block{std::move(block)}, begin{this->block.c_str()} {}
        DataBlock(DataBlock &&rhs) noexcept { *this = std::move(rhs); }
        DataBlock &operator=(DataBlock &&rhs) noexcept {
            if (this != &rhs) {
                if (rhs.block.empty()) {
                    begin = rhs.begin;
                } else {
                    const size_t offset = std::distance(rhs.block.c_str(), rhs.begin);
                    block = std::move(rhs.block);
                    begin = block.c_str() + offset;
                }
            }
            return *this;
        }
        block_type block;
        const byte_type *begin{block.c_str()};
    };

    template <typename T = bool, typename U = uint32_t>
    inline T ShiftUnpack32(uint32_t in, U &out) noexcept {
        out = static_cast<U>(in >> 1);
        return static_cast<T>(in & 1);
    }

    template <typename Reader>
    void ReadEntryNonleaf(Reader &&reader) {
        cur_type_ = ShiftUnpack32<EntryType, size_t>(VByteCompressor::ReadVInt<uint32_t>(suffix_.begin), suffix_length_);
        suffix_begin_ = suffix_.begin;
        suffix_.begin += suffix_length_;

        if (TERM == cur_type_) {
            ++term_count_;
        } else {
            cur_block_start_ = cur_start_ - VByteCompressor::ReadVInt<uint64_t>(suffix_.begin);
        }

        // read after state is updated
        reader(suffix_begin_, suffix_length_);
    }

    template <typename Reader>
    void ReadEntryLeaf(Reader &&reader) {
        assert(leaf_ && cur_ent_ < ent_count_);
        cur_type_ = TERM; // always term
        ++term_count_;
        suffix_length_ = VByteCompressor::ReadVInt<uint32_t>(suffix_.begin);
        reader(suffix_.begin, suffix_length_);
        suffix_begin_ = suffix_.begin;
        suffix_.begin += suffix_length_;
    }

    template <typename Reader>
    SeekResult ScanToTermNonleaf(bytes_view term, Reader &&reader);
    template <typename Reader>
    SeekResult ScanToTermLeaf(bytes_view term, Reader &&reader);

    template <typename Reader>
    SeekResult ScanNonleaf(Reader &&reader);
    template <typename Reader>
    SeekResult ScanLeaf(Reader &&reader);

    DataBlock header_; // suffix block header
    DataBlock suffix_; // suffix data block
    DataBlock stats_;  // stats data block
    TermMeta state_;
    size_t suffix_length_{}; // last matched suffix length
    const byte_type *suffix_begin_{};
    uint64_t start_;     // initial block start pointer
    uint64_t cur_start_; // current block start pointer
    uint64_t cur_end_;   // block end pointer
    // start pointer of the current sub-block entry
    uint64_t cur_block_start_{UNDEFINED_ADDRESS};
    uint32_t prefix_;          // block prefix length, 32k at most
    uint32_t cur_ent_{};       // current entry in a block
    uint32_t ent_count_{};     // number of entries in a current block
    uint32_t term_count_{};    // number terms in a block we have seen
    uint32_t cur_stats_ent_{}; // current position of loaded stats
    uint32_t sub_count_;       // number of sub-blocks
    // next label (of the next sub-block)
    uint16_t next_label_{Block::INVALID_LABEL};
    EntryType cur_type_{INVALID}; // term or block
    byte_type meta_{};            // initial block metadata
    byte_type cur_meta_{};        // current block metadata
    bool dirty_{true};            // current block is dirty
    bool leaf_{false};            // current block is leaf block
};

BlockIterator::BlockIterator(ByteWeight &&header, size_t prefix) noexcept
    : header_{std::move(header)}, prefix_{static_cast<uint32_t>(prefix)}, sub_count_{0} {
    cur_meta_ = meta_ = *header_.begin++;
    cur_end_ = cur_start_ = start_ = VByteCompressor::ReadVInt<uint64_t>(header_.begin);
    if (BlockMeta::Floor(meta_)) {
        sub_count_ = VByteCompressor::ReadVInt<uint32_t>(header_.begin);
        next_label_ = *header_.begin++;
    }
}

template <typename Reader>
SeekResult BlockIterator::ScanLeaf(Reader &&reader) {
    assert(leaf_);
    assert(!dirty_);
    assert(ent_count_ >= cur_ent_);

    SeekResult res = SeekResult::END;
    cur_type_ = TERM; // leaf block contains terms only

    size_t suffix_length = suffix_length_;
    size_t count = 0;

    for (const size_t left = ent_count_ - cur_ent_; count < left;) {
        ++count;
        suffix_length = VByteCompressor::ReadVInt<uint32_t>(suffix_.begin);
        res = reader(suffix_.begin, suffix_length);
        suffix_.begin += suffix_length; // skip to the next term

        if (res != SeekResult::NOT_FOUND) {
            break;
        }
    }

    cur_ent_ += count;
    term_count_ = cur_ent_;

    suffix_begin_ = suffix_.begin - suffix_length;
    suffix_length_ = suffix_length;

    return res;
}

template <typename Reader>
SeekResult BlockIterator::ScanNonleaf(Reader &&reader) {
    assert(!leaf_);
    assert(!dirty_);

    SeekResult res = SeekResult::END;

    while (cur_ent_ < ent_count_) {
        ++cur_ent_;
        cur_type_ = ShiftUnpack32<EntryType, size_t>(VByteCompressor::ReadVInt<uint32_t>(suffix_.begin), suffix_length_);
        const bool is_block = cur_type_ == BLOCK;

        suffix_begin_ = suffix_.begin;
        suffix_.begin += suffix_length_; // skip to the next entry

        if (TERM == cur_type_) {
            ++term_count_;
        } else {
            assert(cur_type_ == BLOCK);
            cur_block_start_ = cur_start_ - VByteCompressor::ReadVInt<uint64_t>(suffix_.begin);
        }

        // FIXME
        // we're not allowed to access/modify any BlockIterator's
        // member as current instance might be already moved due
        // to a reallocation
        res = reader(suffix_begin_, suffix_length_);

        if (res != SeekResult::NOT_FOUND || is_block) {
            break;
        }
    }

    return res;
}

template <typename Reader>
SeekResult BlockIterator::ScanToTermLeaf(bytes_view term, Reader &&reader) {
    assert(leaf_);
    assert(!dirty_);
    assert(term.size() >= prefix_);

    const size_t term_suffix_length = term.size() - prefix_;
    const byte_type *term_suffix = term.data() + prefix_;
    size_t suffix_length = suffix_length_;
    cur_type_ = TERM; // leaf block contains terms only
    SeekResult res = SeekResult::END;

    uint32_t count = 0;
    for (uint32_t left = ent_count_ - cur_ent_; count < left;) {
        ++count;
        suffix_length = VByteCompressor::ReadVInt<uint32_t>(suffix_.begin);

        long cmp = std::memcmp(suffix_.begin, term_suffix, std::min(suffix_length, term_suffix_length));

        if (cmp == 0) {
            cmp = suffix_length - term_suffix_length;
        }

        suffix_.begin += suffix_length; // skip to the next term

        if (cmp >= 0) {
            res = (cmp == 0 ? SeekResult::FOUND       // match!
                            : SeekResult::NOT_FOUND); // after the target, not found
            break;
        }
    }

    cur_ent_ += count;
    term_count_ = cur_ent_;
    suffix_begin_ = suffix_.begin - suffix_length;
    suffix_length_ = suffix_length;
    reader(suffix_begin_, suffix_length);

    return res;
}

template <typename Reader>
SeekResult BlockIterator::ScanToTermNonleaf(bytes_view term, Reader &&reader) {
    assert(!leaf_);
    assert(!dirty_);
    assert(term.size() >= prefix_);

    const size_t term_suffix_length = term.size() - prefix_;
    const byte_type *term_suffix = term.data() + prefix_;
    const byte_type *suffix_begin = suffix_begin_;
    size_t suffix_length = suffix_length_;
    SeekResult res = SeekResult::END;

    while (cur_ent_ < ent_count_) {
        ++cur_ent_;
        cur_type_ = ShiftUnpack32<EntryType, size_t>(VByteCompressor::ReadVInt<uint32_t>(suffix_.begin), suffix_length);
        suffix_begin = suffix_.begin;
        suffix_.begin += suffix_length; // skip to the next entry

        if (TERM == cur_type_) {
            ++term_count_;
        } else {
            assert(BLOCK == cur_type_);
            cur_block_start_ = cur_start_ - VByteCompressor::ReadVInt<uint64_t>(suffix_.begin);
        }

        ptrdiff_t cmp = std::memcmp(suffix_begin, term_suffix, std::min(suffix_length, term_suffix_length));

        if (cmp == 0) {
            cmp = suffix_length - term_suffix_length;
        }

        if (cmp >= 0) {
            res = (cmp == 0 ? SeekResult::FOUND       // match!
                            : SeekResult::NOT_FOUND); // we after the target, not found
            break;
        }
    }

    suffix_begin_ = suffix_begin;
    suffix_length_ = suffix_length;
    reader(suffix_begin, suffix_length);

    return res;
}

void BlockIterator::ScanToSubBlock(byte_type label) {
    assert(sub_count_ != UNDEFINED_COUNT);

    if (!sub_count_ || !BlockMeta::Floor(meta_)) {
        // no sub-blocks, nothing to do
        return;
    }

    const uint16_t target = label; // avoid byte_type vs uint16_t comparison

    if (target < next_label_) {
        // we don't need search
        return;
    }

    // FIXME: binary search???
    uint64_t start_delta = 0;
    for (;;) {
        start_delta = VByteCompressor::ReadVInt<uint64_t>(header_.begin);
        cur_meta_ = *header_.begin++;
        if (--sub_count_) {
            next_label_ = *header_.begin++;

            if (target < next_label_) {
                break;
            }
        } else {
            next_label_ = Block::INVALID_LABEL;
            break;
        }
    }

    if (start_delta) {
        cur_start_ = start_ + start_delta;
        cur_ent_ = 0;
        dirty_ = true;
    }
}

void BlockIterator::ScanToBlock(uint64_t start) {
    if (leaf_) {
        // must be a non leaf block
        return;
    }

    if (cur_block_start_ == start) {
        // nothing to do
        return;
    }

    const uint64_t target = cur_start_ - start; // delta
    for (; cur_ent_ < ent_count_;) {
        ++cur_ent_;
        const auto type = ShiftUnpack32<EntryType, size_t>(VByteCompressor::ReadVInt<uint32_t>(suffix_.begin), suffix_length_);
        suffix_.begin += suffix_length_;

        if (TERM == type) {
            ++term_count_;
        } else {
            assert(BLOCK == type);
            if (VByteCompressor::ReadVInt<uint64_t>(suffix_.begin) == target) {
                cur_block_start_ = target;
                return;
            }
        }
    }
}

void BlockIterator::Reset() {
    if (sub_count_ != UNDEFINED_COUNT) {
        sub_count_ = 0;
    }
    next_label_ = Block::INVALID_LABEL;
    cur_start_ = start_;
    cur_meta_ = meta_;
    if (BlockMeta::Floor(meta_)) {
        header_.begin = header_.block.c_str() + 1;       // +1 to skip meta
        VByteCompressor::VSkip<uint64_t>(header_.begin); // skip address
        sub_count_ = VByteCompressor::ReadVInt<uint32_t>(header_.begin);
        next_label_ = *header_.begin++;
    }
    dirty_ = true;
}

template <typename FST>
using ExplicitMatcher = fst::ExplicitMatcherExt<fst::SortedMatcher<FST>>;

template <typename FST>
class TermIterator {
public:
    using weight_t = typename FST::Weight;
    using stateid_t = typename FST::StateId;

    bool Next();

    SeekResult SeekGe(bytes_view term);

    bool Seek(bytes_view term) { return SeekResult::FOUND == SeekEqual(term, true); }

private:
    friend class BlockIterator;

    struct Arc {
        Arc() = default;
        Arc(stateid_t state, bytes_view weight, size_t block) noexcept : state(state), weight(weight), block(block) {}

        stateid_t state;
        bytes_view weight;
        size_t block;
    };

    static_assert(std::is_nothrow_move_constructible_v<Arc>);

    void Copy(const byte_type *suffix, size_t prefix_size, size_t suffix_size) {
        term_buf_.resize(prefix_size + suffix_size);
        std::memcpy(term_buf_.data() + prefix_size, suffix, suffix_size);
    }

    long SeekCached(size_t &prefix, stateid_t &state, size_t &block, ByteWeight &weight, bytes_view term);

    // Seek to the closest block which contain a specified term
    // prefix - size of the common term/block prefix
    // Returns true if we're already at a requested term
    bool SeekToBlock(bytes_view term, size_t &prefix);

    // Seeks to the specified term using FST
    // There may be several sutuations:
    //   1. There is no term in a block (SeekResult::NOT_FOUND)
    //   2. There is no term in a block and we have
    //      reached the end of the block (SeekResult::END)
    //   3. We have found term in a block (SeekResult::FOUND)
    //
    // Note, that search may end up on a BLOCK entry. In all cases
    // "owner_->term_" will be refreshed with the valid number of
    // common bytes
    SeekResult SeekEqual(bytes_view term, bool exact);

    BlockIterator *PopBlock() noexcept {
        block_stack_.pop_back();
        return &block_stack_.back();
    }

    BlockIterator *PushBlock(bytes_view out, size_t prefix) {
        // ensure final weight correctess
        return &block_stack_.emplace_back(out, prefix);
    }

    BlockIterator *PushBlock(ByteWeight &&out, size_t prefix) {
        // ensure final weight correctess
        assert(out.Size() >= MIN_WEIGHT_SIZE);

        return &block_stack_.emplace_back(std::move(out), prefix);
    }

    BlockIterator *PushBlock(uint64_t start, size_t prefix) { return &block_stack_.emplace_back(start, prefix); }

    bytes_view value_;
    bstring term_buf_;
    ByteWeight weight_;
    const FST *fst_;
    ExplicitMatcher<FST> matcher_;
    std::vector<Arc> sstate_;
    std::vector<BlockIterator> block_stack_;
    BlockIterator *cur_block_{};
};

template <typename FST>
bool TermIterator<FST>::Next() {
    // iterator at the beginning or seek to cached state was called
    if (!cur_block_) {
        if (value_.empty()) {
            // iterator at the beginning
            cur_block_ = PushBlock(fst_->Final(fst_->Start()), 0);
            // cur_block_->load(terms_input(), terms_cipher());
        } else {
            // FIXME(gnusi): consider removing this, as that seems to be impossible
            // anymore

            // seek to the term with the specified state was called from
            // term_iterator::seek(bytes_view, const attribute&),
            // need create temporary "bytes_view" here, since "seek" calls
            // term_.reset() internally,
            // note, that since we do not create extra copy of term_
            // make sure that it does not reallocate memory !!!
            [[maybe_unused]] const auto res = SeekEqual(value_, true);
            assert(SeekResult::FOUND == res);
        }
    }

    // pop finished blocks
    while (cur_block_->Done()) {
        if (cur_block_->NextSubBlock<false>()) {
            // cur_block_->load(terms_input(), terms_cipher());
        } else if (&block_stack_.front() == cur_block_) { // root
            // reset_value();
            cur_block_->Reset();
            sstate_.clear();
            return false;
        } else {
            const uint64_t start = cur_block_->Start();
            cur_block_ = PopBlock();
            // std::get<TermMeta>(attrs_) = cur_block_->State();
            if (cur_block_->Dirty() || cur_block_->BlockStart() != start) {
                // here we're currently at non block that was not loaded yet
                // to sub-block
                cur_block_->ScanToSubBlock(term_buf_[cur_block_->Prefix()]);
                // cur_block_->load(terms_input(), terms_cipher());
                cur_block_->ScanToBlock(start);
            }
        }
    }

    sstate_.resize(std::min(sstate_.size(), cur_block_->Prefix()));

    auto copy_suffix = [this](const byte_type *suffix, size_t suffix_size) { Copy(suffix, cur_block_->Prefix(), suffix_size); };

    // push new block or next term
    for (cur_block_->Next(copy_suffix); EntryType::BLOCK == cur_block_->Type(); cur_block_->Next(copy_suffix)) {
        cur_block_ = PushBlock(cur_block_->BlockStart(), term_buf_.size());
        // cur_block_->load(terms_input(), terms_cipher());
    }

    // refresh_value();
    return true;
}

template <typename FST>
long TermIterator<FST>::SeekCached(size_t &prefix, stateid_t &state, size_t &block, ByteWeight &weight, bytes_view target) {
    const auto term = value_;
    const byte_type *pterm = term.data();
    const byte_type *ptarget = target.data();

    // determine common prefix between target term and current
    {
        auto begin = sstate_.begin();
        auto end = begin + std::min(target.size(), sstate_.size());

        for (; begin != end && *pterm == *ptarget; ++begin, ++pterm, ++ptarget) {
            auto &state_weight = begin->weight;
            weight.PushBack(state_weight.begin(), state_weight.end());
            state = begin->state;
            block = begin->block;
        }

        prefix = size_t(pterm - term.data());
    }

    // inspect suffix and determine our current position
    // with respect to target term (before, after, equal)
    long cmp = char_traits<byte_type>::compare(pterm, ptarget, std::min(target.size(), term.size()) - prefix);

    if (!cmp) {
        cmp = term.size() - target.size();
    }

    if (cmp) {
        // truncate block_stack_ to match path
        const auto begin = block_stack_.begin() + (block + 1);
        block_stack_.erase(begin, block_stack_.end());
    }

    // cmp < 0 : target term is after the current term
    // cmp == 0 : target term is current term
    // cmp > 0 : target term is before current term
    return cmp;
}

template <typename FST>
bool TermIterator<FST>::SeekToBlock(bytes_view term, size_t &prefix) {
    auto &fst = *fst_->GetImpl();

    prefix = 0;                    // number of current symbol to process
    stateid_t state = fst.Start(); // start state
    weight_.Clear();               // clear aggregated fst output
    size_t block = 0;

    if (cur_block_) {
        const long cmp = SeekCached(prefix, state, block, weight_, term);

        if (cmp > 0) {
            // target term is before the current term
            block_stack_[block].Reset();
        } else if (0 == cmp) {
            if (cur_block_->Type() == BLOCK) {
                // we're at the block with matching prefix
                cur_block_ = PushBlock(cur_block_->BlockStart(), term_buf_.size());
                return false;
            } else {
                // we're already at current term
                return true;
            }
        }
    } else {
        PushBlock(fst.Final(state), prefix);
    }

    // reset to common seek prefix
    term_buf_.resize(prefix);
    sstate_.resize(prefix); // remove invalid cached arcs

    while (FSTBuffer::FSTByteBuilder::final != state && prefix < term.size()) {
        matcher_.SetState(state);

        if (!matcher_.Find(term[prefix])) {
            break;
        }

        const auto &arc = matcher_.Value();

        term_buf_ += byte_type(arc.ilabel); // aggregate arc label
        weight_.PushBack(arc.weight.begin(),
                         arc.weight.end()); // aggregate arc weight
        ++prefix;

        const auto &weight = fst.FinalRef(arc.nextstate);

        if (!weight.Empty()) {
            PushBlock(fst::Times(weight_, weight), prefix);
            ++block;
        } else if (FSTBuffer::FSTByteBuilder::final == arc.nextstate) {
            // ensure final state has no weight assigned
            // the only case when it's wrong is degenerated FST composed of only
            // 'fst_byte_builder::final' state.
            // in that case we'll never get there due to the loop condition above.
            PushBlock(std::move(weight_), prefix);
            ++block;
        }

        // cache found arcs, we can reuse it in further seeks
        // avoiding relatively expensive FST lookups
        sstate_.emplace_back(arc.nextstate, arc.weight, block);

        // proceed to the next state
        state = arc.nextstate;
    }

    cur_block_ = &block_stack_[block];
    prefix = cur_block_->Prefix();
    sstate_.resize(prefix);

    if (prefix < term.size()) {
        cur_block_->ScanToSubBlock(term[prefix]);
    }

    return false;
}

template <typename FST>
SeekResult TermIterator<FST>::SeekEqual(bytes_view term, bool exact) {
    size_t prefix;
    if (SeekToBlock(term, prefix)) {
        assert(TERM == cur_block_->Type());
        return SeekResult::FOUND;
    }

    if (exact && cur_block_->NoTerms()) {
        // current block has no terms
        value_ = {term_buf_.c_str(), prefix};
        return SeekResult::NOT_FOUND;
    }

    auto append_suffix = [this](const byte_type *suffix, size_t suffix_size) {
        const auto prefix = cur_block_->Prefix();
        term_buf_.resize(prefix + suffix_size);
        std::memcpy(term_buf_.data() + prefix, suffix, suffix_size);
    };

    // cur_block_->Load(terms_input(), terms_cipher());

    // Finally refresh_value = [this]() noexcept { this->refresh_value(); };
    return cur_block_->ScanToTerm(term, append_suffix);
}

template <typename FST>
SeekResult TermIterator<FST>::SeekGe(bytes_view term) {
    switch (SeekEqual(term, false)) {
        case SeekResult::FOUND:
            return SeekResult::FOUND;
        case SeekResult::NOT_FOUND:
            switch (cur_block_->Type()) {
                case TERM:
                    // we're already at greater term
                    return SeekResult::NOT_FOUND;
                case BLOCK:
                    // we're at the greater block, load it and call next
                    cur_block_ = PushBlock(cur_block_->BlockStart(), term_buf_.size());
                    // cur_block_->load(terms_input(), terms_cipher());
                    break;
                default:
                    return SeekResult::END;
            }
        case SeekResult::END:
            return Next() ? SeekResult::NOT_FOUND // have moved to the next entry
                          : SeekResult::END;      // have no more terms
    }

    return SeekResult::END;
}

class AutomatonARCMatcher {
public:
    AutomatonARCMatcher(const Automaton::Arc *arcs, size_t narcs) noexcept : begin_(arcs), end_(arcs + narcs) {}

    const Automaton::Arc *Seek(uint32_t label) noexcept {
        // linear search is faster for a small number of arcs
        while (++begin_ != end_ && begin_->max < label) {
        }
        return begin_ != end_ && begin_->min <= label ? begin_ : nullptr;
    }

    const Automaton::Arc *Value() const noexcept { return begin_; }

    bool Done() const noexcept { return begin_ == end_; }

private:
    const Automaton::Arc *begin_; // current arc
    const Automaton::Arc *end_;   // end of arcs range
};

template <typename FST>
class FSTARCMatcher {
public:
    FSTARCMatcher(const FST &fst, typename FST::StateId state) noexcept {
        fst::ArcIteratorData<typename FST::Arc> data;
        fst.InitArcIterator(state, &data);
        begin_ = data.arcs;
        end_ = begin_ + data.narcs;
    }

    void Seek(typename FST::Arc::Label label) noexcept {
        // linear search is faster for a small number of arcs
        for (; begin_ != end_; ++begin_) {
            if (label <= begin_->ilabel) {
                break;
            }
        }
    }

    const typename FST::Arc *Value() const noexcept { return begin_; }

    bool Done() const noexcept { return begin_ == end_; }

private:
    const typename FST::Arc *begin_; // current arc
    const typename FST::Arc *end_;   // end of arcs range
};

template <typename FST>
class AutomatonTermIterator {
public:
    AutomatonTermIterator(const FST &fst, AutomatonTableMatcher &matcher)
        : fst_{&fst}, acceptor_{&matcher.GetFst()}, matcher_{&matcher}, fst_matcher_{&fst, fst::MATCH_INPUT}, sink_{matcher.Sink()} {
        assert(fst::kNoStateId != acceptor_->Start());
        assert(acceptor_->NumArcs(acceptor_->Start()));
    }

    bool Next();

    SeekResult SeekGE(bytes_view term) {
        // if (!seek(*this, term)) {
        // return SeekResult::END;
        //}

        return SeekResult::NOT_FOUND;
    }

    bool Seek(bytes_view term) { return SeekResult::FOUND == SeekGE(term); }

private:
    class block_iterator : public BlockIterator {
    public:
        block_iterator(bytes_view out,
                       const FST &fst,
                       size_t prefix,
                       size_t weight_prefix,
                       Automaton::StateId state,
                       typename FST::StateId fst_state,
                       const Automaton::Arc *arcs,
                       size_t narcs) noexcept
            : BlockIterator(out, prefix), arcs_(arcs, narcs), fst_arcs_(fst, fst_state), weight_prefix_(weight_prefix), state_(state),
              fst_state_(fst_state) {}

    public:
        FSTARCMatcher<FST> &FSTArcs() noexcept { return fst_arcs_; }
        AutomatonARCMatcher &Arcs() noexcept { return arcs_; }
        Automaton::StateId AcceptorState() const noexcept { return state_; }
        typename FST::StateId FSTState() const noexcept { return fst_state_; }
        size_t WeightPrefix() const noexcept { return weight_prefix_; }

    private:
        AutomatonARCMatcher arcs_;
        FSTARCMatcher<FST> fst_arcs_;
        size_t weight_prefix_;
        Automaton::StateId state_; // state to which current block belongs
        typename FST::StateId fst_state_;
    };

    void Copy(const byte_type *suffix, size_t prefix_size, size_t suffix_size) {
        term_buf_.resize(prefix_size + suffix_size);
        std::memcpy(term_buf_.data() + prefix_size, suffix, suffix_size);
    }

    block_iterator *PopBlock() noexcept {
        block_stack_.pop_back();
        return &block_stack_.back();
    }

    block_iterator *
    PushBlock(bytes_view out, const FST &fst, size_t prefix, size_t weight_prefix, Automaton::StateId state, typename FST::StateId fst_state) {
        // ensure final weight correctness

        fst::ArcIteratorData<Automaton::Arc> data;
        acceptor_->InitArcIterator(state, &data);

        return &block_stack_.emplace_back(out, fst, prefix, weight_prefix, state, fst_state, data.arcs, data.narcs);
    }

    bytes_view value_;
    bstring term_buf_;
    ByteWeight weight_;
    const FST *fst_;
    const Automaton *acceptor_;
    AutomatonTableMatcher *matcher_;
    ExplicitMatcher<FST> fst_matcher_;
    std::vector<block_iterator> block_stack_;
    block_iterator *cur_block_{};
    Automaton::Weight::PayloadType payload_value_;
    Automaton::StateId sink_;
};

template <typename FST>
bool AutomatonTermIterator<FST>::Next() {
    auto &fst = *fst_matcher_.GetFst().GetImpl();

    // iterator at the beginning or seek to cached state was called
    if (!cur_block_) {
        if (value_.empty()) {
            const auto fst_start = fst.Start();
            cur_block_ = PushBlock(fst.Final(fst_start), *fst_, 0, 0, acceptor_->Start(), fst_start);
            // cur_block_->load(terms_input(), terms_cipher());
        } else {
            // FIXME(gnusi): consider removing this, as that seems to be impossible
            // anymore

            // seek to the term with the specified state was called from
            // term_iterator::seek(bytes_view, const attribute&),
            SeekGE(value_);
        }
    }

    Automaton::StateId state;

    auto read_suffix = [this, &state, &fst](const byte_type *suffix, size_t suffix_size) -> SeekResult {
        if (suffix_size) {
            auto &arcs = cur_block_->arcs();

            const auto *arc = arcs.value();
            const uint32_t lead_label = *suffix;

            if (lead_label < arc->min) {
                return SeekResult::NOT_FOUND;
            }

            if (lead_label > arc->max) {
                arc = arcs.Seek(lead_label);

                if (!arc) {
                    if (arcs.Done()) {
                        return SeekResult::END; // pop current block
                    }

                    return SeekResult::NOT_FOUND;
                }
            }

            assert(*suffix >= arc->min && *suffix <= arc->max);
            state = arc->nextstate;

            if (state == sink_) {
                return SeekResult::NOT_FOUND;
            }

            const auto *end = suffix + suffix_size;
            const auto *begin = suffix + 1; // already match first suffix label

            for (; begin < end; ++begin) {
                state = matcher_->Peek(state, *begin);

                if (fst::kNoStateId == state) {
                    // suffix doesn't match
                    return SeekResult::NOT_FOUND;
                }
            }
        } else {
            state = cur_block_->AcceptorState();
        }

        if (TERM == cur_block_->Type()) {
            const auto weight = acceptor_->Final(state);
            if (weight) {
                payload_value_ = weight.Payload();
                Copy(suffix, cur_block_->Prefix(), suffix_size);

                return SeekResult::FOUND;
            }
        } else {
            fst::ArcIteratorData<Automaton::Arc> data;
            acceptor_->InitArcIterator(state, &data);

            if (data.narcs) {
                Copy(suffix, cur_block_->Prefix(), suffix_size);

                weight_.Resize(cur_block_->WeightPrefix());
                auto fst_state = cur_block_->FSTState();

                if (const auto *end = suffix + suffix_size; suffix < end) {
                    auto &fst_arcs = cur_block_->FSTArcs();
                    fst_arcs.seek(*suffix++);
                    const auto *arc = fst_arcs.value();
                    weight_.PushBack(arc->weight.begin(), arc->weight.end());

                    fst_state = fst_arcs.value()->nextstate;
                    for (fst_matcher_.SetState(fst_state); suffix < end; ++suffix) {
                        [[maybe_unused]] const bool found = fst_matcher_.Find(*suffix);

                        const auto &arc = fst_matcher_.Value();
                        fst_state = arc.nextstate;
                        fst_matcher_.SetState(fst_state);
                        weight_.PushBack(arc.weight.begin(), arc.weight.end());
                    }
                }

                const auto &weight = fst.FinalRef(fst_state);
                // IRS_ASSERT(!weight.Empty() || fst_buffer::fst_byte_builder::final == fst_state);
                const auto weight_prefix = weight_.Size();
                weight_.PushBack(weight.begin(), weight.end());
                block_stack_
                    .emplace_back(static_cast<bytes_view>(weight_), *fst_, term_buf_.size(), weight_prefix, state, fst_state, data.arcs, data.narcs);
                cur_block_ = &block_stack_.back();

                // IRS_ASSERT(block_stack_.size() < 2 || (++block_stack_.rbegin())->block_start() == cur_block_->start());

                if (!acceptor_->Final(state)) {
                    cur_block_->ScanToSubBlock(data.arcs->min);
                }

                // cur_block_->load(terms_input(), terms_cipher());
            }
        }

        return SeekResult::NOT_FOUND;
    };

    for (;;) {
        // pop finished blocks
        while (cur_block_->Done()) {
            if (cur_block_->SubCount()) {
                // we always instantiate block with header
                const uint32_t next_label = cur_block_->NextLabel();

                auto &arcs = cur_block_->Arcs();
                const auto *arc = arcs.Value();

                if (next_label < arc->min) {
                    cur_block_->ScanToSubBlock(byte_type(arc->min));
                } else if (arc->max < next_label) {
                    arc = arcs.Seek(next_label);

                    if (arcs.Done()) {
                        if (&block_stack_.front() == cur_block_) {
                            // need to pop root block, we're done
                            // reset_value();
                            cur_block_->Reset();
                            return false;
                        }

                        cur_block_ = PopBlock();
                        continue;
                    }

                    if (!arc) {
                        cur_block_->ScanToSubBlock(byte_type(arcs.Value()->min));
                    } else {
                        cur_block_->template NextSubBlock<true>();
                    }
                } else {
                    cur_block_->template NextSubBlock<true>();
                }

                // cur_block_->load(terms_input(), terms_cipher());
            } else if (&block_stack_.front() == cur_block_) { // root
                // reset_value();
                cur_block_->Reset();
                return false;
            } else {
                const uint64_t start = cur_block_->Start();
                cur_block_ = PopBlock();
                // std::get<version10::term_meta>(attrs_) = cur_block_->state();
                if (cur_block_->Dirty() || cur_block_->BlockStart() != start) {
                    // here we're currently at non block that was not loaded yet
                    // to sub-block
                    cur_block_->ScanToSubBlock(term_buf_[cur_block_->Prefix()]);
                    // cur_block_->load(terms_input(), terms_cipher());
                    cur_block_->ScanToBlock(start);
                }
            }
        }

        const auto res = cur_block_->Scan(read_suffix);

        if (SeekResult::FOUND == res) {
            // refresh_value();
            return true;
        } else if (SeekResult::END == res) {
            if (&block_stack_.front() == cur_block_) {
                // need to pop root block, we're done
                // reset_value();
                cur_block_->Reset();
                return false;
            }

            // continue with popped block
            cur_block_ = PopBlock();
        }
    }
}

} // namespace infinity