module;

import stl;
import byte_slice;
import byte_slice_writer;

import file_writer;
import file_reader;
import index_defines;
import posting_byte_slice;
import skiplist_writer;
import position_list_format_option;
import posting_list_format;
import inmem_position_list_decoder;
export module position_list_encoder;

namespace infinity {

export class PositionListEncoder {
public:
    PositionListEncoder(const PostingFormatOption &format_option,
                        const PositionListFormat *pos_list_format = nullptr);

    ~PositionListEncoder();

    void AddPosition(pos_t pos);
    void EndDocument();
    void Flush();
    void Dump(const SharedPtr<FileWriter> &file, bool spill = false);
    void Load(const SharedPtr<FileReader> &file);
    u32 GetDumpLength() const;

    InMemPositionListDecoder *GetInMemPositionListDecoder() const;

    const ByteSliceList *GetPositionList() const { return pos_list_buffer_.GetByteSliceList(); }

    const PostingByteSlice *GetBufferedByteSlice() const { return &pos_list_buffer_; }

    const PositionListFormat *GetPositionListFormat() const { return pos_list_format_; }

private:
    void CreatePosSkipListWriter();
    void AddPosSkipListItem(u32 total_pos_count, u32 compressed_pos_size, bool need_flush);
    void FlushPositionBuffer();

private:
    PostingByteSlice pos_list_buffer_;
    pos_t last_pos_in_cur_doc_;                       // 4byte
    u32 total_pos_count_;                             // 4byte
    PostingFormatOption format_option_;
    bool is_own_format_;                              // 1byte
    UniquePtr<SkipListWriter> pos_skiplist_writer_;
    const PositionListFormat *pos_list_format_;
};

} // namespace infinity
