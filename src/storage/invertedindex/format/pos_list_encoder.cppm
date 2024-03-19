module;

import stl;
import byte_slice;
import byte_slice_writer;
import memory_pool;
import file_writer;
import file_reader;
import index_defines;
import buffered_byte_slice;
import buffered_skiplist_writer;
import pos_list_format_option;
import inmem_pos_list_decoder;
export module pos_list_encoder;

namespace infinity {

export class PositionListEncoder {
public:
    PositionListEncoder(const PositionListFormatOption &pos_list_format_option,
                        MemoryPool *byte_slice_pool,
                        MemoryPool *buffer_pool,
                        const PositionListFormat *pos_list_format = nullptr)
        : pos_list_buffer_(byte_slice_pool, buffer_pool), last_pos_in_cur_doc_(0), total_pos_count_(0),
          pos_list_format_option_(pos_list_format_option), is_own_format_(false), pos_skiplist_writer_(nullptr), pos_list_format_(pos_list_format),
          byte_slice_pool_(byte_slice_pool) {
        if (!pos_list_format) {
            pos_list_format_ = new PositionListFormat(pos_list_format_option);
            is_own_format_ = true;
        }
        pos_list_buffer_.Init(pos_list_format_);
    }

    ~PositionListEncoder() {
        if (pos_skiplist_writer_) {
            pos_skiplist_writer_->~BufferedSkipListWriter();
            pos_skiplist_writer_ = nullptr;
        }
        if (is_own_format_) {
            delete pos_list_format_;
            pos_list_format_ = nullptr;
        }
    }

    void AddPosition(pos_t pos);
    void EndDocument();
    void Flush();
    void Dump(const SharedPtr<FileWriter> &file, bool spill = false);
    void Load(const SharedPtr<FileReader> &file);
    u32 GetDumpLength() const;

    InMemPositionListDecoder *GetInMemPositionListDecoder(MemoryPool *session_pool) const;

    const ByteSliceList *GetPositionList() const { return pos_list_buffer_.GetByteSliceList(); }

    const BufferedByteSlice *GetBufferedByteSlice() const { return &pos_list_buffer_; }

    void SetDocSkipListWriter(BufferedSkipListWriter *writer) { pos_skiplist_writer_ = writer; }

    const PositionListFormat *GetPositionListFormat() const { return pos_list_format_; }

    BufferedSkipListWriter *GetBufferedSkipListWriter() const { return pos_skiplist_writer_; }

private:
    void CreatePosSkipListWriter();
    void AddPosSkipListItem(u32 total_pos_count, u32 compressed_pos_size, bool need_flush);
    void FlushPositionBuffer();

private:
    BufferedByteSlice pos_list_buffer_;
    pos_t last_pos_in_cur_doc_;                       // 4byte
    u32 total_pos_count_;                             // 4byte
    PositionListFormatOption pos_list_format_option_; // 1byte
    bool is_own_format_;                              // 1byte
    BufferedSkipListWriter *pos_skiplist_writer_;
    const PositionListFormat *pos_list_format_;
    MemoryPool *byte_slice_pool_;
};

} // namespace infinity
