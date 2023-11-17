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

#include <atomic>
#include <filesystem>

#include "formats/formats.hpp"
#include "index/index_reader.hpp"
#include "store/fs_directory.hpp"
#include "utils/index_utils.hpp"

import stl;
import iresearch_document;
import iresearch_analyzer;
import third_party;
import data_block;
import column_vector;

module iresearch_datastore;

namespace infinity {
IRSDataStore::IRSDataStore(const String &directory) : directory_(directory), path_{Path(directory_)} {
    std::error_code ec;
    bool path_exists = std::filesystem::exists(path_);
    if (!path_exists) {
        std::filesystem::create_directories(path_, ec);
    }
    auto open_mode = path_exists ? (irs::OM_CREATE | irs::OM_APPEND) : irs::OM_CREATE;

    IRSIndexReaderOptions reader_options;
    auto format = irs::formats::get("1_5simd");
    irs_directory_ = MakeUnique<irs::FSDirectory>(directory_.c_str());

    IRSIndexWriterOptions options;
    options.reader_options = reader_options;
    options.segment_memory_max = 256 * (1 << 20); // 256MB
    options.lock_repository = false;              //?
    options.comparator = nullptr;
    index_writer_ = IRSIndexWriter::Make(*(irs_directory_), Move(format), OpenMode(open_mode), options);
    if (!path_exists) {
        index_writer_->Commit();
    }
    auto reader = index_writer_->GetSnapshot();
    auto data = MakeShared<DataSnapshot>(Move(reader));
    StoreSnapshot(data);
}

void IRSDataStore::StoreSnapshot(DataSnapshotPtr snapshot) { std::atomic_store_explicit(&snapshot_, Move(snapshot), std::memory_order_release); }

IRSDataStore::DataSnapshotPtr IRSDataStore::LoadSnapshot() const { return std::atomic_load_explicit(&snapshot_, std::memory_order_acquire); }

void IRSDataStore::Commit() {
    UniqueLock<Mutex> lk(commit_mutex_);
    auto reader = index_writer_->GetSnapshot();
    auto data = MakeShared<DataSnapshot>(Move(reader));
    StoreSnapshot(data);
}

void IRSDataStore::BatchInsert(SharedPtr<DataBlock> data_block) {
    Vector<SharedPtr<ColumnVector>> column_vectors = data_block->column_vectors;
    for (SizeT i = 0; i < column_vectors.size(); ++i) {
    }
}

void IRSDataStore::Reset() { index_writer_.reset(); }

const IRSSubReader *ViewSnapshot::operator[](SizeT i) noexcept { return segments_[i].segment_; }

const ViewSegment &ViewSnapshot::GetSegment(SizeT i) noexcept { return segments_[i]; }

} // namespace infinity