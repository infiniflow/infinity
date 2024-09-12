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
#include <cassert>
import stl;
module fst;
import :bytes;
import :error;
import :node;
import :registry;
import :writer;

namespace infinity {

void FstBuilder::Finish() {
    CompileFrom(0);
    BuilderNode &root_node = unfinished_.TopNode();
    SizeT root_addr = Compile(root_node);
    unfinished_.Pop();
    IoWriteU64LE(len_, wtr_);
    IoWriteU64LE(root_addr, wtr_);
    u32 sum = wtr_.MaskedChecksum();
    Writer &inner_writer = wtr_.GetRef();
    IoWriteU32LE(sum, inner_writer);
    inner_writer.Flush();
}

void FstBuilder::InsertOutput(u8 *bs_ptr, SizeT bs_len, u64 val) {
    Output out(val);
    if (bs_len == 0) {
        len_ = 1; // must be first key, so length is always 1
        unfinished_.SetRootOutput(out);
        return;
    }
    SizeT prefix_len = unfinished_.FindCommonPrefixAndSetOutput(bs_ptr, bs_len, out);
    if (prefix_len == bs_len) {
        // If the prefix found consumes the entire set of bytes, then
        // the prefix *equals* the bytes given. This means it is a
        // duplicate value with no output. So we can give up here.
        //
        // If the below assert fails, then that means we let a duplicate
        // value through even when inserting outputs.
        assert(out.IsZero());
        return;
    }
    len_++;
    CompileFrom(prefix_len);
    unfinished_.AddSuffix(bs_ptr + prefix_len, bs_len - prefix_len, out);
}

void FstBuilder::CompileFrom(SizeT istate) {
    SizeT addr = NONE_ADDRESS;
    SizeT remained = unfinished_.Len() - (istate + 1);
    for (SizeT i = 0; i < remained; i++) {
        if (addr != NONE_ADDRESS) {
            unfinished_.TopLastFreeze(addr);
        }
        BuilderNode &node = unfinished_.TopNode();
        addr = Compile(node);
        assert(addr != NONE_ADDRESS);
        unfinished_.Pop();
    }
    unfinished_.TopLastFreeze(addr);
}

CompiledAddr FstBuilder::Compile(BuilderNode &node) {
    if (node.is_final_ && node.trans_.empty() && node.final_output_.IsZero()) {
        return EMPTY_ADDRESS;
    }
    RegistryEntry ent = registry_.Find(node);
    if (ent.found_)
        return ent.value_;
    CompiledAddr start_addr = wtr_.Count();
    Node::Compile(wtr_, last_addr_, start_addr, node);
    last_addr_ = wtr_.Count() - 1;
    registry_.Insert(ent.slot_, node, last_addr_);
    return last_addr_;
}

void FstBuilder::CheckLastKey(u8 *bs_ptr, SizeT bs_len, bool check_dupe) {
    if (last_.empty())
        return;
    if (check_dupe && last_.size() == bs_len && std::memcmp(last_.data(), bs_ptr, bs_len) == 0)
        throw FstError::DuplicatedKey(bs_ptr, bs_len);
    SizeT min_len = std::min(static_cast<SizeT>(last_.size()), bs_len);
    for (SizeT i = 0; i < min_len; i++) {
        if (last_[i] > bs_ptr[i]) {
            throw FstError::OutOfOrder(last_.data(), last_.size(), bs_ptr, bs_len);
        }
    }
    if (bs_len < min_len)
        throw FstError::OutOfOrder(last_.data(), last_.size(), bs_ptr, bs_len);
    last_.clear();
    last_.insert(last_.end(), bs_ptr, bs_ptr + bs_len);
}

} // namespace infinity