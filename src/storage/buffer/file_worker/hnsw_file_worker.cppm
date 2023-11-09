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

import infinity_exception;
import stl;
import file_worker;
import dist_func;
import knn_hnsw;

export module hnsw_file_worker;

namespace infinity {

export template <typename DataType>
class HnswFileWorker : public FileWorker {
    explicit HnswFileWorker(SharedPtr<String> file_dir, SharedPtr<String> file_name, SizeT buffer_size, UniquePtr<SpaceBase<DataType>> space)
        : FileWorker(Move(file_dir), Move(file_name), buffer_size), space_(Move(space)) {}

    virtual ~HnswFileWorker() override;

public:
    void AllocateInMemory() override;

    void FreeInMemory() override;

protected:
    void WriteToFileImpl(bool &prepare_success) override;

    void ReadFromFileImpl() override;

private:
    UniquePtr<SpaceBase<DataType>> space_;
};

template <typename DataType>
HnswFileWorker<DataType>::~HnswFileWorker() {
    if (data_ != nullptr) {
        FreeInMemory();
        data_ = nullptr;
    }
}

template <typename DataType>
void HnswFileWorker<DataType>::AllocateInMemory() {
    if (data_) {
        Error<StorageException>("Data is already allocated.");
    }
}

template <typename DataType>
void HnswFileWorker<DataType>::FreeInMemory() {
    if (!data_) {
        Error<StorageException>("Data is not allocated.");
    }
    auto hnsw_index_ptr = static_cast<KnnHnsw<DataType> *>(data_);
    delete hnsw_index_ptr;
    data_ = nullptr;
}

template <typename DataType>
void HnswFileWorker<DataType>::WriteToFileImpl(bool &prepare_success) {
    auto *hnsw_index_ptr = static_cast<KnnHnsw<DataType> *>(data_);
    hnsw_index_ptr->SaveIndexInner(*file_handler_);
    prepare_success = true;
}

template <typename DataType>
void HnswFileWorker<DataType>::ReadFromFileImpl() {
    KnnHnsw<DataType>::LoadIndexInner(*file_handler_, *space_);
}

} // namespace infinity