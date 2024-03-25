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

export module column_length_io;

import stl;
import index_defines;
import internal_types;

namespace infinity {
class SegmentIndexEntry;
class IndexReader;
class FileSystem;
class FileHandler;

// write array of u32 to file
// multiple threads will write to different offsets of the file at different times
export class FullTextColumnLengthFileHandler {
public:
    FullTextColumnLengthFileHandler(UniquePtr<FileSystem> file_system, const String &path, SegmentIndexEntry *segment_index_entry);
    ~FullTextColumnLengthFileHandler();
    void WriteColumnLength(const u32 *column_length_array, u32 column_length_count, u32 start_from_offset);

private:
    std::mutex mutex_;
    UniquePtr<FileSystem> file_system_;
    UniquePtr<FileHandler> file_handler_;
    SegmentIndexEntry *segment_index_entry_;
};

export class FullTextColumnLengthUpdateJob {
public:
    FullTextColumnLengthUpdateJob(SharedPtr<FullTextColumnLengthFileHandler> file_handler, u32 column_length_count, u32 start_from_offset);
    u32 *GetColumnLengthArray() { return column_length_array_.get(); }
    void DumpToFile();

private:
    SharedPtr<FullTextColumnLengthFileHandler> file_handler_;
    UniquePtr<u32[]> column_length_array_;
    u32 column_length_count_;
    u32 start_from_offset_;
};

export class FullTextColumnLengthReader {
public:
    FullTextColumnLengthReader(UniquePtr<FileSystem> file_system,
                               const String &index_dir,
                               const Vector<String> &base_names,
                               const Vector<RowID> &base_row_ids);

    inline u32 GetColumnLength(RowID row_id) {
        // assume that there is a file which contains row_id
        if (row_id >= next_base_rowid_) [[unlikely]] {
            SeekFile(row_id);
        }
        return column_length_array_[row_id - current_base_rowid_];
    }

    void SeekFile(RowID row_id);

private:
    UniquePtr<FileSystem> file_system_;
    const String &index_dir_;
    const Vector<String> &base_names_;
    const Vector<RowID> &base_row_ids_; // must in ascending order, have an INVALID_ROWID at the end
    RowID current_base_rowid_ = INVALID_ROWID;
    RowID next_base_rowid_ = INVALID_ROWID;
    u32 next_base_rowid_index_ = 1;
    UniquePtr<u32[]> column_length_array_;
    u32 column_length_array_capacity_ = 0;
};

export class ColumnLengthReader {
    Vector<FullTextColumnLengthReader> column_length_vector_;

public:
    void LoadColumnLength(RowID first_doc_id, IndexReader &index_reader, const Vector<u64> &column_ids, Vector<float> &avg_column_length);

    inline u32 GetColumnLength(u32 scorer_column_idx, RowID row_id) { return column_length_vector_[scorer_column_idx].GetColumnLength(row_id); }
};

} // namespace infinity