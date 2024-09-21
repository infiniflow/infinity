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

#include "miniocpp/signer.h"

#include <openssl/evp.h>
#include <openssl/hmac.h>

#include <array>
#include <cstdio>
#include <string>
#include <type_traits>

#include "miniocpp/http.h"
#include "miniocpp/utils.h"

namespace minio::signer {

std::string GetScope(const utils::UtcTime& time, const std::string& region,
                     const std::string& service_name) {
  return time.ToSignerDate() + "/" + region + "/" + service_name +
         "/aws4_request";
}

std::string GetCanonicalRequestHash(const std::string& method,
                                    const std::string& uri,
                                    const std::string& query_string,
                                    const std::string& headers,
                                    const std::string& signed_headers,
                                    const std::string& content_sha256) {
  // CanonicalRequest =
  //   HTTPRequestMethod + '\n' +
  //   CanonicalURI + '\n' +
  //   CanonicalQueryString + '\n' +
  //   CanonicalHeaders + '\n\n' +
  //   SignedHeaders + '\n' +
  //   HexEncode(Hash(RequestPayload))
  std::string canonical_request = method + "\n" + uri + "\n" + query_string +
                                  "\n" + headers + "\n\n" + signed_headers +
                                  "\n" + content_sha256;
  return utils::Sha256Hash(canonical_request);
}

std::string GetStringToSign(const utils::UtcTime& date,
                            const std::string& scope,
                            const std::string& canonical_request_hash) {
  return "AWS4-HMAC-SHA256\n" + date.ToAmzDate() + "\n" + scope + "\n" +
         canonical_request_hash;
}

std::string HmacHash(std::string_view key, std::string_view data) {
  std::array<unsigned char, EVP_MAX_MD_SIZE> hash;
  unsigned int hash_len;

  HMAC(EVP_sha256(), key.data(), static_cast<int>(key.size()),
       reinterpret_cast<unsigned char const*>(data.data()),
       static_cast<int>(data.size()), hash.data(), &hash_len);

  return std::string{reinterpret_cast<char const*>(hash.data()), hash_len};
}

std::string GetSigningKey(const std::string& secret_key,
                          const utils::UtcTime& date, std::string_view region,
                          std::string_view service_name) {
  std::string date_key = HmacHash("AWS4" + secret_key, date.ToSignerDate());
  std::string date_region_key = HmacHash(date_key, region);
  std::string date_region_service_key = HmacHash(date_region_key, service_name);
  return HmacHash(date_region_service_key, "aws4_request");
}

std::string GetSignature(std::string_view signing_key,
                         std::string_view string_to_sign) {
  std::string hash = HmacHash(signing_key, string_to_sign);
  std::string signature;
  char buf[3];
  for (std::size_t i = 0, n_size = hash.size(); i < n_size; ++i) {
    snprintf(buf, 3, "%02x", (unsigned char)hash[i]);
    signature += buf;
  }
  return signature;
}

std::string GetAuthorization(const std::string& access_key,
                             const std::string& scope,
                             const std::string& signed_headers,
                             const std::string& signature) {
  return "AWS4-HMAC-SHA256 Credential=" + access_key + "/" + scope + ", " +
         "SignedHeaders=" + signed_headers + ", " + "Signature=" + signature;
}

utils::Multimap SignV4(const std::string& service_name, http::Method method,
                       const std::string& uri, const std::string& region,
                       utils::Multimap& headers, utils::Multimap query_params,
                       const std::string& access_key,
                       const std::string& secret_key,
                       const std::string& content_sha256,
                       const utils::UtcTime& date) {
  std::string scope = GetScope(date, region, service_name);

  std::string signed_headers;
  std::string canonical_headers;
  headers.GetCanonicalHeaders(signed_headers, canonical_headers);

  std::string canonical_query_string = query_params.GetCanonicalQueryString();

  std::string methodstring = http::MethodToString(method);
  std::string canonical_request_hash = GetCanonicalRequestHash(
      methodstring, uri, canonical_query_string, canonical_headers,
      signed_headers, content_sha256);

  std::string string_to_sign =
      GetStringToSign(date, scope, canonical_request_hash);

  std::string signing_key =
      GetSigningKey(secret_key, date, region, service_name);

  std::string signature = GetSignature(signing_key, string_to_sign);

  std::string authorization =
      GetAuthorization(access_key, scope, signed_headers, signature);

  headers.Add("Authorization", authorization);
  return headers;
}

utils::Multimap SignV4S3(http::Method method, const std::string& uri,
                         const std::string& region, utils::Multimap& headers,
                         utils::Multimap query_params,
                         const std::string& access_key,
                         const std::string& secret_key,
                         const std::string& content_sha256,
                         const utils::UtcTime& date) {
  std::string service_name = "s3";
  return SignV4(service_name, method, uri, region, headers,
                std::move(query_params), access_key, secret_key, content_sha256,
                date);
}

utils::Multimap SignV4STS(http::Method method, const std::string& uri,
                          const std::string& region, utils::Multimap& headers,
                          utils::Multimap query_params,
                          const std::string& access_key,
                          const std::string& secret_key,
                          const std::string& content_sha256,
                          const utils::UtcTime& date) {
  std::string service_name = "sts";
  return SignV4(service_name, method, uri, region, headers,
                std::move(query_params), access_key, secret_key, content_sha256,
                date);
}

utils::Multimap PresignV4(http::Method method, const std::string& host,
                          const std::string& uri, const std::string& region,
                          utils::Multimap& query_params,
                          const std::string& access_key,
                          const std::string& secret_key,
                          const utils::UtcTime& date, unsigned int expires) {
  std::string service_name = "s3";
  std::string scope = GetScope(date, region, service_name);
  std::string canonical_headers = "host:" + host;
  std::string signed_headers = "host";

  query_params.Add("X-Amz-Algorithm", "AWS4-HMAC-SHA256");
  query_params.Add("X-Amz-Credential", access_key + "/" + scope);
  query_params.Add("X-Amz-Date", date.ToAmzDate());
  query_params.Add("X-Amz-Expires", std::to_string(expires));
  query_params.Add("X-Amz-SignedHeaders", signed_headers);
  std::string canonical_query_string = query_params.GetCanonicalQueryString();
  std::string methodstring = http::MethodToString(method);
  std::string content_sha256 = "UNSIGNED-PAYLOAD";
  std::string canonical_request_hash = GetCanonicalRequestHash(
      methodstring, uri, canonical_query_string, canonical_headers,
      signed_headers, content_sha256);

  std::string string_to_sign =
      GetStringToSign(date, scope, canonical_request_hash);
  std::string signing_key =
      GetSigningKey(secret_key, date, region, service_name);
  std::string signature = GetSignature(signing_key, string_to_sign);
  query_params.Add("X-Amz-Signature", signature);
  return query_params;
}

std::string PostPresignV4(const std::string& string_to_sign,
                          const std::string& secret_key,
                          const utils::UtcTime& date,
                          const std::string& region) {
  std::string service_name = "s3";
  std::string signing_key =
      GetSigningKey(secret_key, date, region, service_name);
  return GetSignature(signing_key, string_to_sign);
}

}  // namespace minio::signer
