module;

#include <miniocpp/client.h>

export module s3_client;

import stl;
import status;

namespace infinity {

export class S3Client {
public:
    S3Client(String _url = "http://localhost:9000", bool _https = false, String _access_key = "minioadmin", String _secret_key = "minioadmin")
        : url(_url), https(_https), access_key(_access_key), secret_key(_secret_key) {}

    virtual ~S3Client() = default;

    virtual Status DownloadObject(const String &bucket_name, const String &object_name, const String &file_path) = 0;

    virtual Status UploadObject(const String &bucket_name, const String &object_name, const String &file_path) = 0;

    virtual Status RemoveObject(const String &bucket_name, const String &object_name) = 0;

    virtual Status
    CopyObject(const String &src_bucket_name, const String &src_object_name, const String &dst_bucket_name, const String &dst_object_name) = 0;

protected:
    String url;
    bool https;
    String access_key;
    String secret_key;
};
} // namespace infinity