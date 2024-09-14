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

#ifndef MINIO_CPP_REQUEST_H_INCLUDED
#define MINIO_CPP_REQUEST_H_INCLUDED

#include <string>

#include "error.h"
#include "http.h"
#include "providers.h"
#include "utils.h"

namespace minio::s3 {

error::Error getAwsInfo(const std::string& host, bool https,
                        std::string& region, std::string& aws_s3_prefix,
                        std::string& aws_domain_suffix, bool& dualstack);

std::string extractRegion(const std::string& host);

struct BaseUrl {
  bool https = true;
  std::string host;
  unsigned int port = 0;
  std::string region;
  std::string aws_s3_prefix;
  std::string aws_domain_suffix;
  bool dualstack = false;
  bool virtual_style = false;

  BaseUrl() = default;
  explicit BaseUrl(std::string host, bool https = true,
                   std::string region = {});
  ~BaseUrl() = default;

  error::Error BuildUrl(http::Url& url, http::Method method,
                        const std::string& region,
                        const utils::Multimap& query_params,
                        const std::string& bucket_name,
                        const std::string& object_name);

  explicit operator bool() const { return !err_ && !host.empty(); }

  error::Error Error() const {
    if (host.empty() && !err_) {
      return error::Error("empty host");
    }
    return err_;
  }

 private:
  error::Error err_;

  error::Error BuildAwsUrl(http::Url& url, const std::string& bucket_name,
                           bool enforce_path_style, const std::string& region);
  void BuildListBucketsUrl(http::Url& url, const std::string& region);
};  // struct Url

struct Request {
  http::Method method;
  std::string region;
  BaseUrl& base_url;

  std::string user_agent;

  utils::Multimap headers;
  utils::Multimap query_params;

  std::string bucket_name;
  std::string object_name;

  std::string_view body;

  http::DataFunction datafunc = nullptr;
  void* userdata = nullptr;

  http::ProgressFunction progressfunc = nullptr;
  void* progress_userdata = nullptr;

  std::string sha256;
  utils::UtcTime date;

  bool debug = false;
  bool ignore_cert_check = false;
  std::string ssl_cert_file;

  Request(http::Method method, std::string region, BaseUrl& baseurl,
          utils::Multimap extra_headers, utils::Multimap extra_query_params);

  ~Request() = default;

  http::Request ToHttpRequest(creds::Provider* const provider = nullptr);

 private:
  void BuildHeaders(http::Url& url, creds::Provider* const provider);
};  // struct Request

}  // namespace minio::s3

#endif  // MINIO_CPP_REQUEST_H_INCLUDED
