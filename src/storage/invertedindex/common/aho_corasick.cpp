module;

#include <cstddef>
#include <vector>

module aho_corasick;

import stl;
import darts;

namespace infinity {

int AhoCorasick::Build(const Vector<String> &patterns) {
    Vector<const char *> keys;
    Vector<std::size_t> lengths;
    std::size_t num_keys = patterns.size();
    for (auto &pattern : patterns) {
        keys.push_back(pattern.c_str());
        lengths.push_back(pattern.size());
    }
    int ret = this->build(num_keys, keys.data(), lengths.data(), nullptr, nullptr);
    if (ret != 0) {
        return ret;
    }
    // Set depth_.
    auto *buf = new unsigned int[this->size()]();
    for (std::size_t i = 0; i < num_keys; ++i) {
        std::size_t node_pos = 0;
        for (std::size_t j = 0; j < lengths[i]; ++j) {
            std::size_t mutable_key_pos = j;
            if (this->traverse(keys[i], node_pos, mutable_key_pos, j + 1) == -2) {
                return -1;
            }
            buf[node_pos] = j + 1;
        }
    }
    this->depth_ = buf;

    // Build a failure function.
    return BuildFailureLinks(num_keys, keys.data(), lengths.data());
}

u32 AhoCorasick::Find(const String &text, ResultType *results, u32 max_num_results) const {
    u32 num_results = 0;
    std::size_t node_pos = 0; // starts from the root node.

    const DoubleArrayUnit *array = static_cast<const DoubleArrayUnit *>(this->array());

    const char *key = text.c_str();
    std::size_t length = text.size();
    DoubleArrayUnit unit = array[node_pos];
    for (std::size_t i = 0; i < length; ++i) {
        while (true) {
            auto next_node_pos = node_pos ^ unit.offset() ^ static_cast<unsigned char>(key[i]);
            auto next_unit = array[next_node_pos];

            if (next_unit.label() == static_cast<unsigned char>(key[i])) {
                node_pos = next_node_pos;
                unit = next_unit;
                break;
            }

            // root node cannot follow the failure link.
            if (node_pos == 0) {
                break;
            }

            node_pos = this->failure_[node_pos];
            unit = array[node_pos];
        }

        if (unit.has_leaf()) {
            if (num_results < max_num_results) {
                SetResult(&results[num_results],
                          static_cast<value_type>(array[node_pos ^ unit.offset()].value()),
                          depth_[node_pos],
                          i + 1 - depth_[node_pos]);
            }
            ++num_results;
        }

        // TODO: this is little bit inefficient because we traverse the failure
        // links again without knowning there's a match or not. So we can improve
        // this by checking the failure links before traversing.
        auto node_pos_for_output = failure_[node_pos];
        while (node_pos_for_output != 0) {
            auto unit_for_output = array[node_pos_for_output];
            if (unit_for_output.has_leaf()) {
                if (num_results < max_num_results) {
                    SetResult(&results[num_results],
                              static_cast<value_type>(array[node_pos_for_output ^ unit_for_output.offset()].value()),
                              depth_[node_pos_for_output],
                              i + 1 - depth_[node_pos_for_output]);
                }
                ++num_results;
            }
            node_pos_for_output = failure_[node_pos_for_output];
        }
    }

    return num_results;
}

int AhoCorasick::BuildFailureLinks(std::size_t num_keys, const char *const *keys, const std::size_t *lengths) {
    // Allocate memory for the failure function and fill zeros.
    auto *buf = new std::size_t[this->size()]();

    std::size_t max_length = 0;
    for (std::size_t i = 0; i < num_keys; ++i) {
        if (max_length < lengths[i]) {
            max_length = lengths[i];
        }
    }

    for (std::size_t i = 0; i < max_length; ++i) {
        // Find a failure link for each node.
        for (std::size_t key_index = 0; key_index < num_keys; ++key_index) {
            const char *const key = keys[key_index];
            const std::size_t length = lengths[key_index];

            if (length <= i) {
                // The key is too short. Skip.
                continue;
            }

            std::size_t parent_node_pos = 0;
            std::size_t key_pos = 0;
            if (i != 0) {
                auto val = this->traverse(key, parent_node_pos, key_pos, i);
                // when the key is not found in the trie. this is not expected to
                // happen.
                if (val == -2) {
                    delete[] buf;
                    return -1;
                }
            }

            std::size_t node_pos = parent_node_pos;
            auto val = this->traverse(key, node_pos, key_pos, i + 1);
            if (val == -2) {
                delete[] buf;
                return -1;
            }

            if (buf[node_pos] != 0) {
                // The failure link is already set. Skip.
                continue;
            }

            // Find a failure link.
            const auto failure_node_pos = FindFailureLinks(buf, parent_node_pos, key, i);
            buf[node_pos] = failure_node_pos;
        }
    }

    failure_ = buf;
    return 0;
}

std::size_t AhoCorasick::FindFailureLinks(std::size_t *buf, std::size_t parent_node_pos, const char *key, std::size_t key_pos) const {
    if (parent_node_pos == 0) // skip.
        return 0;

    while (true) {
        const auto failure_node_pos = buf[parent_node_pos];

        std::size_t mutable_key_pos = 0;
        std::size_t mutable_node_pos = failure_node_pos;
        auto ret = this->traverse(key + key_pos, mutable_node_pos, mutable_key_pos, 1);
        if (ret != -2) {
            return mutable_node_pos;
        }

        if (failure_node_pos == 0) {
            return 0;
        }

        parent_node_pos = failure_node_pos;
    }

    return 0;
}

} // namespace infinity