module;

#include <string>

module infinity_core:s3_client_minio.impl;

import :s3_client_minio;

import :stl;
import :infinity_exception;
import third_party;
import :logger;

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
    LOG_INFO(fmt::format("Downloading object {} from {} to {}", object_name, bucket_name, file_path));
    if (auto resp = client_->DownloadObject(args)) {
        LOG_INFO(fmt::format("{}/{} downloaded to {} successfully", bucket_name, object_name, file_path));
    } else {
        UnrecoverableError(fmt::format("Unable to download object: {}/{}, reason: {}", bucket_name, object_name, resp.Error().String()));
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
    LOG_INFO(fmt::format("Uploading object {} to {} from {}", object_name, bucket_name, file_path));
    if (auto resp = client_->UploadObject(args)) {
        LOG_INFO(fmt::format("{} uploaded to {}/{} successfully", file_path, bucket_name, object_name));
    } else {
        UnrecoverableError(fmt::format("Unable to upload object: {}/{}, code: {}, reason: {}", bucket_name, object_name, resp.code, resp.Error().String()));
    }
    return Status::OK();
}

Status S3ClientMinio::RemoveObject(const String &bucket_name, const String &object_name) {
    // Create remove object arguments.
    minio::s3::RemoveObjectArgs args;
    args.bucket = bucket_name;
    args.object = object_name;

    // Call remove object.
    LOG_INFO(fmt::format("Removing object {} from {}", object_name, bucket_name));
    if (auto resp = client_->RemoveObject(args)) {
        LOG_INFO(fmt::format("{} is removed from {} successfully", object_name, bucket_name));
    } else {
        UnrecoverableError(fmt::format("Unable to remove object: {}/{}, reason: {}", bucket_name, object_name, resp.Error().String()));
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
    if (auto resp = client_->CopyObject(args)) {
        LOG_TRACE(fmt::format("{} is copied to {} successfully", src_object_name, dst_object_name));
    } else {
        UnrecoverableError(fmt::format("Unable to do copy object: {}", resp.Error().String()));
    }
    return Status::OK();
}

Status S3ClientMinio::BucketExists(const String &bucket_name) {
    // Create bucket exists arguments.
    minio::s3::BucketExistsArgs args;
    args.bucket = bucket_name;

    // Call bucket exists.
    if (auto resp = client_->BucketExists(args)) {
        if (resp.exist) {
            return Status::OK();
        } else {
            return Status::MinioBucketNotExists(bucket_name);
        }
    } else {
        switch (resp.status_code) {
            case 403: {
                return Status::MinioInvalidAccessKey(resp.Error().String());
            }
            default: {
                UnrecoverableError(
                    fmt::format("Unable to do bucket existence check: {}, Please check if the MINIO connection", resp.Error().String()));
                return Status::OK();
            }
        }
    }
}

Status S3ClientMinio::MakeBucket(const String &bucket_name) {
    // Create make bucket arguments.
    minio::s3::MakeBucketArgs args;
    args.bucket = bucket_name;

    // Call make bucket.
    if (auto resp = client_->MakeBucket(args)) {
        LOG_TRACE(fmt::format("{} is created successfully", bucket_name));
    } else {
        UnrecoverableError(fmt::format("Unable to create bucket: {}", resp.Error().String()));
    }
    return Status::OK();
}

} // namespace infinity
