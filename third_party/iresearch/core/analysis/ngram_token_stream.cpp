////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2019 ArangoDB GmbH, Cologne, Germany
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
/// @author Andrey Abramov
////////////////////////////////////////////////////////////////////////////////

#include "ngram_token_stream.hpp"

#include <frozen/string.h>
#include <frozen/unordered_map.h>

#include <string_view>

#include "utils/hash_utils.hpp"
#include "utils/rst/strings/str_cat.h"
#include "utils/utf8_utils.hpp"

namespace irs {
namespace analysis {

template<irs::analysis::ngram_token_stream_base::InputType StreamType>
analyzer::ptr ngram_token_stream<StreamType>::make(
  const ngram_token_stream_base::Options& options) {
  return std::make_unique<ngram_token_stream<StreamType>>(options);
}

void ngram_token_stream_base::init() {}

ngram_token_stream_base::ngram_token_stream_base(
  const ngram_token_stream_base::Options& options)
  : options_(options),
    start_marker_empty_(options.start_marker.empty()),
    end_marker_empty_(options.end_marker.empty()) {
  options_.min_gram = std::max(options_.min_gram, size_t(1));
  options_.max_gram = std::max(options_.max_gram, options_.min_gram);
}

template<irs::analysis::ngram_token_stream_base::InputType StreamType>
ngram_token_stream<StreamType>::ngram_token_stream(
  const ngram_token_stream_base::Options& options)
  : ngram_token_stream_base(options) {
  IRS_ASSERT(StreamType == options_.stream_bytes_type);
}

void ngram_token_stream_base::emit_original() noexcept {
  auto& term = std::get<term_attribute>(attrs_);
  auto& offset = std::get<irs::offset>(attrs_);
  auto& inc = std::get<increment>(attrs_);

  switch (emit_original_) {
    case EmitOriginal::WithoutMarkers:
      term.value = data_;
      IRS_ASSERT(data_.size() <= std::numeric_limits<uint32_t>::max());
      offset.end = uint32_t(data_.size());
      emit_original_ = EmitOriginal::None;
      inc.value = next_inc_val_;
      break;
    case EmitOriginal::WithEndMarker:
      marked_term_buffer_.clear();
      IRS_ASSERT(marked_term_buffer_.capacity() >=
                 (options_.end_marker.size() + data_.size()));
      marked_term_buffer_.append(data_.data(), data_end_);
      marked_term_buffer_.append(options_.end_marker.begin(),
                                 options_.end_marker.end());
      term.value = marked_term_buffer_;
      IRS_ASSERT(marked_term_buffer_.size() <=
                 std::numeric_limits<uint32_t>::max());
      offset.start = 0;
      offset.end = uint32_t(data_.size());
      emit_original_ = EmitOriginal::None;  // end marker is emitted last, so we
                                            // are done emitting original
      inc.value = next_inc_val_;
      break;
    case EmitOriginal::WithStartMarker:
      marked_term_buffer_.clear();
      IRS_ASSERT(marked_term_buffer_.capacity() >=
                 (options_.start_marker.size() + data_.size()));
      marked_term_buffer_.append(options_.start_marker.begin(),
                                 options_.start_marker.end());
      marked_term_buffer_.append(data_.data(), data_end_);
      term.value = marked_term_buffer_;
      IRS_ASSERT(marked_term_buffer_.size() <=
                 std::numeric_limits<uint32_t>::max());
      offset.start = 0;
      offset.end = uint32_t(data_.size());
      emit_original_ = options_.end_marker.empty()
                         ? EmitOriginal::None
                         : EmitOriginal::WithEndMarker;
      inc.value = next_inc_val_;
      break;
    default:
      IRS_ASSERT(false);  // should not be called when None
      break;
  }
  next_inc_val_ = 0;
}

bool ngram_token_stream_base::reset(std::string_view value) noexcept {
  if (value.size() > std::numeric_limits<uint32_t>::max()) {
    // can't handle data which is longer than
    // std::numeric_limits<uint32_t>::max()
    return false;
  }

  auto& term = std::get<term_attribute>(attrs_);
  auto& offset = std::get<irs::offset>(attrs_);

  // reset term attribute
  term.value = {};

  // reset offset attribute
  offset.start = std::numeric_limits<uint32_t>::max();
  offset.end = std::numeric_limits<uint32_t>::max();

  // reset stream
  data_ = ViewCast<byte_type>(value);
  begin_ = data_.data();
  ngram_end_ = begin_;
  data_end_ = data_.data() + data_.size();
  offset.start = 0;
  length_ = 0;
  if (options_.preserve_original) {
    if (!start_marker_empty_) {
      emit_original_ = EmitOriginal::WithStartMarker;
    } else if (!end_marker_empty_) {
      emit_original_ = EmitOriginal::WithEndMarker;
    } else {
      emit_original_ = EmitOriginal::WithoutMarkers;
    }
  } else {
    emit_original_ = EmitOriginal::None;
  }
  next_inc_val_ = 1;
  IRS_ASSERT(length_ < options_.min_gram);
  const size_t max_marker_size =
    std::max(options_.start_marker.size(), options_.end_marker.size());
  if (max_marker_size > 0) {
    // we have at least one marker. As we need to append marker to ngram and
    // provide term value as continious buffer, we can`t return pointer to some
    // byte inside input stream but rather we return pointer to buffer with
    // copied values of ngram and marker For sake of performance we allocate
    // requested memory right now
    size_t buffer_size = options_.preserve_original
                           ? data_.size()
                           : std::min(data_.size(), options_.max_gram);
    buffer_size += max_marker_size;
    if (buffer_size >
        marked_term_buffer_
          .capacity()) {  // until c++20 this check is needed to avoid shrinking
      marked_term_buffer_.reserve(buffer_size);
    }
  }
  return true;
}

template<irs::analysis::ngram_token_stream_base::InputType StreamType>
bool ngram_token_stream<StreamType>::next_symbol(
  const byte_type*& it) const noexcept {
  IRS_ASSERT(it);
  if (it < data_end_) {
    if constexpr (StreamType == InputType::Binary) {
      ++it;
    } else if constexpr (StreamType == InputType::UTF8) {
      it = irs::utf8_utils::next(it, data_end_);
    }
    return true;
  }
  return false;
}

template<irs::analysis::ngram_token_stream_base::InputType StreamType>
bool ngram_token_stream<StreamType>::next() noexcept {
  auto& term = std::get<term_attribute>(attrs_);
  auto& offset = std::get<irs::offset>(attrs_);
  auto& inc = std::get<increment>(attrs_);

  while (begin_ < data_end_) {
    if (length_ < options_.max_gram && next_symbol(ngram_end_)) {
      // we have next ngram from current position
      ++length_;
      if (length_ >= options_.min_gram) {
        IRS_ASSERT(begin_ <= ngram_end_);
        IRS_ASSERT(static_cast<size_t>(std::distance(begin_, ngram_end_)) <=
                   std::numeric_limits<uint32_t>::max());
        const auto ngram_byte_len =
          static_cast<uint32_t>(std::distance(begin_, ngram_end_));
        if (EmitOriginal::None == emit_original_ || 0 != offset.start ||
            ngram_byte_len != data_.size()) {
          offset.end = offset.start + ngram_byte_len;
          inc.value = next_inc_val_;
          next_inc_val_ = 0;
          if ((0 != offset.start || start_marker_empty_) &&
              (end_marker_empty_ || ngram_end_ != data_end_)) {
            term.value = irs::bytes_view(begin_, ngram_byte_len);
          } else if (0 == offset.start && !start_marker_empty_) {
            marked_term_buffer_.clear();
            IRS_ASSERT(marked_term_buffer_.capacity() >=
                       (options_.start_marker.size() + ngram_byte_len));
            marked_term_buffer_.append(options_.start_marker.begin(),
                                       options_.start_marker.end());
            marked_term_buffer_.append(begin_, ngram_byte_len);
            term.value = marked_term_buffer_;
            IRS_ASSERT(marked_term_buffer_.size() <=
                       std::numeric_limits<uint32_t>::max());
            if (ngram_byte_len == data_.size() && !end_marker_empty_) {
              // this term is whole original stream and we have end marker, so
              // we need to emit this term again with end marker just like
              // original, so pretend we need to emit original
              emit_original_ = EmitOriginal::WithEndMarker;
            }
          } else {
            IRS_ASSERT(!end_marker_empty_ && ngram_end_ == data_end_);
            marked_term_buffer_.clear();
            IRS_ASSERT(marked_term_buffer_.capacity() >=
                       (options_.end_marker.size() + ngram_byte_len));
            marked_term_buffer_.append(begin_, ngram_byte_len);
            marked_term_buffer_.append(options_.end_marker.begin(),
                                       options_.end_marker.end());
            term.value = marked_term_buffer_;
          }
        } else {
          // if ngram covers original stream we need to process it specially
          emit_original();
        }
        return true;
      }
    } else {
      // need to move to next position
      if (EmitOriginal::None == emit_original_) {
        if (next_symbol(begin_)) {
          next_inc_val_ = 1;
          length_ = 0;
          ngram_end_ = begin_;
          offset.start =
            static_cast<uint32_t>(std::distance(data_.data(), begin_));
        } else {
          return false;  // stream exhausted
        }
      } else {
        // as stream has unsigned incremet attribute
        // we cannot go back, so we must emit original before we leave start pos
        // in stream (as it starts from pos=0 in stream)
        emit_original();
        return true;
      }
    }
  }
  return false;
}

}  // namespace analysis
}  // namespace irs

// Making library export see template instantinations
template class irs::analysis::ngram_token_stream<
  irs::analysis::ngram_token_stream_base::InputType::Binary>;
template class irs::analysis::ngram_token_stream<
  irs::analysis::ngram_token_stream_base::InputType::UTF8>;
