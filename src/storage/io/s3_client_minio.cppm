module;

export module infinity_core:s3_client_minio;

import :stl;
import :s3_client;
import third_party;

import global_resource_usage;

namespace infinity {

export class S3ClientMinio final : public S3Client {
public:
    S3ClientMinio(const String &url = "http://localhost:9000",
                  bool https = false,
                  const String &access_key = "minioadmin",
                  const String &secret_key = "minioadmin")
        : S3Client(url, https, access_key, secret_key), base_url(url, https), provider(access_key, secret_key) {
        client_ = MakeUnique<minio::s3::Client>(base_url, &provider);

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

    Status DownloadObject(const String &bucket_name, const String &object_name, const String &file_path) final;
    Status UploadObject(const String &bucket_name, const String &object_name, const String &file_path) final;
    Status RemoveObject(const String &bucket_name, const String &object_name) final;
    Status
    CopyObject(const String &src_bucket_name, const String &src_object_name, const String &dst_bucket_name, const String &dst_object_name) final;
    Status BucketExists(const String &bucket_name) final;
    Status MakeBucket(const String &bucket_name) final;

private:
    minio::s3::BaseUrl base_url;
    minio::creds::StaticProvider provider;
    UniquePtr<minio::s3::Client> client_{};
};
} // namespace infinity