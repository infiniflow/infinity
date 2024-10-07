module;

#include <miniocpp/client.h>

module s3_client_minio;

import stl;
import infinity_exception;
import status;

namespace infinity {
Status S3ClientMinio::DownloadObject(const String &bucket_name, const String &object_name, const String &file_path) {
    // Create S3 client.
    minio::s3::Client client = GetClient();

    // Create download object arguments.
    minio::s3::DownloadObjectArgs args;
    args.bucket = bucket_name;
    args.object = object_name;
    args.filename = file_path;

    // Call download object.
    minio::s3::DownloadObjectResponse resp = client.DownloadObject(args);

    // Handle response.
    if (resp) {
        std::cout << object_name << " is successfully downloaded to " << file_path << std::endl;
    } else {
        UnrecoverableError("unable to download object; " + resp.Error().String());
    }
    return Status::OK();
}

Status S3ClientMinio::UploadObject(const String &bucket_name, const String &object_name, const String &file_path) {
    // Create S3 client.
    minio::s3::Client client = GetClient();

    // Create upload object arguments.
    minio::s3::UploadObjectArgs args;
    args.bucket = bucket_name;
    args.object = object_name;
    args.filename = file_path;

    // Call upload object.
    minio::s3::UploadObjectResponse resp = client.UploadObject(args);

    // Handle response.
    if (resp) {
        std::cout << file_path << " is successfully uploaded to " << object_name << std::endl;
    } else {
        UnrecoverableError("unable to upload object; " + resp.Error().String());
    }
    return Status::OK();
}

Status S3ClientMinio::RemoveObject(const String &bucket_name, const String &object_name) {
    // Create S3 client.
    minio::s3::Client client = GetClient();
    // Create remove object arguments.
    minio::s3::RemoveObjectArgs args;
    args.bucket = bucket_name;
    args.object = object_name;

    // Call remove object.
    minio::s3::RemoveObjectResponse resp = client.RemoveObject(args);

    // Handle response.
    if (resp) {
        std::cout << object_name << " is removed successfully" << std::endl;
    } else {
        UnrecoverableError("unable to remove object; " + resp.Error().String());
    }
    return Status::OK();
}

Status S3ClientMinio::CopyObject(const String &src_bucket_name,
                                 const String &src_object_name,
                                 const String &dst_bucket_name,
                                 const String &dst_object_name) {
    // Create S3 client.
    minio::s3::Client client = GetClient();
    // Create copy object arguments.
    minio::s3::CopyObjectArgs args;
    args.bucket = dst_bucket_name;
    args.object = dst_object_name;

    minio::s3::CopySource source;
    source.bucket = src_bucket_name;
    source.object = src_object_name;
    args.source = source;

    // Call copy object.
    minio::s3::CopyObjectResponse resp = client.CopyObject(args);

    // Handle response.
    if (resp) {
        std::cout << dst_object_name << " is successfully created from " << src_object_name << std::endl;
    } else {
        UnrecoverableError("unable to do copy object; " + resp.Error().String());
    }
    return Status::OK();
}

} // namespace infinity
