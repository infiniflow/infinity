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

module bmp_index_file_worker;

import index_bmp;
import logger;
import infinity_exception;
import third_party;
import internal_types;
import bmp_util;
import bmp_alg;
import abstract_bmp;

namespace infinity {

BMPIndexFileWorker::~BMPIndexFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

void BMPIndexFileWorker::AllocateInMemory() {
    if (data_) {
        const auto error_message = "Data is already allocated.";
        UnrecoverableError(error_message);
    }
    data_ = static_cast<void *>(new AbstractBMP());
}

void BMPIndexFileWorker::FreeInMemory() {
    if (!data_) {
        const auto error_message = "Data is not allocated.";
        UnrecoverableError(error_message);
    }
    delete reinterpret_cast<AbstractBMP *>(data_);
    data_ = nullptr;
}

void BMPIndexFileWorker::WriteToFileImpl(bool to_spill, bool &prepare_success) {
    if (!data_) {
        UnrecoverableError("Data is not allocated.");
    }
    auto *bmp_index = reinterpret_cast<AbstractBMP *>(data_);
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                index->Save(*file_handler_, index_size_);
            }
        },
        *bmp_index);
    prepare_success = true;
}

void BMPIndexFileWorker::ReadFromFileImpl() {
    if (data_ != nullptr) {
        UnrecoverableError("Data is already allocated.");
    }
    auto *bmp_index = reinterpret_cast<AbstractBMP *>(data_);
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                index = new IndexT(IndexT::Load(*file_handler_));
            }
        },
        *bmp_index);
}

} // namespace infinity
