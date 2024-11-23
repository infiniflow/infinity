module;

export module ik_dict_segment;

import stl;
import hit;
namespace infinity {

export class DictSegment {
public:
    static HashMap<wchar_t, wchar_t> char_map_;

    static const int ARRAY_LENGTH_LIMIT = 3;

    wchar_t node_char_;
    int store_size_ = 0;
    int node_state_ = 0;

    DictSegment(wchar_t node_char_);

    wchar_t GetNodeChar() const { return node_char_; }

    bool HasNextNode() const { return store_size_ > 0; }

    Hit *Match(const Vector<wchar_t> &charArray) { return Match(charArray, 0, charArray.size(), nullptr); }

    Hit *Match(const Vector<wchar_t> &charArray, int begin, int length) { return Match(charArray, begin, length, nullptr); }

    Hit *Match(const Vector<wchar_t> &charArray, int begin, int length, Hit *searchHit);

    void FillSegment(const Vector<wchar_t> &charArray) { FillSegment(charArray, 0, charArray.size(), 1); }

    void DisableSegment(const Vector<wchar_t> &charArray) { FillSegment(charArray, 0, charArray.size(), 0); }

private:
    HashMap<wchar_t, DictSegment *> children_map_;

    Vector<DictSegment *> children_array_;

    void FillSegment(const Vector<wchar_t> &charArray, int begin, int length, int enabled);

    DictSegment *LookforSegment(wchar_t keyChar, int create);

    Vector<DictSegment *> &GetChildrenArray() {
        if (children_array_.empty()) {
            children_array_.resize(ARRAY_LENGTH_LIMIT);
        }
        return children_array_;
    }

    HashMap<wchar_t, DictSegment *> &GetChildrenMap() {
        std::lock_guard<std::mutex> lock(mutex);
        if (children_map_.empty()) {
            children_map_.reserve(ARRAY_LENGTH_LIMIT * 2);
        }
        return children_map_;
    }

    void Migrate(const Vector<DictSegment *> &segmentArray, HashMap<wchar_t, DictSegment *> &segmentMap);

    int CompareTo(const DictSegment &o) const { return node_char_ - o.node_char_; }

private:
    std::mutex mutex;
};

} // namespace infinity