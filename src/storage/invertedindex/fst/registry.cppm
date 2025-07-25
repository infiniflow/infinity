// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

export module infinity_core:fst.registry;

import :stl;

namespace infinity {

/**
A registry is fixed-capacity hash table that can be used to store the pairs of (Key, Val).
*/

template <typename Key, typename Val>
struct RegistryCell {
    Key key_;
    Val value_;
};

export template <typename Val>
struct RegistryEntry {
    bool found_{false};
    union {
        SizeT slot_; // valid iff found==false
        Val value_;  // valid iff found==true
    };
    RegistryEntry() = default;
};

export template <typename Key, typename Val>
class Registry {
private:
    Vector<RegistryCell<Key, Val>> table_;
    SizeT mask_;

public:
    explicit Registry(SizeT table_size_shift) : mask_((1ULL << table_size_shift) - 1ULL) { table_.resize(1ULL << table_size_shift); }

    // Find the entry with the given key.
    RegistryEntry<Val> Find(const Key &key) {
        RegistryEntry<Val> ent;
        SizeT slot = key.Hash() & mask_;
        if (table_[slot].key_ == key) {
            ent.found_ = true;
            ent.value_ = table_[slot].value_;
        } else {
            ent.slot_ = slot;
        }
        return ent;
    }

    // Insert the pair into the given slot. Assume key.Hash()==slot. The existing one at the given slot is discared silently.
    void Insert(SizeT slot, Key &key, Val value) {
        table_[slot].key_ = std::move(key);
        table_[slot].value_ = value;
    }
};

} // namespace infinity