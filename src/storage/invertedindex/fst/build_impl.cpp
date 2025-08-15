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

module infinity_core:fst.build.impl;

import :fst.build;
import :stl;
import :infinity_exception;
import :fst.bytes;
import :fst.node;
import :fst.registry;
import :fst.writer;

import third_party;

namespace infinity {

void FstBuilder::Finish() {
    CompileFrom(0);
    BuilderNode &root_node = unfinished_.TopNode();
    size_t root_addr = Compile(root_node);
    unfinished_.Pop();
    IoWriteU64LE(len_, wtr_);
    IoWriteU64LE(root_addr, wtr_);
    u32 sum = wtr_.MaskedChecksum();
    Writer &inner_writer = wtr_.GetRef();
    IoWriteU32LE(sum, inner_writer);
    inner_writer.Flush();
}

void FstBuilder::InsertOutput(u8 *bs_ptr, size_t bs_len, u64 val) {
    Output out(val);
    if (bs_len == 0) {
        len_ = 1; // must be first key, so length is always 1
        unfinished_.SetRootOutput(out);
        return;
    }
    size_t prefix_len = unfinished_.FindCommonPrefixAndSetOutput(bs_ptr, bs_len, out);
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

void FstBuilder::CompileFrom(size_t istate) {
    size_t addr = NONE_ADDRESS;
    size_t remained = unfinished_.Len() - (istate + 1);
    for (size_t i = 0; i < remained; i++) {
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

std::string FormatBytes(u8 *bs_data, size_t bs_len) {
    std::string output = "[";
    for (size_t i = 0; i < bs_len; i++) {
        output += fmt::format("{:02X}", bs_data[i]);
    }
    output += "]";
    return output;
}

void FstBuilder::CheckLastKey(u8 *bs_ptr, size_t bs_len, bool check_dupe) {
    if (last_.empty())
        return;
    if (check_dupe && last_.size() == bs_len && std::memcmp(last_.data(), bs_ptr, bs_len) == 0)
        UnrecoverableError(fmt::format("FST duplicated key {}", FormatBytes(bs_ptr, bs_len)));
    size_t min_len = std::min(static_cast<size_t>(last_.size()), bs_len);
    for (size_t i = 0; i < min_len; i++) {
        if (last_[i] > bs_ptr[i]) {
            UnrecoverableError(
                fmt::format("Out of order key, prev {}, got {}", FormatBytes(last_.data(), last_.size()), FormatBytes(bs_ptr, bs_len)));
        }
    }
    if (bs_len < min_len)
        UnrecoverableError(fmt::format("Out of order key, prev {}, got {}", FormatBytes(last_.data(), last_.size()), FormatBytes(bs_ptr, bs_len)));
    last_.clear();
    last_.insert(last_.end(), bs_ptr, bs_ptr + bs_len);
}

} // namespace infinity