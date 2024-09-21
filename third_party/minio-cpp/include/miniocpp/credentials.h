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

#ifndef MINIO_CPP_CREDENTIALS_H_INCLUDED
#define MINIO_CPP_CREDENTIALS_H_INCLUDED

#include <string>
#include <type_traits>

#include "error.h"
#include "utils.h"

namespace minio::creds {

bool expired(const utils::UtcTime& expiration);

/**
 * Credentials contains access key and secret key with optional session token
 * and expiration.
 */
struct Credentials {
  error::Error err;
  std::string access_key = {};
  std::string secret_key = {};
  std::string session_token = {};
  utils::UtcTime expiration = {};

  Credentials() = default;
  explicit Credentials(error::Error err) : err(std::move(err)) {}

  explicit Credentials(error::Error err, std::string access_key,
                       std::string secret_key)
      : err(std::move(err)),
        access_key(std::move(access_key)),
        secret_key(std::move(secret_key)) {}

  explicit Credentials(error::Error err, std::string access_key,
                       std::string secret_key, std::string session_token)
      : err(std::move(err)),
        access_key(std::move(access_key)),
        secret_key(std::move(secret_key)),
        session_token(std::move(session_token)) {}

  explicit Credentials(error::Error err, std::string access_key,
                       std::string secret_key, std::string session_token,
                       utils::UtcTime expiration)
      : err(std::move(err)),
        access_key(std::move(access_key)),
        secret_key(std::move(secret_key)),
        session_token(std::move(session_token)),
        expiration(std::move(expiration)) {}

  ~Credentials() = default;

  bool IsExpired() const { return expired(expiration); }

  explicit operator bool() const {
    return !err && !access_key.empty() && expired(expiration);
  }

  static Credentials ParseXML(std::string_view data, const std::string& root);
};  // class Credentials

}  // namespace minio::creds

#endif  // MINIO_CPP_CREDENTIALS_H_INCLUDED
