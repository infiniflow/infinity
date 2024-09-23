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

#include "miniocpp/response.h"

#include <cstring>
#include <curlpp/cURLpp.hpp>
#include <list>
#include <map>
#include <pugixml.hpp>
#include <string>
#include <type_traits>

#include "miniocpp/error.h"
#include "miniocpp/types.h"
#include "miniocpp/utils.h"

namespace minio::s3 {

Response::Response() {}

Response::~Response() {}

error::Error Response::Error() const {
  if (err_) return err_;
  if (!code.empty()) {
    return error::Error(code + ": " + message);
  }
  if (status_code && (status_code < 200 || status_code > 299)) {
    return error::Error("failed with HTTP status code " +
                        std::to_string(status_code));
  }
  return error::SUCCESS;
}

Response Response::ParseXML(std::string_view data, int status_code,
                            utils::Multimap headers) {
  Response resp;
  resp.status_code = status_code;
  resp.headers = headers;

  pugi::xml_document xdoc;
  pugi::xml_parse_result result = xdoc.load_string(data.data());
  if (!result) {
    resp.err_ = error::Error("unable to parse XML; " + std::string(data));
    return resp;
  }

  auto root = xdoc.select_node("/Error");
  pugi::xpath_node text;

  text = root.node().select_node("Code/text()");
  resp.code = text.node().value();

  text = root.node().select_node("Message/text()");
  resp.message = text.node().value();

  text = root.node().select_node("Resource/text()");
  resp.resource = text.node().value();

  text = root.node().select_node("RequestId/text()");
  resp.request_id = text.node().value();

  text = root.node().select_node("HostId/text()");
  resp.host_id = text.node().value();

  text = root.node().select_node("BucketName/text()");
  resp.bucket_name = text.node().value();

  text = root.node().select_node("Key/text()");
  resp.object_name = text.node().value();

  return resp;
}

ListBucketsResponse ListBucketsResponse::ParseXML(std::string_view data) {
  std::list<Bucket> buckets;

  pugi::xml_document xdoc;
  pugi::xml_parse_result result = xdoc.load_string(data.data());
  if (!result) {
    return error::make<ListBucketsResponse>("unable to parse XML");
  }
  pugi::xpath_node_set xnodes =
      xdoc.select_nodes("/ListAllMyBucketsResult/Buckets/Bucket");
  for (auto xnode : xnodes) {
    std::string name;
    utils::UtcTime creation_date;
    if (auto node = xnode.node().select_node("Name/text()").node()) {
      name = node.value();
    }
    if (auto node = xnode.node().select_node("CreationDate/text()").node()) {
      std::string value = node.value();
      creation_date = utils::UtcTime::FromISO8601UTC(value.c_str());
    }

    buckets.push_back(Bucket(std::move(name), std::move(creation_date)));
  }

  return ListBucketsResponse(buckets);
}

CompleteMultipartUploadResponse CompleteMultipartUploadResponse::ParseXML(
    std::string_view data, std::string version_id) {
  CompleteMultipartUploadResponse resp;

  pugi::xml_document xdoc;
  pugi::xml_parse_result result = xdoc.load_string(data.data());
  if (!result) {
    return error::make<CompleteMultipartUploadResponse>("unable to parse XML");
  }
  auto root = xdoc.select_node("/CompleteMultipartUploadOutput");

  pugi::xpath_node text;

  text = root.node().select_node("Bucket/text()");
  resp.bucket_name = text.node().value();

  text = root.node().select_node("Key/text()");
  resp.object_name = text.node().value();

  text = root.node().select_node("Location/text()");
  resp.location = text.node().value();

  text = root.node().select_node("ETag/text()");
  resp.etag = utils::Trim(text.node().value(), '"');

  resp.version_id = version_id;

  return resp;
}

ListObjectsResponse ListObjectsResponse::ParseXML(std::string_view data,
                                                  bool version) {
  ListObjectsResponse resp;

  pugi::xml_document xdoc;
  pugi::xml_parse_result result = xdoc.load_string(data.data());
  if (!result) {
    return error::make<ListObjectsResponse>("unable to parse XML");
  }
  std::string xpath = version ? "/ListVersionsResult" : "/ListBucketResult";

  auto root = xdoc.select_node(xpath.c_str());

  pugi::xpath_node text;
  std::string value;

  text = root.node().select_node("Name/text()");
  resp.name = text.node().value();

  text = root.node().select_node("EncodingType/text()");
  resp.encoding_type = text.node().value();

  text = root.node().select_node("Prefix/text()");
  value = text.node().value();
  resp.prefix = (resp.encoding_type == "url") ? curlpp::unescape(value) : value;

  text = root.node().select_node("Delimiter/text()");
  resp.delimiter = text.node().value();

  text = root.node().select_node("IsTruncated/text()");
  value = text.node().value();
  if (!value.empty()) resp.is_truncated = utils::StringToBool(value);

  text = root.node().select_node("MaxKeys/text()");
  value = text.node().value();
  if (!value.empty()) resp.max_keys = static_cast<unsigned>(std::stoul(value));

  // ListBucketResult V1
  {
    text = root.node().select_node("Marker/text()");
    value = text.node().value();
    resp.marker =
        (resp.encoding_type == "url") ? curlpp::unescape(value) : value;

    text = root.node().select_node("NextMarker/text()");
    value = text.node().value();
    resp.next_marker =
        (resp.encoding_type == "url") ? curlpp::unescape(value) : value;
  }

  // ListBucketResult V2
  {
    text = root.node().select_node("KeyCount/text()");
    value = text.node().value();
    if (!value.empty())
      resp.key_count = static_cast<unsigned>(std::stoul(value));

    text = root.node().select_node("StartAfter/text()");
    value = text.node().value();
    resp.start_after =
        (resp.encoding_type == "url") ? curlpp::unescape(value) : value;

    text = root.node().select_node("ContinuationToken/text()");
    resp.continuation_token = text.node().value();

    text = root.node().select_node("NextContinuationToken/text()");
    resp.next_continuation_token = text.node().value();
  }

  // ListVersionsResult
  {
    text = root.node().select_node("KeyMarker/text()");
    value = text.node().value();
    resp.key_marker =
        (resp.encoding_type == "url") ? curlpp::unescape(value) : value;

    text = root.node().select_node("NextKeyMarker/text()");
    value = text.node().value();
    resp.next_key_marker =
        (resp.encoding_type == "url") ? curlpp::unescape(value) : value;

    text = root.node().select_node("VersionIdMarker/text()");
    resp.version_id_marker = text.node().value();

    text = root.node().select_node("NextVersionIdMarker/text()");
    resp.next_version_id_marker = text.node().value();
  }

  Item last_item;

  auto populate = [&resp = resp, &last_item = last_item](
                      std::list<Item>& items, pugi::xpath_node_set& contents,
                      bool is_delete_marker) -> void {
    for (auto content : contents) {
      pugi::xpath_node text;
      std::string value;
      Item item;

      text = content.node().select_node("ETag/text()");
      item.etag = utils::Trim(text.node().value(), '"');

      text = content.node().select_node("Key/text()");
      value = text.node().value();
      item.name =
          (resp.encoding_type == "url") ? curlpp::unescape(value) : value;

      text = content.node().select_node("LastModified/text()");
      value = text.node().value();
      item.last_modified = utils::UtcTime::FromISO8601UTC(value.c_str());

      text = content.node().select_node("Owner/ID/text()");
      item.owner_id = text.node().value();

      text = content.node().select_node("Owner/DisplayName/text()");
      item.owner_name = text.node().value();

      text = content.node().select_node("Size/text()");
      value = text.node().value();
      if (!value.empty()) item.size = static_cast<size_t>(std::stoull(value));

      text = content.node().select_node("StorageClass/text()");
      item.storage_class = text.node().value();

      text = content.node().select_node("IsLatest/text()");
      value = text.node().value();
      if (!value.empty()) item.is_latest = utils::StringToBool(value);

      text = content.node().select_node("VersionId/text()");
      item.version_id = text.node().value();

      auto user_metadata = content.node().select_node("UserMetadata");
      for (auto metadata = user_metadata.node().first_child(); metadata;
           metadata = metadata.next_sibling()) {
        item.user_metadata[metadata.name()] = metadata.child_value();
      }

      item.is_delete_marker = is_delete_marker;

      items.push_back(item);
      last_item = item;
    }
  };

  auto contents = root.node().select_nodes(version ? "Version" : "Contents");
  populate(resp.contents, contents, false);
  // Only for ListObjectsV1.
  if (resp.is_truncated && resp.next_marker.empty()) {
    resp.next_marker = last_item.name;
  }

  auto common_prefixes = root.node().select_nodes("CommonPrefixes");
  for (auto common_prefix : common_prefixes) {
    Item item;

    text = common_prefix.node().select_node("Prefix/text()");
    value = text.node().value();
    item.name = (resp.encoding_type == "url") ? curlpp::unescape(value) : value;

    item.is_prefix = true;

    resp.contents.push_back(item);
  }

  auto delete_markers = root.node().select_nodes("DeleteMarker");
  populate(resp.contents, delete_markers, true);

  return resp;
}

RemoveObjectsResponse RemoveObjectsResponse::ParseXML(std::string_view data) {
  RemoveObjectsResponse resp;

  pugi::xml_document xdoc;
  pugi::xml_parse_result result = xdoc.load_string(data.data());
  if (!result) {
    return error::make<RemoveObjectsResponse>("unable to parse XML");
  }
  auto root = xdoc.select_node("/DeleteResult");

  auto deletedList = root.node().select_nodes("Deleted");
  for (auto deleted : deletedList) {
    pugi::xpath_node text;
    std::string value;
    DeletedObject object;

    text = deleted.node().select_node("Key/text()");
    object.name = text.node().value();

    text = deleted.node().select_node("VersionId/text()");
    object.version_id = text.node().value();

    text = deleted.node().select_node("DeleteMarkerVersionId/text()");
    object.delete_marker_version_id = text.node().value();

    text = deleted.node().select_node("DeleteMarker/text()");
    value = text.node().value();
    if (!value.empty()) object.delete_marker = utils::StringToBool(value);

    resp.objects.push_back(object);
  }

  auto errorList = root.node().select_nodes("Error");
  for (auto error : errorList) {
    pugi::xpath_node text;
    std::string value;
    DeleteError err;

    text = error.node().select_node("Key/text()");
    err.object_name = text.node().value();

    text = error.node().select_node("VersionId/text()");
    err.version_id = text.node().value();

    text = error.node().select_node("Code/text()");
    err.code = text.node().value();

    text = error.node().select_node("Message/text()");
    err.message = text.node().value();

    resp.errors.push_back(err);
  }

  return resp;
}

GetBucketNotificationResponse GetBucketNotificationResponse::ParseXML(
    std::string_view data) {
  NotificationConfig config;

  pugi::xml_document xdoc;
  pugi::xml_parse_result result = xdoc.load_string(data.data());
  if (!result) {
    return error::make<GetBucketNotificationResponse>("unable to parse XML");
  }
  auto root = xdoc.select_node("/NotificationConfiguration");

  auto populate = [](pugi::xpath_node& common) -> NotificationCommonConfig {
    NotificationCommonConfig cfg;
    pugi::xpath_node text;

    auto events = common.node().select_nodes("Event");
    for (auto event : events) cfg.events.push_back(event.node().value());

    text = common.node().select_node("Id/text()");
    cfg.id = text.node().value();

    if (common.node().select_node("Filter")) {
      auto filters = common.node().select_nodes("Filter/S3Key/FilterRule");
      for (auto filter : filters) {
        text = filter.node().select_node("Name/text()");
        auto name = text.node().value();

        text = filter.node().select_node("Value/text()");
        auto value = text.node().value();

        if (strcmp(name, "prefix") == 0) {
          cfg.prefix_filter_rule = PrefixFilterRule(value);
        } else {
          cfg.suffix_filter_rule = SuffixFilterRule(value);
        }
      }
    }

    return cfg;
  };

  pugi::xpath_node text;
  std::string value;

  auto cloudfuncs = root.node().select_nodes("CloudFunctionConfiguration");
  for (auto cloudfunc : cloudfuncs) {
    CloudFuncConfig cfg;

    text = cloudfunc.node().select_node("CloudFunction/text()");
    cfg.cloud_func = text.node().value();

    NotificationCommonConfig common = populate(cloudfunc);
    cfg.events = common.events;
    cfg.id = common.id;
    cfg.prefix_filter_rule = common.prefix_filter_rule;
    cfg.suffix_filter_rule = common.suffix_filter_rule;

    config.cloud_func_config_list.push_back(cfg);
  }

  auto queues = root.node().select_nodes("QueueConfiguration");
  for (auto queue : queues) {
    QueueConfig cfg;

    text = queue.node().select_node("Queue/text()");
    cfg.queue = text.node().value();

    NotificationCommonConfig common = populate(queue);
    cfg.events = common.events;
    cfg.id = common.id;
    cfg.prefix_filter_rule = common.prefix_filter_rule;
    cfg.suffix_filter_rule = common.suffix_filter_rule;

    config.queue_config_list.push_back(cfg);
  }

  auto topics = root.node().select_nodes("TopicConfiguration");
  for (auto topic : topics) {
    TopicConfig cfg;

    text = topic.node().select_node("Topic/text()");
    cfg.topic = text.node().value();

    NotificationCommonConfig common = populate(topic);
    cfg.events = common.events;
    cfg.id = common.id;
    cfg.prefix_filter_rule = common.prefix_filter_rule;
    cfg.suffix_filter_rule = common.suffix_filter_rule;

    config.topic_config_list.push_back(cfg);
  }

  return GetBucketNotificationResponse(config);
}

GetBucketEncryptionResponse GetBucketEncryptionResponse::ParseXML(
    std::string_view data) {
  SseConfig config;

  pugi::xml_document xdoc;
  pugi::xml_parse_result result = xdoc.load_string(data.data());
  if (!result) {
    return error::make<GetBucketEncryptionResponse>("unable to parse XML");
  }
  auto root = xdoc.select_node(
      "/ServerSideEncryptionConfiguration/Rule"
      "/ApplyServerSideEncryptionByDefault");

  pugi::xpath_node text = root.node().select_node("SSEAlgorithm/text()");
  config.sse_algorithm = text.node().value();

  text = root.node().select_node("KMSMasterKeyID/text()");
  config.kms_master_key_id = text.node().value();

  return GetBucketEncryptionResponse(config);
}

GetBucketReplicationResponse GetBucketReplicationResponse::ParseXML(
    std::string_view data) {
  ReplicationConfig config;

  pugi::xml_document xdoc;
  pugi::xml_parse_result result = xdoc.load_string(data.data());
  if (!result) {
    return error::make<GetBucketReplicationResponse>("unable to parse XML");
  }
  auto root = xdoc.select_node("/ReplicationConfiguration");

  pugi::xpath_node text;
  std::string value;

  if (root.node().select_node("Role")) {
    text = root.node().select_node("Role/text()");
    config.role = text.node().value();
  }

  auto rules = root.node().select_nodes("Rule");
  for (auto& rule : rules) {
    ReplicationRule rrule;

    text = rule.node().select_node("ID/text()");
    rrule.id = text.node().value();

    text = rule.node().select_node("Status/text()");
    value = text.node().value();
    rrule.status = (value == "Enabled");

    auto destination = rule.node().select_node("Destination");

    text = destination.node().select_node("Bucket/text()");
    rrule.destination.bucket_arn = text.node().value();

    text = destination.node().select_node("Account/text()");
    rrule.destination.account = text.node().value();

    text = destination.node().select_node("StorageClass/text()");
    rrule.destination.storage_class = text.node().value();

    if (destination.node().select_node("AccessControlTranslation")) {
      rrule.destination.access_control_translation.Enable();
      text = destination.node().select_node(
          "AccessControlTranslation/Owner/text()");
      rrule.destination.access_control_translation.owner = text.node().value();
    }
    if (destination.node().select_node("EncryptionConfiguration")) {
      rrule.destination.encryption_config.Enable();
      text = destination.node().select_node(
          "EncryptionConfiguration/ReplicaKmsKeyID/text()");
      rrule.destination.encryption_config.replica_kms_key_id =
          text.node().value();
    }
    if (destination.node().select_node("Metrics")) {
      rrule.destination.metrics.Enable();

      text = destination.node().select_node(
          "Metrics/EventThreshold/Minutes/text()");
      value = text.node().value();
      rrule.destination.metrics.event_threshold_minutes =
          static_cast<unsigned>(std::stoul(value));

      text = destination.node().select_node(
          "Metrics/EventThreshold/Status/text()");
      value = text.node().value();
      rrule.destination.metrics.status = (value == "Enabled");
    }
    if (destination.node().select_node("ReplicationTime")) {
      rrule.destination.replication_time.Enable();

      text = destination.node().select_node("ReplicationTime/Time/text()");
      value = text.node().value();
      rrule.destination.replication_time.time_minutes =
          static_cast<unsigned>(std::stoul(value));

      text = destination.node().select_node("ReplicationTime/Status/text()");
      value = text.node().value();
      rrule.destination.replication_time.status = (value == "Enabled");
    }

    if (rule.node().select_node("DeleteMarkerReplication")) {
      text = rule.node().select_node("DeleteMarkerReplication/Status/text()");
      value = text.node().value();
      rrule.delete_marker_replication_status = (value == "Enabled");
    }

    if (rule.node().select_node("ExistingObjectReplication")) {
      text = rule.node().select_node("ExistingObjectReplication/Status/text()");
      value = text.node().value();
      rrule.existing_object_replication_status = (value == "Enabled");
    }

    if (rule.node().select_node("Filter")) {
      auto filter = rule.node().select_node("Filter");

      if (filter.node().select_node("And")) {
        if (filter.node().select_node("And/Prefix")) {
          text = filter.node().select_node("And/Prefix/text()");
          rrule.filter.and_operator.prefix = Prefix(text.node().value());
        }

        if (filter.node().select_node("And/Tag")) {
          auto tags = root.node().select_nodes("And/Tag");
          for (auto& tag : tags) {
            (void)tag;
            text = filter.node().select_node("Key/text()");
            std::string key = text.node().value();

            text = filter.node().select_node("Value/text()");
            std::string value = text.node().value();

            rrule.filter.and_operator.tags[key] = value;
          }
        }
      }

      if (filter.node().select_node("Prefix")) {
        text = filter.node().select_node("Prefix/text()");
        rrule.filter.prefix = Prefix(text.node().value());
      }

      if (filter.node().select_node("Tag")) {
        text = filter.node().select_node("Tag/Key/text()");
        rrule.filter.tag.key = text.node().value();

        text = filter.node().select_node("Tag/Value/text()");
        rrule.filter.tag.value = text.node().value();
      }
    }

    if (rule.node().select_node("Prefix")) {
      text = rule.node().select_node("Prefix/text()");
      rrule.prefix = Prefix(text.node().value());
    }

    if (rule.node().select_node("Priority")) {
      text = rule.node().select_node("Priority/text()");
      value = text.node().value();
      rrule.priority = Integer(std::stoi(value));
    }

    if (rule.node().select_node("SourceSelectionCriteria")) {
      rrule.source_selection_criteria.Enable();
      if (rule.node().select_node(
              "SourceSelectionCriteria/SseKmsEncryptedObjects")) {
        text = rule.node().select_node(
            "SourceSelectionCriteria/SseKmsEncryptedObjects/Status/text()");
        value = text.node().value();
        rrule.source_selection_criteria.sse_kms_encrypted_objects_status =
            (value == "Enabled");
      }
    }

    if (rule.node().select_node("DeleteReplication")) {
      text = rule.node().select_node("DeleteReplication/Status/text()");
      value = text.node().value();
      rrule.delete_replication_status = (value == "Enabled");
    }

    config.rules.push_back(rrule);
  }

  return GetBucketReplicationResponse(config);
}

GetBucketLifecycleResponse GetBucketLifecycleResponse::ParseXML(
    std::string_view data) {
  LifecycleConfig config;

  pugi::xml_document xdoc;
  pugi::xml_parse_result result = xdoc.load_string(data.data());
  if (!result) {
    return error::make<GetBucketLifecycleResponse>("unable to parse XML");
  }
  auto root = xdoc.select_node("/LifecycleConfiguration");

  pugi::xpath_node text;
  std::string value;

  auto rules = root.node().select_nodes("Rule");
  for (auto& rule : rules) {
    LifecycleRule lrule;

    if (rule.node().select_node("AbortIncompleteMultipartUpload")) {
      text = rule.node().select_node(
          "AbortIncompleteMultipartUpload/DaysAfterInitiation/text()");
      value = text.node().value();
      lrule.abort_incomplete_multipart_upload_days_after_initiation =
          std::stoi(value);
    }

    if (rule.node().select_node("Expiration")) {
      if (rule.node().select_node("Expiration/Date")) {
        text = rule.node().select_node("Expiration/Date/text()");
        lrule.expiration_date =
            utils::UtcTime::FromISO8601UTC(text.node().value());
      }

      if (rule.node().select_node("Expiration/Days")) {
        text = rule.node().select_node("Expiration/Days/text()");
        value = text.node().value();
        lrule.expiration_days = std::stoi(value);
      }

      if (rule.node().select_node("Expiration/ExpiredObjectDeleteMarker")) {
        text = rule.node().select_node(
            "Expiration/ExpiredObjectDeleteMarker/text()");
        lrule.expiration_expired_object_delete_marker =
            utils::BoolToString(text.node().value());
      }
    }

    auto filter = rule.node().select_node("Filter");
    if (filter.node().select_node("And")) {
      if (filter.node().select_node("And/Prefix")) {
        text = filter.node().select_node("And/Prefix/text()");
        lrule.filter.and_operator.prefix = Prefix(text.node().value());
      }

      if (filter.node().select_node("And/Tag")) {
        auto tags = root.node().select_nodes("And/Tag");
        for (auto& tag : tags) {
          (void)tag;
          text = filter.node().select_node("Key/text()");
          std::string key = text.node().value();

          text = filter.node().select_node("Value/text()");
          std::string value = text.node().value();

          lrule.filter.and_operator.tags[key] = value;
        }
      }
    }
    if (filter.node().select_node("Prefix")) {
      text = filter.node().select_node("Prefix/text()");
      lrule.filter.prefix = Prefix(text.node().value());
    }
    if (filter.node().select_node("Tag")) {
      text = filter.node().select_node("Tag/Key/text()");
      lrule.filter.tag.key = text.node().value();

      text = filter.node().select_node("Tag/Value/text()");
      lrule.filter.tag.value = text.node().value();
    }

    text = rule.node().select_node("ID/text()");
    lrule.id = text.node().value();

    if (rule.node().select_node("NoncurrentVersionExpiration")) {
      text = rule.node().select_node(
          "NoncurrentVersionExpiration/NoncurrentDays/text()");
      value = text.node().value();
      lrule.noncurrent_version_expiration_noncurrent_days = std::stoi(value);
    }

    if (rule.node().select_node("NoncurrentVersionTransition")) {
      text = rule.node().select_node(
          "NoncurrentVersionTransition/NoncurrentDays/text()");
      value = text.node().value();
      lrule.noncurrent_version_transition_noncurrent_days = std::stoi(value);

      text = rule.node().select_node(
          "NoncurrentVersionTransition/StorageClass/text()");
      lrule.noncurrent_version_transition_storage_class = text.node().value();
    }

    text = rule.node().select_node("Status/text()");
    value = text.node().value();
    lrule.status = (value == "Enabled");

    if (rule.node().select_node("Transition")) {
      if (rule.node().select_node("Transition/Date")) {
        text = rule.node().select_node("Transition/Date/text()");
        lrule.transition_date =
            utils::UtcTime::FromISO8601UTC(text.node().value());
      }

      if (rule.node().select_node("Transition/Days")) {
        text = rule.node().select_node("Transition/Days/text()");
        value = text.node().value();
        lrule.transition_days = std::stoi(value);
      }

      text = rule.node().select_node("Transition/StorageClass/text()");
      lrule.transition_storage_class = text.node().value();
    }

    config.rules.push_back(lrule);
  }

  return GetBucketLifecycleResponse(config);
}

GetBucketTagsResponse GetBucketTagsResponse::ParseXML(std::string_view data) {
  std::map<std::string, std::string> map;

  pugi::xml_document xdoc;
  pugi::xml_parse_result result = xdoc.load_string(data.data());
  if (!result) {
    return error::make<GetBucketTagsResponse>("unable to parse XML");
  }
  auto tags = xdoc.select_nodes("/Tagging/TagSet/Tag");

  pugi::xpath_node text;

  for (auto& tag : tags) {
    text = tag.node().select_node("Key/text()");
    std::string key = text.node().value();

    text = tag.node().select_node("Value/text()");
    std::string value = text.node().value();

    map[key] = value;
  }

  return map;
}

GetObjectTagsResponse GetObjectTagsResponse::ParseXML(std::string_view data) {
  std::map<std::string, std::string> map;

  pugi::xml_document xdoc;
  pugi::xml_parse_result result = xdoc.load_string(data.data());
  if (!result) {
    return error::make<GetObjectTagsResponse>("unable to parse XML");
  }
  auto tags = xdoc.select_nodes("/Tagging/TagSet/Tag");

  pugi::xpath_node text;

  for (auto& tag : tags) {
    text = tag.node().select_node("Key/text()");
    std::string key = text.node().value();

    text = tag.node().select_node("Value/text()");
    std::string value = text.node().value();

    map[key] = value;
  }

  return map;
}

}  // namespace minio::s3
