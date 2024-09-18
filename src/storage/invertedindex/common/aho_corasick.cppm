module;

#include <cstddef>

export module aho_corasick;

import stl;
import logger;
import darts;

namespace infinity {

// Aho-corasick based on double array trie
// Modified from https://github.com/jeongukjae/darts-ac
export class AhoCorasick : DoubleArrayImpl<void, void, int, void> {
public:
    AhoCorasick() = default;
    ~AhoCorasick() { Clear(); }

    typedef int value_type;

    typedef value_type result_type;

    struct ResultType {
        value_type value;
        std::size_t length;
        std::size_t position;
    };

    int Build(const Vector<String> &patterns);

    u32 Find(const String &text, ResultType *results, u32 max_num_results) const;

    void Clear() {
        DoubleArrayImpl<void, void, int, void>::clear();

        if (failure_ != nullptr) {
            delete[] failure_;
            failure_ = nullptr;
        }

        if (depth_ != nullptr) {
            delete[] depth_;
            depth_ = nullptr;
        }
    }

private:
    const std::size_t *failure_ = nullptr;
    const unsigned int *depth_ = nullptr;

    int BuildFailureLinks(std::size_t num_keys, const char *const *keys, const std::size_t *lengths);

    std::size_t FindFailureLinks(std::size_t *buf, std::size_t parent_node_pos, const char *key, std::size_t key_pos) const;

    void SetResult(ResultType *result, value_type value, std::size_t length, std::size_t position) const {
        result->value = value;
        result->length = length;
        result->position = position;
    }
};
} // namespace infinity
