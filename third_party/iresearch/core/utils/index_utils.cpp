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
#include <iostream>
#include <map>
#include <memory>
#include <set>

#include "formats/format_utils.hpp"
#include "priority_queue.hpp"

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

ConsolidationPolicy MakePolicy(const ConsolidateCount& options) {
  return [options](Consolidation& candidates, const IndexReader& reader,
                   const ConsolidatingSegments&) {
    // merge first 'threshold' segments
    for (size_t i = 0, count = std::min(options.threshold, reader.size());
         i < count; ++i) {
      candidates.emplace_back(&reader[i]);
    }
    std::sort(candidates.begin(), candidates.end(),
              [](const SubReader*& a, const SubReader*& b) {
                return a->Meta().base_doc < b->Meta().base_doc;
              });
    std::cout << "candidates segments: " << std::endl;
    for (auto &segment : candidates) {
        auto &meta = segment->Meta();
        std::cout << "seg " << meta.name << " base " << meta.base_doc << " num_doc " << meta.docs_count << " ";
    }
    std::cout << std::endl;

  };
}

struct MergeBarrelEntry {
  MergeBarrelEntry(const irs::SubReader& reader)
    : reader{&reader}, meta{&reader.Meta()} {}

  MergeBarrelEntry(const MergeBarrelEntry& other)
    : reader{other.reader}, meta{other.meta} {}

  size_t NumDocs() { return meta->docs_count; }

  doc_id_t BaseDocID() { return meta->base_doc; }

  const irs::SubReader* reader;
  const irs::SegmentInfo* meta;
  friend class MergeBarrelQueue;
};

class MergeBarrelQueue : public irs::PriorityQueue<MergeBarrelEntry*> {
 public:
  MergeBarrelQueue(size_t max_size) { Initialize(max_size, true); }
  ~MergeBarrelQueue() {}

 private:
  bool LessThan(MergeBarrelEntry* o1, MergeBarrelEntry* o2) {
    return o1->NumDocs() > o2->NumDocs();
  }
};
#define MAXLEVEL 30
#define COLLISION_FACTOR_FOR_LEVEL_1 3

struct BTPolicy {
  BTPolicy();

  struct BTLayer {
    BTLayer(size_t l, size_t level_size, size_t max_size)
      : level_(l), level_size_(level_size) {
      entry_queue_ = new MergeBarrelQueue(max_size);
    }

    ~BTLayer() {
      delete entry_queue_;
      entry_queue_ = NULL;
    }

    void Add(MergeBarrelEntry* entry) { entry_queue_->Put(entry); }

    std::pair<doc_id_t, doc_id_t> GetBaseDocIDRange() const {
      const size_t count = entry_queue_->Size();
      if (count == 0) return std::make_pair(-1, -1);

      doc_id_t base_min, base_max;
      base_min = base_max = entry_queue_->Get(0)->BaseDocID();
      for (size_t i = 1; i < count; ++i) {
        doc_id_t base = entry_queue_->Get(i)->BaseDocID();
        if (base < base_min)
          base_min = base;
        else if (base > base_max)
          base_max = base;
      }

      return std::make_pair(base_min, base_max);
    }

    size_t level_;
    size_t level_size_;
    MergeBarrelQueue* entry_queue_;
  };

  void AddBarrel(const irs::SubReader& reader, Consolidation& candidates);

  size_t GetLevel(size_t level_size);

  void TriggerMerge(BTLayer* p_level, Consolidation& candidates);

  size_t GetC(size_t level);

  BTLayer* CombineLayer(BTLayer* layer1, BTLayer* layer2) const;

  constexpr static size_t MAX_LAYER_SIZE = 100;
  std::map<size_t, size_t> cmap_;
  std::map<size_t, BTLayer*> nodes_map_;
};

BTPolicy::BTPolicy() {
  for (size_t i = 0; i < MAXLEVEL; i++) cmap_[i] = COLLISION_FACTOR_FOR_LEVEL_1;
}

size_t BTPolicy::GetC(size_t level) { return cmap_[level]; }

void BTPolicy::AddBarrel(const irs::SubReader& reader,
                         Consolidation& candidates) {
  MergeBarrelEntry* entry = new MergeBarrelEntry(reader);
  size_t level_size = entry->NumDocs();
  size_t level = GetLevel(level_size);
  size_t nC = GetC(level);
  BTLayer* p_level = NULL;
  std::map<size_t, BTLayer*>::iterator iter = nodes_map_.find(level);
  if (iter != nodes_map_.end()) {
    p_level = iter->second;
    p_level->level_size_ += level_size;
    p_level->Add(entry);
    if (p_level->entry_queue_->Size() >= nC) TriggerMerge(p_level, candidates);
  } else {
    p_level = new BTLayer(level, level_size, MAX_LAYER_SIZE);
    p_level->Add(entry);
    nodes_map_.insert(std::make_pair(level, p_level));
  }
}

void BTPolicy::TriggerMerge(BTLayer* p_level, Consolidation& candidates) {
  BTLayer* level1 = p_level;
  std::map<size_t, BTLayer*>::iterator iter2;

  // if it would trigger another merge event in upper level,
  // combine them directly
  size_t newLevel = GetLevel(level1->level_size_);
  while (newLevel > level1->level_ &&
         (iter2 = nodes_map_.find(newLevel)) != nodes_map_.end()) {
    BTLayer* pLevel2 = iter2->second;
    if ((int)pLevel2->entry_queue_->Size() + 1 < GetC(newLevel)) break;

    level1 = CombineLayer(level1, pLevel2);
    newLevel = GetLevel(level1->level_size_);
  }

  // iterate barrels in other layer, merge them if within the base doc id range
  std::pair<doc_id_t, doc_id_t> range1 = level1->GetBaseDocIDRange();
  for (iter2 = nodes_map_.begin(); iter2 != nodes_map_.end(); ++iter2) {
    // ignore current level
    if (iter2->second == level1) continue;

    // check the base doc id of other layer
    BTLayer* pLevel2 = iter2->second;
    const size_t count = (int)pLevel2->entry_queue_->Size();
    bool combine_layer = false;
    for (size_t i = 0; i < count; ++i) {
      doc_id_t base_doc = pLevel2->entry_queue_->Get(i)->BaseDocID();
      // equal is used in case of some barrel is empty
      if ((base_doc >= range1.first) && (base_doc <= range1.second)) {
        combine_layer = true;
        break;
      }
    }
    if (combine_layer) {
      level1 = CombineLayer(level1, pLevel2);
      range1 = level1->GetBaseDocIDRange();
    }
  }

  // merge and clear the level
  if (level1->entry_queue_->Size() > 0) {
    level1->level_size_ = 0;
    size_t count = level1->entry_queue_->Size();
    for (size_t i = 0; i < count; ++i) {
      MergeBarrelEntry* entry = level1->entry_queue_->Get(i);
      candidates.push_back(entry->reader);
    }
  }
}

size_t BTPolicy::GetLevel(size_t level_size) {
  if (level_size <= 0) return 0;

  size_t level = 0;
  size_t max_size = 1;
  for (; level < MAXLEVEL; ++level) {
    max_size *= 3;
    if (level_size < max_size) break;
  }
  return level;
}

BTPolicy::BTLayer* BTPolicy::CombineLayer(BTLayer* layer1,
                                          BTLayer* layer2) const {
  assert(layer1 != layer2 && layer1->level_ != layer2->level_);

  BTLayer* lower = layer1;
  BTLayer* higher = layer2;
  if (lower->level_ > higher->level_) std::swap(lower, higher);

  // copy elements to upper level
  while (lower->entry_queue_->Size() > 0)
    higher->entry_queue_->Put(lower->entry_queue_->Pop());

  higher->level_size_ += lower->level_size_;
  lower->level_size_ = 0;

  return higher;
}

struct NaivePolicy {
  void AddSegment(const irs::SubReader& reader);

  void GetCandidates(Consolidation& candidates);

  std::vector<MergeBarrelEntry> entries_;
};
void NaivePolicy::AddSegment(const irs::SubReader& reader) {
  entries_.emplace_back(reader);
}

constexpr static size_t MAX_MERGE_ONE_TIME = 3;
void NaivePolicy::GetCandidates(Consolidation& candidates) {
  std::sort(entries_.begin(), entries_.end(),
            [](const MergeBarrelEntry& a, const MergeBarrelEntry& b) {
              return a.meta->base_doc < b.meta->base_doc;
            });
  doc_id_t min_count = std::numeric_limits<doc_id_t>::max();
  for (size_t i = 0; i < entries_.size(); ++i) {
    if (entries_[i].meta->docs_count < min_count) {
      min_count = entries_[i].meta->docs_count;
    }
  }
  int index = 0;
  for (; index < entries_.size(); ++index) {
    if (entries_[index].meta->docs_count == min_count) {
      break;
    }
  }
  int forward_iter = index;
  int backward_iter = index;
  size_t merge_count = std::min(MAX_MERGE_ONE_TIME, entries_.size());
  while (candidates.size() < merge_count) {
      if (forward_iter == entries_.size()) {
          if (backward_iter >= 0) {
              candidates.push_back(entries_[backward_iter].reader);
              if (forward_iter == backward_iter)
                  forward_iter++;
              backward_iter--;
          } else
              break;
      } else {
          if (backward_iter >= 0) {
              if (entries_[forward_iter].meta->docs_count < entries_[backward_iter].meta->docs_count) {
                  candidates.push_back(entries_[forward_iter].reader);
                  if (forward_iter == backward_iter)
                      backward_iter--;
                  forward_iter++;
              } else {
                  candidates.push_back(entries_[backward_iter].reader);
                  if (forward_iter == backward_iter)
                      forward_iter++;
                  backward_iter--;
              }
          } else {
              candidates.push_back(entries_[forward_iter].reader);
              if (forward_iter == backward_iter)
                  backward_iter--;
              forward_iter++;
          }
      }
  }
}

ConsolidationPolicy MakePolicy(const ConsolidateByOrder& options) {
  return [](Consolidation& candidates, const IndexReader& reader,
            const ConsolidatingSegments& consolidating_segments) {
    // merge segment if: {threshold} >= #segment_docs{valid} /
    // (#segment_docs{valid} + #segment_docs{removed})
    std::vector<doc_id_t> base_docs;
    std::cout << "all segments: " << std::endl;
    for (auto& segment : reader) {
      auto& meta = segment.Meta();
      base_docs.push_back(meta.base_doc);
      std::cout << "seg " << meta.name << " base " << meta.base_doc
                << " num_doc " << meta.docs_count << " ";
    }
    std::cout << std::endl;
    if (base_docs.empty() || base_docs.size() == 1) return;
    std::sort(base_docs.begin(), base_docs.end());
    Consolidation candidates_by_size;
    // ConsolidateTier inner_options;
    // ConsolidationPolicy policy = MakePolicy(inner_options);
    // policy(candidates_by_size, reader, consolidating_segments);

    // BTPolicy policy;
    // for (auto& segment : reader) {
    //   policy.AddBarrel(segment, candidates_by_size);
    //   if (!candidates_by_size.empty()) break;
    // }
    NaivePolicy policy;
    for (auto& segment : reader) {
      policy.AddSegment(segment);
    }
    policy.GetCandidates(candidates_by_size);
    if (candidates_by_size.empty() || candidates_by_size.size() == 1) return;
    std::sort(candidates_by_size.begin(), candidates_by_size.end(),
              [](const SubReader*& a, const SubReader*& b) {
                return a->Meta().base_doc < b->Meta().base_doc;
              });
    std::cout << "initial candidates: " << std::endl;
    for (size_t i = 0; i < candidates_by_size.size(); ++i) {
      std::cout << "seg " << candidates_by_size[i]->Meta().name << " base "
                << candidates_by_size[i]->Meta().base_doc << " num_doc "
                << candidates_by_size[i]->Meta().docs_count << " ";
    }
    std::cout << std::endl;
    auto pre_it = std::lower_bound(base_docs.begin(), base_docs.end(),
                                   candidates_by_size[0]->Meta().base_doc);
    auto pre_index = std::distance(base_docs.begin(), pre_it);
    candidates.push_back(candidates_by_size[0]);
    for (size_t i = 1; i < candidates_by_size.size(); ++i) {
      auto it = std::lower_bound(base_docs.begin(), base_docs.end(),
                                 candidates_by_size[i]->Meta().base_doc);
      auto index = std::distance(base_docs.begin(), it);
      if (index - pre_index > 1) {
        pre_index = index;
        continue;
      }
      pre_index = index;
      candidates.push_back(candidates_by_size[i]);
    }

    std::cout << "candidates: " << std::endl;
    for (size_t i = 0; i < candidates.size(); ++i) {
      std::cout << "seg " << candidates[i]->Meta().name << " base "
                << candidates[i]->Meta().base_doc << " num_doc "
                << candidates[i]->Meta().docs_count << " ";
    }
    std::cout << std::endl;
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
