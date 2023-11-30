module;

import stl;
import byte_slice;
import byte_slice_reader;
import skiplist_reader;
import index_defines;
export module tri_value_skiplist_reader;

namespace infinity {
export class TriValueSkipListReader : public SkipListReader {
public:
    TriValueSkipListReader();

    TriValueSkipListReader(const TriValueSkipListReader &other) noexcept;

    ~TriValueSkipListReader();

public:
    void Load(const ByteSliceList *byte_slice_list, u32 start, u32 end, const u32 &item_count);

    void Load(ByteSlice *byte_slice, u32 start, u32 end, const u32 &item_count);

    bool SkipTo(u32 query_doc_id, u32 &doc_id, u32 &prev_doc_id, u32 &offset, u32 &delta);

    bool SkipTo(u32 query_doc_id, u32 &doc_id, u32 &offset, u32 &delta) { return SkipTo(query_doc_id, doc_id, prev_doc_id_, offset, delta); }

    u32 GetPrevDocId() const { return prev_doc_id_; }

    u32 GetCurrentDocId() const { return current_doc_id_; }

    u32 GetCurrentTTF() const { return current_ttf_; }

    u32 GetPrevTTF() const { return prev_ttf_; }

    virtual u32 GetLastValueInBuffer() const { return 0; }

    virtual u32 GetLastKeyInBuffer() const { return 0; }

protected:
    void InitMember();

    virtual Pair<int, bool> LoadBuffer();

private:
    void Load_(u32 start, u32 end, const u32 &item_count);

protected:
    u32 current_doc_id_;
    u32 current_offset_;
    u32 current_ttf_;
    u32 prev_doc_id_;
    u32 prev_offset_;
    u32 prev_ttf_;
    u32 doc_id_buffer_[SKIP_LIST_BUFFER_SIZE];
    u32 offset_buffer_[SKIP_LIST_BUFFER_SIZE];
    u32 ttf_buffer_[SKIP_LIST_BUFFER_SIZE];
    u32 current_cursor_;
    u32 num_in_buffer_;
};

} // namespace infinity