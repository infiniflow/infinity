export module infinity_core:s3_client;

import :status;

import std;

import third_party;

namespace infinity {

export class S3Client {
public:
    S3Client(std::string _url = "http://localhost:9000",
             bool _https = false,
             std::string _access_key = "minioadmin",
             std::string _secret_key = "minioadmin")
        : url(_url), https(_https), access_key(_access_key), secret_key(_secret_key) {}

    virtual ~S3Client() = default;

    virtual Status Init() = 0;
    virtual Status UnInit() = 0;

    virtual Status DownloadObject(const std::string &bucket_name, const std::string &object_name, const std::string &file_path) = 0;

    virtual Status UploadObject(const std::string &bucket_name, const std::string &object_name, const std::string &file_path) = 0;

    virtual Status RemoveObject(const std::string &bucket_name, const std::string &object_name) = 0;

    virtual Status CopyObject(const std::string &src_bucket_name,
                              const std::string &src_object_name,
                              const std::string &dst_bucket_name,
                              const std::string &dst_object_name) = 0;

    virtual Status BucketExists(const std::string &bucket_name) = 0;
    virtual Status MakeBucket(const std::string &bucket_name) = 0;

    virtual Status ListObjects(const std::string &bucket_name, const std::string &prefix, std::vector<std::string> &object_names) = 0;

protected:
    std::string url;
    bool https;
    std::string access_key;
    std::string secret_key;
};
} // namespace infinity