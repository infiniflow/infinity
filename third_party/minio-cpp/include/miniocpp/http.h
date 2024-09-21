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

#ifndef MINIO_CPP_HTTP_H_INCLUDED
#define MINIO_CPP_HTTP_H_INCLUDED

#include <curlpp/Easy.hpp>
#include <curlpp/Multi.hpp>
#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <type_traits>

#include "error.h"
#include "utils.h"

namespace minio::http {

enum class Method { kGet, kHead, kPost, kPut, kDelete };

//! MethodToString converts http Method enum to string.
const char* MethodToString(Method method) noexcept;

/**
 * Url represents HTTP URL and it's components.
 */
struct Url {
  bool https = false;
  std::string host;
  unsigned int port = 0;
  std::string path;
  std::string query_string;

  Url() = default;
  explicit Url(bool https, std::string host, unsigned int port,
               std::string path, std::string query_string)
      : https(https),
        host(std::move(host)),
        port(port),
        path(std::move(path)),
        query_string(std::move(query_string)) {}
  ~Url() = default;

  explicit operator bool() const { return !host.empty(); }

  std::string String() const;
  std::string HostHeaderValue() const;
  static Url Parse(std::string value);
};  // struct Url

struct DataFunctionArgs;

using DataFunction = std::function<bool(DataFunctionArgs)>;

struct ProgressFunctionArgs;

using ProgressFunction = std::function<bool(ProgressFunctionArgs)>;

struct Response;

struct DataFunctionArgs {
  curlpp::Easy* handle = nullptr;
  Response* response = nullptr;
  std::string datachunk;
  void* userdata = nullptr;

  DataFunctionArgs() = default;
  DataFunctionArgs(curlpp::Easy* handle, Response* response, void* userdata)
      : handle(handle), response(response), userdata(userdata) {}
  DataFunctionArgs(curlpp::Easy* handle, Response* response,
                   std::string datachunk, void* userdata)
      : handle(handle),
        response(response),
        datachunk(std::move(datachunk)),
        userdata(userdata) {}

  ~DataFunctionArgs() = default;
};  // struct DataFunctionArgs

struct ProgressFunctionArgs {
  double download_total_bytes = 0.0;
  double downloaded_bytes = 0.0;
  double upload_total_bytes = 0.0;
  double uploaded_bytes = 0.0;
  double download_speed = 0.0;
  double upload_speed = 0.0;
  void* userdata = nullptr;
};  // struct ProgressFunctionArgs

struct Request {
  Method method;
  http::Url url;
  utils::Multimap headers;
  std::string_view body;
  DataFunction datafunc = nullptr;
  void* userdata = nullptr;
  ProgressFunction progressfunc = nullptr;
  void* progress_userdata = nullptr;
  bool debug = false;
  bool ignore_cert_check = false;
  std::string ssl_cert_file;
  std::string key_file;
  std::string cert_file;

  Request(Method method, Url url);
  ~Request() = default;

  Response Execute();

  explicit operator bool() const {
    if (method < Method::kGet || method > Method::kDelete) return false;
    return static_cast<bool>(url);
  }

 private:
  Response execute();
};  // struct Request

struct Response {
  std::string error;
  DataFunction datafunc = nullptr;
  void* userdata = nullptr;
  int status_code = 0;
  utils::Multimap headers;
  std::string body;

  Response() = default;
  ~Response() = default;

  size_t ResponseCallback(curlpp::Multi* const requests,
                          curlpp::Easy* const request, const char* const buffer,
                          size_t size, size_t length);

  explicit operator bool() const {
    return error.empty() && status_code >= 200 && status_code <= 299;
  }

  error::Error Error() const;

 private:
  std::string response_;
  bool continue100_ = false;
  bool status_code_read_ = false;
  bool headers_read_ = false;

  error::Error ReadStatusCode();
  error::Error ReadHeaders();
};  // struct Response

}  // namespace minio::http

#endif  // MINIO_CPP_HTTP_H_INCLUDED
