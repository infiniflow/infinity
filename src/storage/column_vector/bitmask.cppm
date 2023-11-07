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

import stl;
import bitmask_buffer;

export module bitmask;

namespace infinity {

// use to indicate if the null or missing is set
export struct Bitmask {
    static inline SharedPtr<Bitmask> Make(SizeT count) {
        SharedPtr<Bitmask> ptr = MakeShared<Bitmask>();
        ptr->Initialize(count);
        return ptr;
    }

public:
    Bitmask();

    ~Bitmask();

    void Reset();

    void Initialize(SizeT count);

    void DeepCopy(const Bitmask &ptr);

    void ShallowCopy(const Bitmask &ptr);

    [[nodiscard]] u64 *GetData() const { return data_ptr_; }

    void Resize(SizeT new_capacity);

    String ToString(SizeT from, SizeT to);

    [[nodiscard]] bool IsAllTrue() const;

    bool IsTrue(SizeT row_index);

    void SetTrue(SizeT row_index);

    void SetFalse(SizeT row_index);

    void Set(SizeT row_index, bool valid);

    void SetAllTrue();

    void SetAllFalse();

    [[nodiscard]] SizeT CountTrue() const;

    [[nodiscard]] SizeT CountFalse() const { return count_ - CountTrue(); }

    void Merge(const Bitmask &other);

    [[nodiscard]] SizeT count() const { return count_; }

    bool operator==(const Bitmask &other) const;
    bool operator!=(const Bitmask &other) const { return !(*this == other); }

    // Estimated serialized size in bytes
    i32 GetSizeInBytes() const;
    // Write to a char buffer
    void WriteAdv(char *&ptr) const;
    // Read from a serialized version
    static SharedPtr<Bitmask> ReadAdv(char *&ptr, i32 maxbytes);

private:
    u64 *data_ptr_{nullptr};
    SharedPtr<BitmaskBuffer> buffer_ptr{nullptr};
    SizeT count_{0};
};

} // namespace infinity
