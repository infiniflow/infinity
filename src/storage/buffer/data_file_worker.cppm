module;

import stl;
import file_worker;

export module data_file_worker;

namespace infinity {

export class DataFileWorker : public FileWorker {
public:
    explicit DataFileWorker(SharedPtr<String> relative_dir, SharedPtr<String> file_name, SizeT buffer_size)
        : FileWorker(Move(relative_dir), Move(file_name), buffer_size) {}

public:
    void AllocateInMemory() override;

    void FreeInMemory() override;

protected:
    void WriteToFileImpl(bool &prepare_success, SizeT buffer_size) override;

    void ReadFromFileImpl() override;
};
} // namespace infinity