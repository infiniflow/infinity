#ifndef BASE64_HPP_
#define BASE64_HPP_

#include <cstdint>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <string_view>

namespace base64 {

inline constexpr std::string_view base64_chars{
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/"};

template <class OutputBuffer, class InputIterator>
inline OutputBuffer encode_into(InputIterator begin, InputIterator end) {
  static_assert(std::is_same_v<std::decay_t<decltype(*begin)>, char> ||
                std::is_same_v<std::decay_t<decltype(*begin)>, unsigned char> ||
                std::is_same_v<std::decay_t<decltype(*begin)>, std::byte>);

  size_t counter = 0;
  uint32_t bit_stream = 0;
  size_t offset = 0;
  OutputBuffer encoded;
  encoded.reserve(static_cast<size_t>(
      1.5 * static_cast<double>(std::distance(begin, end))));
  while (begin != end) {
    auto const num_val = static_cast<unsigned char>(*begin);
    offset = 16 - counter % 3 * 8;
    bit_stream += static_cast<uint32_t>(num_val << offset);
    if (offset == 16) {
      encoded.push_back(base64_chars[bit_stream >> 18 & 0x3f]);
    }
    if (offset == 8) {
      encoded.push_back(base64_chars[bit_stream >> 12 & 0x3f]);
    }
    if (offset == 0 && counter != 3) {
      encoded.push_back(base64_chars[bit_stream >> 6 & 0x3f]);
      encoded.push_back(base64_chars[bit_stream & 0x3f]);
      bit_stream = 0;
    }
    ++counter;
    ++begin;
  }
  if (offset == 16) {
    encoded.push_back(base64_chars[bit_stream >> 12 & 0x3f]);
    encoded.push_back('=');
    encoded.push_back('=');
  }
  if (offset == 8) {
    encoded.push_back(base64_chars[bit_stream >> 6 & 0x3f]);
    encoded.push_back('=');
  }
  return encoded;
}

inline std::string to_base64(std::string_view data) {
  return encode_into<std::string>(std::begin(data), std::end(data));
}

template <class OutputBuffer>
inline OutputBuffer decode_into(std::string_view data) {
  using value_type = typename OutputBuffer::value_type;
  static_assert(std::is_same_v<value_type, char> ||
                std::is_same_v<value_type, unsigned char> ||
                std::is_same_v<value_type, std::byte>);

  size_t counter = 0;
  uint32_t bit_stream = 0;
  OutputBuffer decoded;
  const size_t encoded_size = std::size(data);
  if ((encoded_size % 4) != 0) {
    throw std::runtime_error{
        "Invalid base64 encoded data - Size not divisible by 4"};
  }
  const size_t numlasteqs = std::count(data.rbegin(), data.rbegin() + 4, '=');
  if (numlasteqs > 2) {
    throw std::runtime_error{
        "Invalid base64 encoded data - Found more than 2 padding signs"};
  }
  decoded.reserve(encoded_size);
  for (std::string_view::value_type c : data) {
    auto const num_val = base64_chars.find(c);
    if (num_val != std::string::npos) {
      auto const offset = 18 - counter % 4 * 6;
      bit_stream += static_cast<uint32_t>(num_val) << offset;
      if (offset == 12) {
        decoded.push_back(static_cast<value_type>(bit_stream >> 16 & 0xff));
      }
      if (offset == 6) {
        decoded.push_back(static_cast<value_type>(bit_stream >> 8 & 0xff));
      }
      if (offset == 0 && counter != 4) {
        decoded.push_back(static_cast<value_type>(bit_stream & 0xff));
        bit_stream = 0;
      }
    } else if (c != '=') {
      throw std::runtime_error{
          "Invalid base64 encoded data - Found invalid character"};
    }
    counter++;
  }
  return decoded;
}

inline std::string from_base64(std::string_view data) {
  return decode_into<std::string>(data);
}

}  // namespace base64

#endif  // BASE64_HPP_
