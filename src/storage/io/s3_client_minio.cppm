module;

export module infinity_core:s3_client_minio;

import :stl;
import :s3_client;

import third_party;
import global_resource_usage;

namespace infinity {

export class S3ClientMinio final : public S3Client {
public:
    S3ClientMinio(const std::string &url = "http://localhost:9000",
                  bool https = false,
                  const std::string &access_key = "minioadmin",
                  const std::string &secret_key = "minioadmin")
        : S3Client(url, https, access_key, secret_key), base_url(url, https), provider(access_key, secret_key) {
        client_ = std::make_unique<minio::s3::Client>(base_url, &provider);

#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("S3ClientMinio");
#endif
    }

    ~S3ClientMinio() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("S3ClientMinio");
#endif
    }

    Status Init() final;
    Status UnInit() final;

    Status DownloadObject(const std::string &bucket_name, const std::string &object_name, const std::string &file_path) final;
    Status UploadObject(const std::string &bucket_name, const std::string &object_name, const std::string &file_path) final;
    Status RemoveObject(const std::string &bucket_name, const std::string &object_name) final;
    Status
    CopyObject(const std::string &src_bucket_name, const std::string &src_object_name, const std::string &dst_bucket_name, const std::string &dst_object_name) final;
    Status BucketExists(const std::string &bucket_name) final;
    Status MakeBucket(const std::string &bucket_name) final;

private:
    minio::s3::BaseUrl base_url;
    minio::creds::StaticProvider provider;
    std::unique_ptr<minio::s3::Client> client_{};
};
} // namespace infinity