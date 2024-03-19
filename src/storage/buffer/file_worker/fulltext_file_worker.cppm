// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

export module fulltext_file_worker;

import stl;
import index_file_worker;
import file_worker;

import index_base;
import infinity_exception;
import default_values;
import column_def;
import file_system;
import logger;

namespace infinity {

export struct FullTextColumnLengthData {
    f32 avg_length_{};
    u32 row_count_{};
    f32 total_length_{};        // sum of column_length
    Vector<u32> column_length_; // column_length[i] is the length of the TermList of row with SegmentOffset i

    void SaveIndexInner(FileHandler &file_handler) const {
        file_handler.Write(&avg_length_, sizeof(avg_length_));
        file_handler.Write(&row_count_, sizeof(row_count_));
        file_handler.Write(&total_length_, sizeof(total_length_));
        u32 column_length_size = column_length_.size();
        file_handler.Write(&column_length_size, sizeof(column_length_size));
        file_handler.Write(column_length_.data(), column_length_size * sizeof(u32));
    }

    void ReadIndexInner(FileHandler &file_handler) {
        file_handler.Read(&avg_length_, sizeof(avg_length_));
        file_handler.Read(&row_count_, sizeof(row_count_));
        file_handler.Read(&total_length_, sizeof(total_length_));
        u32 column_length_size = 0;
        file_handler.Read(&column_length_size, sizeof(column_length_size));
        column_length_.resize(column_length_size);
        file_handler.Read(column_length_.data(), column_length_size * sizeof(u32));
    }
};

export class FullTextColumnLengthFileWorker final : public IndexFileWorker {
public:
    explicit FullTextColumnLengthFileWorker(SharedPtr<String> file_dir,
                                            SharedPtr<String> file_name,
                                            const IndexBase *index_base,
                                            const ColumnDef *column_def)
        : IndexFileWorker(file_dir, file_name, index_base, column_def) {}

    ~FullTextColumnLengthFileWorker() final {
        if (data_ != nullptr) {
            FreeInMemory();
            data_ = nullptr;
        }
    }

public:
    void AllocateInMemory() final {
        if (data_) [[unlikely]] {
            UnrecoverableError("AllocateInMemory: Already allocated.");
        } else {
            data_ = static_cast<void *>(new FullTextColumnLengthData());
        }
    }

    void FreeInMemory() final {
        if (data_) [[likely]] {
            auto len_data = static_cast<FullTextColumnLengthData *>(data_);
            delete len_data;
            data_ = nullptr;
            LOG_TRACE("Finished FreeInMemory(), deleted data_ ptr.");
        } else {
            UnrecoverableError("FreeInMemory: Data is not allocated.");
        }
    }

protected:
    void WriteToFileImpl(bool &prepare_success) final {
        if (data_) [[likely]] {
            auto len_data = static_cast<FullTextColumnLengthData *>(data_);
            len_data->SaveIndexInner(*file_handler_);
            prepare_success = true;
            LOG_TRACE("Finished WriteToFileImpl(bool &prepare_success).");
        } else {
            UnrecoverableError("WriteToFileImpl: data_ is nullptr");
        }
    }

    void ReadFromFileImpl() final {
        if (!data_) [[likely]] {
            auto len_data = new FullTextColumnLengthData();
            len_data->ReadIndexInner(*file_handler_);
            data_ = static_cast<void *>(len_data);
            LOG_TRACE("Finished ReadFromFileImpl().");
        } else {
            UnrecoverableError("ReadFromFileImpl: data_ is not nullptr");
        }
    }
};

} // namespace infinity