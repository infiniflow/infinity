module;

#include <algorithm>

module ik_dict_segment;

import stl;
import hit;
import character_util;
import third_party;

namespace infinity {

HashMap<wchar_t, wchar_t> DictSegment::char_map_;

DictSegment::DictSegment(wchar_t node_char) : node_char_(node_char) {}

Hit *DictSegment::Match(const Vector<wchar_t> &char_array, int begin, int length, Hit *search_hit) {
    if (search_hit == nullptr) {
        search_hit = new Hit();
        search_hit->SetBegin(begin);
    } else {
        search_hit->SetUnmatch();
    }
    search_hit->SetEnd(begin);

    wchar_t key_char = char_array[begin];
    DictSegment *ds = nullptr;

    if (!children_array_.empty()) {
        UniquePtr<DictSegment> key_segment = MakeUnique<DictSegment>(key_char);
        auto it = std::lower_bound(children_array_.begin(),
                                   children_array_.begin() + store_size_,
                                   key_segment,
                                   [](const UniquePtr<DictSegment> &a, const UniquePtr<DictSegment> &b) { return a->node_char_ < b->node_char_; });
        if (it != children_array_.begin() + store_size_ && (*it)->node_char_ == key_char) {
            ds = (*it).get();
        }
    } else if (!children_map_.empty()) {
        auto it = children_map_.find(key_char);
        if (it != children_map_.end())
            ds = it->second.get();
    }

    if (ds != nullptr) {
        if (length > 1) {
            return ds->Match(char_array, begin + 1, length - 1, search_hit);
        } else if (length == 1) {
            if (ds->node_state_ == 1) {
                search_hit->SetMatch();
            }
            if (ds->HasNextNode()) {
                search_hit->SetPrefix();
                search_hit->SetMatchedDictSegment(ds);
            }
            return search_hit;
        }
    }
    return search_hit;
}

void DictSegment::FillSegment(const Vector<wchar_t> &char_array, int begin, int length, int enabled) {
    wchar_t begin_char = char_array[begin];
    wchar_t key_char;
    HashMap<wchar_t, wchar_t>::iterator it = char_map_.find(begin_char);
    if (it == char_map_.end()) {
        char_map_[begin_char] = begin_char;
        key_char = begin_char;
    } else {
        key_char = it->second;
    }
    DictSegment *ds = LookforSegment(key_char, enabled);
    if (ds != nullptr) {
        if (length > 1) {
            ds->FillSegment(char_array, begin + 1, length - 1, enabled);
        } else if (length == 1) {
            ds->node_state_ = enabled;
        }
    }
}

DictSegment *DictSegment::LookforSegment(wchar_t key_char, int create) {
    DictSegment *ds = nullptr;

    if (store_size_ <= ARRAY_LENGTH_LIMIT) {
        Vector<UniquePtr<DictSegment>> &children_array_ = GetChildrenArray();
        UniquePtr<DictSegment> key_segment = MakeUnique<DictSegment>(key_char);
        auto it = std::lower_bound(children_array_.begin(),
                                   children_array_.begin() + store_size_,
                                   key_segment,
                                   [](const UniquePtr<DictSegment> &a, const UniquePtr<DictSegment> &b) { return a->node_char_ < b->node_char_; });
        if (it != children_array_.begin() + store_size_ && (*it)->node_char_ == key_char) {
            ds = (*it).get();
        }

        if (ds == nullptr && create == 1) {
            UniquePtr<DictSegment> ds_ptr = MakeUnique<DictSegment>(key_char);
            ds = ds_ptr.get();
            if (store_size_ < ARRAY_LENGTH_LIMIT) {
                children_array_[store_size_] = std::move(ds_ptr);
                store_size_++;
                std::sort(children_array_.begin(),
                          children_array_.begin() + store_size_,
                          [](const UniquePtr<DictSegment> &a, const UniquePtr<DictSegment> &b) { return a->node_char_ < b->node_char_; });
            } else {
                for (auto &segment : children_array_) {
                    if (segment.get() != nullptr) {
                        children_map_[segment->node_char_] = std::move(segment);
                    }
                }
                children_map_[key_char] = std::move(ds_ptr);
                store_size_++;
                children_array_.clear();
            }
        }
    } else {
        ds = children_map_[key_char].get();
        if (ds == nullptr && create == 1) {
            UniquePtr<DictSegment> ds_ptr = MakeUnique<DictSegment>(key_char);
            ds = ds_ptr.get();
            children_map_[key_char] = std::move(ds_ptr);
            store_size_++;
        }
    }

    return ds;
}

} // namespace infinity
