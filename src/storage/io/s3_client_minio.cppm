module;

export module s3_client_minio;

import stl;
import s3_client;
import status;
import third_party;

namespace infinity {

export class S3ClientMinio final : public S3Client {
public:
    S3ClientMinio(String _url = "http://localhost:9000", bool _https = false, String _access_key = "minioadmin", String _secret_key = "minioadmin")
        : S3Client(_url, _https, _access_key, _secret_key), base_url(_url, _https), provider(_access_key, _secret_key) {
        client_ = MakeUnique<minio::s3::Client>(base_url, &provider);
    }

    ~S3ClientMinio() = default;

    Status Init() final;
    Status UnInit() final;

    Status DownloadObject(const String &bucket_name, const String &object_name, const String &file_path) final;
    Status UploadObject(const String &bucket_name, const String &object_name, const String &file_path) final;
    Status RemoveObject(const String &bucket_name, const String &object_name) final;
    Status
    CopyObject(const String &src_bucket_name, const String &src_object_name, const String &dst_bucket_name, const String &dst_object_name) final;
    bool BucketExists(const String &bucket_name) final;
    Status MakeBucket(const String &bucket_name) final;

private:
    minio::s3::BaseUrl base_url;
    minio::creds::StaticProvider provider;
    UniquePtr<minio::s3::Client> client_{};
};
} // namespace infinity