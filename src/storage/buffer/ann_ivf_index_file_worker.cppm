//
// Created by yzq on 11/8/23.
//
module;
import stl;
import index_data;
import file_worker;

export module ann_ivf_index_file_worker;

namespace infinity {

export struct AnnIVFFlatIndexPtr {
    AnnIVFFlatIndexData<f32> *ann_index_data_{};

    AnnIVFFlatIndexPtr(AnnIVFFlatIndexData<f32> *ann_index_data) : ann_index_data_(ann_index_data) {}

    // Not destruct here
    ~AnnIVFFlatIndexPtr() = default;
};

/*
export class AnnIVFFlatIndexFileWorker : public FileWorker {
public:
    explicit AnnIVFFlatIndexFileWorker(SharedPtr<String> file_dir, SharedPtr<String> file_name, SizeT buffer_size)
        : FileWorker(Move(file_dir), Move(file_name), buffer_size) {}

    virtual ~AnnIVFFlatIndexFileWorker() override{};

public:
    void AllocateInMemory() override{};

    void FreeInMemory() override{};

protected:
    void WriteToFileImpl(bool &prepare_success) override{};

    void ReadFromFileImpl() override{};
};
*/
} // namespace infinity
