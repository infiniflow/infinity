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

export module infinity_core:bmp_index_file_worker;

import :stl;
import :index_file_worker;
import :file_worker;
import :index_base;
import column_def;
import :file_worker_type;
import sparse_info;
import :persistence_manager;

namespace infinity {

export class BMPIndexFileWorker final : public IndexFileWorker {
public:
    explicit BMPIndexFileWorker(SharedPtr<String> data_dir,
                                SharedPtr<String> temp_dir,
                                SharedPtr<String> file_dir,
                                SharedPtr<String> file_name,
                                SharedPtr<IndexBase> index_base,
                                SharedPtr<ColumnDef> column_def,
                                PersistenceManager *persistence_manager,
                                SizeT index_size = 0);

    ~BMPIndexFileWorker() override;

public:
    void AllocateInMemory() override;

    void FreeInMemory() override;

    FileWorkerType Type() const override { return FileWorkerType::kBMPIndexFile; }

    SizeT GetMemoryCost() const override { return index_size_; }

protected:
    bool WriteToFileImpl(bool to_spill, bool &prepare_success, const FileWorkerSaveCtx &ctx) override;

    void ReadFromFileImpl(SizeT file_size, bool from_spill) override;

    bool ReadFromMmapImpl(const void *ptr, SizeT size) override;

    void FreeFromMmapImpl() override;

private:
    SizeT index_size_{};
};

} // namespace infinity