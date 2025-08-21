export module infinity_core:buf_writer;

import std.compat;

namespace infinity {
// A simple buffer writer that writes data to a file.
// Now only used for ColumnInverter
// ColumnInverter will use BufWriter sequentially write data and use spill_file pointer randomly write data
export struct BufWriter {
    BufWriter(FILE *spill_file, size_t spill_buf_size) : spill_file_(spill_file), spill_buf_size_(spill_buf_size) {
        spill_buffer_ = std::make_unique<char[]>(spill_buf_size_);
    }

    void Write(const char *data, size_t data_size) {
        if (spill_buf_idx_ + data_size > spill_buf_size_) {
            Flush();
        }
        memcpy(spill_buffer_.get() + spill_buf_idx_, data, data_size);
        spill_buf_idx_ += data_size;
    }

    void Flush() {
        if (spill_buf_idx_) {
            fwrite(spill_buffer_.get(), spill_buf_idx_, 1, spill_file_);
            spill_buf_idx_ = 0;
        }
    }

    size_t Tell() { return ftell(spill_file_); }

    FILE *spill_file_{nullptr};
    size_t spill_buf_idx_{0};
    std::unique_ptr<char[]> spill_buffer_{};
    size_t spill_buf_size_{0};
};
} // namespace infinity