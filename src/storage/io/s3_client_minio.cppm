module;

import stl;
import s3_client;
#include <miniocpp/client.h>

export module s3_client_minio;

namespace infinity {

class S3ClientMinio final: public S3Client {
    public:
        S3ClientMinio(String _url = "http://localhost:9000",
                     bool _https = false,
                     String _access_key = "minioadmin",
                     String _secret_key = "minioadmin") : S3Client(_url, _https, _access_key, _secret_key),
                                                          base_url(_url, _https),
                                                          provider(_access_key, _secret_key) {}

        ~S3ClientMinio() = default;

        void DownloadObject(const String & bucket_name, const String &object_name, const String &file_path);

        void UploadObject(const String & bucket_name, const String &object_name, const String &file_path);

        void RemoveObject(const String & bucket_name, const String &object_name);

        void CopyObject(const String & src_bucket_name, const String &src_object_name, const String & dst_bucket_name, const String &dst_object_name);

        minio::s3::Client GetClient() { return minio::s3::Client(base_url, &provider); }

    private:
        minio::s3::BaseUrl base_url;
        minio::creds::StaticProvider provider;
};
}