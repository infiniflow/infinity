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

#include "faiss/impl/FaissException.h"
#include "faiss/impl/io.h"
#include "faiss/index_io.h"

import stl;
import infinity_exception;

import file_system;
import third_party;

module faiss_index_file_worker;

namespace infinity {

FaissIndexFileWorker::~FaissIndexFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

struct FSIOReader : faiss::IOReader {
    FSIOReader(FileHandler *file_handler) : file_handler_(file_handler) {}
    size_t operator()(void *ptr, size_t size, size_t nitems) override {
        file_handler_->Read(ptr, size * nitems);
        return nitems;
    }
    FileHandler *file_handler_;
};

struct FSIOWriter : faiss::IOWriter {
    FSIOWriter(FileHandler *file_handler) : file_handler_(file_handler) {}
    size_t operator()(const void *ptr, size_t size, size_t nitems) override {
        file_handler_->Write(ptr, size * nitems);
        return nitems;
    }
    FileHandler *file_handler_;
};

void FaissIndexFileWorker::AllocateInMemory() {
    if (data_) {
        Error<StorageException>("Data is already allocated.");
    }
    auto faiss_index_ptr = new FaissIndexPtr(nullptr, nullptr);
    data_ = static_cast<void *>(faiss_index_ptr);
}

void FaissIndexFileWorker::FreeInMemory() {
    if (!data_) {
        Error<StorageException>("Data is not allocated.");
    }
    auto faiss_index_ptr = static_cast<FaissIndexPtr *>(data_);
    delete faiss_index_ptr->index_;
    delete faiss_index_ptr->quantizer_;
    delete faiss_index_ptr;
    data_ = nullptr;
}

void FaissIndexFileWorker::WriteToFileImpl(bool &prepare_success) {
    try {
        auto faiss_index_ptr = static_cast<FaissIndexPtr *>(data_);
        FSIOWriter writer(file_handler_.get());
        faiss::write_index(faiss_index_ptr->index_, &writer);
        prepare_success = true; // Not run defer_fn
    } catch (faiss::FaissException &xcp) {
        Error<StorageException>(Format("Faiss exception: {}", xcp.what()));
    }
}

void FaissIndexFileWorker::ReadFromFileImpl() {
    FSIOReader reader(file_handler_.get());
    auto index = faiss::read_index(&reader);
    data_ = static_cast<void *>(new FaissIndexPtr(index, nullptr));
}
} // namespace infinity