////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2020 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Max Neunhoeffer
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "velocypack/velocypack-common.h"
#include "velocypack/Options.h"
#include "velocypack/Slice.h"

namespace arangodb::velocypack {
struct Sink;

// Dumps VPack into a JSON output string
class Dumper {
 public:
  Options const* options;

  Dumper(Dumper const&) = delete;
  Dumper& operator=(Dumper const&) = delete;

  explicit Dumper(Sink* sink, Options const* options = &Options::Defaults);

  ~Dumper() = default;

  Sink* sink() const { return _sink; }

  void dump(Slice slice);

  [[deprecated]] void dump(Slice const* slice) { dump(*slice); }

  static void dump(Slice slice, Sink* sink,
                   Options const* options = &Options::Defaults);

  static void dump(Slice const* slice, Sink* sink,
                   Options const* options = &Options::Defaults);

  static std::string toString(Slice slice,
                              Options const* options = &Options::Defaults);

  [[deprecated]] static std::string toString(Slice const* slice,
                                             Options const* options = &Options::Defaults);

  void append(Slice slice) { dumpValue(slice); }

  [[deprecated]] void append(Slice const* slice) { dumpValue(*slice); }

  void appendString(char const* src, ValueLength len);

  void appendString(std::string_view str) {
    appendString(str.data(), str.size());
  }

  void appendInt(int64_t v);

  void appendUInt(uint64_t v);

  void appendDouble(double v);

 private:
  void dumpUnicodeCharacter(uint16_t value);

  void dumpInteger(Slice slice);

  void dumpString(char const* src, ValueLength len);

  void dumpValue(Slice slice, Slice const* base = nullptr);

  void indent();

  void handleUnsupportedType(Slice slice);

  Sink* _sink;

  int _indentation;
};

}  // namespace arangodb::velocypack

using VPackDumper = arangodb::velocypack::Dumper;
