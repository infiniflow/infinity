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

#include "miniocpp/request.h"

#include <exception>
#include <iosfwd>
#include <iostream>
#include <ostream>
#include <regex>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "miniocpp/credentials.h"
#include "miniocpp/error.h"
#include "miniocpp/http.h"
#include "miniocpp/providers.h"
#include "miniocpp/signer.h"
#include "miniocpp/utils.h"

namespace minio::s3 {

static const std::string AWS_S3_PREFIX =
    "^(((bucket\\.|accesspoint\\.)"
    "vpce(-(?!_)[a-z_\\d]+)+\\.s3\\.)|"
    "((?!s3)(?!-)(?!_)[a-z_\\d-]{1,63}\\.)"
    "s3-control(-(?!_)[a-z_\\d]+)*\\.|"
    "(s3(-(?!_)[a-z_\\d]+)*\\.))";
static const std::regex HOSTNAME_REGEX(
    "^((?!-)(?!_)[a-z_\\d-]{1,63}\\.)*"
    "((?!_)(?!-)[a-z_\\d-]{1,63})$",
    std::regex_constants::icase);
static const std::regex AWS_ENDPOINT_REGEX(".*\\.amazonaws\\.com(|\\.cn)$",
                                           std::regex_constants::icase);
static const std::regex AWS_S3_ENDPOINT_REGEX(
    AWS_S3_PREFIX + "((?!s3)(?!-)(?!_)[a-z_\\d-]{1,63}\\.)*" +
        "amazonaws\\.com(|\\.cn)$",
    std::regex_constants::icase);
static const std::regex AWS_ELB_ENDPOINT_REGEX(
    "^(?!-)(?!_)[a-z_\\d-]{1,63}\\."
    "(?!-)(?!_)[a-z_\\d-]{1,63}\\."
    "elb\\.amazonaws\\.com$",
    std::regex_constants::icase);
static const std::regex AWS_S3_PREFIX_REGEX(AWS_S3_PREFIX,
                                            std::regex_constants::icase);
static const std::regex REGION_REGEX("^((?!_)(?!-)[a-z_\\d-]{1,63})$",
                                     std::regex_constants::icase);

static constexpr char EMPTY_SHA256[] =
    "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

static bool awsRegexMatch(std::string_view value, const std::regex& regex) {
  if (!std::regex_search(value.data(), regex)) return false;

  std::stringstream str_stream(value.data());
  std::string token;
  while (std::getline(str_stream, token, '.')) {
    if (token.back() == '-' || token.back() == '_') return false;
  }

  return true;
}

error::Error getAwsInfo(const std::string& host, bool https,
                        std::string& region, std::string& aws_s3_prefix,
                        std::string& aws_domain_suffix, bool& dualstack) {
  if (!awsRegexMatch(host, HOSTNAME_REGEX)) return error::SUCCESS;

  if (awsRegexMatch(host, AWS_ELB_ENDPOINT_REGEX)) {
    std::string token = host.substr(0, host.rfind(".elb.amazonaws.com") - 1);
    std::string region_in_host = token.substr(token.rfind('.') + 1);
    if (region.empty()) region = region_in_host;
    return error::SUCCESS;
  }

  if (!awsRegexMatch(host, AWS_ENDPOINT_REGEX)) return error::SUCCESS;

  if (!awsRegexMatch(host, AWS_S3_ENDPOINT_REGEX)) {
    return error::Error("invalid Amazon AWS host " + host);
  }

  std::smatch match;
  std::regex_search(host, match, AWS_S3_PREFIX_REGEX);
  aws_s3_prefix = host.substr(0, static_cast<size_t>(match.length()));

  if (utils::Contains(aws_s3_prefix, "s3-accesspoint") && !https) {
    return error::Error("use HTTPS scheme for host " + host);
  }

  std::stringstream str_stream(
      host.substr(static_cast<size_t>(match.length())));
  std::string token;
  std::vector<std::string> tokens;
  while (std::getline(str_stream, token, '.')) tokens.push_back(token);

  dualstack = tokens[0] == "dualstack";
  if (dualstack) tokens.erase(tokens.begin());
  std::string region_in_host;
  if (tokens[0] != "vpce" && tokens[0] != "amazonaws") {
    region_in_host = tokens[0];
    tokens.erase(tokens.begin());
  }

  aws_domain_suffix = utils::Join(tokens, ".");

  if (host == "s3-external-1.amazonaws.com") region_in_host = "us-east-1";
  if (host == "s3-us-gov-west-1.amazonaws.com" ||
      host == "s3-fips-us-gov-west-1.amazonaws.com") {
    region_in_host = "us-gov-west-1";
  }

  if (utils::EndsWith(aws_domain_suffix, ".cn") &&
      !utils::EndsWith(aws_s3_prefix, "s3-accelerate.") && region.empty()) {
    return error::Error("region missing in Amazon S3 China endpoint " + host);
  }

  if (region.empty()) region = region_in_host;

  return error::SUCCESS;
}

std::string extractRegion(const std::string& host) {
  std::stringstream str_stream(host);
  std::string token;
  std::vector<std::string> tokens;
  while (std::getline(str_stream, token, '.')) tokens.push_back(token);

  token = tokens[1];

  // If token is "dualstack", then region might be in next token.
  if (token == "dualstack") token = tokens[2];

  // If token is equal to "amazonaws", region is not passed in the host.
  if (token == "amazonaws") return {};

  // Return token as region.
  return token;
}

BaseUrl::BaseUrl(std::string host, bool https, std::string region)
    : https(https), region(std::move(region)) {
  const http::Url url = http::Url::Parse(host);
  if (!url.path.empty() || !url.query_string.empty()) {
    this->err_ = error::Error(
        "host value must contain only hostname and optional port number");
    return;
  }

  this->host = url.host;
  this->port = url.port;

  if (!this->region.empty() && !awsRegexMatch(this->region, REGION_REGEX)) {
    this->err_ = error::Error("invalid region " + this->region);
    return;
  }

  if (error::Error err =
          getAwsInfo(this->host, this->https, this->region, this->aws_s3_prefix,
                     this->aws_domain_suffix, this->dualstack)) {
    this->err_ = err;
    return;
  }
  this->virtual_style = !this->aws_domain_suffix.empty() ||
                        utils::EndsWith(this->host, "aliyuncs.com");
}

error::Error BaseUrl::BuildAwsUrl(http::Url& url,
                                  const std::string& bucket_name,
                                  bool enforce_path_style,
                                  const std::string& region) {
  std::string host = this->aws_s3_prefix + this->aws_domain_suffix;
  if (host == "s3-external-1.amazonaws.com" ||
      host == "s3-us-gov-west-1.amazonaws.com" ||
      host == "s3-fips-us-gov-west-1.amazonaws.com") {
    url.host = host;
    return error::SUCCESS;
  }

  host = this->aws_s3_prefix;
  if (utils::Contains(this->aws_s3_prefix, "s3-accelerate")) {
    if (utils::Contains(bucket_name, '.')) {
      return error::Error("bucket name '" + bucket_name +
                          "' with '.' is not allowed for accelerate endpoint");
    }

    if (enforce_path_style) host.replace(host.find("-accelerate"), 11, "");
  }

  if (this->dualstack) host += "dualstack.";
  if (!utils::Contains(this->aws_s3_prefix, "s3-accelerate")) {
    host += region + ".";
  }
  host += this->aws_domain_suffix;

  url.host = host;

  return error::SUCCESS;
}

void BaseUrl::BuildListBucketsUrl(http::Url& url, const std::string& region) {
  if (this->aws_domain_suffix.empty()) return;

  std::string host = this->aws_s3_prefix + this->aws_domain_suffix;
  if (host == "s3-external-1.amazonaws.com" ||
      host == "s3-us-gov-west-1.amazonaws.com" ||
      host == "s3-fips-us-gov-west-1.amazonaws.com") {
    url.host = host;
    return;
  }

  std::string s3_prefix = this->aws_s3_prefix;
  std::string domain_suffix = this->aws_domain_suffix;
  if (utils::StartsWith(s3_prefix, "s3.") ||
      utils::StartsWith(s3_prefix, "s3-")) {
    s3_prefix = "s3.";
    domain_suffix = "amazonaws.com";
    if (utils::EndsWith(this->aws_domain_suffix, ".cn")) domain_suffix += ".cn";
  }
  url.host = s3_prefix + region + "." + domain_suffix;
}

error::Error BaseUrl::BuildUrl(http::Url& url, http::Method method,
                               const std::string& region,
                               const utils::Multimap& query_params,
                               const std::string& bucket_name,
                               const std::string& object_name) {
  if (err_) return err_;

  if (bucket_name.empty() && !object_name.empty()) {
    return error::Error("empty bucket name for object name " + object_name);
  }

  url = http::Url(https, std::string(this->host), port, "/",
                  query_params.ToQueryString());

  if (bucket_name.empty()) {
    this->BuildListBucketsUrl(url, region);
    return error::SUCCESS;
  }

  bool enforce_path_style = (
      // CreateBucket API requires path style in Amazon AWS S3.
      (method == http::Method::kPut && object_name.empty() && !query_params) ||

      // GetBucketLocation API requires path style in Amazon AWS S3.
      query_params.Contains("location") ||

      // Use path style for bucket name containing '.' which causes
      // SSL certificate validation error.
      (utils::Contains(bucket_name, '.') && https));

  if (!this->aws_domain_suffix.empty()) {
    if (error::Error err =
            this->BuildAwsUrl(url, bucket_name, enforce_path_style, region)) {
      return err;
    }
  }

  std::string host = url.host;
  std::string path = "/";

  if (enforce_path_style || !this->virtual_style) {
    path = "/" + bucket_name;
  } else {
    host = bucket_name + "." + host;
  }

  if (!object_name.empty()) {
    if (!utils::EndsWith(path, "/")) path += '/';
    path += utils::EncodePath(object_name);
  }

  url.host = host;
  url.path = path;

  return error::SUCCESS;
}

Request::Request(http::Method method, std::string region, BaseUrl& baseurl,
                 utils::Multimap extra_headers,
                 utils::Multimap extra_query_params)
    : method(method),
      region(std::move(region)),
      base_url(baseurl),
      headers(std::move(extra_headers)),
      query_params(std::move(extra_query_params)) {}

void Request::BuildHeaders(http::Url& url, creds::Provider* const provider) {
  headers.Add("Host", url.HostHeaderValue());
  headers.Add("User-Agent", user_agent);

  bool md5sum_added = headers.Contains("Content-MD5");
  std::string md5sum;

  switch (method) {
    case http::Method::kPut:
    case http::Method::kPost:
      headers.Add("Content-Length", std::to_string(body.size()));
      if (!headers.Contains("Content-Type")) {
        headers.Add("Content-Type", "application/octet-stream");
      }
      if (provider != nullptr) {
        sha256 = utils::Sha256Hash(body);
      } else if (!md5sum_added) {
        md5sum = utils::Md5sumHash(body);
      }
      break;
    default:
      if (provider != nullptr) {
        sha256 = EMPTY_SHA256;
      }
  }

  if (!md5sum.empty()) headers.Add("Content-MD5", md5sum);
  if (!sha256.empty()) headers.Add("x-amz-content-sha256", sha256);

  date = utils::UtcTime::Now();
  headers.Add("x-amz-date", date.ToAmzDate());

  if (provider != nullptr) {
    creds::Credentials creds = provider->Fetch();
    if (!creds.session_token.empty()) {
      headers.Add("X-Amz-Security-Token", creds.session_token);
    }

    signer::SignV4S3(method, url.path, region, headers, query_params,
                     creds.access_key, creds.secret_key, sha256, date);
  }
}

http::Request Request::ToHttpRequest(creds::Provider* const provider) {
  http::Url url;
  if (error::Error err = base_url.BuildUrl(url, method, region, query_params,
                                           bucket_name, object_name)) {
    std::cerr << "failed to build url. error=" << err
              << ". This should not happen" << std::endl;
    std::terminate();
  }
  BuildHeaders(url, provider);

  http::Request request(method, url);
  request.body = body;
  request.headers = headers;
  request.datafunc = datafunc;
  request.userdata = userdata;
  request.progressfunc = progressfunc;
  request.progress_userdata = progress_userdata;
  request.debug = debug;
  request.ignore_cert_check = ignore_cert_check;
  request.ssl_cert_file = ssl_cert_file;

  return request;
}

}  // namespace minio::s3
