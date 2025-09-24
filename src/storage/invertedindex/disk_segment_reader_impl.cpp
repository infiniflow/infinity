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
#include <sys/mman.h>

module infinity_core:disk_index_segment_reader.impl;

import :disk_index_segment_reader;
import :segment_posting;
import :index_defines;
import :index_segment_reader;
import :file_reader;
import :dict_reader;
import :term_meta;
import :byte_slice;
import :posting_list_format;
import :byte_slice_reader;
import :infinity_exception;
import :status;
import :logger;
import :persistence_manager;
import :infinity_context;
import :persist_result_handler;
import :virtual_store;

import internal_types;
import third_party;

namespace infinity {

DiskIndexSegmentReader::DiskIndexSegmentReader(SegmentID segment_id,
                                               ChunkID chunk_id,
                                               const std::string &index_dir,
                                               const std::string &base_name,
                                               RowID base_row_id,
                                               optionflag_t flag)
    : IndexSegmentReader(segment_id, chunk_id), base_row_id_(base_row_id) {
    std::filesystem::path path = std::filesystem::path(InfinityContext::instance().config()->DataDir()) / index_dir / base_name;
    std::string path_str = path.string();
    PersistenceManager *pm = InfinityContext::instance().persistence_manager();

    posting_file_ = path_str;
    posting_file_.append(POSTING_SUFFIX);
    std::string posting_file = posting_file_;
    if (nullptr != pm) {
        PersistResultHandler handler(pm);
        PersistReadResult result = pm->GetObjCache(posting_file);
        LOG_DEBUG(fmt::format("DiskIndexSegmentReader pm->GetObjCache(posting_file) {}", posting_file));
        const ObjAddr &obj_addr = handler.HandleReadResult(result);
        if (!obj_addr.Valid()) {
            // Empty posting
            return;
        }
        posting_file_obj_ = pm->GetObjPath(obj_addr.obj_key_);
        posting_file = posting_file_obj_;
    }
    if (posting_file.empty() || std::filesystem::file_size(posting_file) == 0) {
        // Empty posting
        return;
    }
    i32 rc = VirtualStore::MmapFile(posting_file, data_ptr_, data_len_);
    assert(rc == 0);
    if (rc != 0) {
        Status status = Status::MmapFileError(posting_file);
        RecoverableError(status);
    }

    dict_file_ = path_str;
    dict_file_.append(DICT_SUFFIX);
    std::string dict_file = dict_file_;
    if (nullptr != pm) {
        PersistResultHandler handler(pm);
        PersistReadResult result = pm->GetObjCache(dict_file);
        LOG_DEBUG(fmt::format("DiskIndexSegmentReader pm->GetObjCache(dict_file) {}", dict_file));
        const ObjAddr &obj_addr = handler.HandleReadResult(result);
        dict_file = pm->GetObjPath(obj_addr.obj_key_);
    }
    dict_reader_ = std::make_shared<DictionaryReader>(dict_file, PostingFormatOption(flag));
}

DiskIndexSegmentReader::~DiskIndexSegmentReader() {
    if (data_len_ == 0)
        return;
    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    std::string posting_file = posting_file_;
    if (nullptr != pm) {
        posting_file = posting_file_obj_;
    }
    i32 rc = VirtualStore::MunmapFile(posting_file);
    assert(rc == 0);
    if (rc != 0) {
        Status status = Status::MunmapFileError(posting_file);
        RecoverableError(status);
    }
    if (nullptr != pm) {
        PersistResultHandler handler(pm);
        PersistWriteResult res1 = pm->PutObjCache(dict_file_);
        LOG_DEBUG(fmt::format("~DiskIndexSegmentReader pm->PutObjCache(dict_file) {}", dict_file_));
        PersistWriteResult res2 = pm->PutObjCache(posting_file_);
        LOG_DEBUG(fmt::format("~DiskIndexSegmentReader pm->PutObjCache(posting_file) {}", posting_file_));
        handler.HandleWriteResult(res1);
        handler.HandleWriteResult(res2);
    }
}

bool DiskIndexSegmentReader::GetSegmentPosting(const std::string &term, SegmentPosting &seg_posting, bool fetch_position) const {
    TermMeta term_meta;
    if (!dict_reader_.get() || !dict_reader_->Lookup(term, term_meta)) {
        return false;
    }
    LOG_TRACE(fmt::format("DiskIndexSegmentReader::GetSegmentPosting term: {}, term_meta.doc_freq_: {}", term, term_meta.doc_freq_));
    u64 file_length = fetch_position ? (term_meta.pos_end_ - term_meta.doc_start_) : (term_meta.pos_start_ - term_meta.doc_start_);
    ByteSlice *slice = ByteSlice::NewSlice(data_ptr_ + term_meta.doc_start_, file_length);
    std::shared_ptr<ByteSliceList> byte_slice_list = std::make_shared<ByteSliceList>(slice);
    seg_posting.Init(std::move(byte_slice_list), base_row_id_, term_meta.doc_freq_, term_meta);
    return true;
}

} // namespace infinity
