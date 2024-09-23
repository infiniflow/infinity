// MinIO C++ Library for Amazon S3 Compatible Cloud Storage
// Copyright 2022-2024 MinIO, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef MINIO_CPP_BASECLIENT_H_INCLUDED
#define MINIO_CPP_BASECLIENT_H_INCLUDED

#include <map>
#include <string>
#include <type_traits>

#include "args.h"
#include "config.h"
#include "error.h"
#include "http.h"
#include "providers.h"
#include "request.h"
#include "response.h"
#include "utils.h"

#if defined(_WIN32) && defined(GetObject)
#pragma push_macro("GetObject")
#undef GetObject
#define MINIO_CPP_GET_OBJECT_DEFINED
#endif

namespace minio::s3 {

utils::Multimap GetCommonListObjectsQueryParams(
    const std::string& delimiter, const std::string& encoding_type,
    unsigned int max_keys, const std::string& prefix);

/**
 * Base client to perform S3 APIs.
 */
class BaseClient {
 protected:
  BaseUrl base_url_;
  creds::Provider* const provider_ = nullptr;
  std::map<std::string, std::string> region_map_;
  bool debug_ = false;
  bool ignore_cert_check_ = false;
  std::string ssl_cert_file_;
  std::string user_agent_ = DEFAULT_USER_AGENT;

 public:
  explicit BaseClient(BaseUrl base_url,
                      creds::Provider* const provider = nullptr);

  virtual ~BaseClient() = default;

  void Debug(bool flag) { debug_ = flag; }

  void IgnoreCertCheck(bool flag) { ignore_cert_check_ = flag; }

  void SetSslCertFile(std::string ssl_cert_file) {
    ssl_cert_file_ = std::move(ssl_cert_file);
  }

  error::Error SetAppInfo(std::string_view app_name,
                          std::string_view app_version);

  void HandleRedirectResponse(std::string& code, std::string& message,
                              int status_code, http::Method method,
                              const utils::Multimap& headers,
                              const std::string& bucket_name,
                              bool retry = false);
  Response GetErrorResponse(http::Response resp, std::string_view resource,
                            http::Method method, const std::string& bucket_name,
                            const std::string& object_name);
  Response execute(Request& req);
  Response Execute(Request& req);
  GetRegionResponse GetRegion(const std::string& bucket_name,
                              const std::string& region);

  AbortMultipartUploadResponse AbortMultipartUpload(
      AbortMultipartUploadArgs args);
  BucketExistsResponse BucketExists(BucketExistsArgs args);
  CompleteMultipartUploadResponse CompleteMultipartUpload(
      CompleteMultipartUploadArgs args);
  CreateMultipartUploadResponse CreateMultipartUpload(
      CreateMultipartUploadArgs args);
  DeleteBucketEncryptionResponse DeleteBucketEncryption(
      DeleteBucketEncryptionArgs args);
  DisableObjectLegalHoldResponse DisableObjectLegalHold(
      DisableObjectLegalHoldArgs args);
  DeleteBucketLifecycleResponse DeleteBucketLifecycle(
      DeleteBucketLifecycleArgs args);
  DeleteBucketNotificationResponse DeleteBucketNotification(
      DeleteBucketNotificationArgs args);
  DeleteBucketPolicyResponse DeleteBucketPolicy(DeleteBucketPolicyArgs args);
  DeleteBucketReplicationResponse DeleteBucketReplication(
      DeleteBucketReplicationArgs args);
  DeleteBucketTagsResponse DeleteBucketTags(DeleteBucketTagsArgs args);
  DeleteObjectLockConfigResponse DeleteObjectLockConfig(
      DeleteObjectLockConfigArgs args);
  DeleteObjectTagsResponse DeleteObjectTags(DeleteObjectTagsArgs args);
  EnableObjectLegalHoldResponse EnableObjectLegalHold(
      EnableObjectLegalHoldArgs args);
  GetBucketEncryptionResponse GetBucketEncryption(GetBucketEncryptionArgs args);
  GetBucketLifecycleResponse GetBucketLifecycle(GetBucketLifecycleArgs args);
  GetBucketNotificationResponse GetBucketNotification(
      GetBucketNotificationArgs args);
  GetBucketPolicyResponse GetBucketPolicy(GetBucketPolicyArgs args);
  GetBucketReplicationResponse GetBucketReplication(
      GetBucketReplicationArgs args);
  GetBucketTagsResponse GetBucketTags(GetBucketTagsArgs args);
  GetBucketVersioningResponse GetBucketVersioning(GetBucketVersioningArgs args);
  GetObjectResponse GetObject(GetObjectArgs args);
  GetObjectLockConfigResponse GetObjectLockConfig(GetObjectLockConfigArgs args);
  GetObjectRetentionResponse GetObjectRetention(GetObjectRetentionArgs args);
  GetObjectTagsResponse GetObjectTags(GetObjectTagsArgs args);
  GetPresignedObjectUrlResponse GetPresignedObjectUrl(
      GetPresignedObjectUrlArgs args);
  GetPresignedPostFormDataResponse GetPresignedPostFormData(PostPolicy policy);
  IsObjectLegalHoldEnabledResponse IsObjectLegalHoldEnabled(
      IsObjectLegalHoldEnabledArgs args);
  ListBucketsResponse ListBuckets(ListBucketsArgs args);
  ListBucketsResponse ListBuckets();
  ListenBucketNotificationResponse ListenBucketNotification(
      ListenBucketNotificationArgs args);
  ListObjectsResponse ListObjectsV1(ListObjectsV1Args args);
  ListObjectsResponse ListObjectsV2(ListObjectsV2Args args);
  ListObjectsResponse ListObjectVersions(ListObjectVersionsArgs args);
  MakeBucketResponse MakeBucket(MakeBucketArgs args);
  PutObjectResponse PutObject(PutObjectApiArgs args);
  RemoveBucketResponse RemoveBucket(RemoveBucketArgs args);
  RemoveObjectResponse RemoveObject(RemoveObjectArgs args);
  RemoveObjectsResponse RemoveObjects(RemoveObjectsApiArgs args);
  SetBucketEncryptionResponse SetBucketEncryption(SetBucketEncryptionArgs args);
  SetBucketLifecycleResponse SetBucketLifecycle(SetBucketLifecycleArgs args);
  SetBucketNotificationResponse SetBucketNotification(
      SetBucketNotificationArgs args);
  SetBucketPolicyResponse SetBucketPolicy(SetBucketPolicyArgs args);
  SetBucketReplicationResponse SetBucketReplication(
      SetBucketReplicationArgs args);
  SetBucketTagsResponse SetBucketTags(SetBucketTagsArgs args);
  SetBucketVersioningResponse SetBucketVersioning(SetBucketVersioningArgs args);
  SetObjectLockConfigResponse SetObjectLockConfig(SetObjectLockConfigArgs args);
  SetObjectRetentionResponse SetObjectRetention(SetObjectRetentionArgs args);
  SetObjectTagsResponse SetObjectTags(SetObjectTagsArgs args);
  SelectObjectContentResponse SelectObjectContent(SelectObjectContentArgs args);
  StatObjectResponse StatObject(StatObjectArgs args);
  UploadPartResponse UploadPart(UploadPartArgs args);
  UploadPartCopyResponse UploadPartCopy(UploadPartCopyArgs args);

  // Windows API fix:
  //
  // Windows API headers define `GetObject()` as a macro that expands to either
  // `GetObjectA()` or `GetObjectW()`. This means that users can get link errors
  // in case that one compilation unit used `GetObject()` macro and other
  // didn't. This fixes the issue by providing both functions `GetObject()` can
  // expand to as inline wrappers.
#if defined(_WIN32)
  inline GetObjectResponse GetObjectA(const GetObjectArgs& args) {
    return GetObject(args);
  }

  inline GetObjectResponse GetObjectW(const GetObjectArgs& args) {
    return GetObject(args);
  }
#endif  // _WIN32

};  // class BaseClient

}  // namespace minio::s3

#if defined(_WIN32) && defined(MINIO_CPP_GET_OBJECT_DEFINED)
#undef MINIO_CPP_GET_OBJECT_DEFINED
#pragma pop_macro("GetObject")
#endif

#endif  // MINIO_CPP_BASECLIENT_H_INCLUDED
