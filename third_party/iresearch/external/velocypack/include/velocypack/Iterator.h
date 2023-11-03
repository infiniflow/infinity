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
#include <iterator>
#include <iosfwd>
#include <tuple>

#include "velocypack/velocypack-common.h"
#include "velocypack/Exception.h"
#include "velocypack/Slice.h"
#include "velocypack/ValueType.h"

namespace arangodb::velocypack {

class ArrayIterator {
 public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = Slice;
  using pointer = Slice*;
  using reference = Slice&;

  struct Empty {};

  ArrayIterator() = delete;

  // optimization for an empty array
  explicit ArrayIterator(Empty) noexcept
      : _slice{Slice::emptyArraySlice()},
        _current{nullptr},
        _size{0},
        _position{0} {}

  explicit ArrayIterator(Slice slice)
      : _slice{slice}, _current{nullptr}, _size{0}, _position{0} {
    auto const head = slice.head();
    if (VELOCYPACK_UNLIKELY(slice.type(head) != ValueType::Array)) {
      throw Exception{Exception::InvalidValueType, "Expecting Array slice"};
    }
    _size = slice.arrayLength();
    _current = first();
  }

  ArrayIterator& operator++() noexcept {
    next();
    return *this;
  }

  [[nodiscard]] ArrayIterator operator++(int) noexcept {
    ArrayIterator prev{*this};
    next();
    return prev;
  }

  [[nodiscard]] bool operator==(ArrayIterator const& other) const noexcept {
    return _position == other._position;
  }

  [[nodiscard]] Slice operator*() const { return value(); }

  [[nodiscard]] ArrayIterator begin() const noexcept { return {*this}; }

#if (defined(_MSC_VER) ||                                     \
     (defined(__clang_version__) && __clang_major__ >= 12) || \
     (!defined(__clang__) && defined(__GNUC__) && __GNUC__ >= 10))
  [[nodiscard]] std::default_sentinel_t end() const noexcept { return {}; }
  [[nodiscard]] bool operator==(std::default_sentinel_t) const noexcept {
    return !valid();
  }
#else
  [[nodiscard]] ArrayIterator end() const noexcept {
    ArrayIterator it{*this};
    it._position = it._size;
    return it;
  }
  [[nodiscard]] bool operator!=(ArrayIterator const& other) const noexcept {
    return !operator==(other);
  }
#endif

  [[nodiscard]] bool valid() const noexcept { return _position != _size; }

  [[nodiscard]] Slice value() const {
    if (VELOCYPACK_UNLIKELY(!valid())) {
      throw Exception{Exception::IndexOutOfBounds};
    }
    VELOCYPACK_ASSERT(_current != nullptr);
    return Slice{_current};
  }

  void next() noexcept {
    if (VELOCYPACK_UNLIKELY(!valid())) {
      return;
    }
    ++_position;
    VELOCYPACK_ASSERT(_current != nullptr);
    _current += Slice(_current).byteSize();
  }

  [[nodiscard]] ValueLength index() const noexcept { return _position; }

  [[nodiscard]] ValueLength size() const noexcept { return _size; }

  [[deprecated(
      "Dangerous API, you don't really know valid or not. "
      "You can use index() and size().")]] bool
  isFirst() const noexcept {
    return _position == 0;
  }

  [[deprecated(
      "Dangerous API, you don't really know valid or not. "
      "You can use index() and size().")]] bool
  isLast() const noexcept {
    return _position + 1 >= _size;
  }

  void forward(ValueLength count) noexcept {
    _position += count;
    if (VELOCYPACK_UNLIKELY(_position >= _size)) {
      _position = _size;
      return;
    }
    auto const head = _slice.head();
    if (head == 0x13) {
      while (count-- != 0) {
        _current += Slice(_current).byteSize();
      }
    } else {
      _current = _slice.at(_position).start();
    }
  }

  void reset() noexcept {
    _position = 0;
    _current = first();
  }

  template<typename... Ts>
  [[nodiscard]] std::tuple<Ts...> unpackPrefixAsTuple() {
    return unpackTupleInternal(unpack_helper<Ts...>{});
  }

 private:
  template<typename...>
  struct unpack_helper {};

  template<typename T, typename... Ts>
  [[nodiscard]] std::tuple<T, Ts...> unpackTupleInternal(
      unpack_helper<T, Ts...>) {
    auto slice = value();  // this does out-of-bounds checking
    next();
    return std::tuple_cat(std::make_tuple(slice.extract<T>()),
                          unpackTupleInternal(unpack_helper<Ts...>{}));
  }

  [[nodiscard]] static std::tuple<> unpackTupleInternal(unpack_helper<>) {
    return std::make_tuple<>();
  }

  [[nodiscard]] uint8_t const* first() const noexcept {
    if (VELOCYPACK_UNLIKELY(_size == 0)) {
      return nullptr;
    }
    auto const head = _slice.head();
    VELOCYPACK_ASSERT(head != 0x01);  // no empty array allowed here
    if (head == 0x13) {
      return _slice.start() + _slice.getStartOffsetFromCompact();
    }
    return _slice.begin() + _slice.findDataOffset(head);
  }

  Slice _slice;
  uint8_t const* _current;
  ValueLength _size;
  ValueLength _position;
};

struct ObjectIteratorPair {
  Slice const key;
  Slice const value;
};

class ObjectIterator {
 public:
  using ObjectPair = ObjectIteratorPair;

  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = ObjectIteratorPair;
  using pointer = ObjectIteratorPair*;
  using reference = ObjectIteratorPair&;

  ObjectIterator() = delete;

  // The useSequentialIteration flag indicates whether or not the iteration
  // simply jumps from key/value pair to key/value pair without using the
  // index. The default `false` is to use the index if it is there.
  explicit ObjectIterator(Slice slice, bool useSequentialIteration = false)
      : _slice{slice}, _current{nullptr}, _size{0}, _position{0} {
    auto const head = slice.head();
    if (VELOCYPACK_UNLIKELY(slice.type(head) != ValueType::Object)) {
      throw Exception{Exception::InvalidValueType, "Expecting Object slice"};
    }
    _size = slice.objectLength();
    _current = first(useSequentialIteration);
  }

  ObjectIterator& operator++() noexcept {
    next();
    return *this;
  }

  [[nodiscard]] ObjectIterator operator++(int) noexcept {
    ObjectIterator prev{*this};
    next();
    return prev;
  }

  [[nodiscard]] bool operator==(ObjectIterator const& other) const noexcept {
    return _position == other._position;
  }

  [[nodiscard]] ObjectPair operator*() const {
    if (_current != nullptr) {
      Slice key{_current};
      return {key.makeKey(), Slice(_current + key.byteSize())};
    }
    // intentionally no out-of-bounds checking here,
    // as it will be performed by Slice::getNthOffset()
    Slice key{_slice.getNthKeyUntranslated(_position)};
    return {key.makeKey(), Slice(key.begin() + key.byteSize())};
  }

  [[nodiscard]] ObjectIterator begin() const noexcept { return {*this}; }

#if (defined(_MSC_VER) ||                                     \
     (defined(__clang_version__) && __clang_major__ >= 12) || \
     (!defined(__clang__) && defined(__GNUC__) && __GNUC__ >= 10))
  [[nodiscard]] bool operator==(std::default_sentinel_t) const noexcept {
    return !valid();
  }
  [[nodiscard]] std::default_sentinel_t end() const noexcept { return {}; }
#else
  [[nodiscard]] ObjectIterator end() const noexcept {
    ObjectIterator it{*this};
    it._position = it._size;
    return it;
  }
  [[nodiscard]] bool operator!=(ObjectIterator const& other) const noexcept {
    return !operator==(other);
  }
#endif

  [[nodiscard]] bool valid() const noexcept { return _position != _size; }

  [[nodiscard]] Slice key(bool translate = true) const {
    if (VELOCYPACK_UNLIKELY(!valid())) {
      throw Exception{Exception::IndexOutOfBounds};
    }
    if (_current != nullptr) {
      Slice s{_current};
      return translate ? s.makeKey() : s;
    }
    return _slice.getNthKey(_position, translate);
  }

  [[nodiscard]] Slice value() const {
    if (VELOCYPACK_UNLIKELY(!valid())) {
      throw Exception{Exception::IndexOutOfBounds};
    }
    if (_current != nullptr) {
      Slice key(_current);
      return Slice(_current + key.byteSize());
    }
    return _slice.getNthValue(_position);
  }

  void next() noexcept {
    if (VELOCYPACK_UNLIKELY(!valid())) {
      return;
    }
    ++_position;
    if (_current != nullptr) {
      // skip over key
      _current += Slice(_current).byteSize();
      // skip over value
      _current += Slice(_current).byteSize();
    }
  }

  [[nodiscard]] ValueLength index() const noexcept { return _position; }

  [[nodiscard]] ValueLength size() const noexcept { return _size; }

  [[deprecated(
      "Dangerous API, you don't really know valid or not. "
      "You can use index() and size().")]] [[nodiscard]] bool
  isFirst() const noexcept {
    return _position == 0;
  }

  [[deprecated(
      "Dangerous API, you don't really know valid or not. "
      "You can use index() and size().")]] [[nodiscard]] bool
  isLast() const noexcept {
    return _position + 1 >= _size;
  }

  void reset() noexcept {
    _position = 0;
    _current = first(_current != nullptr);
  }

 private:
  [[nodiscard]] uint8_t const* first(bool useSequentialIteration) const noexcept {
    if (VELOCYPACK_UNLIKELY(_size == 0)) {
      return nullptr;
    }
    auto const head = _slice.head();
    VELOCYPACK_ASSERT(head != 0x0a);  // no empty object allowed here
    if (head == 0x14) {
      return _slice.start() + _slice.getStartOffsetFromCompact();
    } else if (useSequentialIteration) {
      return _slice.start() + _slice.findDataOffset(head);
    }
    return nullptr;
  }

  Slice _slice;
  uint8_t const* _current;
  ValueLength _size;
  ValueLength _position;
};

}  // namespace arangodb::velocypack

std::ostream& operator<<(std::ostream&,
                         arangodb::velocypack::ArrayIterator const*);

std::ostream& operator<<(std::ostream&,
                         arangodb::velocypack::ArrayIterator const&);

std::ostream& operator<<(std::ostream&,
                         arangodb::velocypack::ObjectIterator const*);

std::ostream& operator<<(std::ostream&,
                         arangodb::velocypack::ObjectIterator const&);

using VPackArrayIterator = arangodb::velocypack::ArrayIterator;
using VPackObjectIterator = arangodb::velocypack::ObjectIterator;
