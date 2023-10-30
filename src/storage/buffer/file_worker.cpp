module;

import stl;
import infinity_assert;
import infinity_exception;
import local_file_system;
import third_party;
import file_system_type;
import defer_op;

module file_worker;

namespace infinity {

FileWorker::~FileWorker() {
    // if (file_handler_.get() != nullptr) {
    //     LocalFileSystem fs;
    //     fs.Close(*file_handler_);
    // }
}

void FileWorker::Sync() {
    LocalFileSystem fs;
    fs.SyncFile(*file_handler_);
}

void FileWorker::CloseFile() {
    LocalFileSystem fs;
    fs.Close(*file_handler_);
}

void FileWorker::WriteToFile(bool to_spill, SizeT buffer_size) {
    if (buffer_size == 0) {
        buffer_size = buffer_size_;
    } else {
        Assert<StorageException>(buffer_size <= buffer_size_, "Invalid buffer size.", __FILE_NAME__, __LINE__);
    }
    if (data_ == nullptr) {
        Error<StorageException>("No data will be written.", __FILE_NAME__, __LINE__);
    }
    LocalFileSystem fs;

    String write_dir = ChooseFileDir(to_spill);
    if (!fs.Exists(write_dir)) {
        fs.CreateDirectory(write_dir);
    }
    String write_path = Format("{}/{}", write_dir, *file_name_);

    u8 flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG;
    file_handler_ = fs.OpenFile(write_path, flags, FileLockType::kWriteLock);
    bool prepare_success = false;
    DeferFn defer_fn([&]() {
        if (!prepare_success) {
            file_handler_->Close();
            file_handler_ = nullptr;
        }
    });
    WriteToFileImpl(prepare_success, buffer_size);
}

void FileWorker::ReadFromFile(bool from_spill) {
    LocalFileSystem fs;

    String read_path = Format("{}/{}", ChooseFileDir(from_spill), *file_name_);
    u8 flags = FileFlags::READ_FLAG;
    file_handler_ = fs.OpenFile(read_path, flags, FileLockType::kReadLock);
    DeferFn defer_fn([&]() {
        file_handler_->Close();
        file_handler_ = nullptr;
    });
    ReadFromFileImpl();
}

void FileWorker::MoveFile() {
    LocalFileSystem fs;

    String src_path = Format("{}/{}", ChooseFileDir(true), *file_name_);
    String dest_dir = ChooseFileDir(false);
    String dest_path = Format("{}/{}", dest_dir, *file_name_);
    if (!fs.Exists(src_path)) {
        Error<StorageException>(Format("File {} doesn't exist.", src_path), __FILE_NAME__, __LINE__);
    }
    if (!fs.Exists(dest_dir)) {
        fs.CreateDirectory(dest_dir);
    }
    if (fs.Exists(dest_path)) {
        Error<StorageException>(Format("File {} was already been created before.", dest_path), __FILE_NAME__, __LINE__);
    }
    fs.Rename(src_path, dest_path);
}

} // namespace infinity