module;

#include "faiss/index_io.h"

import stl;
import faiss;
import file_worker;

export module faiss_index_file_worker;

namespace infinity {

export struct FaissIndexPtr {
    faiss::Index *index_{};
    faiss::Index *quantizer_{};

    FaissIndexPtr(faiss::Index *index, faiss::Index *quantizer) : index_(index), quantizer_(quantizer) {}

    // Not destruct here
    ~FaissIndexPtr() = default;
};

export class FaissIndexFileWorker : public FileWorker {
public:
    explicit FaissIndexFileWorker(SharedPtr<String> relative_dir, SharedPtr<String> file_name, SizeT buffer_size)
        : FileWorker(Move(relative_dir), Move(file_name), buffer_size) {}

    virtual ~FaissIndexFileWorker() override;

public:
    void AllocateInMemory() override;

    void FreeInMemory() override;

protected:
    void WriteToFileImpl(bool &prepare_success, SizeT buffer_size) override;

    void ReadFromFileImpl() override;
};

} // namespace infinity