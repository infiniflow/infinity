
#include "storage/meta/entry/base_entry.h"
#include "common/utility/infinity_assert.h"

namespace infinity {

// Merge two reverse-ordered list inplace.
void MergeLists(std::list<std::unique_ptr<BaseEntry>> &list1, std::list<std::unique_ptr<BaseEntry>> &list2) {
    auto it = list1.begin();
    auto it2 = list2.begin();
    while (it != list1.end() && it2 != list2.end()) {
        if ((*it)->entry_type_ == EntryType::kDummy) {
            ++it;
        } else if ((*it2)->entry_type_ == EntryType::kDummy) {
            ++it2;
        } else {
            StorageAssert((*it)->Committed() && (*it2)->Committed(), "MergeLists requires entries be committed");
            if ((*it)->commit_ts_ > (*it2)->commit_ts_) {
                ++it;
            } else if ((*it)->commit_ts_ < (*it2)->commit_ts_) {
                list1.insert(it, std::move(*it2));
                ++it2;
            } else {
                (*it)->MergeFrom(**it2);
                ++it;
                ++it2;
            }
        }
    }
    list1.splice(it, list2, it2);
    list2.clear();
}

} // namespace infinity