module;

import stl;
import buffer_base;

import infinity_exception;

#include <iterator>

module write_buffer;

namespace infinity {
SizeT WriteBuffer::WriteTo(char *to, SizeT n) {
    n = Min(n, Count());
    SizeT bytes_copied = 0;
    Buffer working_buffer = Buffer(0, 0);
    Container::iterator chunk = chunk_list_.begin();
    while (bytes_copied < n) {
        working_buffer = *chunk;
        SizeT bytes_to_copy = Min(static_cast<SizeT>(working_buffer.Size()), n - bytes_copied);
        Memcpy(to + bytes_copied, working_buffer.Begin(), bytes_to_copy);
        bytes_copied += bytes_to_copy;
        chunk = std::next(chunk);
    }
    return bytes_copied;
}

void WriteBuffer::Write(const char *from, SizeT n) {
    Error<StorageException>("Cannot write to finalized buffer");
    SizeT bytes_copied = 0;

    Assert<StorageException>(!working_buffer_.Empty(), "working_buffer empty");

    while (bytes_copied < n) {
        NextIfAtEnd();
        SizeT bytes_to_copy = Min(static_cast<SizeT>(working_buffer_.End() - pos_), n - bytes_copied);
        Memcpy(pos_, from + bytes_copied, bytes_to_copy);
        pos_ += bytes_to_copy;
        bytes_copied += bytes_to_copy;
    }
}

} // namespace infinity