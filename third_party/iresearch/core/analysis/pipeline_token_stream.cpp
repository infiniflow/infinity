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
/// @author Andrei Lobov
////////////////////////////////////////////////////////////////////////////////

#include "pipeline_token_stream.hpp"

#include <string_view>

#include "utils/rst/strings/str_cat.h"

namespace {

const irs::offset NO_OFFSET;
using options_normalize_t = std::vector<std::pair<std::string, std::string>>;

irs::payload* find_payload(
  std::span<const irs::analysis::analyzer::ptr> pipeline) {
  for (auto it = pipeline.rbegin(); it != pipeline.rend(); ++it) {
    auto payload = irs::get_mutable<irs::payload>(it->get());
    if (payload) {
      return payload;
    }
  }
  return nullptr;
}

bool all_have_offset(std::span<const irs::analysis::analyzer::ptr> pipeline) {
  return std::all_of(pipeline.begin(), pipeline.end(),
                     [](const irs::analysis::analyzer::ptr& v) {
                       return nullptr != irs::get<irs::offset>(*v);
                     });
}

}  // namespace

namespace irs {
namespace analysis {

pipeline_token_stream::pipeline_token_stream(
  pipeline_token_stream::options_t&& options)
  : attrs_{{},
           options.empty()
             ? nullptr
             : irs::get_mutable<term_attribute>(options.back().get()),
           all_have_offset(options) ? &offs_ : attribute_ptr<offset>{},
           find_payload(options)} {
  const auto track_offset = irs::get<offset>(*this) != nullptr;
  pipeline_.reserve(options.size());
  for (auto& p : options) {
    IRS_ASSERT(p);
    pipeline_.emplace_back(std::move(p), track_offset);
  }
  options.clear();  // mimic move semantic
  if (pipeline_.empty()) {
    pipeline_.emplace_back();
  }
  top_ = pipeline_.begin();
  bottom_ = --pipeline_.end();
}

/// Moves pipeline to next token.
/// Term is taken from last analyzer in pipeline
/// Offset is recalculated accordingly (only if ALL analyzers in pipeline )
/// Payload is taken from lowest analyzer having this attribute
/// Increment is calculated according to following position change rules
///  - If none of pipeline members changes position - whole pipeline holds
///  position
///  - If one or more pipeline member moves - pipeline moves( change from max->0
///  is not move, see rules below!).
///    All position gaps are accumulated (e.g. if one member has inc 2(1 pos
///    gap) and other has inc 3(2 pos gap)  - pipeline has inc 4 (1+2 pos gap))
///  - All position changes caused by parent analyzer move next (e.g. transfer
///  from max->0 by first next after reset) are collapsed.
///    e.g if parent moves after next, all its children are resetted to new
///    token and also moves step froward - this whole operation is just one step
///    for pipeline (if any of children has moved more than 1 step - gaps are
///    preserved!)
///  - If parent after next is NOT moved (inc == 0) than pipeline makes one step
///  forward if at least one child changes
///    position from any positive value back to 0 due to reset (additional gaps
///    also preserved!) as this is not change max->0 and position is indeed
///    changed.
bool pipeline_token_stream::next() {
  uint32_t pipeline_inc;
  bool step_for_rollback{false};
  do {
    while (!current_->next()) {
      if (current_ ==
          top_) {  // reached pipeline top and next has failed - we are done
        return false;
      }
      --current_;
    }
    pipeline_inc = current_->inc->value;
    const auto top_holds_position = current_->inc->value == 0;
    // go down to lowest pipe to get actual tokens
    while (current_ != bottom_) {
      const auto prev_term = current_->term->value;
      const auto prev_start = current_->start();
      const auto prev_end = current_->end();
      ++current_;
      // check do we need to do step forward due to rollback to 0.
      step_for_rollback |=
        top_holds_position && current_->pos != 0 &&
        current_->pos != std::numeric_limits<uint32_t>::max();
      if (!current_->reset(prev_start, prev_end,
                           irs::ViewCast<char>(prev_term))) {
        return false;
      }
      if (!current_->next()) {  // empty one found. Another round from the very
                                // beginning.
        IRS_ASSERT(current_ != top_);
        --current_;
        break;
      }
      pipeline_inc += current_->inc->value;
      IRS_ASSERT(current_->inc->value >
                 0);  // first increment after reset should
                      // be positive to give 0 or next pos!
      IRS_ASSERT(pipeline_inc > 0);
      pipeline_inc--;  // compensate placing sub_analyzer from max to 0 due to
                       // reset as this step actually does not move whole
                       // pipeline sub analyzer just stays same pos as it`s
                       // parent (step for rollback to 0 will be done below if
                       // necessary!)
    }
  } while (current_ != bottom_);
  if (step_for_rollback) {
    pipeline_inc++;
  }
  std::get<increment>(attrs_).value = pipeline_inc;
  offs_.start = current_->start();
  offs_.end = current_->end();
  return true;
}

bool pipeline_token_stream::reset(std::string_view data) {
  current_ = top_;
  return pipeline_.front().reset(0, static_cast<uint32_t>(data.size()), data);
}

void pipeline_token_stream::init() {}

pipeline_token_stream::sub_analyzer_t::sub_analyzer_t(
  irs::analysis::analyzer::ptr a, bool track_offset)
  : term(irs::get<irs::term_attribute>(*a)),
    inc(irs::get<irs::increment>(*a)),
    offs(track_offset ? irs::get<irs::offset>(*a) : &NO_OFFSET),
    analyzer(std::move(a)) {
  IRS_ASSERT(inc);
  IRS_ASSERT(term);
}

pipeline_token_stream::sub_analyzer_t::sub_analyzer_t()
  : term(nullptr),
    inc(nullptr),
    offs(nullptr),
    analyzer(std::make_unique<empty_analyzer>()) {}

}  // namespace analysis
}  // namespace irs
