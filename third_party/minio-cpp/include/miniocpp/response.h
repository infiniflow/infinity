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

#ifndef MINIO_CPP_RESPONSE_H_INCLUDED
#define MINIO_CPP_RESPONSE_H_INCLUDED

#include <list>
#include <map>
#include <string>
#include <type_traits>

#include "error.h"
#include "types.h"
#include "utils.h"

namespace minio::s3 {

struct Response {
 public:
  int status_code = 0;
  utils::Multimap headers;
  std::string data;

  std::string code;
  std::string message;
  std::string resource;
  std::string request_id;
  std::string host_id;
  std::string bucket_name;
  std::string object_name;

 private:
  error::Error err_;

 public:
  Response();
  explicit Response(error::Error err) : err_(std::move(err)) {}

  Response(const Response& resp) = default;
  Response& operator=(const Response& resp) = default;

  Response(Response&& resp) = default;
  Response& operator=(Response&& resp) = default;

  ~Response();

  explicit operator bool() const {
    return !err_ && code.empty() && message.empty() &&
           (status_code == 0 || (status_code >= 200 && status_code <= 299));
  }

  error::Error Error() const;

  static Response ParseXML(std::string_view data, int status_code,
                           utils::Multimap headers);
};  // struct Response

#define MINIO_S3_DERIVE_FROM_RESPONSE(DerivedName)                       \
  struct DerivedName : public Response {                                 \
    DerivedName() = default;                                             \
    ~DerivedName() = default;                                            \
                                                                         \
    DerivedName(const DerivedName&) = default;                           \
    DerivedName& operator=(const DerivedName&) = default;                \
                                                                         \
    DerivedName(DerivedName&&) = default;                                \
    DerivedName& operator=(DerivedName&&) = default;                     \
                                                                         \
    explicit DerivedName(error::Error err) : Response(std::move(err)) {} \
    explicit DerivedName(const Response& resp) : Response(resp) {}       \
  };

#define MINIO_S3_DERIVE_FROM_PUT_OBJECT_RESPONSE(DerivedName)               \
  struct DerivedName : public PutObjectResponse {                           \
    DerivedName() = default;                                                \
    ~DerivedName() = default;                                               \
                                                                            \
    DerivedName(const DerivedName&) = default;                              \
    DerivedName& operator=(const DerivedName&) = default;                   \
                                                                            \
    DerivedName(DerivedName&&) = default;                                   \
    DerivedName& operator=(DerivedName&&) = default;                        \
                                                                            \
    explicit DerivedName(error::Error err)                                  \
        : PutObjectResponse(std::move(err)) {}                              \
    explicit DerivedName(const PutObjectResponse& resp)                     \
        : PutObjectResponse(resp) {}                                        \
    explicit DerivedName(const Response& resp) : PutObjectResponse(resp) {} \
                                                                            \
    explicit DerivedName(const CompleteMultipartUploadResponse& resp)       \
        : PutObjectResponse(resp) {}                                        \
  };

struct GetRegionResponse : public Response {
  std::string region;

  explicit GetRegionResponse(std::string region) : region(std::move(region)) {}

  explicit GetRegionResponse(error::Error err) : Response(std::move(err)) {}

  explicit GetRegionResponse(const Response& resp) : Response(resp) {}

  ~GetRegionResponse() = default;
};  // struct GetRegionResponse

MINIO_S3_DERIVE_FROM_RESPONSE(MakeBucketResponse)

struct ListBucketsResponse : public Response {
  std::list<Bucket> buckets;

  explicit ListBucketsResponse(std::list<Bucket> buckets)
      : buckets(std::move(buckets)) {}

  explicit ListBucketsResponse(error::Error err) : Response(std::move(err)) {}

  explicit ListBucketsResponse(const Response& resp) : Response(resp) {}

  ~ListBucketsResponse() = default;

  static ListBucketsResponse ParseXML(std::string_view data);
};  // struct ListBucketsResponse

struct BucketExistsResponse : public Response {
  bool exist = false;

  explicit BucketExistsResponse(bool exist) : exist(exist) {}

  explicit BucketExistsResponse(error::Error err) : Response(std::move(err)) {}

  explicit BucketExistsResponse(const Response& resp) : Response(resp) {}

  ~BucketExistsResponse() = default;
};  // struct BucketExistsResponse

MINIO_S3_DERIVE_FROM_RESPONSE(RemoveBucketResponse)
MINIO_S3_DERIVE_FROM_RESPONSE(AbortMultipartUploadResponse)

struct CompleteMultipartUploadResponse : public Response {
  std::string location;
  std::string etag;
  std::string version_id;

  CompleteMultipartUploadResponse() = default;

  explicit CompleteMultipartUploadResponse(error::Error err)
      : Response(std::move(err)) {}

  explicit CompleteMultipartUploadResponse(const Response& resp)
      : Response(resp) {}

  ~CompleteMultipartUploadResponse() = default;

  static CompleteMultipartUploadResponse ParseXML(std::string_view data,
                                                  std::string version_id);
};  // struct CompleteMultipartUploadResponse

struct CreateMultipartUploadResponse : public Response {
  std::string upload_id;

  explicit CreateMultipartUploadResponse(std::string upload_id)
      : upload_id(std::move(upload_id)) {}

  explicit CreateMultipartUploadResponse(error::Error err)
      : Response(std::move(err)) {}

  explicit CreateMultipartUploadResponse(const Response& resp)
      : Response(resp) {}

  ~CreateMultipartUploadResponse() = default;
};  // struct CreateMultipartUploadResponse

struct PutObjectResponse : public Response {
  std::string etag;
  std::string version_id;

  PutObjectResponse() = default;

  explicit PutObjectResponse(error::Error err) : Response(std::move(err)) {}

  explicit PutObjectResponse(const Response& resp) : Response(resp) {}

  explicit PutObjectResponse(const CompleteMultipartUploadResponse& resp)
      : Response(resp), etag(resp.etag), version_id(resp.version_id) {}

  ~PutObjectResponse() = default;
};  // struct PutObjectResponse

MINIO_S3_DERIVE_FROM_PUT_OBJECT_RESPONSE(UploadPartResponse)
MINIO_S3_DERIVE_FROM_PUT_OBJECT_RESPONSE(UploadPartCopyResponse)

struct StatObjectResponse : public Response {
  std::string version_id;
  std::string etag;
  size_t size = 0;
  utils::UtcTime last_modified;
  RetentionMode retention_mode;
  utils::UtcTime retention_retain_until_date;
  LegalHold legal_hold;
  bool delete_marker;
  utils::Multimap user_metadata;

  StatObjectResponse() = default;

  explicit StatObjectResponse(error::Error err) : Response(std::move(err)) {}

  explicit StatObjectResponse(const Response& resp) : Response(resp) {}

  ~StatObjectResponse() = default;
};  // struct StatObjectResponse

MINIO_S3_DERIVE_FROM_RESPONSE(RemoveObjectResponse)
MINIO_S3_DERIVE_FROM_RESPONSE(DownloadObjectResponse)
MINIO_S3_DERIVE_FROM_RESPONSE(GetObjectResponse)

struct Item : public Response {
  std::string etag;  // except DeleteMarker
  std::string name;
  utils::UtcTime last_modified;
  std::string owner_id;
  std::string owner_name;
  size_t size = 0;  // except DeleteMarker
  std::string storage_class;
  bool is_latest = false;  // except ListObjects V1/V2
  std::string version_id;  // except ListObjects V1/V2
  std::map<std::string, std::string> user_metadata;
  bool is_prefix = false;
  bool is_delete_marker = false;
  std::string encoding_type;

  Item() = default;

  explicit Item(error::Error err) : Response(std::move(err)) {}

  explicit Item(const Response& resp) : Response(resp) {}

  ~Item() = default;
};  // struct Item

struct ListObjectsResponse : public Response {
  // Common
  std::string name;
  std::string encoding_type;
  std::string prefix;
  std::string delimiter;
  bool is_truncated;
  unsigned int max_keys;
  std::list<Item> contents;

  // ListObjectsV1
  std::string marker;
  std::string next_marker;

  // ListObjectsV2
  unsigned int key_count;
  std::string start_after;
  std::string continuation_token;
  std::string next_continuation_token;

  // ListObjectVersions
  std::string key_marker;
  std::string next_key_marker;
  std::string version_id_marker;
  std::string next_version_id_marker;

  ListObjectsResponse() = default;

  explicit ListObjectsResponse(error::Error err) : Response(std::move(err)) {}

  explicit ListObjectsResponse(const Response& resp) : Response(resp) {}

  ~ListObjectsResponse() = default;

  static ListObjectsResponse ParseXML(std::string_view data, bool version);
};  // struct ListObjectsResponse

MINIO_S3_DERIVE_FROM_PUT_OBJECT_RESPONSE(CopyObjectResponse)
MINIO_S3_DERIVE_FROM_PUT_OBJECT_RESPONSE(ComposeObjectResponse)
MINIO_S3_DERIVE_FROM_PUT_OBJECT_RESPONSE(UploadObjectResponse)

struct DeletedObject : public Response {
  std::string name;
  std::string version_id;
  bool delete_marker;
  std::string delete_marker_version_id;

  DeletedObject() = default;
  ~DeletedObject() = default;
};  // struct DeletedObject

struct DeleteError : public Response {
  std::string version_id;

  DeleteError() = default;

  explicit DeleteError(error::Error err) : Response(std::move(err)) {}

  explicit DeleteError(const Response& resp) : Response(resp) {}

  ~DeleteError() = default;
};  // struct DeleteError

struct RemoveObjectsResponse : public Response {
  std::list<DeletedObject> objects;
  std::list<DeleteError> errors;

  RemoveObjectsResponse() = default;

  explicit RemoveObjectsResponse(error::Error err) : Response(std::move(err)) {}

  explicit RemoveObjectsResponse(const Response& resp) : Response(resp) {}

  ~RemoveObjectsResponse() = default;

  static RemoveObjectsResponse ParseXML(std::string_view data);
};  // struct RemoveObjectsResponse

MINIO_S3_DERIVE_FROM_RESPONSE(SelectObjectContentResponse)
MINIO_S3_DERIVE_FROM_RESPONSE(ListenBucketNotificationResponse)
MINIO_S3_DERIVE_FROM_RESPONSE(DeleteBucketPolicyResponse)

struct GetBucketPolicyResponse : public Response {
  std::string policy;

  explicit GetBucketPolicyResponse(std::string policy)
      : policy(std::move(policy)) {}

  explicit GetBucketPolicyResponse(error::Error err)
      : Response(std::move(err)) {}

  explicit GetBucketPolicyResponse(const Response& resp) : Response(resp) {}

  ~GetBucketPolicyResponse() = default;
};  // struct GetBucketPolicyResponse

MINIO_S3_DERIVE_FROM_RESPONSE(SetBucketPolicyResponse)
MINIO_S3_DERIVE_FROM_RESPONSE(DeleteBucketNotificationResponse)

struct GetBucketNotificationResponse : public Response {
  NotificationConfig config;

  explicit GetBucketNotificationResponse(NotificationConfig config)
      : config(std::move(config)) {}

  explicit GetBucketNotificationResponse(error::Error err)
      : Response(std::move(err)) {}

  explicit GetBucketNotificationResponse(const Response& resp)
      : Response(resp) {}

  ~GetBucketNotificationResponse() = default;

  static GetBucketNotificationResponse ParseXML(std::string_view data);
};  // struct GetBucketNotificationResponse

MINIO_S3_DERIVE_FROM_RESPONSE(SetBucketNotificationResponse)
MINIO_S3_DERIVE_FROM_RESPONSE(DeleteBucketEncryptionResponse)

struct GetBucketEncryptionResponse : public Response {
  SseConfig config;

  explicit GetBucketEncryptionResponse(SseConfig config)
      : config(std::move(config)) {}

  explicit GetBucketEncryptionResponse(error::Error err)
      : Response(std::move(err)) {}

  explicit GetBucketEncryptionResponse(const Response& resp) : Response(resp) {}

  ~GetBucketEncryptionResponse() = default;

  static GetBucketEncryptionResponse ParseXML(std::string_view data);
};  // struct GetBucketEncryptionResponse

MINIO_S3_DERIVE_FROM_RESPONSE(SetBucketEncryptionResponse)

struct GetBucketVersioningResponse : public Response {
  Boolean status;
  Boolean mfa_delete;

  GetBucketVersioningResponse() = default;

  explicit GetBucketVersioningResponse(error::Error err)
      : Response(std::move(err)) {}

  explicit GetBucketVersioningResponse(const Response& resp) : Response(resp) {}

  ~GetBucketVersioningResponse() = default;

  std::string Status() const {
    if (!status) return "Off";
    return status.Get() ? "Enabled" : "Suspended";
  }

  std::string MfaDelete() const {
    if (!mfa_delete) {
      return {};
    }
    return mfa_delete.Get() ? "Enabled" : "Disabled";
  }
};  // struct GetBucketVersioningResponse

MINIO_S3_DERIVE_FROM_RESPONSE(SetBucketVersioningResponse)
MINIO_S3_DERIVE_FROM_RESPONSE(DeleteBucketReplicationResponse)

struct GetBucketReplicationResponse : public Response {
  ReplicationConfig config;

  explicit GetBucketReplicationResponse(ReplicationConfig config)
      : config(std::move(config)) {}

  explicit GetBucketReplicationResponse(error::Error err)
      : Response(std::move(err)) {}

  explicit GetBucketReplicationResponse(const Response& resp)
      : Response(resp) {}

  ~GetBucketReplicationResponse() = default;

  static GetBucketReplicationResponse ParseXML(std::string_view data);
};  // struct GetBucketReplicationResponse

MINIO_S3_DERIVE_FROM_RESPONSE(SetBucketReplicationResponse)
MINIO_S3_DERIVE_FROM_RESPONSE(DeleteBucketLifecycleResponse)

struct GetBucketLifecycleResponse : public Response {
  LifecycleConfig config;

  explicit GetBucketLifecycleResponse(LifecycleConfig config)
      : config(std::move(config)) {}

  explicit GetBucketLifecycleResponse(error::Error err)
      : Response(std::move(err)) {}

  explicit GetBucketLifecycleResponse(const Response& resp) : Response(resp) {}

  static GetBucketLifecycleResponse ParseXML(std::string_view data);
};  // struct GetBucketLifecycleResponse

MINIO_S3_DERIVE_FROM_RESPONSE(SetBucketLifecycleResponse)
MINIO_S3_DERIVE_FROM_RESPONSE(DeleteBucketTagsResponse)

struct GetBucketTagsResponse : public Response {
  std::map<std::string, std::string> tags;

  GetBucketTagsResponse(std::map<std::string, std::string> tags)
      : tags(std::move(tags)) {}

  explicit GetBucketTagsResponse(error::Error err) : Response(std::move(err)) {}

  explicit GetBucketTagsResponse(const Response& resp) : Response(resp) {}

  ~GetBucketTagsResponse() = default;

  static GetBucketTagsResponse ParseXML(std::string_view data);
};  // struct GetBucketTagsResponse

MINIO_S3_DERIVE_FROM_RESPONSE(SetBucketTagsResponse)
MINIO_S3_DERIVE_FROM_RESPONSE(DeleteObjectLockConfigResponse)

struct GetObjectLockConfigResponse : public Response {
  ObjectLockConfig config;

  explicit GetObjectLockConfigResponse(ObjectLockConfig config)
      : config(std::move(config)) {}

  explicit GetObjectLockConfigResponse(error::Error err)
      : Response(std::move(err)) {}

  explicit GetObjectLockConfigResponse(const Response& resp) : Response(resp) {}

  ~GetObjectLockConfigResponse() = default;
};  // struct GetObjectLockConfigResponse

MINIO_S3_DERIVE_FROM_RESPONSE(SetObjectLockConfigResponse)
MINIO_S3_DERIVE_FROM_RESPONSE(DeleteObjectTagsResponse)

struct GetObjectTagsResponse : public Response {
  std::map<std::string, std::string> tags;

  GetObjectTagsResponse(std::map<std::string, std::string> tags)
      : tags(std::move(tags)) {}

  explicit GetObjectTagsResponse(error::Error err) : Response(std::move(err)) {}

  explicit GetObjectTagsResponse(const Response& resp) : Response(resp) {}

  ~GetObjectTagsResponse() = default;

  static GetObjectTagsResponse ParseXML(std::string_view data);
};  // struct GetObjectTagsResponse

MINIO_S3_DERIVE_FROM_RESPONSE(SetObjectTagsResponse)
MINIO_S3_DERIVE_FROM_RESPONSE(EnableObjectLegalHoldResponse)
MINIO_S3_DERIVE_FROM_RESPONSE(DisableObjectLegalHoldResponse)

struct IsObjectLegalHoldEnabledResponse : public Response {
  bool enabled = false;

  explicit IsObjectLegalHoldEnabledResponse(bool enabled) : enabled(enabled) {}

  explicit IsObjectLegalHoldEnabledResponse(error::Error err)
      : Response(std::move(err)) {}

  explicit IsObjectLegalHoldEnabledResponse(const Response& resp)
      : Response(resp) {}

  ~IsObjectLegalHoldEnabledResponse() = default;
};  // struct IsObjectLegalHoldEnabledResponse

struct GetObjectRetentionResponse : public Response {
  RetentionMode retention_mode;
  utils::UtcTime retain_until_date;

  GetObjectRetentionResponse() = default;

  explicit GetObjectRetentionResponse(error::Error err)
      : Response(std::move(err)) {}

  explicit GetObjectRetentionResponse(const Response& resp) : Response(resp) {}

  ~GetObjectRetentionResponse() = default;
};  // struct GetObjectRetentionResponse

MINIO_S3_DERIVE_FROM_RESPONSE(SetObjectRetentionResponse)

struct GetPresignedObjectUrlResponse : public Response {
  std::string url;

  explicit GetPresignedObjectUrlResponse(std::string url)
      : url(std::move(url)) {}

  explicit GetPresignedObjectUrlResponse(error::Error err)
      : Response(std::move(err)) {}

  explicit GetPresignedObjectUrlResponse(const Response& resp)
      : Response(resp) {}

  ~GetPresignedObjectUrlResponse() = default;
};  // struct GetPresignedObjectUrlResponse

struct GetPresignedPostFormDataResponse : public Response {
  std::map<std::string, std::string> form_data;

  GetPresignedPostFormDataResponse(std::map<std::string, std::string> form_data)
      : form_data(std::move(form_data)) {}

  explicit GetPresignedPostFormDataResponse(error::Error err)
      : Response(std::move(err)) {}

  explicit GetPresignedPostFormDataResponse(const Response& resp)
      : Response(resp) {}

  ~GetPresignedPostFormDataResponse() = default;
};  // struct GetPresignedPostFormDataResponse

#undef MINIO_S3_DERIVE_FROM_PUT_OBJECT_RESPONSE
#undef MINIO_S3_DERIVE_FROM_RESPONSE

}  // namespace minio::s3

#endif  // MINIO_CPP_RESPONSE_H_INCLUDED
