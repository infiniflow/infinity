
#include "storage/meta/entry/base_entry.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

// Merge two reverse-ordered list inplace.
void MergeLists(std::list<std::unique_ptr<BaseEntry>> &list1, std::list<std::unique_ptr<BaseEntry>> &list2) {
    auto it1 = list1.begin();
    auto it2 = list2.begin();

    while (it1 != list1.end() && it2 != list2.end()) {
        if ((*it1)->entry_type_ == EntryType::kDummy) {
            ++it1;
        } else if ((*it2)->entry_type_ == EntryType::kDummy) {
            ++it2;
        } else {
            StorageAssert((*it1)->Committed() && (*it2)->Committed(), "MergeLists requires entries be committed");
            if ((*it1)->commit_ts_ > (*it2)->commit_ts_) {
                ++it1;
            } else if ((*it1)->commit_ts_ < (*it2)->commit_ts_) {
                list1.insert(it1, std::move(*it2));
                ++it2;
            } else {
                (*it1)->MergeFrom(**it2);
                ++it1;
                ++it2;
            }
        }
    }
    list1.splice(it1, list2, it2, list2.end());
    list2.clear();
}

} // namespace infinity