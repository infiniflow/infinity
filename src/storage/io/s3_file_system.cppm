module;

#include <miniocpp/client.h>
import stl;
import file_system;
import file_system_type;
import status;
import infinity_exception;

export module s3_file_system;

namespace infinity {

export class S3FileHandler : public FileHandler {
public:
    S3FileHandler(FileSystem &file_system, String path, i32 fd) : FileHandler(file_system, std::move(path)), fd_(fd) {}

    ~S3FileHandler() override;

public:
    i32 fd_{-1};
};

export struct MmapInfo {
    u8 *data_ptr_{};
    SizeT data_len_{};
    SizeT rc_{};
};

export class S3FileSystem final : public FileSystem {
public:
    S3FileSystem(
        String url = "http://localhost:9000", 
        bool https = false, 
        String access_key = "minioadmin", 
        String secret_key = "minioadmin",
        String _bucket_name = "infinity") : 
        FileSystem(FileSystemType::kS3), 
        base_url(url, https), 
        provider(access_key, secret_key),
        bucket_name(_bucket_name) {
            minio::s3::Client client(base_url, &provider);
            minio::s3::BucketExistsArgs args;
            args.bucket = bucket_name;
            minio::s3::BucketExistsResponse resp = client.BucketExists(args);
            if (resp) {
                if (resp.exist) {
                std::cout << "bucket "<<bucket_name<< "exists" << std::endl;
                } else {
                    std::cout << "bucket "<<bucket_name<< "not exists" <<std::endl;
                    std::cout << "creating bucket "<<bucket_name<< std::endl;
                    minio::s3::MakeBucketArgs args;
                    args.bucket = bucket_name;
                    minio::s3::MakeBucketResponse resp1 = client.MakeBucket(args);
                    if (resp1) {
                        std::cout << "bucket "<<bucket_name<< "created" << std::endl;
                    } else {
                        UnrecoverableError("unable to create bucket "+bucket_name+"; "+resp1.Error().String());
                    }
                }
            } else {
                UnrecoverableError("unable to do bucket existence check; "+resp.Error().String());
            }
        }

    ~S3FileSystem() override = default;

    Pair<UniquePtr<FileHandler>, Status> OpenFile(const String &path, u8 flags, FileLockType lock_type) final;

    i64 Read(FileHandler &file_handler, void *data, u64 nbytes) final;

    i64 Write(FileHandler &file_handler, const void *data, u64 nbytes) final;

    i64 ReadAt(FileHandler &file_handler, i64 file_offset, void *data, u64 nbytes) final;

    i64 WriteAt(FileHandler &file_handler, i64 file_offset, const void *data, u64 nbytes) final;

    void Rename(const String &old_path, const String &new_path) final;

    void Seek(FileHandler &file_handler, i64 pos) final;

    SizeT GetFileSize(FileHandler &file_handler) final;

    void DeleteFile(const String &file_name) final;

    void SyncFile(FileHandler &file_handler) final;

    void Close(FileHandler &file_handler) final;

    void AppendFile(const String &dst_path, const String &src_path) final;

    void Truncate(const String &file_name, SizeT length);

    // Directory related methods
    bool Exists(const String &path) final; // if file or directory exists

    void CreateDirectory(const String &path) final;

    // return true if successfully created directory
    bool CreateDirectoryNoExp(const String &path);

    u64 DeleteDirectory(const String &path) final;

    // Remove everything recursively under given directory. Create it if it doesn't exist.
    void CleanupDirectory(const String &path) final;

    Vector<SharedPtr<DirEntry>> ListDirectory(const String &path) final;

    String GetAbsolutePath(const String &path);

    static u64 GetFileSizeByPath(const String& path);

    static u64 GetFolderSizeByPath(const String& path);

    static String ConcatenateFilePath(const String& dir_path, const String& file_path);

    int MmapFile(const String &file_path, u8 *&data_ptr, SizeT &data_len);

    int MunmapFile(const String &file_path);

    minio::s3::Client GetClient() { return minio::s3::Client(base_url, &provider); }

    void DownloadObject(const String &file_path);

    void UploadObject(const String &file_path);

    void RemoveObject(const String &file_path);

    void CopyObject(const String &src_path, const String &dst_path);

    void RemoveDirectory(const String &dir_path);

private:
    static std::mutex mtx_;
    static HashMap<String, MmapInfo> mapped_files_;

    minio::s3::BaseUrl base_url;
    minio::creds::StaticProvider provider;
    String bucket_name;
};

} // namespace infinity