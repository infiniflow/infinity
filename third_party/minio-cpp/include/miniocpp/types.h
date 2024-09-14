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

#ifndef MINIO_CPP_TYPES_H_INCLUDED
#define MINIO_CPP_TYPES_H_INCLUDED

#include <exception>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <nlohmann/json_fwd.hpp>
#include <ostream>
#include <string>
#include <type_traits>

#include "error.h"
#include "utils.h"

namespace minio::s3 {

enum class RetentionMode { kGovernance, kCompliance };

// StringToRetentionMode converts string to retention mode enum.
RetentionMode StringToRetentionMode(std::string_view str) noexcept;

constexpr bool IsRetentionModeValid(RetentionMode retention) {
  switch (retention) {
    case RetentionMode::kGovernance:
    case RetentionMode::kCompliance:
      return true;
  }
  return false;
}

// RetentionModeToString converts retention mode enum to string.
constexpr const char* RetentionModeToString(RetentionMode retention) noexcept {
  switch (retention) {
    case RetentionMode::kGovernance:
      return "GOVERNANCE";
    case RetentionMode::kCompliance:
      return "COMPLIANCE";
    default: {
      std::cerr << "ABORT: Unknown retention mode. This should not happen."
                << std::endl;
      std::terminate();
    }
  }
  return nullptr;
}

enum class LegalHold { kOn, kOff };

// StringToLegalHold converts string to legal hold enum.
LegalHold StringToLegalHold(std::string_view str) noexcept;

constexpr bool IsLegalHoldValid(LegalHold legal_hold) {
  switch (legal_hold) {
    case LegalHold::kOn:
    case LegalHold::kOff:
      return true;
  }
  return false;
}

// LegalHoldToString converts legal hold enum to string.
constexpr const char* LegalHoldToString(LegalHold legal_hold) noexcept {
  switch (legal_hold) {
    case LegalHold::kOn:
      return "ON";
    case LegalHold::kOff:
      return "OFF";
    default: {
      std::cerr << "ABORT: Unknown legal hold. This should not happen."
                << std::endl;
      std::terminate();
    }
  }
  return nullptr;
}

enum class Directive { kCopy, kReplace };

// StringToDirective converts string to directive enum.
Directive StringToDirective(std::string_view str) noexcept;

// DirectiveToString converts directive enum to string.
constexpr const char* DirectiveToString(Directive directive) noexcept {
  switch (directive) {
    case Directive::kCopy:
      return "COPY";
    case Directive::kReplace:
      return "REPLACE";
    default: {
      std::cerr << "ABORT: Unknown directive. This should not happen."
                << std::endl;
      std::terminate();
    }
  }
  return nullptr;
}

enum class CompressionType { kNone, kGZip, kBZip2 };

// CompressionTypeToString converts compression type enum to string.
constexpr const char* CompressionTypeToString(CompressionType ctype) noexcept {
  switch (ctype) {
    case CompressionType::kNone:
      return "NONE";
    case CompressionType::kGZip:
      return "GZIP";
    case CompressionType::kBZip2:
      return "BZIP2";
    default: {
      std::cerr << "ABORT: Unknown compression type. This should not happen."
                << std::endl;
      std::terminate();
    }
  }
  return nullptr;
}

enum class FileHeaderInfo { kUse, kIgnore, kNone };

// FileHeaderInfoToString converts file header info enum to string.
constexpr const char* FileHeaderInfoToString(FileHeaderInfo info) noexcept {
  switch (info) {
    case FileHeaderInfo::kUse:
      return "USE";
    case FileHeaderInfo::kIgnore:
      return "IGNORE";
    case FileHeaderInfo::kNone:
      return "NONE";
    default: {
      std::cerr << "ABORT: Unknown file header info. This should not happen."
                << std::endl;
      std::terminate();
    }
  }
  return nullptr;
}

enum class JsonType { kDocument, kLines };

// JsonTypeToString converts JSON type enum to string.
constexpr const char* JsonTypeToString(JsonType jtype) noexcept {
  switch (jtype) {
    case JsonType::kDocument:
      return "DOCUMENT";
    case JsonType::kLines:
      return "LINES";
    default: {
      std::cerr << "ABORT: Unknown JSON type. This should not happen."
                << std::endl;
      std::terminate();
    }
  }
  return nullptr;
}

enum class QuoteFields { kAlways, kAsNeeded };

// QuoteFieldsToString converts quote fields enum to string.
constexpr const char* QuoteFieldsToString(QuoteFields qtype) noexcept {
  switch (qtype) {
    case QuoteFields::kAlways:
      return "ALWAYS";
    case QuoteFields::kAsNeeded:
      return "ASNEEDED";
    default: {
      std::cerr << "ABORT: Unknown quote fields. This should not happen."
                << std::endl;
      std::terminate();
    }
  }
  return nullptr;
}

struct CsvInputSerialization {
  CompressionType* compression_type = nullptr;
  bool allow_quoted_record_delimiter = false;
  char comments = 0;
  char field_delimiter = 0;
  FileHeaderInfo* file_header_info = nullptr;
  char quote_character = 0;
  char quote_escape_character = 0;
  char record_delimiter = 0;

  CsvInputSerialization() = default;
  ~CsvInputSerialization() = default;
};  // struct CsvInputSerialization

struct JsonInputSerialization {
  CompressionType* compression_type = nullptr;
  JsonType* json_type = nullptr;

  JsonInputSerialization() = default;
  ~JsonInputSerialization() = default;
};  // struct JsonInputSerialization

struct ParquetInputSerialization {
  ParquetInputSerialization() = default;
  ~ParquetInputSerialization() = default;
};  // struct ParquetInputSerialization

struct CsvOutputSerialization {
  char field_delimiter = 0;
  char quote_character = 0;
  char quote_escape_character = 0;
  QuoteFields* quote_fields = nullptr;
  char record_delimiter = 0;

  CsvOutputSerialization() = default;
  ~CsvOutputSerialization() = default;
};  // struct CsvOutputSerialization

struct JsonOutputSerialization {
  char record_delimiter = 0;

  JsonOutputSerialization() = default;
  ~JsonOutputSerialization() = default;
};  // struct JsonOutputSerialization

struct SelectRequest {
  std::string expr;
  CsvInputSerialization* csv_input = nullptr;
  JsonInputSerialization* json_input = nullptr;
  ParquetInputSerialization* parquet_input = nullptr;
  CsvOutputSerialization* csv_output = nullptr;
  JsonOutputSerialization* json_output = nullptr;
  bool request_progress = false;
  size_t* scan_start_range = nullptr;
  size_t* scan_end_range = nullptr;

  SelectRequest(std::string expression, CsvInputSerialization* csv_input,
                CsvOutputSerialization* csv_output)
      : expr(std::move(expression)),
        csv_input(csv_input),
        csv_output(csv_output) {}

  SelectRequest(std::string expression, CsvInputSerialization* csv_input,
                JsonOutputSerialization* json_output)
      : expr(std::move(expression)),
        csv_input(csv_input),
        json_output(json_output) {}

  SelectRequest(std::string expression, JsonInputSerialization* json_input,
                CsvOutputSerialization* csv_output)
      : expr(std::move(expression)),
        json_input(json_input),
        csv_output(csv_output) {}

  SelectRequest(std::string expression, JsonInputSerialization* json_input,
                JsonOutputSerialization* json_output)
      : expr(std::move(expression)),
        json_input(json_input),
        json_output(json_output) {}

  SelectRequest(std::string expression,
                ParquetInputSerialization* parquet_input,
                CsvOutputSerialization* csv_output)
      : expr(std::move(expression)),
        parquet_input(parquet_input),
        csv_output(csv_output) {}

  SelectRequest(std::string expression,
                ParquetInputSerialization* parquet_input,
                JsonOutputSerialization* json_output)
      : expr(std::move(expression)),
        parquet_input(parquet_input),
        json_output(json_output) {}

  ~SelectRequest() = default;

  std::string ToXML() const;
};  // struct SelectRequest

struct SelectResult {
  error::Error err = error::SUCCESS;
  bool ended = false;
  long int bytes_scanned = -1;
  long int bytes_processed = -1;
  long int bytes_returned = -1;
  std::string records;

  SelectResult() : ended(true) {}

  explicit SelectResult(error::Error err) : err(std::move(err)), ended(true) {}

  SelectResult(long int bytes_scanned, long int bytes_processed,
               long int bytes_returned)
      : bytes_scanned(bytes_scanned),
        bytes_processed(bytes_processed),
        bytes_returned(bytes_returned) {}

  SelectResult(std::string records) : records(std::move(records)) {}

  ~SelectResult() = default;
};

using SelectResultFunction = std::function<bool(SelectResult)>;

struct Bucket {
  std::string name;
  utils::UtcTime creation_date;

  Bucket() = default;
  explicit Bucket(std::string name, utils::UtcTime creation_date)
      : name(std::move(name)), creation_date(std::move(creation_date)) {}
  ~Bucket() = default;
};  // struct Bucket

struct Part {
  unsigned int number;
  std::string etag;
  utils::UtcTime last_modified = {};
  size_t size = 0;

  Part() = default;
  explicit Part(unsigned int number, std::string etag)
      : number(number), etag(std::move(etag)) {}
  ~Part() = default;
};  // struct Part

struct Retention {
  RetentionMode mode;
  utils::UtcTime retain_until_date;

  Retention() = default;
  ~Retention() = default;
};  // struct Retention

struct DeleteObject {
  std::string name = {};
  std::string version_id = {};

  DeleteObject() = default;
  ~DeleteObject() = default;
};  // struct DeleteObject

struct NotificationRecord {
  std::string event_version;
  std::string event_source;
  std::string aws_region;
  std::string event_time;
  std::string event_name;
  struct {
    std::string principal_id;
  } user_identity;
  struct {
    std::string principal_id;
    std::string region;
    std::string source_ip_address;
  } request_parameters;
  struct {
    std::string content_length;
    std::string x_amz_request_id;
    std::string x_minio_deployment_id;
    std::string x_minio_origin_endpoint;
  } response_elements;
  struct {
    std::string s3_schema_version;
    std::string configuration_id;
    struct {
      std::string name;
      std::string arn;
      struct {
        std::string principal_id;
      } owner_identity;
    } bucket;
    struct {
      std::string key;
      size_t size;
      std::string etag;
      std::string content_type;
      std::map<std::string, std::string> user_metadata;
      std::string sequencer;
    } object;
  } s3;
  struct {
    std::string host;
    std::string port;
    std::string user_agent;
  } source;

  NotificationRecord() = default;
  ~NotificationRecord() = default;

  static NotificationRecord ParseJSON(nlohmann::json j_record);
};  // struct NotificationRecord

using NotificationRecordsFunction =
    std::function<bool(std::list<NotificationRecord>)>;

struct FilterValue {
 private:
  std::string value_;
  bool is_value_set_ = false;

 public:
  FilterValue() = default;

  explicit FilterValue(std::string value)
      : value_(std::move(value)), is_value_set_(true) {}

  ~FilterValue() = default;

  explicit operator bool() const { return is_value_set_; }
  std::string Value() const { return value_; }
};  // struct FilterValue

struct PrefixFilterRule : public FilterValue {
  static constexpr const char* name = "prefix";

  PrefixFilterRule() = default;

  explicit PrefixFilterRule(std::string value)
      : FilterValue(std::move(value)) {}

  ~PrefixFilterRule() = default;
};  // struct PrefixFilterRule

struct SuffixFilterRule : public FilterValue {
  static constexpr const char* name = "suffix";

  SuffixFilterRule() = default;

  explicit SuffixFilterRule(std::string value)
      : FilterValue(std::move(value)) {}

  ~SuffixFilterRule() = default;
};  // struct SuffixFilterRule

struct NotificationCommonConfig {
  std::list<std::string> events;
  std::string id;
  PrefixFilterRule prefix_filter_rule;
  SuffixFilterRule suffix_filter_rule;

  NotificationCommonConfig() = default;
  ~NotificationCommonConfig() = default;
};  // struct NotificationCommonConfig

struct CloudFuncConfig : public NotificationCommonConfig {
  std::string cloud_func;

  CloudFuncConfig() = default;
  ~CloudFuncConfig() = default;
};  // struct CloudFuncConfig

struct QueueConfig : public NotificationCommonConfig {
  std::string queue;

  QueueConfig() = default;
  ~QueueConfig() = default;
};  // struct QueueConfig

struct TopicConfig : public NotificationCommonConfig {
  std::string topic;

  TopicConfig() = default;
  ~TopicConfig() = default;
};  // struct TopicConfig

struct NotificationConfig {
  std::list<CloudFuncConfig> cloud_func_config_list;
  std::list<QueueConfig> queue_config_list;
  std::list<TopicConfig> topic_config_list;

  NotificationConfig() = default;
  ~NotificationConfig() = default;

  std::string ToXML() const;
};  // struct NotificationConfig

struct SseConfig {
  std::string sse_algorithm;
  std::string kms_master_key_id;

  SseConfig() = default;
  ~SseConfig() = default;

  static SseConfig S3();
  static SseConfig Kms(std::string masterkeyid = {});

  explicit operator bool() const { return !sse_algorithm.empty(); }
};  //  struct SseConfig

struct Tag {
  std::string key;
  std::string value;

  Tag() = default;
  ~Tag() = default;

  explicit operator bool() const { return !key.empty(); }
};  // struct Tag

struct Prefix {
 private:
  std::string value_;
  bool is_set_ = false;

 public:
  Prefix() = default;

  explicit Prefix(std::string value)
      : value_(std::move(value)), is_set_(true) {}

  ~Prefix() = default;

  explicit operator bool() const { return is_set_; }
  std::string Get() const { return value_; }

  friend std::ostream& operator<<(std::ostream& s, const Prefix& v) {
    return s << v.value_;
  }
};  // struct Prefix

struct Integer {
 private:
  int value_ = 0;
  bool is_set_ = false;

 public:
  Integer() = default;

  explicit Integer(int value) : value_(value), is_set_(true) {}

  Integer& operator=(int value) {
    value_ = value;
    is_set_ = true;
    return *this;
  }

  Integer(const Integer&) = default;
  Integer& operator=(const Integer&) = default;

  Integer(Integer&&) = default;
  Integer& operator=(Integer&&) = default;

  ~Integer() = default;

  explicit operator bool() const { return is_set_; }
  int Get() const { return value_; }

  friend std::ostream& operator<<(std::ostream& s, const Integer& v) {
    return s << v.value_;
  }
};  // struct Integer

struct Boolean {
 private:
  bool value_ = false;
  bool is_set_ = false;

 public:
  Boolean() = default;

  explicit Boolean(bool value) : value_(value), is_set_(true) {}

  Boolean& operator=(bool value) {
    value_ = value;
    is_set_ = true;
    return *this;
  }

  Boolean(const Boolean&) = default;
  Boolean& operator=(const Boolean&) = default;

  Boolean(Boolean&&) = default;
  Boolean& operator=(Boolean&&) = default;

  ~Boolean() = default;

  explicit operator bool() const { return is_set_; }
  bool Get() const { return value_; }

  friend std::ostream& operator<<(std::ostream& s, const Boolean& v) {
    return s << utils::BoolToString(v.value_);
  }
};  // struct Boolean

struct AndOperator {
  Prefix prefix;
  std::map<std::string, std::string> tags;

  AndOperator() = default;
  ~AndOperator() = default;

  explicit operator bool() const { return prefix || !tags.empty(); }
};  // struct AndOperator

struct Filter {
  AndOperator and_operator;
  Prefix prefix;
  Tag tag;

  Filter() = default;
  ~Filter() = default;

  explicit operator bool() const {
    return static_cast<bool>(and_operator) ^ static_cast<bool>(prefix) ^
           static_cast<bool>(tag);
  }
};  // struct Filter

struct AccessControlTranslation {
  std::string owner = "Destination";

  AccessControlTranslation() = default;
  ~AccessControlTranslation() = default;

  void Enable() { enabled_ = true; }
  explicit operator bool() const { return enabled_; }

 private:
  bool enabled_ = false;
};  // struct AccessControlTranslation

struct EncryptionConfig {
  std::string replica_kms_key_id;

  EncryptionConfig() = default;
  ~EncryptionConfig() = default;

  void Enable() { enabled_ = true; }
  explicit operator bool() const { return enabled_; }

 private:
  bool enabled_ = false;
};  // struct EncryptionConfig

struct Metrics {
  unsigned int event_threshold_minutes = 15;
  bool status = false;

  Metrics() = default;
  ~Metrics() = default;

  void Enable() { enabled_ = true; }
  explicit operator bool() const { return enabled_; }

 private:
  bool enabled_ = false;
};  // struct Metrics

struct ReplicationTime {
  unsigned int time_minutes = 15;
  bool status = false;

  ReplicationTime() = default;
  ~ReplicationTime() = default;

  void Enable() { enabled_ = true; }
  explicit operator bool() const { return enabled_; }

 private:
  bool enabled_ = false;
};  // struct ReplicationTime

struct Destination {
  std::string bucket_arn;
  AccessControlTranslation access_control_translation;
  std::string account;
  EncryptionConfig encryption_config;
  Metrics metrics;
  ReplicationTime replication_time;
  std::string storage_class;

  Destination() = default;
  ~Destination() = default;
};  // struct Destination

struct SourceSelectionCriteria {
  Boolean sse_kms_encrypted_objects_status;

  SourceSelectionCriteria() = default;
  ~SourceSelectionCriteria() = default;

  void Enable() { enabled_ = true; }
  explicit operator bool() const { return enabled_; }

 private:
  bool enabled_ = false;
};  // struct SourceSelectionCriteria

struct ReplicationRule {
  Destination destination;
  Boolean delete_marker_replication_status;
  Boolean existing_object_replication_status;
  Filter filter;
  std::string id;
  Prefix prefix;
  Integer priority;
  SourceSelectionCriteria source_selection_criteria;
  Boolean delete_replication_status;
  bool status = false;

  ReplicationRule() = default;
  ~ReplicationRule() = default;
};  // struct ReplicationRule

struct ReplicationConfig {
  std::string role;
  std::list<ReplicationRule> rules;

  ReplicationConfig() = default;
  ~ReplicationConfig() = default;

  std::string ToXML() const;
};  // status ReplicationConfig

struct LifecycleRule {
  Integer abort_incomplete_multipart_upload_days_after_initiation;
  utils::UtcTime expiration_date;
  Integer expiration_days;
  Boolean expiration_expired_object_delete_marker;
  Filter filter;
  std::string id;
  Integer noncurrent_version_expiration_noncurrent_days;
  Integer noncurrent_version_transition_noncurrent_days;
  std::string noncurrent_version_transition_storage_class;
  bool status = false;
  utils::UtcTime transition_date;
  Integer transition_days;
  std::string transition_storage_class;

  LifecycleRule() = default;
  ~LifecycleRule() = default;

  error::Error Validate() const;
};  // struct LifecycleRule

struct LifecycleConfig {
  std::list<LifecycleRule> rules;

  LifecycleConfig() = default;
  ~LifecycleConfig() = default;

  std::string ToXML() const;
};  // struct LifecycleConfig

struct ObjectLockConfig {
  RetentionMode retention_mode;
  Integer retention_duration_days;
  Integer retention_duration_years;

  ObjectLockConfig() = default;
  ~ObjectLockConfig() = default;

  error::Error Validate() const;
};  // struct ObjectLockConfig

}  // namespace minio::s3

#endif  // MINIO_CPP_TYPES_H_INCLUDED
