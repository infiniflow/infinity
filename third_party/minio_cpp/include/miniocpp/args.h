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

#ifndef MINIO_CPP_ARGS_H_INCLUDED
#define MINIO_CPP_ARGS_H_INCLUDED

#include <functional>
#include <list>
#include <map>
#include <string>
#include <type_traits>

#include "error.h"
#include "http.h"
#include "sse.h"
#include "types.h"
#include "utils.h"

namespace minio::s3 {

struct BaseArgs {
  utils::Multimap extra_headers;
  utils::Multimap extra_query_params;

  BaseArgs() = default;
  ~BaseArgs() = default;
};  // struct BaseArgs

struct BucketArgs : public BaseArgs {
  std::string bucket;
  std::string region;

  BucketArgs() = default;
  ~BucketArgs() = default;

  error::Error Validate() const;
};  // struct BucketArgs

struct ObjectArgs : public BucketArgs {
  std::string object;

  ObjectArgs() = default;
  ~ObjectArgs() = default;

  error::Error Validate() const;
};  // struct ObjectArgs

struct ObjectWriteArgs : public ObjectArgs {
  utils::Multimap headers;
  utils::Multimap user_metadata;
  Sse* sse = nullptr;
  std::map<std::string, std::string> tags;
  Retention* retention = nullptr;
  bool legal_hold = false;

  ObjectWriteArgs() = default;
  ~ObjectWriteArgs() = default;

  utils::Multimap Headers() const;
};  // struct ObjectWriteArgs

struct ObjectVersionArgs : public ObjectArgs {
  std::string version_id;

  ObjectVersionArgs() = default;
  ~ObjectVersionArgs() = default;
};  // struct ObjectVersionArgs

struct ObjectReadArgs : public ObjectVersionArgs {
  SseCustomerKey* ssec = nullptr;

  ObjectReadArgs() = default;
  ~ObjectReadArgs() = default;
};  // struct ObjectReadArgs

struct ObjectConditionalReadArgs : public ObjectReadArgs {
  size_t* offset = nullptr;
  size_t* length = nullptr;
  std::string match_etag;
  std::string not_match_etag;
  utils::UtcTime modified_since;
  utils::UtcTime unmodified_since;

  ObjectConditionalReadArgs() = default;
  ~ObjectConditionalReadArgs() = default;

  utils::Multimap Headers() const;
  utils::Multimap CopyHeaders() const;
};  // struct ObjectConditionalReadArgs

struct MakeBucketArgs : public BucketArgs {
  bool object_lock = false;

  MakeBucketArgs() = default;
  ~MakeBucketArgs() = default;

  error::Error Validate() const;
};  // struct MakeBucketArgs

using ListBucketsArgs = BaseArgs;

using BucketExistsArgs = BucketArgs;

using RemoveBucketArgs = BucketArgs;

struct AbortMultipartUploadArgs : public ObjectArgs {
  std::string upload_id;

  AbortMultipartUploadArgs() = default;
  ~AbortMultipartUploadArgs() = default;

  error::Error Validate() const;
};  // struct AbortMultipartUploadArgs

struct CompleteMultipartUploadArgs : public ObjectArgs {
  std::string upload_id;
  std::list<Part> parts;

  CompleteMultipartUploadArgs() = default;
  ~CompleteMultipartUploadArgs() = default;

  error::Error Validate() const;
};  // struct CompleteMultipartUploadArgs

struct CreateMultipartUploadArgs : public ObjectArgs {
  utils::Multimap headers;

  CreateMultipartUploadArgs() = default;
  ~CreateMultipartUploadArgs() = default;
};  // struct CreateMultipartUploadArgs

struct PutObjectBaseArgs : public ObjectWriteArgs {
  long object_size = -1;
  size_t part_size = 0;
  long part_count = 0;
  std::string content_type;

  PutObjectBaseArgs() = default;
  ~PutObjectBaseArgs() = default;
};  // struct PutObjectBaseArgs

struct PutObjectApiArgs : public PutObjectBaseArgs {
  std::string_view data;
  utils::Multimap query_params;
  http::ProgressFunction progressfunc = nullptr;
  void* progress_userdata = nullptr;

  PutObjectApiArgs() = default;
  ~PutObjectApiArgs() = default;
};  // struct PutObjectApiArgs

struct UploadPartArgs : public ObjectWriteArgs {
  std::string upload_id;
  unsigned int part_number;
  std::string_view data;
  http::ProgressFunction progressfunc = nullptr;
  void* progress_userdata = nullptr;

  UploadPartArgs() = default;
  ~UploadPartArgs() = default;

  error::Error Validate() const;
};  // struct UploadPartArgs

struct UploadPartCopyArgs : public ObjectWriteArgs {
  std::string upload_id;
  unsigned int part_number;
  utils::Multimap headers;

  UploadPartCopyArgs() = default;
  ~UploadPartCopyArgs() = default;

  error::Error Validate() const;
};  // struct UploadPartCopyArgs

using StatObjectArgs = ObjectConditionalReadArgs;

using RemoveObjectArgs = ObjectVersionArgs;

struct DownloadObjectArgs : public ObjectReadArgs {
  std::string filename;
  bool overwrite;
  http::ProgressFunction progressfunc = nullptr;
  void* progress_userdata = nullptr;

  DownloadObjectArgs() = default;
  ~DownloadObjectArgs() = default;

  error::Error Validate() const;
};  // struct DownloadObjectArgs

struct GetObjectArgs : public ObjectConditionalReadArgs {
  http::DataFunction datafunc;
  void* userdata = nullptr;
  http::ProgressFunction progressfunc = nullptr;
  void* progress_userdata = nullptr;

  GetObjectArgs() = default;
  ~GetObjectArgs() = default;

  error::Error Validate() const;
};  // struct GetObjectArgs

struct ListObjectsArgs : public BucketArgs {
  std::string delimiter;
  bool use_url_encoding_type = true;
  std::string marker;       // only for ListObjectsV1.
  std::string start_after;  // only for ListObjectsV2.
  std::string key_marker;   // only for GetObjectVersions.
  unsigned int max_keys = 1000;
  std::string prefix;
  std::string continuation_token;      // only for ListObjectsV2.
  bool fetch_owner = false;            // only for ListObjectsV2.
  std::string version_id_marker;       // only for GetObjectVersions.
  bool include_user_metadata = false;  // MinIO extension for ListObjectsV2.
  bool recursive = false;
  bool use_api_v1 = false;
  bool include_versions = false;

  ListObjectsArgs() = default;
  ~ListObjectsArgs() = default;

  ListObjectsArgs(const ListObjectsArgs&) = default;
  ListObjectsArgs& operator=(const ListObjectsArgs&) = default;

  ListObjectsArgs(ListObjectsArgs&&) = default;
  ListObjectsArgs& operator=(ListObjectsArgs&&) = default;
};  // struct ListObjectsArgs

struct ListObjectsCommonArgs : public BucketArgs {
  std::string delimiter;
  std::string encoding_type;
  unsigned int max_keys = 1000;
  std::string prefix;

  ListObjectsCommonArgs() = default;
  ~ListObjectsCommonArgs() = default;

  ListObjectsCommonArgs(const ListObjectsCommonArgs&) = default;
  ListObjectsCommonArgs& operator=(const ListObjectsCommonArgs&) = default;

  ListObjectsCommonArgs(ListObjectsCommonArgs&&) = default;
  ListObjectsCommonArgs& operator=(ListObjectsCommonArgs&&) = default;
};  // struct ListObjectsCommonArgs

struct ListObjectsV1Args : public ListObjectsCommonArgs {
  std::string marker;

  ListObjectsV1Args();

  explicit ListObjectsV1Args(ListObjectsArgs args);
  ListObjectsV1Args& operator=(ListObjectsArgs args);

  ~ListObjectsV1Args() = default;

  ListObjectsV1Args(const ListObjectsV1Args&) = default;
  ListObjectsV1Args& operator=(const ListObjectsV1Args&) = default;

  ListObjectsV1Args(ListObjectsV1Args&&) = default;
  ListObjectsV1Args& operator=(ListObjectsV1Args&&) = default;
};  // struct ListObjectsV1Args

struct ListObjectsV2Args : public ListObjectsCommonArgs {
  std::string start_after;
  std::string continuation_token;
  bool fetch_owner = false;
  bool include_user_metadata = false;

  ListObjectsV2Args();

  explicit ListObjectsV2Args(ListObjectsArgs args);
  ListObjectsV2Args& operator=(ListObjectsArgs args);

  ~ListObjectsV2Args() = default;

  ListObjectsV2Args(const ListObjectsV2Args&) = default;
  ListObjectsV2Args& operator=(const ListObjectsV2Args&) = default;

  ListObjectsV2Args(ListObjectsV2Args&&) = default;
  ListObjectsV2Args& operator=(ListObjectsV2Args&&) = default;
};  // struct ListObjectsV2Args

struct ListObjectVersionsArgs : public ListObjectsCommonArgs {
  std::string key_marker;
  std::string version_id_marker;

  ListObjectVersionsArgs();

  explicit ListObjectVersionsArgs(ListObjectsArgs args);
  ListObjectVersionsArgs& operator=(ListObjectsArgs args);

  ~ListObjectVersionsArgs() = default;

  ListObjectVersionsArgs(const ListObjectVersionsArgs&) = default;
  ListObjectVersionsArgs& operator=(const ListObjectVersionsArgs&) = default;

  ListObjectVersionsArgs(ListObjectVersionsArgs&&) = default;
  ListObjectVersionsArgs& operator=(ListObjectVersionsArgs&&) = default;
};  // struct ListObjectVersionsArgs

struct PutObjectArgs : public PutObjectBaseArgs {
  std::istream& stream;
  http::ProgressFunction progressfunc = nullptr;
  void* progress_userdata = nullptr;

  PutObjectArgs(std::istream& stream, long object_size, long part_size);
  ~PutObjectArgs() = default;

  error::Error Validate();
};  // struct PutObjectArgs

using CopySource = ObjectConditionalReadArgs;

struct CopyObjectArgs : public ObjectWriteArgs {
  CopySource source;
  Directive* metadata_directive = nullptr;
  Directive* tagging_directive = nullptr;

  CopyObjectArgs() = default;
  ~CopyObjectArgs() = default;

  error::Error Validate() const;
};  // struct CopyObjectArgs

struct ComposeSource : public ObjectConditionalReadArgs {
  ComposeSource() = default;
  ~ComposeSource() = default;

  error::Error BuildHeaders(size_t object_size, const std::string& etag);
  size_t ObjectSize() const;
  utils::Multimap Headers() const;

 private:
  long object_size_ = -1;
  utils::Multimap headers_;
};  // struct ComposeSource

struct ComposeObjectArgs : public ObjectWriteArgs {
  std::list<ComposeSource> sources;

  ComposeObjectArgs() = default;
  ~ComposeObjectArgs() = default;

  error::Error Validate() const;
};  // struct ComposeObjectArgs

struct UploadObjectArgs : public PutObjectBaseArgs {
  std::string filename;
  http::ProgressFunction progressfunc = nullptr;
  void* progress_userdata = nullptr;

  UploadObjectArgs() = default;
  ~UploadObjectArgs() = default;

  error::Error Validate();
};  // struct UploadObjectArgs

struct RemoveObjectsApiArgs : public BucketArgs {
  bool bypass_governance_mode = false;
  bool quiet = true;
  std::list<DeleteObject> objects;

  RemoveObjectsApiArgs() = default;
  ~RemoveObjectsApiArgs() = default;
};  // struct RemoveObjectsApiArgs

using DeleteObjectFunction = std::function<bool(DeleteObject&)>;

struct RemoveObjectsArgs : public BucketArgs {
  bool bypass_governance_mode = false;
  DeleteObjectFunction func = nullptr;

  RemoveObjectsArgs() = default;
  ~RemoveObjectsArgs() = default;

  error::Error Validate() const;
};  // struct RemoveObjectsArgs

struct SelectObjectContentArgs : public ObjectReadArgs {
  SelectRequest& request;
  SelectResultFunction resultfunc = nullptr;

  SelectObjectContentArgs(SelectRequest& req, SelectResultFunction func)
      : request(req), resultfunc(func) {}

  ~SelectObjectContentArgs() = default;

  error::Error Validate() const;
};  // struct SelectObjectContentArgs

struct ListenBucketNotificationArgs : public BucketArgs {
  std::string prefix;
  std::string suffix;
  std::list<std::string> events;
  NotificationRecordsFunction func = nullptr;

  ListenBucketNotificationArgs() = default;
  ~ListenBucketNotificationArgs() = default;

  error::Error Validate() const;
};  // struct ListenBucketNotificationArgs

using DeleteBucketPolicyArgs = BucketArgs;

using GetBucketPolicyArgs = BucketArgs;

struct SetBucketPolicyArgs : public BucketArgs {
  std::string policy;

  SetBucketPolicyArgs() = default;
  ~SetBucketPolicyArgs() = default;

  error::Error Validate() const;
};  // struct SetBucketPolicy

using DeleteBucketNotificationArgs = BucketArgs;

using GetBucketNotificationArgs = BucketArgs;

struct SetBucketNotificationArgs : public BucketArgs {
  NotificationConfig& config;

  explicit SetBucketNotificationArgs(NotificationConfig& configvalue)
      : config(configvalue) {}

  ~SetBucketNotificationArgs() = default;
};  // struct SetBucketNotification

using DeleteBucketEncryptionArgs = BucketArgs;

using GetBucketEncryptionArgs = BucketArgs;

struct SetBucketEncryptionArgs : public BucketArgs {
  SseConfig& config;

  explicit SetBucketEncryptionArgs(SseConfig& sseconfig) : config(sseconfig) {}

  ~SetBucketEncryptionArgs() = default;

  error::Error Validate() const;
};  // struct SetBucketEncryption

using GetBucketVersioningArgs = BucketArgs;

struct SetBucketVersioningArgs : public BucketArgs {
  Boolean status;
  Boolean mfa_delete;

  SetBucketVersioningArgs() = default;
  ~SetBucketVersioningArgs() = default;

  error::Error Validate() const;
};  // struct SetBucketVersioning

using DeleteBucketReplicationArgs = BucketArgs;

using GetBucketReplicationArgs = BucketArgs;

struct SetBucketReplicationArgs : public BucketArgs {
  ReplicationConfig& config;

  explicit SetBucketReplicationArgs(ReplicationConfig& value) : config(value) {}

  ~SetBucketReplicationArgs() = default;
};  // struct SetBucketReplication

using DeleteBucketLifecycleArgs = BucketArgs;

using GetBucketLifecycleArgs = BucketArgs;

struct SetBucketLifecycleArgs : public BucketArgs {
  LifecycleConfig& config;

  explicit SetBucketLifecycleArgs(LifecycleConfig& value) : config(value) {}

  ~SetBucketLifecycleArgs() = default;
};  // struct SetBucketLifecycle

using DeleteBucketTagsArgs = BucketArgs;

using GetBucketTagsArgs = BucketArgs;

struct SetBucketTagsArgs : public BucketArgs {
  std::map<std::string, std::string> tags;

  SetBucketTagsArgs() = default;
  ~SetBucketTagsArgs() = default;

  error::Error Validate() const;
};  // struct SetBucketTags

using DeleteObjectLockConfigArgs = BucketArgs;

using GetObjectLockConfigArgs = BucketArgs;

struct SetObjectLockConfigArgs : public BucketArgs {
  ObjectLockConfig config;

  SetObjectLockConfigArgs() = default;
  ~SetObjectLockConfigArgs() = default;

  error::Error Validate() const;
};  // struct SetObjectLockConfig

using DeleteObjectTagsArgs = ObjectVersionArgs;

using GetObjectTagsArgs = ObjectVersionArgs;

struct SetObjectTagsArgs : public ObjectVersionArgs {
  std::map<std::string, std::string> tags;

  SetObjectTagsArgs() = default;
  ~SetObjectTagsArgs() = default;

  error::Error Validate() const;
};  // struct SetObjectTags

using EnableObjectLegalHoldArgs = ObjectVersionArgs;

using DisableObjectLegalHoldArgs = ObjectVersionArgs;

using IsObjectLegalHoldEnabledArgs = ObjectVersionArgs;

using GetObjectRetentionArgs = ObjectVersionArgs;

struct SetObjectRetentionArgs : public ObjectVersionArgs {
  RetentionMode retention_mode;
  utils::UtcTime retain_until_date;

  SetObjectRetentionArgs() = default;
  ~SetObjectRetentionArgs() = default;

  error::Error Validate() const;
};  // struct SetObjectRetention

inline constexpr unsigned int kDefaultExpirySeconds =
    (60 * 60 * 24 * 7);  // 7 days

struct GetPresignedObjectUrlArgs : public ObjectVersionArgs {
  http::Method method;
  unsigned int expiry_seconds = kDefaultExpirySeconds;
  utils::UtcTime request_time;

  GetPresignedObjectUrlArgs() = default;
  ~GetPresignedObjectUrlArgs() = default;

  error::Error Validate() const;
};  // struct GetPresignedObjectUrlArgs

struct PostPolicy {
  std::string bucket;
  std::string region;

  PostPolicy(std::string bucket, utils::UtcTime expiration)
      : bucket(std::move(bucket)), expiration_(std::move(expiration)) {}

  ~PostPolicy() = default;

  explicit operator bool() const { return !bucket.empty() && expiration_; }

  error::Error AddEqualsCondition(std::string element, std::string value);
  error::Error RemoveEqualsCondition(std::string element);
  error::Error AddStartsWithCondition(std::string element, std::string value);
  error::Error RemoveStartsWithCondition(std::string element);
  error::Error AddContentLengthRangeCondition(size_t lower_limit,
                                              size_t upper_limit);
  void RemoveContentLengthRangeCondition();

  error::Error FormData(std::map<std::string, std::string>& data,
                        std::string access_key, std::string secret_key,
                        std::string session_token, std::string region);

 private:
  static constexpr const char* eq_ = "eq";
  static constexpr const char* starts_with_ = "starts-with";
  static constexpr const char* algorithm_ = "AWS4-HMAC-SHA256";

  utils::UtcTime expiration_;
  std::map<std::string, std::map<std::string, std::string>> conditions_;
  Integer lower_limit_;
  Integer upper_limit_;

  static std::string trimDollar(std::string value);
  static std::string getCredentialString(std::string access_key,
                                         utils::UtcTime date,
                                         std::string region);
  static bool isReservedElement(std::string element);
};  // struct PostPolicy

}  // namespace minio::s3

#endif  // _MINIO_CPP_ARGS_H_INCLUDED
