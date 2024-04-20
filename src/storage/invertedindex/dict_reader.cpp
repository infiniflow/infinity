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

module dict_reader;

import stl;
import term_meta;
import posting_list_format;
import fst;
import mmap;
import infinity_exception;

namespace infinity {

DictionaryReader::DictionaryReader(const String &dict_path, const PostingFormatOption &option)
    : dict_path_(dict_path), meta_loader_(option), data_ptr_(nullptr), data_len_(0) {
    int rc = MmapFile(dict_path, data_ptr_, data_len_);
    if (rc < 0) {
        throw UnrecoverableException("MmapFile failed");
    }
    // fst_root_addr + addr_offset(21) == fst_len
    SizeT fst_root_addr = ReadU64LE(data_ptr_ + data_len_ - 4 - 8);
    SizeT fst_len = fst_root_addr + 21;
    u8 *fst_data = data_ptr_ + (data_len_ - fst_len);
    fst_ = MakeUnique<Fst>(fst_data, fst_len);
    s_ = MakeUnique<FstStream>(*fst_);
}

DictionaryReader::~DictionaryReader() {
    if (data_ptr_ != nullptr) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
        int rc = MunmapFile(data_ptr_, data_len_);
        assert(rc == 0);
#pragma clang diagnostic pop
    }
}

bool DictionaryReader::Lookup(const String &key, TermMeta &term_meta) {
    u64 val;
    bool found = fst_->Get((u8 *)key.c_str(), key.length(), val);
    if (!found) {
        return false;
    }
    u8 *data_cursor = data_ptr_ + val;
    SizeT left_size = data_len_ - val;
    meta_loader_.Load(data_cursor, left_size, term_meta);
    return true;
}

void DictionaryReader::InitIterator(const String &min, const String &max) {
    s_->Reset((u8 *)min.c_str(), min.length(), (u8 *)max.c_str(), max.length());
}

void DictionaryReader::InitIterator(const String &prefix) { s_->Reset((u8 *)prefix.c_str(), prefix.length()); }

bool DictionaryReader::Next(String &term, TermMeta &term_meta) {
    Vector<u8> key;
    u64 val;
    if (!s_->Next(key, val)) {
        return false;
    }
    term = String((char *)key.data(), key.size());
    u8 *data_cursor = data_ptr_ + val;
    SizeT left_size = data_len_ - val;
    meta_loader_.Load(data_cursor, left_size, term_meta);
    return true;
}

} // namespace infinity