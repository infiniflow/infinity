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

#include "miniocpp/credentials.h"

#include <pugixml.hpp>
#include <string>
#include <type_traits>

#include "miniocpp/error.h"
#include "miniocpp/utils.h"

namespace minio::creds {

bool expired(const utils::UtcTime& expiration) {
  if (!expiration) return false;
  utils::UtcTime now = utils::UtcTime::Now();
  now.Add(10);
  return expiration < now;
}

Credentials Credentials::ParseXML(std::string_view data,
                                  const std::string& root) {
  pugi::xml_document xdoc;
  pugi::xml_parse_result result = xdoc.load_string(data.data());
  if (!result) {
    return error::make<Credentials>("unable to parse XML");
  }
  auto credentials = xdoc.select_node((root + "/Credentials").c_str());

  auto text = credentials.node().select_node("AccessKeyId/text()");
  std::string access_key = text.node().value();

  text = credentials.node().select_node("SecretAccessKey/text()");
  std::string secret_key = text.node().value();

  text = credentials.node().select_node("SessionToken/text()");
  std::string session_token = text.node().value();

  text = credentials.node().select_node("Expiration/text()");
  auto expiration = utils::UtcTime::FromISO8601UTC(text.node().value());

  return Credentials(error::SUCCESS, std::move(access_key),
                     std::move(secret_key), std::move(session_token),
                     expiration);
}

}  // namespace minio::creds
