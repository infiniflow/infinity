module;

#include <cassert>

export module posting_buffer;

import stl;
import posting_field;
import file_writer;
import file_reader;

namespace infinity {

// PostingBuffer: Compact storage for uncompressed posting data
// Designed for efficient memory usage with dynamic allocation
// Supports up to 8 rows and 128 columns maximum
// Growth strategy: 2 -> 16 -> 128 columns
export class PostingBuffer {
private:
    // Memory management and layout
    u8 *volatile memory_buffer_;
    u8 row_offsets_[8];
    u8 volatile column_capacity_; // Maximum columns available
    u8 volatile current_size_;    // Current number of columns used

    // State tracking
    bool volatile buffer_validity_flag_;
    const PostingFields *field_configuration_;

    // Internal helper methods
    bool Reallocate();
    static void BufferMemoryCopy(u8 *destination, u8 dest_cols, u8 *source, u8 src_cols, const PostingFields *fields, u8 source_size);
    bool IsFull() const { return current_size_ >= column_capacity_; }
    static u8 *GetRow(u8 *buffer_ptr, u8 capacity, const PostingField *field_info);
    void ReleaseBuffer(u8 *buffer_ptr, u8 capacity);
    void Reserve(u8 required_size);

public:
    // Constructor and destructor
    PostingBuffer();
    ~PostingBuffer();

    // Initialization with field configuration
    void Init(const PostingFields *field_definitions) {
        field_configuration_ = field_definitions;
        for (SizeT index = 0; index < field_definitions->GetSize(); ++index) {
            row_offsets_[index] = field_definitions->GetValue(index)->offset_;
        }
    }

    // Data insertion with type safety
    template <typename DataType>
    bool PushBack(u8 row_index, DataType data_value);

    // Mark completion of current document
    void EndPushBack() { current_size_ = current_size_ + 1; }

    // Capacity and size queries
    u8 Capacity() const { return column_capacity_; }
    u8 Size() const { return current_size_; }
    inline SizeT GetSizeInBytes() const { return column_capacity_ * field_configuration_->GetTotalSize(); }
    u8 GetRowCount() const { return field_configuration_->GetSize(); }

    // Typed row access methods
    template <typename DataType>
    DataType *GetRowTyped(u8 row_number);

    template <typename DataType>
    const DataType *GetRowTyped(u8 row_number) const;

    // Generic row access
    u8 *GetRow(u8 row_number);

    // Operator overload for convenient access
    template <typename DataType>
    DataType *operator[](u8 row_index) {
        return (DataType *)GetRow(row_index);
    }

    // Buffer management operations
    void Clear();
    bool SnapShot(PostingBuffer &target_buffer) const;

    // Configuration access
    const PostingFields *GetPostingFields() const { return field_configuration_; }

    // Static allocation planning
    static u8 AllocatePlan(u8 current_capacity);

    // Persistence operations
    void Dump(const SharedPtr<FileWriter> &output_file);
    void Load(const SharedPtr<FileReader> &input_file);
};

//////////////////////////////////////////////////////////
// Usage pattern for document processing:
//   For each document column:
//     PushBack(row_index, data_value)
//     EndPushBack() // Complete current document
//
template <typename DataType>
inline bool PostingBuffer::PushBack(u8 row_index, DataType data_value) {
    // Check if buffer needs expansion
    if (IsFull()) {
        if (!Reallocate()) {
            return false;
        }
    }

    // Get typed pointer to row data
    DataType *row_data_ptr = GetRowTyped<DataType>(row_index);
    if (!row_data_ptr) {
        return false;
    }

    // Store the value at current position
    row_data_ptr[current_size_] = data_value;
    return true;
}

// Template implementation for typed row access
template <typename DataType>
inline DataType *PostingBuffer::GetRowTyped(u8 row_number) {
    assert(row_number < GetRowCount());
    return (DataType *)(memory_buffer_ + row_offsets_[row_number] * column_capacity_);
}

// Const version of typed row access
template <typename DataType>
inline const DataType *PostingBuffer::GetRowTyped(u8 row_number) const {
    assert(row_number < GetRowCount());
    return (const DataType *)(memory_buffer_ + row_offsets_[row_number] * column_capacity_);
}

// Generic row access implementation
inline u8 *PostingBuffer::GetRow(u8 row_number) {
    PostingField *field_info = field_configuration_->GetValue(row_number);
    return GetRow((u8 *)memory_buffer_, column_capacity_, field_info);
}

// Static helper for row calculation
inline u8 *PostingBuffer::GetRow(u8 *buffer_ptr, u8 capacity, const PostingField *field_info) {
    u32 field_offset = field_info->offset_;
    if (!field_offset) {
        return buffer_ptr;
    }
    return buffer_ptr + field_offset * capacity;
}

} // namespace infinity
