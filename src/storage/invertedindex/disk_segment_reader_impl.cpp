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

enum class Tag {
    kInvalid = 0,
    kTemp,
    kData,
    kS3Cache,
};

DiskIndexSegmentReader::DiskIndexSegmentReader(SegmentID segment_id,
                                               ChunkID chunk_id,
                                               const std::string &index_dir,
                                               const std::string &base_name,
                                               RowID base_row_id,
                                               optionflag_t flag)
    : IndexSegmentReader(segment_id, chunk_id), base_row_id_(base_row_id) {
    auto temp_path = std::filesystem::path(InfinityContext::instance().config()->TempDir()) / index_dir / base_name;
    auto data_path = std::filesystem::path(InfinityContext::instance().config()->DataDir()) / index_dir / base_name;
    auto temp_path_str = temp_path.string();
    auto data_path_str = data_path.string();
    auto *pm = InfinityContext::instance().persistence_manager();

    auto temp_posting_path_str = temp_path_str + POSTING_SUFFIX;
    auto data_posting_path_str = data_path_str + POSTING_SUFFIX;

    if (VirtualStore::Exists(temp_posting_path_str)) {
        posting_file_ = temp_posting_path_str;
        [[maybe_unused]] i32 rc = VirtualStore::MmapFile(posting_file_, data_ptr_, data_len_);
        // assert(rc == 0);
        // if (rc != 0) {
        //     RecoverableError(Status::MmapFileError(posting_file_));
        // }
    } else if (pm) {
        posting_file_ = data_posting_path_str;
        PersistResultHandler handler(pm);
        auto result = pm->GetObjCache(posting_file_);
        const auto &obj_addr = handler.HandleReadResult(result);
        if (!obj_addr.Valid()) {
            // Empty posting
            return;
        }
        const auto &[key, offset, size] = obj_addr;
        posting_file_obj_ = pm->GetObjPath(key);
        [[maybe_unused]] i32 rc = VirtualStore::MmapFilePart(posting_file_obj_, offset, size, data_ptr_);
        // assert(rc == 0);
        // if (rc != 0) {
        //     RecoverableError(Status::MmapFileError(posting_file_));
        // }
    } else if (VirtualStore::Exists(data_posting_path_str)) {
        posting_file_ = data_posting_path_str;
        [[maybe_unused]] i32 rc = VirtualStore::MmapFile(posting_file_, data_ptr_, data_len_);
        // assert(rc == 0);
        // if (rc != 0) {
        //     RecoverableError(Status::MmapFileError(posting_file_));
        // }
    } else {
        UnrecoverableError("Missing fulltext posting file.");
        return;
    }

    auto temp_dict_path_str = temp_path_str + DICT_SUFFIX;
    auto data_dict_path_str = data_path_str + DICT_SUFFIX;

    if (VirtualStore::Exists(temp_dict_path_str)) {
        dict_file_ = temp_dict_path_str;
    } else if (pm) {
        dict_file_ = data_dict_path_str;
        PersistResultHandler handler(pm);
        PersistReadResult result = pm->GetObjCache(dict_file_);
        LOG_DEBUG(fmt::format("DiskIndexSegmentReader pm->GetObjCache(dict_file) {}", dict_file_));
        const ObjAddr &obj_addr = handler.HandleReadResult(result);
        dict_file_ = pm->GetObjPath(obj_addr.obj_key_);
    } else if (VirtualStore::Exists(data_dict_path_str)) {
        dict_file_ = data_dict_path_str;
    } else {
        UnrecoverableError("Missing fulltext dict file.");
        return;
    }
    dict_reader_ = std::make_shared<DictionaryReader>(dict_file_, PostingFormatOption(flag));
}

DiskIndexSegmentReader::~DiskIndexSegmentReader() {
    if (data_len_ == 0) {
        return;
    }
    auto *pm = InfinityContext::instance().persistence_manager();
    if (!posting_file_obj_.empty()) {
        PersistResultHandler handler(pm);
        auto result = pm->GetObjCache(posting_file_);
        const auto &obj_addr = handler.HandleReadResult(result);
        if (!obj_addr.Valid()) {
            // Empty posting
            return;
        }
        const auto &[key, offset, size] = obj_addr;

        i32 rc = VirtualStore::MunmapFilePart(data_ptr_, offset, data_len_);
        assert(rc == 0);
        if (rc != 0) {
            RecoverableError(Status::MunmapFileError(posting_file_obj_));
        }
    } else {
        i32 rc = VirtualStore::MunmapFile(posting_file_);
        assert(rc == 0);
        if (rc != 0) {
            RecoverableError(Status::MunmapFileError(posting_file_));
        }
    }
    // if (pm) {
    //     PersistResultHandler handler(pm);
    //     PersistWriteResult res1 = pm->PutObjCache(dict_file_);
    //     LOG_DEBUG(fmt::format("~DiskIndexSegmentReader pm->PutObjCache(dict_file) {}", dict_file_));
    //     PersistWriteResult res2 = pm->PutObjCache(posting_file_);
    //     LOG_DEBUG(fmt::format("~DiskIndexSegmentReader pm->PutObjCache(posting_file) {}", posting_file_));
    //     handler.HandleWriteResult(res1);
    //     handler.HandleWriteResult(res2);
    // }
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
