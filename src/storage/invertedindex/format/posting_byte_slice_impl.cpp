module infinity_core:posting_byte_slice.impl;

import :posting_byte_slice;
import :posting_field;
import :flush_info;
import :file_writer;
import :file_reader;

namespace infinity {

// Constructor implementation - initialize with empty buffer and writer
PostingByteSlice::PostingByteSlice() : data_buffer_(), slice_writer_() {}

// Initialize the posting byte slice with field configuration
void PostingByteSlice::Init(const PostingFields *field_config) { data_buffer_.Init(field_config); }

// Internal flush implementation - encode all posting fields to writer
size_t PostingByteSlice::DoFlush() {
    u32 total_flush_size = 0;
    const PostingFields *field_definitions = data_buffer_.GetPostingFields();

    // Iterate through all posting fields and encode them
    for (size_t field_index = 0; field_index < field_definitions->GetSize(); ++field_index) {
        PostingField *current_field = field_definitions->GetValue(field_index);
        u8 *row_buffer = data_buffer_.GetRow(current_field->location_);
        size_t encoded_size = current_field->Encode(slice_writer_, row_buffer, data_buffer_.Size() * current_field->GetSize());
        total_flush_size += encoded_size;
    }
    return total_flush_size;
}

// Main flush operation - flush buffer to persistent storage
size_t PostingByteSlice::Flush() {
    // Early return if buffer is empty
    if (data_buffer_.Size() == 0) {
        return 0;
    }

    // Perform the actual flush operation
    size_t bytes_flushed = DoFlush();

    // Update flush metadata
    FlushInfo updated_flush_info;
    updated_flush_info.SetFlushCount(flush_metadata_.GetFlushCount() + data_buffer_.Size());
    updated_flush_info.SetFlushLength(flush_metadata_.GetFlushLength() + bytes_flushed);
    updated_flush_info.SetIsValidPostingBuffer(false);
    flush_metadata_ = updated_flush_info;

    // Clear the buffer after successful flush
    data_buffer_.Clear();
    return bytes_flushed;
}

// Dump operation - write data to file with optional spill handling
void PostingByteSlice::Dump(const std::shared_ptr<FileWriter> &output_file, bool enable_spill) {
    if (enable_spill) {
        // Write buffer data first
        data_buffer_.Dump(output_file);

        // Write flush metadata
        output_file->WriteVLong(flush_metadata_.flush_info_);

        // Write slice size information
        u32 slice_data_size = slice_writer_.GetSize();
        output_file->WriteVInt(slice_data_size);

        // Early return if no slice data to write
        if (slice_data_size == 0)
            return;
    }

    // Write the actual slice data
    slice_writer_.Dump(output_file);
}

// Load operation - read data from file
void PostingByteSlice::Load(const std::shared_ptr<FileReader> &input_file) {
    // Load buffer data
    data_buffer_.Load(input_file);

    // Load flush metadata
    flush_metadata_.flush_info_ = input_file->ReadVLong();

    // Load slice size and data
    u32 slice_data_size = input_file->ReadVInt();
    if (slice_data_size == 0)
        return;

    slice_writer_.Load(input_file, slice_data_size);
}

// Snapshot operation - create a copy of current state
void PostingByteSlice::SnapShot(PostingByteSlice *target_buffer) const {
    // Initialize target with same field configuration
    target_buffer->Init(GetPostingFields());

    // Copy flush metadata
    target_buffer->flush_metadata_ = flush_metadata_;

    // Create snapshots of writer and buffer
    slice_writer_.SnapShot(target_buffer->slice_writer_);
    data_buffer_.SnapShot(target_buffer->data_buffer_);

    // Handle case where flush length has increased
    if (flush_metadata_.GetFlushLength() > target_buffer->flush_metadata_.GetFlushLength()) {
        target_buffer->data_buffer_.Clear();
        target_buffer->flush_metadata_ = flush_metadata_;
        slice_writer_.SnapShot(target_buffer->slice_writer_);
    }
}

} // namespace infinity