// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "butil/version.h"

#include <stddef.h>

#include <algorithm>

#include "butil/logging.h"
#include "butil/strings/string_number_conversions.h"
#include "butil/strings/string_split.h"
#include "butil/strings/string_util.h"

namespace butil {

namespace {

// Parses the |numbers| vector representing the different numbers
// inside the version string and constructs a vector of valid integers. It stops
// when it reaches an invalid item (including the wildcard character). |parsed|
// is the resulting integer vector. Function returns true if all numbers were
// parsed successfully, false otherwise.
bool ParseVersionNumbers(const std::string& version_str,
                         std::vector<uint16_t>* parsed) {
  std::vector<std::string> numbers;
  SplitString(version_str, '.', &numbers);
  if (numbers.empty())
    return false;

  for (std::vector<std::string>::const_iterator it = numbers.begin();
       it != numbers.end(); ++it) {
    int num;
    if (!StringToInt(*it, &num))
      return false;

    if (num < 0)
      return false;

    const uint16_t max = 0xFFFF;
    if (num > max)
      return false;

    // This throws out things like +3, or 032.
    if (IntToString(num) != *it)
      return false;

    parsed->push_back(static_cast<uint16_t>(num));
  }
  return true;
}

// Compares version components in |components1| with components in
// |components2|. Returns -1, 0 or 1 if |components1| is less than, equal to,
// or greater than |components2|, respectively.
int CompareVersionComponents(const std::vector<uint16_t>& components1,
                             const std::vector<uint16_t>& components2) {
  const size_t count = std::min(components1.size(), components2.size());
  for (size_t i = 0; i < count; ++i) {
    if (components1[i] > components2[i])
      return 1;
    if (components1[i] < components2[i])
      return -1;
  }
  if (components1.size() > components2.size()) {
    for (size_t i = count; i < components1.size(); ++i) {
      if (components1[i] > 0)
        return 1;
    }
  } else if (components1.size() < components2.size()) {
    for (size_t i = count; i < components2.size(); ++i) {
      if (components2[i] > 0)
        return -1;
    }
  }
  return 0;
}

}  // namespace

Version::Version() {
}

Version::~Version() {
}

Version::Version(const std::string& version_str) {
  std::vector<uint16_t> parsed;
  if (!ParseVersionNumbers(version_str, &parsed))
    return;

  components_.swap(parsed);
}

bool Version::IsValid() const {
  return (!components_.empty());
}

// static
bool Version::IsValidWildcardString(const std::string& wildcard_string) {
  std::string version_string = wildcard_string;
  if (EndsWith(wildcard_string.c_str(), ".*", false))
    version_string = wildcard_string.substr(0, wildcard_string.size() - 2);

  Version version(version_string);
  return version.IsValid();
}

bool Version::IsOlderThan(const std::string& version_str) const {
  Version proposed_ver(version_str);
  if (!proposed_ver.IsValid())
    return false;
  return (CompareTo(proposed_ver) < 0);
}

int Version::CompareToWildcardString(const std::string& wildcard_string) const {
  DCHECK(IsValid());
  DCHECK(Version::IsValidWildcardString(wildcard_string));

  // Default behavior if the string doesn't end with a wildcard.
  if (!EndsWith(wildcard_string.c_str(), ".*", false)) {
    Version version(wildcard_string);
    DCHECK(version.IsValid());
    return CompareTo(version);
  }

  std::vector<uint16_t> parsed;
  const bool success = ParseVersionNumbers(
      wildcard_string.substr(0, wildcard_string.length() - 2), &parsed);
  DCHECK(success);
  const int comparison = CompareVersionComponents(components_, parsed);
  // If the version is smaller than the wildcard version's |parsed| vector,
  // then the wildcard has no effect (e.g. comparing 1.2.3 and 1.3.*) and the
  // version is still smaller. Same logic for equality (e.g. comparing 1.2.2 to
  // 1.2.2.* is 0 regardless of the wildcard). Under this logic,
  // 1.2.0.0.0.0 compared to 1.2.* is 0.
  if (comparison == -1 || comparison == 0)
    return comparison;

  // Catch the case where the digits of |parsed| are found in |components_|,
  // which means that the two are equal since |parsed| has a trailing "*".
  // (e.g. 1.2.3 vs. 1.2.* will return 0). All other cases return 1 since
  // components is greater (e.g. 3.2.3 vs 1.*).
  DCHECK_GT(parsed.size(), 0UL);
  const size_t min_num_comp = std::min(components_.size(), parsed.size());
  for (size_t i = 0; i < min_num_comp; ++i) {
    if (components_[i] != parsed[i])
      return 1;
  }
  return 0;
}

bool Version::Equals(const Version& that) const {
  DCHECK(IsValid());
  DCHECK(that.IsValid());
  return (CompareTo(that) == 0);
}

int Version::CompareTo(const Version& other) const {
  DCHECK(IsValid());
  DCHECK(other.IsValid());
  return CompareVersionComponents(components_, other.components_);
}

const std::string Version::GetString() const {
  DCHECK(IsValid());
  std::string version_str;
  size_t count = components_.size();
  for (size_t i = 0; i < count - 1; ++i) {
    version_str.append(IntToString(components_[i]));
    version_str.append(".");
  }
  version_str.append(IntToString(components_[count - 1]));
  return version_str;
}

}  // namespace butil
