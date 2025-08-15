module;

export module infinity_core:posting_byte_slice;

import :stl;

import :byte_slice;
import :byte_slice_writer;
import :file_writer;
import :file_reader;
import :flush_info;
import :posting_field;
import :index_defines;
import :posting_buffer;

namespace infinity {

// PostingByteSlice manages posting data with buffering and flushing capabilities
// It combines a PostingBuffer for temporary storage and ByteSliceWriter for persistent storage
export class PostingByteSlice {
protected:
    // Internal state management
    FlushInfo flush_metadata_;
    PostingBuffer data_buffer_;
    ByteSliceWriter slice_writer_;

protected:
    // Internal flush operation implementation
    SizeT DoFlush();

public:
    // Constructor initializes empty state
    PostingByteSlice();

    // Virtual destructor for proper cleanup
    virtual ~PostingByteSlice() = default;

    // Initialize with posting field configuration
    void Init(const PostingFields *field_config);

    // Template method for adding typed data to specific row
    template <typename DataType>
    void PushBack(u8 row_index, DataType data_value);

    // Mark end of current push operation and validate buffer
    void EndPushBack() {
        flush_metadata_.SetIsValidPostingBuffer(true);
        data_buffer_.EndPushBack();
    }

    // Check if flush is needed based on buffer size
    bool NeedFlush(u8 flush_threshold = MAX_DOC_PER_RECORD) const { return data_buffer_.Size() == flush_threshold; }

    // Access to underlying byte slice list
    const ByteSliceList *GetByteSliceList() const { return slice_writer_.GetByteSliceList(); }

    // Get posting field configuration
    const PostingFields *GetPostingFields() const { return data_buffer_.GetPostingFields(); }

    // Create snapshot of current state
    void SnapShot(PostingByteSlice *target_buffer) const;

    // Check buffer validity status
    bool IsPostingBufferValid() const { return flush_metadata_.IsValidPostingBuffer(); }

    // Access to internal buffer (read-only)
    const PostingBuffer &GetBuffer() const { return data_buffer_; }

    // Get current buffer size
    SizeT GetBufferSize() const { return data_buffer_.Size(); }

    // Calculate total count including flushed and buffered data
    SizeT GetTotalCount() const { return flush_metadata_.GetFlushCount() + data_buffer_.Size(); }

    // Get flush information
    FlushInfo GetFlushInfo() const { return flush_metadata_; }

    // Flush buffer to persistent storage
    SizeT Flush();

    // Dump data to file with optional spill flag
    void Dump(const SharedPtr<FileWriter> &output_file, bool enable_spill = false);

    // Load data from file
    void Load(const SharedPtr<FileReader> &input_file);

    // Estimate size needed for dump operation
    SizeT EstimateDumpSize() const { return slice_writer_.GetSize(); }

    // Calculate total memory usage
    inline SizeT GetSizeInBytes() const { return data_buffer_.GetSizeInBytes() + slice_writer_.GetSize(); }
};

// Template implementation for type-safe data insertion
template <typename DataType>
inline void PostingByteSlice::PushBack(u8 row_index, DataType data_value) {
    data_buffer_.PushBack(row_index, data_value);
}

} // namespace infinity
