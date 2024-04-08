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
#include <cassert>

module disk_index_segment_reader;

import stl;
import memory_pool;
import segment_posting;
import index_defines;
import index_segment_reader;
import file_reader;
import dict_reader;
import term_meta;
import byte_slice;
import posting_list_format;
import internal_types;
import third_party;
import byte_slice_reader;

namespace infinity {

DiskIndexSegmentReader::DiskIndexSegmentReader(const String &index_dir, const String &base_name, RowID base_row_id, optionflag_t flag)
    : base_row_id_(base_row_id) {
    Path path = Path(index_dir) / base_name;
    String path_str = path.string();
    String dict_file = path_str;
    dict_file.append(DICT_SUFFIX);
    dict_reader_ = MakeShared<DictionaryReader>(dict_file, PostingFormatOption(flag));
    String posting_file = path_str;
    posting_file.append(POSTING_SUFFIX);
    posting_reader_ = MakeShared<FileReader>(fs_, posting_file, 1024);
}

DiskIndexSegmentReader::~DiskIndexSegmentReader() {}

bool DiskIndexSegmentReader::GetSegmentPostingBack(const String &term, SegmentPosting &seg_posting, MemoryPool *session_pool) const {
    TermMeta term_meta;
    if (!dict_reader_.get() || !dict_reader_->Lookup(term, term_meta)) {
        return false;
    }
    u64 file_length = term_meta.pos_end_ - term_meta.doc_start_;
    fmt::print("term = {}, file_length = {}\n", term, file_length);
    fmt::print("file path = {}\n", posting_reader_->path_);
    ByteSlice *slice = ByteSlice::CreateSlice(file_length, session_pool);
    {
        std::lock_guard<std::mutex> lock(mutex_);
        posting_reader_->Seek(term_meta.doc_start_);
        posting_reader_->Read((char *)slice->data_, file_length);

        ByteSliceReader byte_slice_reader;
        byte_slice_reader.Open(slice);

        auto doc_skiplist_size = byte_slice_reader.ReadVUInt32();
        auto doc_list_size = byte_slice_reader.ReadVUInt32();
        auto pos_list_begin = byte_slice_reader.Tell() + doc_skiplist_size + doc_list_size;
        byte_slice_reader.Seek(pos_list_begin);
        auto pos_skiplist_size = byte_slice_reader.ReadVUInt32();
        auto pos_list_size = byte_slice_reader.ReadVUInt32();
        fmt::print("doc_skiplist_size = {}\n", doc_skiplist_size);
        fmt::print("doc_list_size = {}\n", doc_list_size);
        fmt::print("pos_skiplist_size = {}\n", pos_skiplist_size);
        fmt::print("pos_list_size = {}\n", pos_list_size);
        fmt::print("slice->data_ size = {}\n", slice->size_);

    }
    SharedPtr<ByteSliceList> byte_slice_list = MakeShared<ByteSliceList>(slice, session_pool);
    fmt::print("slice size: {}, byte_slice_list total size: {}\n", slice->size_, byte_slice_list->GetTotalSize());
    seg_posting.Init(std::move(byte_slice_list), base_row_id_, term_meta.doc_freq_, term_meta);
    return true;
}

bool DiskIndexSegmentReader::GetSegmentPosting(const String &term, SegmentPosting &seg_posting, MemoryPool *session_pool) const {
    TermMeta term_meta;
    if (!dict_reader_.get() || !dict_reader_->Lookup(term, term_meta)) {
        return false;
    }
    u64 file_length = term_meta.pos_end_ - term_meta.doc_start_;
    u64 doc_header_length = sizeof(u32) * 2;
    u64 pos_header_length = sizeof(u32) * 2;
    fmt::print("term = {}, file_length = {}\n", term, file_length);
    fmt::print("file path = {}\n", posting_reader_->path_);


    {
        std::lock_guard<std::mutex> lock(mutex_);
        ByteSlice *slice = ByteSlice::CreateSlice(file_length, session_pool);
        posting_reader_->Seek(term_meta.doc_start_);
        posting_reader_->Read((char *)slice->data_, file_length);

        ByteSliceReader byte_slice_reader;
        byte_slice_reader.Open(slice);
        auto doc_begin = byte_slice_reader.Tell();
        auto doc_skiplist_size = byte_slice_reader.ReadVUInt32();
        auto doc_list_size = byte_slice_reader.ReadVUInt32();
        auto doc_end = byte_slice_reader.Tell() + doc_skiplist_size + doc_list_size;
        auto doc_size = doc_end - doc_begin;

        auto pos_list_begin = byte_slice_reader.Tell() + doc_skiplist_size + doc_list_size;
        byte_slice_reader.Seek(pos_list_begin);
        auto pos_skiplist_size = byte_slice_reader.ReadVUInt32();
        auto pos_list_size = byte_slice_reader.ReadVUInt32();
        auto pos_end = byte_slice_reader.Tell() + pos_skiplist_size + pos_list_size;
        fmt::print("=========read all info=========\n");
        fmt::print("doc_skiplist_size = {}\n", doc_skiplist_size);
        fmt::print("doc_list_size = {}\n", doc_list_size);
        fmt::print("pos_skiplist_size = {}\n", pos_skiplist_size);
        fmt::print("pos_list_size = {}\n", pos_list_size);
        auto pos_size = pos_end - pos_list_begin;

        fmt::print("doc begin = {}, pos begin = {}\n", doc_begin, pos_list_begin);
        fmt::print("doc size = {}, pos size = {}\n", doc_size, pos_size);
        fmt::print("=========end read all info=========\n");
    }



    ByteSlice *doc_header_slice = ByteSlice::CreateSlice(doc_header_length, session_pool);
    ByteSlice *pos_header_slice = ByteSlice::CreateSlice(pos_header_length, session_pool);
    ByteSlice *doc_slice = nullptr;
    ByteSlice *pos_slice = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);

        posting_reader_->Seek(term_meta.doc_start_);
        posting_reader_->Read((char *)doc_header_slice->data_, doc_header_length);

        ByteSliceReader doc_byte_slice_reader;
        doc_byte_slice_reader.Open(doc_header_slice);
        auto doc_begin = doc_byte_slice_reader.Tell();
        auto doc_skiplist_size = doc_byte_slice_reader.ReadVUInt32();
        auto doc_list_size = doc_byte_slice_reader.ReadVUInt32();
        auto doc_header_real_length = doc_byte_slice_reader.Tell() - doc_begin;
        auto doc_size = doc_header_real_length + doc_skiplist_size + doc_list_size;
        doc_slice = ByteSlice::CreateSlice(doc_size, session_pool);
        posting_reader_->Seek(term_meta.doc_start_);
        posting_reader_->Read((char *)doc_slice->data_, doc_size);


        auto pos_begin = doc_byte_slice_reader.Tell() + doc_skiplist_size + doc_list_size;
        fmt::print("doc_start: {}, pos_begin: {}, doc_size: {}\n", term_meta.doc_start_, pos_begin, doc_size);
        posting_reader_->Seek(term_meta.doc_start_ + pos_begin);
        posting_reader_->Read((char *)pos_header_slice->data_, pos_header_length);
        ByteSliceReader pos_byte_slice_reader;
        pos_byte_slice_reader.Open(pos_header_slice);
        auto pos_byte_slice_begin = pos_byte_slice_reader.Tell();
        auto pos_skiplist_size = pos_byte_slice_reader.ReadVUInt32();
        auto pos_list_size = pos_byte_slice_reader.ReadVUInt32();
        auto pos_header_real_length = pos_byte_slice_reader.Tell() - pos_byte_slice_begin;
        fmt::print("pos_byte_slice_reader.Tell: {}, pos_byte_slice_begin: {}\n", pos_byte_slice_reader.Tell(), pos_byte_slice_begin);
        fmt::print("pos_header_real_length: {}, pos_skiplist_size: {}, pos_list_size: {}\n", pos_header_real_length, pos_skiplist_size, pos_list_size);
        auto pos_size = pos_header_real_length + pos_skiplist_size + pos_list_size;
        // assert(pos_size + doc_size == file_length);
        fmt::print("doc_size: {}, pos_size: {}, pos_header_real_length: {}\n", doc_size, pos_size, pos_header_real_length);
        pos_slice = ByteSlice::CreateSlice(pos_size, session_pool);
        posting_reader_->Seek(term_meta.doc_start_ + pos_begin);
        posting_reader_->Read((char *)pos_slice->data_, pos_size);
        ByteSliceReader pos_reader;
        pos_reader.Open(pos_slice);
        auto pos_skiplist_size_1 = pos_reader.ReadVUInt32();
        auto pos_list_size_1 = pos_reader.ReadVUInt32();
        fmt::print("pos reader read data, pos_skiplist_size: {}, pos_list_size: {}\n", pos_skiplist_size_1, pos_list_size_1);
    }
    SharedPtr<ByteSliceList> doc_byte_slice_list = MakeShared<ByteSliceList>(doc_slice, session_pool);
    fmt::print("doc slice size: {}, doc_byte_slice_list total size: {}\n", doc_slice->size_, doc_byte_slice_list->GetTotalSize());
//    seg_posting.Init(std::move(doc_byte_slice_list), base_row_id_, term_meta.doc_freq_, term_meta);

    SharedPtr<ByteSliceList> pos_byte_slice_list = MakeShared<ByteSliceList>(pos_slice, session_pool);
    fmt::print("pos slice size: {}, pos_byte_slice_list total size: {}\n", pos_slice->size_, pos_byte_slice_list->GetTotalSize());
//    seg_posting.Init(std::move(pos_byte_slice_list), base_row_id_, term_meta.doc_freq_, term_meta);
    seg_posting.Init(std::move(doc_byte_slice_list), std::move(pos_byte_slice_list), base_row_id_, term_meta.doc_freq_, term_meta);
    return true;
}


} // namespace infinity
