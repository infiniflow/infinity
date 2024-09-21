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

#include "miniocpp/utils.h"

#include "miniocpp/error.h"

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <corecrt.h>
#endif

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/types.h>
#include <time.h>
#include <zconf.h>
#include <zlib.h>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <clocale>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <curlpp/cURLpp.hpp>
#include <exception>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <ostream>
#include <regex>
#include <sstream>
#include <streambuf>
#include <string>
#include <type_traits>
#include <vector>

namespace minio::utils {

static const std::string WEEK_DAYS[] = {"Sun", "Mon", "Tue", "Wed",
                                        "Thu", "Fri", "Sat"};
static const std::string MONTHS[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                     "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static const std::regex MULTI_SPACE_REGEX("( +)");

static const std::regex BUCKET_NAME_REGEX(
    "^[a-z0-9][a-z0-9\\.\\-]{1,61}[a-z0-9]$");
static const std::regex OLD_BUCKET_NAME_REGEX(
    "^[a-z0-9][a-z0-9_\\.\\-\\:]{1,61}[a-z0-9]$", std::regex_constants::icase);
static const std::regex IPV4_REGEX(
    "^((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\\.){3}"
    "(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])$");

#ifdef _WIN32
// strptime is defined here because it's not available on Windows.
static char* strptime(const char* s, const char* f, struct tm* tm) {
  std::istringstream input(s);
  input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
  input >> std::get_time(tm, f);
  if (input.fail()) {
    return nullptr;
  }
  return (char*)(s + input.tellg());
}
#endif

bool GetEnv(std::string& var, const char* name) {
  if (const char* value = std::getenv(name)) {
    var = value;
    return true;
  }
  return false;
}

std::string GetHomeDir() {
  std::string home;
#ifdef _WIN32
  GetEnv(home, "USERPROFILE");
  return home;
#else
  if (GetEnv(home, "HOME")) return home;
  return getpwuid(getuid())->pw_dir;
#endif
}

std::string Printable(const std::string& s) {
  std::stringstream ss;
  for (auto& ch : s) {
    if (ch < 33 || ch > 126) {
      ss << "\\x" << std::hex << std::setfill('0') << std::setw(2)
         << (ch & 0xff);
    } else {
      ss << ch;
    }
  }

  return ss.str();
}

unsigned long CRC32(std::string_view str) {
  return crc32(0, reinterpret_cast<const unsigned char*>(str.data()),
               static_cast<uInt>(str.size()));
}

unsigned int Int(std::string_view str) {
  const unsigned char* data =
      reinterpret_cast<const unsigned char*>(str.data());

  return (unsigned(data[0]) << 24) | (unsigned(data[1]) << 16) |
         (unsigned(data[2]) << 8) | (unsigned(data[3]));
}

bool StringToBool(const std::string& str) {
  std::string s = ToLower(str);
  if (s == "false") return false;
  if (s == "true") return true;

  std::cerr << "ABORT: Unknown bool string. This should not happen."
            << std::endl;
  std::terminate();

  return false;
}

std::string Trim(std::string_view str, char ch) {
  std::size_t start, len;
  for (start = 0; start < str.size() && str[start] == ch; start++);
  for (len = str.size() - start; len > 0 && str[start + len - 1] == ch; len--);
  return std::string(str.substr(start, len));
}

bool CheckNonEmptyString(std::string_view str) {
  return !str.empty() && Trim(str) == str;
}

std::string ToLower(const std::string& str) {
  std::string s(str);
  std::transform(s.begin(), s.end(), s.begin(), ::tolower);
  return s;
}

bool StartsWith(std::string_view str, std::string_view prefix) {
  return (str.size() >= prefix.size() &&
          str.compare(0, prefix.size(), prefix) == 0);
}

bool EndsWith(std::string_view str, std::string_view suffix) {
  return (str.size() >= suffix.size() &&
          str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0);
}

bool Contains(std::string_view str, char ch) {
  return str.find(ch) != std::string::npos;
}

bool Contains(std::string_view str, std::string_view substr) {
  return str.find(substr) != std::string::npos;
}

std::string Join(const std::list<std::string>& values,
                 const std::string& delimiter) {
  std::string result;
  for (const auto& value : values) {
    if (!result.empty()) result += delimiter;
    result += value;
  }
  return result;
}

std::string Join(const std::vector<std::string>& values,
                 const std::string& delimiter) {
  std::string result;
  for (const auto& value : values) {
    if (!result.empty()) result += delimiter;
    result += value;
  }
  return result;
}

std::string EncodePath(const std::string& path) {
  std::stringstream str_stream(path);
  std::string token;
  std::string out;
  while (std::getline(str_stream, token, '/')) {
    if (!token.empty()) {
      if (!out.empty()) out += "/";
      out += curlpp::escape(token);
    }
  }

  if (*(path.begin()) == '/') out = "/" + out;
  if (*(path.end() - 1) == '/' && out != "/") out += "/";

  return out;
}

std::string Sha256Hash(std::string_view str) {
  EVP_MD_CTX* ctx = EVP_MD_CTX_create();
  if (ctx == nullptr) {
    std::cerr << "failed to create EVP_MD_CTX" << std::endl;
    std::terminate();
  }

  if (1 != EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr)) {
    std::cerr << "failed to init SHA-256 digest" << std::endl;
    std::terminate();
  }

  if (1 != EVP_DigestUpdate(ctx, str.data(), str.size())) {
    std::cerr << "failed to update digest" << std::endl;
    std::terminate();
  }

  unsigned int length = static_cast<unsigned>(EVP_MD_size(EVP_sha256()));
  unsigned char* digest = (unsigned char*)OPENSSL_malloc(length);
  if (digest == nullptr) {
    std::cerr << "failed to allocate memory for hash value" << std::endl;
    std::terminate();
  }

  if (1 != EVP_DigestFinal_ex(ctx, digest, &length)) {
    OPENSSL_free(digest);
    std::cerr << "failed to finalize digest" << std::endl;
    std::terminate();
  }

  EVP_MD_CTX_destroy(ctx);

  std::string hash;
  char buf[3];
  for (unsigned int i = 0; i < length; ++i) {
    snprintf(buf, 3, "%02x", digest[i]);
    hash += buf;
  }

  OPENSSL_free(digest);

  return hash;
}

std::string Base64Encode(std::string_view str) {
  const auto base64_memory = BIO_new(BIO_s_mem());
  auto base64 = BIO_new(BIO_f_base64());
  base64 = BIO_push(base64, base64_memory);

  BIO_write(base64, str.data(), static_cast<int>(str.size()));
  BIO_flush(base64);

  BUF_MEM* buf_mem{};
  BIO_get_mem_ptr(base64, &buf_mem);
  auto base64_encoded = std::string(buf_mem->data, buf_mem->length - 1);

  BIO_free_all(base64);

  return base64_encoded;
}

std::string Md5sumHash(std::string_view str) {
  EVP_MD_CTX* ctx = EVP_MD_CTX_create();
  if (ctx == nullptr) {
    std::cerr << "failed to create EVP_MD_CTX" << std::endl;
    std::terminate();
  }

  if (1 != EVP_DigestInit_ex(ctx, EVP_md5(), nullptr)) {
    std::cerr << "failed to init MD5 digest" << std::endl;
    std::terminate();
  }

  if (1 != EVP_DigestUpdate(ctx, str.data(), str.size())) {
    std::cerr << "failed to update digest" << std::endl;
    std::terminate();
  }

  unsigned int length = static_cast<unsigned>(EVP_MD_size(EVP_md5()));
  unsigned char* digest = static_cast<unsigned char*>(OPENSSL_malloc(length));
  if (digest == nullptr) {
    std::cerr << "failed to allocate memory for hash value" << std::endl;
    std::terminate();
  }

  if (1 != EVP_DigestFinal_ex(ctx, digest, &length)) {
    OPENSSL_free(digest);
    std::cerr << "failed to finalize digest" << std::endl;
    std::terminate();
  }

  EVP_MD_CTX_destroy(ctx);

  std::string hash(reinterpret_cast<const char*>(digest), length);
  OPENSSL_free(digest);

  return Base64Encode(hash);
}

std::string FormatTime(const std::tm& time, const char* format) {
  char buf[128];
  std::strftime(buf, 128, format, &time);
  return std::string(buf);
}

std::tm UtcTime::auxLocaltime(const std::time_t& time) {
  std::tm result{};
#ifdef _WIN32
  localtime_s(&result, &time);
#else
  localtime_r(&time, &result);
#endif
  return result;
}

std::time_t UtcTime::toUtcSeconds(const std::time_t secs_local) {
  std::tm result{};
#ifdef _WIN32
  gmtime_s(&result, &secs_local);
#else
  gmtime_r(&secs_local, &result);
#endif
  result.tm_isdst = -1;
  return std::mktime(&result);
}

UtcTime UtcTime::Now() {
  auto usec_now = std::chrono::system_clock::now().time_since_epoch() /
                  std::chrono::microseconds(1);
  return UtcTime(toUtcSeconds(static_cast<time_t>(usec_now / 1000000)),
                 static_cast<long>(usec_now % 1000000));
}

void UtcTime::ToLocalTime(std::tm& time) {
  auto usec_now = std::chrono::system_clock::now().time_since_epoch() /
                  std::chrono::microseconds(1);
  auto secs_local = static_cast<time_t>(usec_now / 1000000);
  auto secs = secs_ + (secs_local - toUtcSeconds(secs_local));
  time = auxLocaltime(secs);
}

std::string UtcTime::ToSignerDate() const {
  return FormatTime(getBrokenDownTime(), "%Y%m%d");
}

std::string UtcTime::ToAmzDate() const {
  return FormatTime(getBrokenDownTime(), "%Y%m%dT%H%M%SZ");
}

std::string UtcTime::ToHttpHeaderValue() const {
  const auto tm = getBrokenDownTime();
  std::stringstream ss;
  ss << WEEK_DAYS[tm.tm_wday] << ", " << FormatTime(tm, "%d ")
     << MONTHS[tm.tm_mon] << FormatTime(tm, " %Y %H:%M:%S GMT");
  return ss.str();
}

UtcTime UtcTime::FromHttpHeaderValue(const char* value) {
  std::string s(value);
  if (s.size() != 29) return UtcTime();

  // Parse week day.
  auto pos =
      std::find(std::begin(WEEK_DAYS), std::end(WEEK_DAYS), s.substr(0, 3));
  if (pos == std::end(WEEK_DAYS)) return UtcTime();
  if (s.at(3) != ',') return UtcTime();
  if (s.at(4) != ' ') return UtcTime();
  auto week_day = pos - std::begin(WEEK_DAYS);

  // Parse day.
  std::tm day{};
  strptime(s.substr(5, 2).c_str(), "%d", &day);
  if (s.at(7) != ' ') return UtcTime();

  // Parse month.
  pos = std::find(std::begin(MONTHS), std::end(MONTHS), s.substr(8, 3));
  if (pos == std::end(MONTHS)) return UtcTime();
  auto month = pos - std::begin(MONTHS);

  // Parse rest of values.
  std::tm ltm{};
  strptime(s.substr(11).c_str(), " %Y %H:%M:%S GMT", &ltm);
  ltm.tm_mday = day.tm_mday;
  ltm.tm_mon = static_cast<int>(month);

  // Validate week day.
  auto t = auxLocaltime(std::mktime(&ltm));
  if (week_day != t.tm_wday) {
    return UtcTime();
  }
  return UtcTime(std::mktime(&t));
}

std::string UtcTime::ToISO8601UTC() const {
  char buf[64];
  snprintf(buf, 64, "%03ld", (long int)usecs_);
  std::string usec_str(buf);
  if (usec_str.size() > 3) usec_str = usec_str.substr(0, 3);
  return FormatTime(getBrokenDownTime(), "%Y-%m-%dT%H:%M:%S.") + usec_str + "Z";
}

UtcTime UtcTime::FromISO8601UTC(const char* value) {
  std::tm t{};
  char* rv = strptime(value, "%Y-%m-%dT%H:%M:%S", &t);
  std::time_t secs = std::mktime(&t);

  unsigned long ul = 0;
#ifdef _WIN32
  static_cast<void>(sscanf_s(rv, ".%lu", &ul));
#else
  static_cast<void>(sscanf(rv, ".%lu", &ul));
#endif
  long usecs = (long)ul;

  return UtcTime(secs, usecs);
}

int UtcTime::Compare(const UtcTime& rhs) const {
  if (secs_ != rhs.secs_) {
    return (secs_ < rhs.secs_) ? -1 : 1;
  }
  if (usecs_ != rhs.usecs_) {
    return (usecs_ < rhs.usecs_) ? -1 : 1;
  }
  return 0;
}

void Multimap::Add(std::string key, std::string value) {
  map_[key].insert(std::move(value));
  keys_[ToLower(key)].insert(std::move(key));
}

void Multimap::AddAll(const Multimap& headers) {
  auto m = headers.map_;
  for (auto& [key, values] : m) {
    map_[key].insert(values.begin(), values.end());
    keys_[ToLower(key)].insert(key);
  }
}

std::list<std::string> Multimap::ToHttpHeaders() const {
  std::list<std::string> headers;
  for (auto& [key, values] : map_) {
    for (auto& value : values) {
      headers.push_back(key + ": " + value);
    }
  }
  return headers;
}

std::string Multimap::ToQueryString() const {
  std::string query_string;
  for (auto& [key, values] : map_) {
    for (auto& value : values) {
      std::string s = curlpp::escape(key) + "=" + curlpp::escape(value);
      if (!query_string.empty()) query_string += "&";
      query_string += s;
    }
  }
  return query_string;
}

bool Multimap::Contains(std::string_view key) const {
  return keys_.find(ToLower(std::string(key))) != keys_.end();
}

std::list<std::string> Multimap::Get(std::string_view key) const {
  std::list<std::string> result;

  if (const auto i = keys_.find(ToLower(std::string(key))); i != keys_.cend()) {
    for (auto& k : i->second) {
      if (const auto j = map_.find(k); j != map_.cend()) {
        result.insert(result.end(), j->second.begin(), j->second.cend());
      }
    }
  }
  return result;
}

std::string Multimap::GetFront(std::string_view key) const {
  std::list<std::string> values = Get(key);
  return (values.size() > 0) ? values.front() : std::string();
}

std::list<std::string> Multimap::Keys() const {
  std::list<std::string> keys;
  for (const auto& [key, _] : keys_) keys.push_back(key);
  return keys;
}

void Multimap::GetCanonicalHeaders(std::string& signed_headers,
                                   std::string& canonical_headers) const {
  std::vector<std::string> signed_headerslist;
  std::map<std::string, std::string> map;

  for (const auto& [k, values] : map_) {
    std::string key = ToLower(k);
    if ("authorization" == key || "user-agent" == key) continue;
    if (std::find(signed_headerslist.begin(), signed_headerslist.end(), key) ==
        signed_headerslist.end()) {
      signed_headerslist.push_back(key);
    }

    std::string value;
    for (const auto& v : values) {
      if (!value.empty()) value += ",";
      value += utils::Trim(std::regex_replace(v, MULTI_SPACE_REGEX, " "));
    }

    map[key] = value;
  }

  std::sort(signed_headerslist.begin(), signed_headerslist.end());
  signed_headers = utils::Join(signed_headerslist, ";");

  std::vector<std::string> canonical_headerslist;
  for (auto& [key, value] : map) {
    canonical_headerslist.push_back(key + ":" + value);
  }

  std::sort(canonical_headerslist.begin(), canonical_headerslist.end());
  canonical_headers = utils::Join(canonical_headerslist, "\n");
}

std::string Multimap::GetCanonicalQueryString() const {
  std::vector<std::string> keys;
  for (auto& [key, _] : map_) {
    keys.push_back(key);
  }

  std::sort(keys.begin(), keys.end());
  std::vector<std::string> values;

  for (auto& key : keys) {
    if (const auto i = map_.find(key); i != map_.cend()) {
      for (auto& value : i->second) {
        values.push_back(curlpp::escape(key) + "=" + curlpp::escape(value));
      }
    }
  }

  return utils::Join(values, "&");
}

error::Error CheckBucketName(std::string_view bucket_name, bool strict) {
  if (Trim(bucket_name).empty()) {
    return error::Error("bucket name cannot be empty");
  }

  if (bucket_name.length() < 3) {
    return error::Error("bucket name cannot be less than 3 characters");
  }

  if (bucket_name.length() > 63) {
    return error::Error("Bucket name cannot be greater than 63 characters");
  }

  if (std::regex_match(bucket_name.data(), IPV4_REGEX)) {
    return error::Error("bucket name cannot be an IP address");
  }

  // unallowed successive characters check.
  if (Contains(bucket_name, "..") || Contains(bucket_name, ".-") ||
      Contains(bucket_name, "-.")) {
    return error::Error(
        "Bucket name contains invalid successive characters '..', '.-' or "
        "'-.'");
  }

  if (strict) {
    if (!std::regex_match(bucket_name.data(), BUCKET_NAME_REGEX)) {
      return error::Error("bucket name does not follow S3 standards strictly");
    }
  } else if (!std::regex_match(bucket_name.data(), OLD_BUCKET_NAME_REGEX)) {
    return error::Error("bucket name does not follow S3 standards");
  }

  return error::SUCCESS;
}

error::Error ReadPart(std::istream& stream, char* buf, size_t size,
                      size_t& bytes_read) {
  stream.read(buf, size);
  bytes_read = stream.gcount();
  return error::SUCCESS;
}

error::Error CalcPartInfo(long object_size, size_t& part_size,
                          long& part_count) {
  if (part_size > 0) {
    if (part_size < kMinPartSize) {
      return error::Error("part size " + std::to_string(part_size) +
                          " is not supported; minimum allowed 5MiB");
    }

    if (part_size > kMaxPartSize) {
      return error::Error("part size " + std::to_string(part_size) +
                          " is not supported; maximum allowed 5GiB");
    }
  }

  if (object_size >= 0) {
    if (static_cast<uint64_t>(object_size) > kMaxObjectSize) {
      return error::Error("object size " + std::to_string(object_size) +
                          " is not supported; maximum allowed 5TiB");
    }
  } else if (part_size <= 0) {
    return error::Error(
        "valid part size must be provided when object size is unknown");
  }

  if (object_size < 0) {
    part_count = -1;
    return error::SUCCESS;
  }

  if (part_size <= 0) {
    // Calculate part size by multiple of kMinPartSize.
    double psize = std::ceil((double)object_size / kMaxMultipartCount);
    part_size = (size_t)std::ceil(psize / kMinPartSize) * kMinPartSize;
  }

  if (static_cast<long>(part_size) > object_size) part_size = object_size;
  part_count = static_cast<long>(
      (part_size > 0) ? ((object_size + part_size - 1) / part_size) : 1);
  if (part_count > kMaxMultipartCount) {
    return error::Error(
        "object size " + std::to_string(object_size) + " and part size " +
        std::to_string(part_size) + " make more than " +
        std::to_string(kMaxMultipartCount) + "parts for upload");
  }

  return error::SUCCESS;
}

CharBuffer::~CharBuffer() {}

std::streambuf::pos_type CharBuffer::seekpos(pos_type sp,
                                             std::ios_base::openmode which) {
  return seekoff(sp - pos_type(off_type(0)), std::ios_base::beg, which);
}

std::streambuf::pos_type CharBuffer::seekoff(
    off_type off, std::ios_base::seekdir dir,
    std::ios_base::openmode /* which */) {
  if (dir == std::ios_base::cur)
    gbump(static_cast<int>(off));
  else if (dir == std::ios_base::end)
    setg(eback(), egptr() + off, egptr());
  else if (dir == std::ios_base::beg)
    setg(eback(), eback() + off, egptr());
  return gptr() - eback();
}

}  // namespace minio::utils
