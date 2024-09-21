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

#ifndef MINIO_CPP_UTILS_H_INCLUDED
#define MINIO_CPP_UTILS_H_INCLUDED

#include <stdint.h>

#ifndef _WIN32
#include <pwd.h>
#endif

#include <ctime>
#include <ios>
#include <list>
#include <map>
#include <set>
#include <streambuf>
#include <string>
#include <vector>

#include "error.h"

namespace minio::utils {

inline constexpr unsigned int kMaxMultipartCount = 10000;      // 10000 parts
inline constexpr uint64_t kMaxObjectSize = 5'497'558'138'880;  // 5TiB
inline constexpr uint64_t kMaxPartSize = 5'368'709'120;        // 5GiB
inline constexpr unsigned int kMinPartSize = 5 * 1024 * 1024;  // 5MiB

// GetEnv copies the environment variable name into var
bool GetEnv(std::string& var, const char* name);

std::string GetHomeDir();

std::string Printable(const std::string& s);

unsigned long CRC32(std::string_view str);

unsigned int Int(std::string_view str);

// FormatTime formats time as per format.
std::string FormatTime(const std::tm& time, const char* format);

// StringToBool converts string to bool.
bool StringToBool(const std::string& str);

// BoolToString converts bool to string.
inline const char* BoolToString(bool b) { return b ? "true" : "false"; }

// Trim trims leading and trailing character of a string.
std::string Trim(std::string_view str, char ch = ' ');

// CheckNonEmptyString checks whether string is not empty after trimming
// whitespaces.
bool CheckNonEmptyString(std::string_view str);

// ToLower converts string to lower case.
std::string ToLower(const std::string& str);

// StartsWith returns whether str starts with prefix or not.
bool StartsWith(std::string_view str, std::string_view prefix);

// EndsWith returns whether str ends with suffix or not.
bool EndsWith(std::string_view str, std::string_view suffix);

// Contains returns whether str has ch.
bool Contains(std::string_view str, char ch);

// Contains returns whether str has substr.
bool Contains(std::string_view str, std::string_view substr);

// Join returns a string of joined values by delimiter.
std::string Join(const std::list<std::string>& values,
                 const std::string& delimiter);

// Join returns a string of joined values by delimiter.
std::string Join(const std::vector<std::string>& values,
                 const std::string& delimiter);

// EncodePath does URL encoding of path. It also normalizes multiple slashes.
std::string EncodePath(const std::string& path);

// Sha256hash computes SHA-256 of data and return hash as hex encoded value.
std::string Sha256Hash(std::string_view str);

// Base64Encode encodes string to base64.
std::string Base64Encode(std::string_view str);

// Md5sumHash computes MD5 of data and return hash as Base64 encoded value.
std::string Md5sumHash(std::string_view str);

error::Error CheckBucketName(std::string_view bucket_name, bool strict = false);
error::Error ReadPart(std::istream& stream, char* buf, size_t size,
                      size_t& bytes_read);
error::Error CalcPartInfo(long object_size, size_t& part_size,
                          long& part_count);

/**
 * UtcTime represents date and time in UTC timezone.
 */
class UtcTime {
 private:
  std::time_t secs_ = {};
  long usecs_ = 0L;

  static std::tm auxLocaltime(const std::time_t& time);
  std::tm getBrokenDownTime() const { return auxLocaltime(secs_); }
  static std::time_t toUtcSeconds(const std::time_t time);

 public:
  UtcTime() = default;

  UtcTime(std::time_t secs) : secs_(secs) {}
  UtcTime(std::time_t secs, long usecs) : secs_(secs), usecs_(usecs) {}

  ~UtcTime() = default;

  void Add(std::time_t seconds) { secs_ += seconds; }

  void ToLocalTime(std::tm& time);

  std::string ToSignerDate() const;

  std::string ToAmzDate() const;

  std::string ToHttpHeaderValue() const;

  static UtcTime FromHttpHeaderValue(const char* value);

  std::string ToISO8601UTC() const;

  static UtcTime FromISO8601UTC(const char* value);

  static UtcTime Now();

  explicit operator bool() const { return secs_ != 0 && usecs_ != 0; }

  int Compare(const UtcTime& rhs) const;

  bool Equal(const UtcTime& rhs) const { return Compare(rhs) == 0; }

  bool operator==(const UtcTime& rhs) const { return Equal(rhs); }

  bool operator!=(const UtcTime& rhs) const { return !operator==(rhs); }

  bool operator<(const UtcTime& rhs) const { return Compare(rhs) < 0; }

  bool operator>(const UtcTime& rhs) const { return Compare(rhs) > 0; }

  bool operator<=(const UtcTime& rhs) const { return !operator>(rhs); }

  bool operator>=(const UtcTime& rhs) const { return !operator<(rhs); }

#if __cplusplus >= 202002L
  auto operator<=>(const UtcTime& rhs) const { return Compare(rhs); }
#endif

  friend std::ostream& operator<<(std::ostream& s, const UtcTime& v) {
    return s << v.ToISO8601UTC();
  }
};  // class UtcTime

/**
 * Multimap represents dictionary of keys and their multiple values.
 */
class Multimap {
 private:
  std::map<std::string, std::set<std::string>> map_;
  std::map<std::string, std::set<std::string>> keys_;

 public:
  Multimap() = default;
  Multimap(const Multimap& headers) = default;
  Multimap& operator=(const Multimap& headers) = default;
  Multimap(Multimap&& headers) = default;
  Multimap& operator=(Multimap&& headers) = default;
  ~Multimap() = default;

  void Add(std::string key, std::string value);

  void AddAll(const Multimap& headers);

  std::list<std::string> ToHttpHeaders() const;

  std::string ToQueryString() const;

  explicit operator bool() const { return !map_.empty(); }

  bool Contains(std::string_view key) const;

  std::list<std::string> Get(std::string_view key) const;

  std::string GetFront(std::string_view key) const;

  std::list<std::string> Keys() const;

  void GetCanonicalHeaders(std::string& signed_headers,
                           std::string& canonical_headers) const;

  std::string GetCanonicalQueryString() const;
};  // class Multimap

/**
 * CharBuffer represents stream buffer wrapping character array and its size.
 */
struct CharBuffer : std::streambuf {
  CharBuffer(char* buf, size_t size) { this->setg(buf, buf, buf + size); }
  virtual ~CharBuffer();

  virtual pos_type seekpos(pos_type sp, std::ios_base::openmode which) override;

  virtual pos_type seekoff(
      off_type off, std::ios_base::seekdir dir,
      std::ios_base::openmode which = std::ios_base::in) override;
};  // struct CharBuffer

}  // namespace minio::utils

#endif  // MINIO_CPP_UTILS_H_INCLUDED
