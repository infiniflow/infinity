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

#include <string>
#include <string_view>
#include <fstream>
#include <sstream>

#include "velocypack/velocypack-common.h"
#include "velocypack/Buffer.h"

namespace arangodb::velocypack {

struct Sink {
  Sink() {}
  Sink(Sink const&) = delete;
  Sink& operator=(Sink const&) = delete;

  virtual ~Sink() = default;
  virtual void push_back(char c) = 0;
  virtual void append(char const* p, ValueLength len) = 0;
  virtual void reserve(ValueLength len) = 0;

  // default implementations, can be overridden
  virtual void append(std::string const& p) { append(p.data(), p.size()); }
  virtual void append(std::string_view p) { append(p.data(), p.size()); }
  [[deprecated]] virtual void append(char const* p) {
    append(p, std::strlen(p));
  }
};

template<typename T>
struct ByteBufferSinkImpl final : public Sink {
  using Sink::append;

  explicit ByteBufferSinkImpl(Buffer<T>* buffer) : _buffer(buffer) {}

  void push_back(char c) final { _buffer->push_back(c); }

  void append(char const* p, ValueLength len) final { _buffer->append(p, len); }

  void reserve(ValueLength len) final { _buffer->reserve(len); }

 private:
  Buffer<T>* _buffer;
};

typedef ByteBufferSinkImpl<char> CharBufferSink;

template<typename T>
struct StringSinkImpl final : public Sink {
  using Sink::append;

  explicit StringSinkImpl(T* buffer) : _buffer(buffer) {}

  void push_back(char c) final { _buffer->push_back(c); }

  void append(char const* p, ValueLength len) final {
    _buffer->append(p, checkOverflow(len));
  }

  void reserve(ValueLength len) final {
    ValueLength length = len + _buffer->size();
    if (length <= _buffer->capacity()) {
      return;
    }
    _buffer->reserve(checkOverflow(len));
  }

 private:
  T* _buffer;
};

typedef StringSinkImpl<std::string> StringSink;

// a sink with an upper bound for the generated output value
template<typename T>
struct SizeConstrainedStringSinkImpl final : public Sink {
  using Sink::append;

  explicit SizeConstrainedStringSinkImpl(T* buffer, ValueLength maxLength)
      : _maxLength(maxLength),
        _buffer(buffer),
        _length(0),
        _overflowed(false) {}

  void push_back(char c) final {
    ++_length;
    if (_buffer->size() < _maxLength) {
      _buffer->push_back(c);
      VELOCYPACK_ASSERT(_buffer->size() <= _maxLength);
    } else {
      _overflowed = true;
    }
  }

  void append(char const* p, ValueLength len) final {
    _length += len;
    if (_buffer->size() < _maxLength) {
      ValueLength total = checkOverflow(_buffer->size() + checkOverflow(len));
      if (total <= _maxLength) {
        _buffer->append(p, len);
        VELOCYPACK_ASSERT(_buffer->size() <= _maxLength);
        return;
      }
      ValueLength left = _maxLength - _buffer->size();
      if (len > left) {
        len = left;
      }
      _buffer->append(p, len);
      VELOCYPACK_ASSERT(_buffer->size() <= _maxLength);
    }
    _overflowed = true;
  }

  void reserve(ValueLength len) final {
    ValueLength total = checkOverflow(_buffer->size() + checkOverflow(len));
    if (total <= _buffer->capacity()) {
      return;
    }
    VELOCYPACK_ASSERT(_buffer->size() <= _maxLength);
    ValueLength left = _maxLength - _buffer->size();
    if (len > left) {
      len = left;
    }
    if (len > 0) {
      _buffer->reserve(checkOverflow(len));
    }
  }

  ValueLength maxLength() const noexcept { return _maxLength; }
  ValueLength unconstrainedLength() const noexcept { return _length; }
  bool overflowed() const noexcept { return _overflowed; }

 private:
  std::size_t const _maxLength;
  T* _buffer;
  ValueLength _length;
  bool _overflowed;
};

typedef SizeConstrainedStringSinkImpl<std::string> SizeConstrainedStringSink;

// only tracks the length of the generated output
struct StringLengthSink final : public Sink {
  using Sink::append;

  StringLengthSink() : _length(0) {}

  void push_back(char) final { ++_length; }

  void append(char const*, ValueLength len) final { _length += len; }

  void reserve(ValueLength) final {}

  ValueLength length() const noexcept { return _length; }

 private:
  ValueLength _length;
};

template<typename T>
struct StreamSinkImpl final : public Sink {
  using Sink::append;

  explicit StreamSinkImpl(T* stream) : _stream(stream) {}

  void push_back(char c) final { *_stream << c; }

  void append(std::string const& p) final { *_stream << p; }

  void append(char const* p, ValueLength len) final {
    _stream->write(p, static_cast<std::streamsize>(len));
  }

  void reserve(ValueLength) final {}

 private:
  T* _stream;
};

typedef StreamSinkImpl<std::ostringstream> StringStreamSink;
typedef StreamSinkImpl<std::ofstream> OutputFileStreamSink;

}  // namespace arangodb::velocypack

using VPackSink = arangodb::velocypack::Sink;
using VPackCharBufferSink = arangodb::velocypack::CharBufferSink;
using VPackStringSink = arangodb::velocypack::StringSink;
using VPackSizeConstrainedStringSink =
    arangodb::velocypack::SizeConstrainedStringSink;
using VPackStringLengthSink = arangodb::velocypack::StringLengthSink;
using VPackStringStreamSink = arangodb::velocypack::StringStreamSink;
using VPackOutputFileStreamSink = arangodb::velocypack::OutputFileStreamSink;
