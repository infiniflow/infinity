module;

import stl;
import hit;

#include <algorithm>
#include <locale>
#include <mutex>
#include <stdexcept>
#include <vector>

module ik_dict_segment;

namespace infinity {

HashMap<wchar_t, wchar_t> DictSegment::char_map_;
DictSegment::DictSegment(wchar_t node_char) {
    if (node_char == L'\0') {
        throw std::invalid_argument("node char cannot be empty");
    }
    node_char_ = node_char;
}

Hit *DictSegment::Match(const std::vector<wchar_t> &charArray, int begin, int length, Hit *searchHit) {
    if (searchHit == nullptr) {
        searchHit = new Hit();
        searchHit->SetBegin(begin);
    } else {
        searchHit->SetUnmatch();
    }
    searchHit->SetEnd(begin);

    wchar_t keyChar = charArray[begin];
    DictSegment *ds = nullptr;

    std::vector<DictSegment *> segmentArray = children_array_;
    HashMap<wchar_t, DictSegment *> segmentMap = children_map_;

    if (!segmentArray.empty()) {
        DictSegment keySegment(keyChar);
        auto it =
            std::lower_bound(segmentArray.begin(), segmentArray.begin() + store_size_, &keySegment, [](const DictSegment *a, const DictSegment *b) {
                return a->node_char_ < b->node_char_;
            });
        if (it != segmentArray.begin() + store_size_ && (*it)->node_char_ == keyChar) {
            ds = *it;
        }
    } else if (!segmentMap.empty()) {
        ds = segmentMap[keyChar];
    }

    if (ds != nullptr) {
        if (length > 1) {
            return ds->Match(charArray, begin + 1, length - 1, searchHit);
        } else if (length == 1) {
            if (ds->node_state_ == 1) {
                searchHit->SetMatch();
            }
            if (ds->HasNextNode()) {
                searchHit->SetPrefix();
                searchHit->SetMatchedDictSegment(ds);
            }
            return searchHit;
        }
    }
    return searchHit;
}

void DictSegment::FillSegment(const std::vector<wchar_t> &charArray, int begin, int length, int enabled) {
    wchar_t beginChar = charArray[begin];
    wchar_t keyChar = char_map_[beginChar];
    if (keyChar == L'\0') {
        char_map_[beginChar] = beginChar;
        keyChar = beginChar;
    }

    DictSegment *ds = LookforSegment(keyChar, enabled);
    if (ds != nullptr) {
        if (length > 1) {
            ds->FillSegment(charArray, begin + 1, length - 1, enabled);
        } else if (length == 1) {
            ds->node_state_ = enabled;
        }
    }
}

DictSegment *DictSegment::LookforSegment(wchar_t keyChar, int create) {
    DictSegment *ds = nullptr;

    if (store_size_ <= ARRAY_LENGTH_LIMIT) {
        std::vector<DictSegment *> &segmentArray = GetChildrenArray();
        DictSegment keySegment(keyChar);
        auto it =
            std::lower_bound(segmentArray.begin(), segmentArray.begin() + store_size_, &keySegment, [](const DictSegment *a, const DictSegment *b) {
                return a->node_char_ < b->node_char_;
            });
        if (it != segmentArray.begin() + store_size_ && (*it)->node_char_ == keyChar) {
            ds = *it;
        }

        if (ds == nullptr && create == 1) {
            ds = new DictSegment(keyChar);
            if (store_size_ < ARRAY_LENGTH_LIMIT) {
                segmentArray[store_size_] = ds;
                store_size_++;
                std::sort(segmentArray.begin(), segmentArray.begin() + store_size_, [](const DictSegment *a, const DictSegment *b) {
                    return a->node_char_ < b->node_char_;
                });
            } else {
                HashMap<wchar_t, DictSegment *> &segmentMap = GetChildrenMap();
                Migrate(segmentArray, segmentMap);
                segmentMap[keyChar] = ds;
                store_size_++;
                children_array_.clear();
            }
        }
    } else {
        HashMap<wchar_t, DictSegment *> &segmentMap = GetChildrenMap();
        ds = segmentMap[keyChar];
        if (ds == nullptr && create == 1) {
            ds = new DictSegment(keyChar);
            segmentMap[keyChar] = ds;
            store_size_++;
        }
    }

    return ds;
}

void DictSegment::Migrate(const std::vector<DictSegment *> &segmentArray, HashMap<wchar_t, DictSegment *> &segmentMap) {
    for (DictSegment *segment : segmentArray) {
        if (segment != nullptr) {
            segmentMap[segment->node_char_] = segment;
        }
    }
}

} // namespace infinity
