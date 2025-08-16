export module infinity_core:ik_dict_segment;

import :hit;

import std;

namespace infinity {

export class DictSegment {
public:
    static std::unordered_map<wchar_t, wchar_t> char_map_;

    static const int ARRAY_LENGTH_LIMIT = 3;

    wchar_t node_char_;
    int store_size_ = 0;
    int node_state_ = 0;

    DictSegment(wchar_t node_char_);

    wchar_t GetNodeChar() const { return node_char_; }

    bool HasNextNode() const { return store_size_ > 0; }

    Hit *Match(const std::vector<wchar_t> &char_array) { return Match(char_array, 0, char_array.size(), nullptr); }

    Hit *Match(const std::vector<wchar_t> &char_array, int begin, int length) { return Match(char_array, begin, length, nullptr); }

    Hit *Match(const std::vector<wchar_t> &char_array, int begin, int length, Hit *search_hit);

    void FillSegment(const std::vector<wchar_t> &char_array) { FillSegment(char_array, 0, char_array.size(), 1); }

    void DisableSegment(const std::vector<wchar_t> &char_array) { FillSegment(char_array, 0, char_array.size(), 0); }

private:
    std::unordered_map<wchar_t, std::unique_ptr<DictSegment>> children_map_;

    std::vector<std::unique_ptr<DictSegment>> children_array_;

    void FillSegment(const std::vector<wchar_t> &char_array, int begin, int length, int enabled);

    DictSegment *LookforSegment(wchar_t key_char, int create);

    std::vector<std::unique_ptr<DictSegment>> &GetChildrenArray() {
        if (children_array_.empty()) {
            children_array_.resize(ARRAY_LENGTH_LIMIT);
        }
        return children_array_;
    }

    std::unordered_map<wchar_t, std::unique_ptr<DictSegment>> &GetChildrenMap() {
        if (children_map_.empty()) {
            children_map_.reserve(ARRAY_LENGTH_LIMIT * 2);
        }
        return children_map_;
    }
};

} // namespace infinity