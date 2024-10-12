module;

#include <string>

module s3_client_minio;

import stl;
import infinity_exception;
import status;
import third_party;
import logger;

namespace infinity {

Status S3ClientMinio::Init() {
    client_ = MakeUnique<minio::s3::Client>(base_url, &provider);
    return Status::OK();
}
Status S3ClientMinio::UnInit() {
    client_.reset();
    return Status::OK();
}

Status S3ClientMinio::DownloadObject(const String &bucket_name, const String &object_name, const String &file_path) {
    // Create download object arguments.
    minio::s3::DownloadObjectArgs args;
    args.bucket = bucket_name;
    args.object = object_name;
    args.filename = file_path;

    // Call download object.
    minio::s3::DownloadObjectResponse resp = client_->DownloadObject(args);

    // Handle response.
    if (resp) {
        LOG_TRACE(fmt::format("{} downloaded to {} successfully", file_path, object_name));
    } else {
        UnrecoverableError("unable to download object; " + resp.Error().String());
    }
    return Status::OK();
}

Status S3ClientMinio::UploadObject(const String &bucket_name, const String &object_name, const String &file_path) {
    // Create upload object arguments.
    minio::s3::UploadObjectArgs args;
    args.bucket = bucket_name;
    args.object = object_name;
    args.filename = file_path;

    // Call upload object.
    minio::s3::UploadObjectResponse resp = client_->UploadObject(args);

    // Handle response.
    if (resp) {
        LOG_TRACE(fmt::format("{} uploaded to {} successfully", file_path, object_name));
    } else {
        UnrecoverableError("unable to upload object; " + resp.Error().String());
    }
    return Status::OK();
}

Status S3ClientMinio::RemoveObject(const String &bucket_name, const String &object_name) {
    // Create remove object arguments.
    minio::s3::RemoveObjectArgs args;
    args.bucket = bucket_name;
    args.object = object_name;

    // Call remove object.
    minio::s3::RemoveObjectResponse resp = client_->RemoveObject(args);

    // Handle response.
    if (resp) {
        LOG_TRACE(fmt::format("{} is removed from {} successfully", object_name, bucket_name));
    } else {
        UnrecoverableError("unable to remove object; " + resp.Error().String());
    }
    return Status::OK();
}

Status S3ClientMinio::CopyObject(const String &src_bucket_name,
                                 const String &src_object_name,
                                 const String &dst_bucket_name,
                                 const String &dst_object_name) {
    // Create copy object arguments.
    minio::s3::CopyObjectArgs args;
    args.bucket = dst_bucket_name;
    args.object = dst_object_name;

    minio::s3::CopySource source;
    source.bucket = src_bucket_name;
    source.object = src_object_name;
    args.source = source;

    // Call copy object.
    minio::s3::CopyObjectResponse resp = client_->CopyObject(args);

    // Handle response.
    if (resp) {
        LOG_TRACE(fmt::format("{} is copied to {} successfully", src_object_name, dst_object_name));
    } else {
        UnrecoverableError("unable to do copy object; " + resp.Error().String());
    }
    return Status::OK();
}

bool S3ClientMinio::BucketExists(const String &bucket_name) {
    // Create bucket exists arguments.
    minio::s3::BucketExistsArgs args;
    args.bucket = bucket_name;

    // Call bucket exists.
    minio::s3::BucketExistsResponse resp = client_->BucketExists(args);
    // Handle response.
    if (resp && resp.exist) {
        return true;
    } else {
        return false;
    }

    return false;
}

} // namespace infinity
