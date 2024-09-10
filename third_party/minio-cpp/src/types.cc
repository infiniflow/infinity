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

#include "miniocpp/types.h"

#include <exception>
#include <iosfwd>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ostream>
#include <sstream>
#include <string>

#include "miniocpp/error.h"

namespace minio::s3 {

RetentionMode StringToRetentionMode(std::string_view str) noexcept {
  if (str == "GOVERNANCE") return RetentionMode::kGovernance;
  if (str == "COMPLIANCE") return RetentionMode::kCompliance;

  std::cerr << "ABORT: Unknown retention mode. This should not happen."
            << std::endl;
  std::terminate();

  return RetentionMode::kGovernance;  // never reaches here.
}

LegalHold StringToLegalHold(std::string_view str) noexcept {
  if (str == "ON") return LegalHold::kOn;
  if (str == "OFF") return LegalHold::kOff;

  std::cerr << "ABORT: Unknown legal hold. This should not happen."
            << std::endl;
  std::terminate();

  return LegalHold::kOff;  // never reaches here.
}

Directive StringToDirective(std::string_view str) noexcept {
  if (str == "COPY") return Directive::kCopy;
  if (str == "REPLACE") return Directive::kReplace;

  std::cerr << "ABORT: Unknown directive. This should not happen." << std::endl;
  std::terminate();

  return Directive::kCopy;  // never reaches here.
}

std::string SelectRequest::ToXML() const {
  std::stringstream ss;
  ss << "<SelectObjectContentRequest>";

  ss << "<Expression>" << expr << "</Expression>";
  ss << "<ExpressionType>SQL</ExpressionType>";

  ss << "<InputSerialization>";

  if (csv_input != nullptr) {
    if (csv_input->compression_type != nullptr) {
      ss << "<CompressionType>"
         << CompressionTypeToString(*csv_input->compression_type)
         << "</CompressionType>";
    }

    ss << "<CSV>";
    if (csv_input->allow_quoted_record_delimiter) {
      ss << "<AllowQuotedRecordDelimiter>true</AllowQuotedRecordDelimiter>";
    }
    if (csv_input->comments) {
      ss << "<Comments>" << csv_input->comments << "</Comments>";
    }
    if (csv_input->field_delimiter) {
      ss << "<FieldDelimiter>" << csv_input->field_delimiter
         << "</FieldDelimiter>";
    }
    if (csv_input->file_header_info != nullptr) {
      ss << "<FileHeaderInfo>"
         << FileHeaderInfoToString(*csv_input->file_header_info)
         << "</FileHeaderInfo>";
    }
    if (csv_input->quote_character) {
      ss << "<QuoteCharacter>" << csv_input->quote_character
         << "</QuoteCharacter>";
    }
    if (csv_input->record_delimiter) {
      ss << "<RecordDelimiter>" << csv_input->record_delimiter
         << "</RecordDelimiter>";
    }
    ss << "</CSV>";
  }

  if (json_input != nullptr) {
    if (json_input->compression_type != nullptr) {
      ss << "<CompressionType>"
         << CompressionTypeToString(*json_input->compression_type)
         << "</CompressionType>";
    }

    ss << "<JSON>";
    if (json_input->json_type != nullptr) {
      ss << "<Type>" << JsonTypeToString(*json_input->json_type) << "</Type>";
    }
    ss << "</JSON>";
  }

  if (parquet_input != nullptr) ss << "<Parquet></Parquet>";

  ss << "</InputSerialization>";

  ss << "<OutputSerialization>";

  if (csv_output != nullptr) {
    ss << "<CSV>";
    if (csv_output->field_delimiter) {
      ss << "<FieldDelimiter>" << csv_output->field_delimiter
         << "</FieldDelimiter>";
    }
    if (csv_output->quote_character) {
      ss << "<QuoteCharacter>" << csv_output->quote_character
         << "</QuoteCharacter>";
    }
    if (csv_output->quote_escape_character) {
      ss << "<QuoteEscapeCharacter>" << csv_output->quote_escape_character
         << "</QuoteEscapeCharacter>";
    }
    if (csv_output->quote_fields != nullptr) {
      ss << "<QuoteFields>" << QuoteFieldsToString(*csv_output->quote_fields)
         << "</QuoteFields>";
    }
    if (csv_output->record_delimiter) {
      ss << "<RecordDelimiter>" << csv_output->record_delimiter
         << "</RecordDelimiter>";
    }
    ss << "</CSV>";
  }

  if (json_output != nullptr) {
    ss << "<JSON>";
    if (json_output->record_delimiter) {
      ss << "<RecordDelimiter>" << json_output->record_delimiter
         << "</RecordDelimiter>";
    }
    ss << "</JSON>";
  }

  ss << "</OutputSerialization>";

  if (request_progress) {
    ss << "<RequestProgress><Enabled>true</Enabled></RequestProgress>";
  }
  if (scan_start_range != nullptr || scan_end_range != nullptr) {
    ss << "<ScanRange>";
    if (scan_start_range != nullptr) {
      ss << "<Start>" << *scan_start_range << "</Start>";
    }
    if (scan_end_range != nullptr) {
      ss << "<End>" << *scan_end_range << "</End>";
    }
    ss << "</ScanRange>";
  }

  ss << "</SelectObjectContentRequest>";

  return ss.str();
}

NotificationRecord NotificationRecord::ParseJSON(nlohmann::json j_record) {
  NotificationRecord record;

  record.event_version = j_record.value("eventVersion", "");
  record.event_source = j_record.value("eventSource", "");
  record.aws_region = j_record.value("awsRegion", "");
  record.event_time = j_record.value("eventTime", "");
  record.event_name = j_record.value("eventName", "");
  if (j_record.contains("userIdentity")) {
    record.user_identity.principal_id =
        j_record["userIdentity"].value("principalId", "");
  }
  if (j_record.contains("requestParameters")) {
    auto& j = j_record["requestParameters"];
    record.request_parameters.principal_id = j.value("principalId", "");
    record.request_parameters.region = j.value("region", "");
    record.request_parameters.source_ip_address =
        j.value("sourceIPAddress", "");
  }
  if (j_record.contains("responseElements")) {
    auto& j = j_record["responseElements"];
    record.response_elements.content_length = j.value("content-length", "");
    record.response_elements.x_amz_request_id = j.value("x-amz-request-id", "");
    record.response_elements.x_minio_deployment_id =
        j.value("x-minio-deployment-id", "");
    record.response_elements.x_minio_origin_endpoint =
        j.value("x-minio-origin-endpoint", "");
  }
  if (j_record.contains("s3")) {
    auto& j_s3 = j_record["s3"];
    record.s3.s3_schema_version = j_s3.value("s3SchemaVersion", "");
    record.s3.configuration_id = j_s3.value("configurationId", "");
    if (j_s3.contains("bucket")) {
      auto& j_bucket = j_s3["bucket"];
      record.s3.bucket.name = j_bucket.value("name", "");
      record.s3.bucket.arn = j_bucket.value("arn", "");
      if (j_bucket.contains("ownerIdentity")) {
        record.s3.bucket.owner_identity.principal_id =
            j_bucket["ownerIdentity"].value("principalId", "");
      }
    }
    if (j_s3.contains("object")) {
      auto& j_object = j_s3["object"];
      record.s3.object.key = j_object.value("key", "");
      record.s3.object.size = j_object.value("size", size_t(0));
      record.s3.object.etag = j_object.value("eTag", "");
      record.s3.object.content_type = j_object.value("contentType", "");
      record.s3.object.sequencer = j_object.value("sequencer", "");
      if (j_object.contains("userMetadata")) {
        for (auto& j : j_object["userMetadata"].items()) {
          record.s3.object.user_metadata[j.key()] = j.value();
        }
      }
    }
  }
  if (j_record.contains("source")) {
    auto& j_source = j_record["source"];
    record.source.host = j_source.value("host", "");
    record.source.port = j_source.value("port", "");
    record.source.user_agent = j_source.value("userAgent", "");
  }

  return record;
}

std::string NotificationConfig::ToXML() const {
  std::stringstream ss;
  ss << "<NotificationConfiguration>";

  for (auto& config : cloud_func_config_list) {
    ss << "<CloudFunctionConfiguration>";
    ss << "<CloudFunction>" << config.cloud_func << "</CloudFunction>";
    for (auto& event : config.events) ss << "<Event>" << event << "</Event>";
    if (!config.id.empty()) ss << "<Id>" << config.id << "</Id>";
    if (config.prefix_filter_rule || config.suffix_filter_rule) {
      ss << "<Filter><S3Key>";
      if (config.prefix_filter_rule) {
        ss << "<FilterRule><Name>prefix</Name>";
        ss << "<Value>" << config.prefix_filter_rule.Value()
           << "</Value></FilterRule>";
      }
      if (config.suffix_filter_rule) {
        ss << "<FilterRule><Name>suffix</Name>";
        ss << "<Value>" << config.suffix_filter_rule.Value()
           << "</Value></FilterRule>";
      }
      ss << "</S3Key></Filter>";
    }
    ss << "</CloudFunctionConfiguration>";
  }

  for (auto& config : queue_config_list) {
    ss << "<QueueConfiguration>";
    ss << "<Queue>" << config.queue << "</Queue>";
    for (auto& event : config.events) ss << "<Event>" << event << "</Event>";
    if (!config.id.empty()) ss << "<Id>" << config.id << "</Id>";
    if (config.prefix_filter_rule || config.suffix_filter_rule) {
      ss << "<Filter><S3Key>";
      if (config.prefix_filter_rule) {
        ss << "<FilterRule><Name>prefix</Name>";
        ss << "<Value>" << config.prefix_filter_rule.Value()
           << "</Value></FilterRule>";
      }
      if (config.suffix_filter_rule) {
        ss << "<FilterRule><Name>suffix</Name>";
        ss << "<Value>" << config.suffix_filter_rule.Value()
           << "</Value></FilterRule>";
      }
      ss << "</S3Key></Filter>";
    }
    ss << "</QueueConfiguration>";
  }

  for (auto& config : topic_config_list) {
    ss << "<TopicConfiguration>";
    ss << "<Topic>" << config.topic << "</Topic>";
    for (auto& event : config.events) ss << "<Event>" << event << "</Event>";
    if (!config.id.empty()) ss << "<Id>" << config.id << "</Id>";
    if (config.prefix_filter_rule || config.suffix_filter_rule) {
      ss << "<Filter><S3Key>";
      if (config.prefix_filter_rule) {
        ss << "<FilterRule><Name>prefix</Name>";
        ss << "<Value>" << config.prefix_filter_rule.Value()
           << "</Value></FilterRule>";
      }
      if (config.suffix_filter_rule) {
        ss << "<FilterRule><Name>suffix</Name>";
        ss << "<Value>" << config.suffix_filter_rule.Value()
           << "</Value></FilterRule>";
      }
      ss << "</S3Key></Filter>";
    }
    ss << "</TopicConfiguration>";
  }

  ss << "</NotificationConfiguration>";

  return ss.str();
}

std::string ReplicationConfig::ToXML() const {
  auto status_xml = [](bool status) -> std::string {
    std::stringstream ss;
    ss << "<Status>" << (status ? "Enabled" : "Disabled") << "</Status>";
    return ss.str();
  };

  auto prefix_xml = [](std::string prefix) -> std::string {
    std::stringstream ss;
    ss << "<Prefix>" << prefix << "</Prefix>";
    return ss.str();
  };

  auto minutes_xml = [](unsigned int minutes) -> std::string {
    std::stringstream ss;
    ss << "<Minutes>" << std::to_string(minutes) << "</Minutes>";
    return ss.str();
  };

  auto tag_xml = [](std::string key, std::string value) -> std::string {
    std::stringstream ss;
    ss << "<Tag>" << "<Key>" << key << "</Key>" << "<Value>" << value
       << "</Value>" << "</Tag>";
    return ss.str();
  };

  std::stringstream ss;
  ss << "<ReplicationConfiguration>";
  if (!role.empty()) ss << "<Role>" << role << "</Role>";
  for (auto& rule : rules) {
    ss << "<Rule>";

    ss << "<Destination>";
    ss << "<Bucket>" << rule.destination.bucket_arn << "</Bucket>";
    if (rule.destination.access_control_translation) {
      ss << "<AccessControlTranslation><Owner>"
         << rule.destination.access_control_translation.owner
         << "</Owner></AccessControlTranslation>";
    }
    if (!rule.destination.account.empty()) {
      ss << "<Account>" << rule.destination.account << "</Account>";
    }
    if (rule.destination.encryption_config) {
      ss << "<EncryptionConfiguration>";
      if (!rule.destination.encryption_config.replica_kms_key_id.empty()) {
        ss << "<ReplicaKmsKeyID>"
           << rule.destination.encryption_config.replica_kms_key_id
           << "</ReplicaKmsKeyID>";
      }
      ss << "</EncryptionConfiguration>";
    }
    if (rule.destination.metrics) {
      ss << "<Metrics>" << "<EventThreshold>";
      if (rule.destination.metrics.event_threshold_minutes > 0) {
        ss << minutes_xml(rule.destination.metrics.event_threshold_minutes);
      }
      ss << status_xml(rule.destination.metrics.status) << "</EventThreshold>"
         << "</Metrics>";
    }
    if (rule.destination.replication_time) {
      ss << "<ReplicationTime>" << "<Time>";
      if (rule.destination.replication_time.time_minutes > 0) {
        ss << minutes_xml(rule.destination.replication_time.time_minutes);
      }
      ss << "</Time>" << status_xml(rule.destination.replication_time.status)
         << "</ReplicationTime>";
    }
    if (!rule.destination.storage_class.empty()) {
      ss << "<StorageClass>" << rule.destination.storage_class
         << "</StorageClass>";
    }
    ss << "</Destination>";

    if (rule.delete_marker_replication_status) {
      ss << "<DeleteMarkerReplication>"
         << status_xml(rule.delete_marker_replication_status.Get())
         << "</DeleteMarkerReplication>";
    }

    if (rule.existing_object_replication_status) {
      ss << "<ExistingObjectReplication>"
         << status_xml(rule.existing_object_replication_status.Get())
         << "</ExistingObjectReplication>";
    }

    if (rule.filter) {
      ss << "<Filter>";
      if (rule.filter.and_operator) {
        ss << "<And>";
        if (rule.filter.and_operator.prefix) {
          ss << prefix_xml(rule.filter.and_operator.prefix.Get());
        }
        if (!rule.filter.and_operator.tags.empty()) {
          for (auto& [key, value] : rule.filter.and_operator.tags) {
            ss << tag_xml(key, value);
          }
        }
        ss << "</And>";
      }
      if (rule.filter.prefix) {
        ss << prefix_xml(rule.filter.prefix.Get());
      }
      if (rule.filter.tag) {
        ss << tag_xml(rule.filter.tag.key, rule.filter.tag.value);
      }
      ss << "</Filter>";
    }

    if (!rule.id.empty()) ss << "<ID>" << rule.id << "</ID>";

    if (rule.prefix) ss << prefix_xml(rule.prefix.Get());

    if (rule.priority) {
      ss << "<Priority>" << rule.priority << "</Priority>";
    }

    if (rule.source_selection_criteria) {
      ss << "<SourceSelectionCriteria>";
      if (rule.source_selection_criteria.sse_kms_encrypted_objects_status) {
        ss << "<SseKmsEncryptedObjects>"
           << status_xml(rule.source_selection_criteria
                             .sse_kms_encrypted_objects_status.Get())
           << "</SseKmsEncryptedObjects>";
      }
      ss << "</SourceSelectionCriteria>";
    }

    if (rule.delete_replication_status) {
      ss << "<DeleteReplication>"
         << status_xml(rule.delete_replication_status.Get())
         << "</DeleteReplication>";
    }

    ss << status_xml(rule.status);

    ss << "</Rule>";
  }
  ss << "</ReplicationConfiguration>";

  return ss.str();
}

error::Error LifecycleRule::Validate() const {
  if (!abort_incomplete_multipart_upload_days_after_initiation &&
      !expiration_date && !expiration_days &&
      !expiration_expired_object_delete_marker &&
      !noncurrent_version_expiration_noncurrent_days &&
      noncurrent_version_transition_storage_class.empty() && !transition_date &&
      !transition_days && transition_storage_class.empty()) {
    return error::Error(
        "at least one of action (AbortIncompleteMultipartUpload, Expiration, "
        "NoncurrentVersionExpiration, NoncurrentVersionTransition or "
        "Transition) must be "
        "specified in a rule");
  }

  if (!filter) {
    return error::Error("valid filter must be provided");
  }

  if (expiration_expired_object_delete_marker) {
    if (expiration_date || expiration_days) {
      return error::Error(
          "ExpiredObjectDeleteMarker must not be provided along with Date and "
          "Days");
    }
  } else if (expiration_date && expiration_days) {
    return error::Error("only one of date or days of expiration must be set");
  }

  if (transition_date && transition_days) {
    return error::Error("only one of date and days of transition must be set");
  }

  if (id.length() > 255) {
    return error::Error("id must be exceed 255 characters");
  }

  return error::SUCCESS;
}

std::string LifecycleConfig::ToXML() const {
  std::stringstream ss;

  ss << "<LifecycleConfiguration>";

  for (auto& rule : rules) {
    ss << "<Rule>";

    if (rule.abort_incomplete_multipart_upload_days_after_initiation) {
      ss << "<AbortIncompleteMultipartUpload><DaysAfterInitiation>"
         << rule.abort_incomplete_multipart_upload_days_after_initiation
         << "</DaysAfterInitiation></AbortIncompleteMultipartUpload>";
    }

    if (rule.expiration_date || rule.expiration_days ||
        rule.expiration_expired_object_delete_marker) {
      ss << "<Expiration>";
      if (rule.expiration_date) {
        ss << "<Date>" << rule.expiration_date.ToISO8601UTC() << "</Date>";
      }
      if (rule.expiration_days) {
        ss << "<Days>" << rule.expiration_days << "</Days>";
      }
      if (rule.expiration_expired_object_delete_marker) {
        ss << "<ExpiredObjectDeleteMarker>"
           << rule.expiration_expired_object_delete_marker
           << "</ExpiredObjectDeleteMarker>";
      }
      ss << "</Expiration>";
    }

    ss << "<Filter>";
    if (rule.filter.and_operator) {
      ss << "<And>";
      if (rule.filter.and_operator.prefix) {
        ss << "<Prefix>" << rule.filter.and_operator.prefix << "</Prefix>";
      }
      if (!rule.filter.and_operator.tags.empty()) {
        for (auto& [key, value] : rule.filter.and_operator.tags) {
          ss << "<Tag>" << "<Key>" << key << "</Key>" << "<Value>" << value
             << "</Value>" << "</Tag>";
        }
      }
      ss << "</And>";
    }
    if (rule.filter.prefix) {
      ss << "<Prefix>" << rule.filter.prefix << "</Prefix>";
    }
    if (rule.filter.tag) {
      ss << "<Tag>" << "<Key>" << rule.filter.tag.key << "</Key>" << "<Value>"
         << rule.filter.tag.value << "</Value>" << "</Tag>";
    }
    ss << "</Filter>";

    if (!rule.id.empty()) ss << "<ID>" << rule.id << "</ID>";

    if (rule.noncurrent_version_expiration_noncurrent_days) {
      ss << "<NoncurrentVersionExpiration><NoncurrentDays>"
         << rule.noncurrent_version_expiration_noncurrent_days
         << "</NoncurrentDays></NoncurrentVersionExpiration>";
    }

    if (rule.noncurrent_version_transition_noncurrent_days ||
        !rule.noncurrent_version_transition_storage_class.empty()) {
      ss << "<NoncurrentVersionTransition>";
      if (rule.noncurrent_version_transition_noncurrent_days) {
        ss << "<NoncurrentDays>"
           << rule.noncurrent_version_transition_noncurrent_days
           << "</NoncurrentDays>";
      }
      if (!rule.noncurrent_version_transition_storage_class.empty()) {
        ss << "<StorageClass>"
           << rule.noncurrent_version_transition_storage_class
           << "</StorageClass>";
      }
      ss << "</NoncurrentVersionTransition>";
    }

    ss << "<Status>" << (rule.status ? "Enabled" : "Disabled") << "</Status>";

    if (rule.transition_date || rule.transition_days ||
        !rule.transition_storage_class.empty()) {
      ss << "<Transition>";
      if (rule.transition_date) {
        ss << "<Date>" << rule.transition_date.ToISO8601UTC() << "</Date>";
      }
      if (rule.transition_days) {
        ss << "<Days>" << rule.transition_days << "</Days>";
      }
      if (!rule.transition_storage_class.empty()) {
        ss << "<StorageClass>" << rule.transition_storage_class
           << "</StorageClass>";
      }
      ss << "</Transition>";
    }

    ss << "</Rule>";
  }

  ss << "</LifecycleConfiguration>";

  return ss.str();
}

error::Error ObjectLockConfig::Validate() const {
  if (IsRetentionModeValid(retention_mode)) {
    if (!(static_cast<bool>(retention_duration_days) ^
          static_cast<bool>(retention_duration_years))) {
      return error::Error(
          "retention mode must be provided with retention duration days or "
          "years");
    }
  } else if (retention_duration_days || retention_duration_years) {
    return error::Error(
        "retention duration days or years must be provided with retention "
        "mode");
  }

  return error::SUCCESS;
}

SseConfig SseConfig::S3() {
  SseConfig config;
  config.sse_algorithm = "AES256";
  return config;
}

SseConfig SseConfig::Kms(std::string masterkeyid) {
  SseConfig config;
  config.sse_algorithm = "aws:kms";
  config.kms_master_key_id = masterkeyid;
  return config;
}

}  // namespace minio::s3
