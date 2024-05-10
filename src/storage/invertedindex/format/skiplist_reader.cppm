module;

export module skiplist_reader;
import stl;
import byte_slice;
import byte_slice_reader;
import index_defines;
import doc_list_format_option;
import memory_pool;
import posting_byte_slice;
import posting_byte_slice_reader;
import position_list_format_option;

namespace infinity {

export class SkipListReader {
public:
    explicit SkipListReader(const DocListFormatOption &doc_list_format_option)
        : has_tf_list_(doc_list_format_option.HasTfList()),
          has_block_max_(doc_list_format_option.HasBlockMax()) {
        if (has_tf_list_) {
            ttf_buffer_ = MakeUnique<u32[]>(SKIP_LIST_BUFFER_SIZE);
        }
        if (has_block_max_) {
            block_max_tf_buffer_ = MakeUnique<u32[]>(SKIP_LIST_BUFFER_SIZE);
            block_max_tf_percentage_buffer_ = MakeUnique<u16[]>(SKIP_LIST_BUFFER_SIZE);
        }
    }

    explicit SkipListReader(const PositionListFormatOption &doc_list_format_option) {}

    virtual ~SkipListReader() = default;

    bool SkipTo(u32 query_doc_id, u32 &doc_id, u32 &prev_doc_id, u32 &offset, u32 &delta);

    bool SkipTo(u32 query_doc_id, u32 &doc_id, u32 &offset, u32 &delta) { return SkipTo(query_doc_id, doc_id, prev_doc_id_, offset, delta); }

    i32 GetSkippedItemCount() const { return std::max(static_cast<i32>(skipped_item_count_) - 1, 0); }

    u32 GetPrevDocId() const { return prev_doc_id_; }

    u32 GetCurrentDocId() const { return current_doc_id_; }

    u32 GetCurrentTTF() const { return current_ttf_; }

    u32 GetPrevTTF() const { return prev_ttf_; }

    u32 GetPrevKey() const { return GetPrevDocId(); }

    u32 GetCurrentKey() const { return GetCurrentDocId(); }

    u32 GetLastValueInBuffer() const;

    u32 GetLastKeyInBuffer() const;

    // u32: block max tf
    // u16: block max (ceil(tf / doc length) * numeric_limits<u16>::max())
    Pair<u32, u16> GetBlockMaxInfo() const { return {current_block_max_tf_, current_block_max_tf_percentage_}; }

protected:
    virtual Pair<int, bool> LoadBuffer() = 0;

    const bool has_tf_list_ = false;
    const bool has_block_max_ = false;
    i32 skipped_item_count_ = 0;
    u32 current_doc_id_ = 0;
    u32 current_offset_ = 0;
    u32 current_ttf_ = 0;
    u32 current_block_max_tf_ = 0;
    u16 current_block_max_tf_percentage_ = 0;
    u32 prev_doc_id_ = 0;
    u32 prev_offset_ = 0;
    u32 prev_ttf_ = 0;
    u32 current_cursor_ = 0;
    u32 num_in_buffer_ = 0;
    u32 doc_id_buffer_[SKIP_LIST_BUFFER_SIZE] = {};
    u32 offset_buffer_[SKIP_LIST_BUFFER_SIZE] = {};
    UniquePtr<u32[]> ttf_buffer_;
    UniquePtr<u32[]> block_max_tf_buffer_;
    UniquePtr<u16[]> block_max_tf_percentage_buffer_;
};

export class SkipListReaderByteSlice final : public SkipListReader {
public:
    explicit SkipListReaderByteSlice(const DocListFormatOption &doc_list_format_option) : SkipListReader(doc_list_format_option) {}

    explicit SkipListReaderByteSlice(const PositionListFormatOption &pos_list_format_option) : SkipListReader(pos_list_format_option) {}

    void Load(const ByteSliceList *byte_slice_list, u32 start, u32 end);

    void Load(ByteSlice *byteSlice, u32 start, u32 end);

    const ByteSliceList *GetByteSliceList() const { return byte_slice_reader_.GetByteSliceList(); }

    u32 GetStart() const { return start_; }

    u32 GetEnd() const { return end_; }

protected:
    Pair<int, bool> LoadBuffer() override;

    ByteSliceReader byte_slice_reader_;
    u32 start_ = 0;
    u32 end_ = 0;
};

export class SkipListReaderPostingByteSlice final : public SkipListReader {
public:
    SkipListReaderPostingByteSlice(const DocListFormatOption &doc_list_format_option, MemoryPool *session_pool)
        : SkipListReader(doc_list_format_option), session_pool_(session_pool) {}

    ~SkipListReaderPostingByteSlice() override;

    void Load(const PostingByteSlice *posting_buffer);

protected:
    Pair<int, bool> LoadBuffer() override;

private:
    MemoryPool *session_pool_;
    PostingByteSlice *skiplist_buffer_ = nullptr;
    PostingByteSliceReader skiplist_reader_;
};

} // namespace infinity
