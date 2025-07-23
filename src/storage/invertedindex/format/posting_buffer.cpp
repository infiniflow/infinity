module;

#include <cstdlib>
module posting_buffer;
import stl;

import index_defines;
import posting_field;
import file_writer;
import file_reader;

namespace infinity {

// Constructor - initialize empty buffer state
PostingBuffer::PostingBuffer()
    : memory_buffer_(nullptr), column_capacity_(0), current_size_(0), buffer_validity_flag_(true), field_configuration_(nullptr) {}

// Destructor - clean up allocated memory
PostingBuffer::~PostingBuffer() { ReleaseBuffer(memory_buffer_, column_capacity_); }

// Reallocate buffer with larger capacity
bool PostingBuffer::Reallocate() {
    u8 previous_capacity = column_capacity_;
    u8 *previous_buffer = (u8 *)memory_buffer_;
    u8 expanded_capacity = AllocatePlan(previous_capacity);

    // Check if expansion is possible
    if (expanded_capacity == previous_capacity) {
        return false;
    }

    // Calculate new buffer size
    SizeT total_buffer_size = expanded_capacity * field_configuration_->GetTotalSize();
    u8 *expanded_buffer = (u8 *)malloc(total_buffer_size);

    // Copy existing data to new buffer
    BufferMemoryCopy(expanded_buffer, expanded_capacity, (u8 *)memory_buffer_, column_capacity_, field_configuration_, current_size_);

    // Update buffer state atomically
    buffer_validity_flag_ = false;
    memory_buffer_ = expanded_buffer;
    column_capacity_ = expanded_capacity;
    buffer_validity_flag_ = true;

    // Release old buffer
    ReleaseBuffer(previous_buffer, previous_capacity);
    return true;
}

// Release allocated buffer memory
void PostingBuffer::ReleaseBuffer(u8 *buffer_ptr, u8 capacity) {
    if (buffer_ptr == nullptr || capacity == 0) {
        return;
    }
    free((void *)buffer_ptr);
}

// Copy buffer contents between different capacity buffers
void PostingBuffer::BufferMemoryCopy(u8 *destination, u8 dest_columns, u8 *source, u8 src_columns, const PostingFields *fields, u8 source_size) {
    // Early return for invalid source
    if (source == nullptr || source_size == 0) {
        return;
    }

    // Copy each field's data
    for (u8 field_index = 0; field_index < fields->GetSize(); ++field_index) {
        const PostingField *field_info = fields->GetValue(field_index);
        std::memcpy(GetRow(destination, dest_columns, field_info), GetRow(source, src_columns, field_info), source_size * field_info->GetSize());
    }
}

// Dump buffer contents to file
void PostingBuffer::Dump(const SharedPtr<FileWriter> &output_file) {
    output_file->WriteVInt(current_size_);
    output_file->WriteVInt(column_capacity_);

    if (current_size_ > 0) {
        // Write each field's data
        for (u8 field_index = 0; field_index < field_configuration_->GetSize(); ++field_index) {
            const PostingField *field_info = field_configuration_->GetValue(field_index);
            u8 *row_buffer = GetRow(field_index);
            output_file->Write((char *)row_buffer, (u32)current_size_ * field_info->GetSize());
        }
    }
}

// Load buffer contents from file
void PostingBuffer::Load(const SharedPtr<FileReader> &input_file) {
    current_size_ = input_file->ReadVInt();
    column_capacity_ = input_file->ReadVInt();

    if (current_size_ > 0) {
        // Ensure buffer has sufficient capacity
        Reserve(column_capacity_);

        // Read each field's data
        for (u8 field_index = 0; field_index < field_configuration_->GetSize(); ++field_index) {
            const PostingField *field_info = field_configuration_->GetValue(field_index);
            u8 *row_buffer = GetRow(field_index);
            input_file->Read((char *)row_buffer, (u32)current_size_ * field_info->GetSize());
        }
    }
}

// Clear buffer contents
void PostingBuffer::Clear() { current_size_ = 0; }

// Create snapshot of current buffer state
bool PostingBuffer::SnapShot(PostingBuffer &target_buffer) const {
    target_buffer.Clear();

    // Validate row count compatibility
    if (target_buffer.GetRowCount() != field_configuration_->GetSize()) {
        return false;
    }

    // Handle empty buffer case
    if (memory_buffer_ == nullptr || current_size_ == 0) {
        return true;
    }

    // Capture current state for snapshot
    u8 snapshot_data_size = current_size_;

    // Ensure target has sufficient capacity
    target_buffer.Reserve(snapshot_data_size);

    u8 captured_capacity = 0;
    u8 *captured_buffer = memory_buffer_;

    // Atomic snapshot with consistency check
    do {
        captured_capacity = column_capacity_;
        captured_buffer = memory_buffer_;

        BufferMemoryCopy((u8 *)target_buffer.memory_buffer_,
                         target_buffer.column_capacity_,
                         (u8 *)captured_buffer,
                         captured_capacity,
                         field_configuration_,
                         snapshot_data_size);
    } while (!buffer_validity_flag_ || memory_buffer_ != captured_buffer || column_capacity_ > captured_capacity);

    target_buffer.current_size_ = snapshot_data_size;
    return true;
}

// Reserve buffer capacity
void PostingBuffer::Reserve(u8 required_capacity) {
    // Skip if capacity is sufficient
    if (required_capacity <= 0 || (memory_buffer_ != nullptr && column_capacity_ >= required_capacity)) {
        return;
    }

    // Calculate total memory needed
    SizeT total_memory_size = required_capacity * field_configuration_->GetTotalSize();
    u8 *new_buffer_ptr = (u8 *)malloc(total_memory_size);

    // Copy existing data if present
    if (memory_buffer_ != nullptr) {
        BufferMemoryCopy(new_buffer_ptr, required_capacity, (u8 *)memory_buffer_, column_capacity_, field_configuration_, current_size_);
        ReleaseBuffer((u8 *)memory_buffer_, column_capacity_);
    }

    // Update buffer pointers
    memory_buffer_ = new_buffer_ptr;
    column_capacity_ = required_capacity;
}

// Static allocation planning strategy
u8 PostingBuffer::AllocatePlan(u8 current_capacity) {
    if (current_capacity < 2) {
        return 2;
    } else if (current_capacity < 16) {
        return 16;
    }
    return MAX_DOC_PER_RECORD;
}

} // namespace infinity
