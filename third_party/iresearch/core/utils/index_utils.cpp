////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2016 by EMC Corporation, All Rights Reserved
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
/// Copyright holder is EMC Corporation
///
/// @author Andrey Abramov
/// @author Vasiliy Nabatchikov
////////////////////////////////////////////////////////////////////////////////

#include "index_utils.hpp"

#include <cmath>
#include <set>

#include "formats/format_utils.hpp"

namespace {

// Returns percentage of live documents
inline double_t FillFactor(const irs::SegmentInfo& segment) noexcept {
  return double(segment.live_docs_count) / segment.docs_count;
}

// Returns approximated size of a segment in the absence of removals
inline size_t SizeWithoutRemovals(const irs::SegmentInfo& segment) noexcept {
  return size_t(segment.byte_size * FillFactor(segment));
}

namespace tier {

struct SegmentStats {
  // cppcheck-suppress noExplicitConstructor
  SegmentStats(const irs::SubReader& reader) noexcept
    : reader{&reader},
      meta{&reader.Meta()},
      size{SizeWithoutRemovals(*meta)},
      fill_factor{FillFactor(*meta)} {}

  bool operator<(const SegmentStats& rhs) const noexcept {
    // cppcheck-suppress constVariable
    auto& lhs = *this;

    if (lhs.size == rhs.size) {
      if (lhs.fill_factor > rhs.fill_factor) {
        return true;
      } else if (lhs.fill_factor < rhs.fill_factor) {
        return false;
      }

      return lhs.meta->name < rhs.meta->name;
    }

    return lhs.size < rhs.size;
  }

  operator const irs::SubReader*() const noexcept { return reader; }

  const irs::SubReader* reader;
  const irs::SegmentInfo* meta;
  size_t size;  // approximate size of segment without removals
  double_t fill_factor;
};

struct ConsolidationCandidate {
  using iterator_t = std::vector<SegmentStats>::const_iterator;
  using range_t = std::pair<iterator_t, iterator_t>;

  explicit ConsolidationCandidate(iterator_t i) noexcept : segments(i, i) {}

  iterator_t begin() const noexcept { return segments.first; }
  iterator_t end() const noexcept { return segments.second; }

  const SegmentStats& front() const noexcept {
    IRS_ASSERT(segments.first != segments.second);
    return *segments.first;
  }

  const SegmentStats& back() const noexcept {
    IRS_ASSERT(segments.first != segments.second);
    auto curr_end = segments.second;
    return *(--curr_end);
  }

  void reset() noexcept {
    segments = range_t();
    count = 0;
    size = 0;
    score = -1.;
  }

  range_t segments;
  size_t count{0};
  size_t size{0};           // estimated size of the level
  double_t score{DBL_MIN};  // how good this permutation is
};

/// @returns score of the consolidation bucket
double_t consolidation_score(const ConsolidationCandidate& consolidation,
                             const size_t segments_per_tier,
                             const size_t floor_segment_bytes) noexcept {
  // to detect how skewed the consolidation we do the following:
  // 1. evaluate coefficient of variation, less is better
  // 2. good candidates are in range [0;1]
  // 3. favor condidates where number of segments is equal to
  // 'segments_per_tier' approx
  // 4. prefer smaller consolidations
  // 5. prefer consolidations which clean removals

  switch (consolidation.count) {
    case 0:
      // empty consolidation makes not sense
      return DBL_MIN;
    case 1: {
      auto& meta = *consolidation.segments.first->meta;

      if (meta.docs_count == meta.live_docs_count) {
        // singletone without removals makes no sense
        return DBL_MIN;
      }

      // FIXME honor number of deletes???
      // signletone with removals makes sense if nothing better is found
      return DBL_MIN + DBL_EPSILON;
    }
  }

  size_t size_before_consolidation = 0;
  size_t size_after_consolidation = 0;
  size_t size_after_consolidation_floored = 0;
  for (auto& segment_stat : consolidation) {
    size_before_consolidation += segment_stat.meta->byte_size;
    size_after_consolidation += segment_stat.size;
    size_after_consolidation_floored +=
      std::max(segment_stat.size, floor_segment_bytes);
  }

  // evaluate coefficient of variation
  double_t sum_square_differences = 0;
  const auto segment_size_after_consolidaton_mean =
    double_t(size_after_consolidation_floored) / consolidation.count;
  for (auto& segment_stat : consolidation) {
    const double_t diff = std::max(segment_stat.size, floor_segment_bytes) -
                          segment_size_after_consolidaton_mean;
    sum_square_differences += diff * diff;
  }

  const auto stdev = std::sqrt(sum_square_differences / consolidation.count);
  const auto cv = (stdev / segment_size_after_consolidaton_mean);

  // evaluate initial score
  auto score = 1. - cv;

  // favor consolidations that contain approximately the requested number of
  // segments
  score *= std::pow(consolidation.count / double_t(segments_per_tier), 1.5);

  // FIXME use relative measure, e.g. cosolidation_size/total_size
  // carefully prefer smaller consolidations over the bigger ones
  score /= std::pow(size_after_consolidation, 0.5);

  // favor consolidations which clean out removals
  score /=
    std::pow(double_t(size_after_consolidation) / size_before_consolidation, 2);

  return score;
}

}  // namespace tier
}  // namespace

namespace irs::index_utils {

ConsolidationPolicy MakePolicy(const ConsolidateBytes& options) {
  return [options](Consolidation& candidates, const IndexReader& reader,
                   const ConsolidatingSegments& consolidating_segments) {
    const auto byte_threshold = options.threshold;
    size_t all_segment_bytes_size = 0;
    const auto segment_count = reader.size();

    for (auto& segment : reader) {
      all_segment_bytes_size += segment.Meta().byte_size;
    }

    const auto threshold = std::clamp(byte_threshold, 0.f, 1.f);
    const auto threshold_bytes_avg =
      (static_cast<float>(all_segment_bytes_size) / segment_count) * threshold;

    // merge segment if: {threshold} > segment_bytes / (all_segment_bytes /
    // #segments)
    for (auto& segment : reader) {
      if (consolidating_segments.contains(segment.Meta().name)) {
        continue;
      }
      const auto segment_bytes_size = segment.Meta().byte_size;
      if (threshold_bytes_avg >= segment_bytes_size) {
        candidates.emplace_back(&segment);
      }
    }
  };
}

ConsolidationPolicy MakePolicy(const ConsolidateBytesAccum& options) {
  return [options](Consolidation& candidates, const IndexReader& reader,
                   const ConsolidatingSegments& consolidating_segments) {
    auto byte_threshold = options.threshold;
    size_t all_segment_bytes_size = 0;
    std::vector<std::pair<size_t, const SubReader*>> segments;
    segments.reserve(reader.size());

    for (auto& segment : reader) {
      if (consolidating_segments.contains(segment.Meta().name)) {
        continue;  // segment is already under consolidation
      }
      segments.emplace_back(SizeWithoutRemovals(segment.Meta()), &segment);
      all_segment_bytes_size += segments.back().first;
    }

    size_t cumulative_size = 0;
    const auto threshold_size =
      all_segment_bytes_size * std::clamp(byte_threshold, 0.f, 1.f);
    struct {
      bool operator()(const std::pair<size_t, const SubReader*>& lhs,
                      const std::pair<size_t, const SubReader*>& rhs) const {
        return lhs.first < rhs.first;
      }
    } segments_less;

    // prefer to consolidate smaller segments
    std::sort(segments.begin(), segments.end(), segments_less);

    // merge segment if: {threshold} >= (segment_bytes +
    // sum_of_merge_candidate_segment_bytes) / all_segment_bytes
    for (auto& entry : segments) {
      const auto segment_bytes_size = entry.first;

      if (cumulative_size + segment_bytes_size <= threshold_size) {
        cumulative_size += segment_bytes_size;
        candidates.emplace_back(entry.second);
      }
    }
  };
}

ConsolidationPolicy MakePolicy(const ConsolidateCount& options) {
  return [options](Consolidation& candidates, const IndexReader& reader,
                   const ConsolidatingSegments&) {
    // merge first 'threshold' segments
    for (size_t i = 0, count = std::min(options.threshold, reader.size());
         i < count; ++i) {
      candidates.emplace_back(&reader[i]);
    }
  };
}

ConsolidationPolicy MakePolicy(const ConsolidateDocsFill& options) {
  return [options](Consolidation& candidates, const IndexReader& reader,
                   const ConsolidatingSegments& consolidating_segments) {
    auto fill_threshold = options.threshold;
    auto threshold = std::clamp(fill_threshold, 0.f, 1.f);

    // merge segment if: {threshold} >= #segment_docs{valid} /
    // (#segment_docs{valid} + #segment_docs{removed})
    for (auto& segment : reader) {
      auto& meta = segment.Meta();
      if (consolidating_segments.contains(meta.name)) {
        continue;
      }
      if (!meta.live_docs_count  // if no valid doc_ids left in segment
          || meta.docs_count * threshold >= meta.live_docs_count) {
        candidates.emplace_back(&segment);
      }
    }
  };
}

ConsolidationPolicy MakePolicy(const ConsolidateDocsLive& options) {
  return [options](Consolidation& candidates, const IndexReader& meta,
                   const ConsolidatingSegments& consolidating_segments) {
    const auto docs_threshold = options.threshold;
    const auto all_segment_docs_count = meta.live_docs_count();
    const auto segment_count = meta.size();

    const auto threshold = std::clamp(docs_threshold, 0.f, 1.f);
    const auto threshold_docs_avg =
      (static_cast<float>(all_segment_docs_count) / segment_count) * threshold;

    // merge segment if: {threshold} >= segment_docs{valid} /
    // (all_segment_docs{valid} / #segments)
    for (auto& segment : meta) {
      auto& info = segment.Meta();
      if (consolidating_segments.contains(info.name)) {
        continue;
      }
      if (!info.live_docs_count  // if no valid doc_ids left in segment
          || threshold_docs_avg >= info.live_docs_count) {
        candidates.emplace_back(&segment);
      }
    }
  };
}

ConsolidationPolicy MakePolicy(const ConsolidateTier& options) {
  // can't merge less than 1 segment
  const auto max_segments_per_tier =
    (std::max)(size_t{1}, options.max_segments);
  // can't merge less than 1 segment
  auto min_segments_per_tier = (std::max)(size_t{1}, options.min_segments);

  // ensure min_segments_per_tier <=  max_segments_per_tier
  min_segments_per_tier =
    (std::min)(min_segments_per_tier, max_segments_per_tier);
  const auto max_segments_bytes =
    (std::max)(size_t{1}, options.max_segments_bytes);
  const auto floor_segment_bytes =
    (std::max)(size_t{1}, options.floor_segment_bytes);
  // skip consolidation that have score less than min_score
  const auto min_score = options.min_score;

  return [max_segments_per_tier, min_segments_per_tier, floor_segment_bytes,
          max_segments_bytes,
          min_score](Consolidation& candidates, const IndexReader& reader,
                     const ConsolidatingSegments& consolidating_segments) {
    // size of segments in bytes that are currently under consolidation
    [[maybe_unused]] size_t consolidating_size = 0;
    // the smallest segment
    size_t min_segment_size = std::numeric_limits<size_t>::max();
    // total size in bytes of all segments in index
    [[maybe_unused]] size_t total_index_size = 0;
    size_t total_docs_count = 0;  // total number of documents in index
    // total number of live documents in index
    size_t total_live_docs_count = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// Stage 0
    /// get sorted list of segments
    ///////////////////////////////////////////////////////////////////////////

    std::vector<tier::SegmentStats> sorted_segments;
    sorted_segments.reserve(reader.size());

    // get segments from index meta
    for (auto& segment : reader) {
      if (segment.live_docs_count()) {
        // skip empty segments, they'll be
        // removed from index by index_writer
        // during 'commit'
        sorted_segments.emplace_back(segment);
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Stage 1
    /// calculate overall stats
    ///////////////////////////////////////////////////////////////////////////

    auto segments_end = sorted_segments.data() + sorted_segments.size();
    for (auto begin = sorted_segments.data(); begin < segments_end;) {
      auto& segment = *begin;

      min_segment_size = std::min(min_segment_size, segment.size);
      total_index_size += segment.size;
      total_live_docs_count += segment.meta->live_docs_count;

      if (consolidating_segments.contains(segment.reader->Meta().name)) {
        consolidating_size += segment.size;
        // exclude removals from stats for consolidating segments
        total_docs_count += segment.meta->live_docs_count;

        // segment is already marked for consolidation, filter it out
        irstd::swap_remove(sorted_segments, begin);
        --segments_end;
      } else {
        total_docs_count += segment.meta->docs_count;
        ++begin;
      }
    }

    if (!total_docs_count) {
      // nothing to consolidate
      return;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Stage 2
    /// filter out "too large segments", segment is meant to be treated as large
    /// if
    /// - segment size is greater than 'max_segments_bytes / 2'
    /// - segment has many documents but only few deletions
    ///////////////////////////////////////////////////////////////////////////

    const double_t total_fill_factor =
      static_cast<double_t>(total_live_docs_count) / total_docs_count;
    const size_t too_big_segments_threshold = max_segments_bytes / 2;
    segments_end = sorted_segments.data() + sorted_segments.size();
    for (auto begin = sorted_segments.data(); begin < segments_end;) {
      auto& segment = *begin;
      const double_t segment_fill_factor =
        static_cast<double_t>(segment.meta->live_docs_count) /
        segment.meta->docs_count;
      if (segment.size > too_big_segments_threshold &&
          (total_fill_factor <= segment_fill_factor)) {
        // filter out segments that are too big
        irstd::swap_remove(sorted_segments, begin);
        --segments_end;
      } else {
        ++begin;
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Stage 3
    /// sort candidates
    ///////////////////////////////////////////////////////////////////////////

    std::sort(sorted_segments.begin(), sorted_segments.end());

    ///////////////////////////////////////////////////////////////////////////
    /// Stage 4
    /// find proper candidates
    ///////////////////////////////////////////////////////////////////////////

    tier::ConsolidationCandidate best(sorted_segments.begin());

    if (sorted_segments.size() >= min_segments_per_tier) {
      for (auto i = sorted_segments.begin(), end = sorted_segments.end();
           i != end; ++i) {
        tier::ConsolidationCandidate candidate(i);

        while (candidate.segments.second != end &&
               candidate.count < max_segments_per_tier) {
          candidate.size += candidate.segments.second->size;

          if (candidate.size > max_segments_bytes) {
            // overcome the limit
            break;
          }

          ++candidate.count;
          ++candidate.segments.second;

          if (candidate.count < min_segments_per_tier) {
            // not enough segments yet
            continue;
          }

          candidate.score = tier::consolidation_score(
            candidate, max_segments_per_tier, floor_segment_bytes);

          if (candidate.score < min_score) {
            // score is too small
            continue;
          }

          if (best.score < candidate.score) {
            best = candidate;
          }
        }
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Stage 4
    /// pick the best candidate
    ///////////////////////////////////////////////////////////////////////////

    std::copy(best.begin(), best.end(), std::back_inserter(candidates));
  };
}

void ReadDocumentMask(irs::DocumentMask& docs_mask, const irs::directory& dir,
                      const irs::SegmentMeta& meta) {
  if (!irs::HasRemovals(meta)) {
    return;  // nothing to read
  }

  auto reader = meta.codec->get_document_mask_reader();
  reader->read(dir, meta, docs_mask);
}

void FlushIndexSegment(directory& dir, IndexSegment& segment) {
  IRS_ASSERT(segment.meta.codec);
  IRS_ASSERT(segment.meta.byte_size);  // Ensure segment size is estimated

  auto writer = segment.meta.codec->get_segment_meta_writer();
  writer->write(dir, segment.filename, segment.meta);
}

}  // namespace irs::index_utils
