// Refers to
// https://github.com/tlx/tlx/blob/master/tlx/container/loser_tree.hpp
module;

#include <cassert>
#include <vector>
export module loser_tree;

import stl;
import third_party;

namespace infinity {

export template <typename ValueType, typename Comparator = std::greater<ValueType>>
class LoserTreeBase {
public:
    using Source = u32;
    static constexpr Source invalid_ = Source(-1);
    static inline Source round_up_to_power_of_two(Source n) {
        --n;
        for (SizeT k = 1; k != 8 * sizeof(n); k <<= 1) {
            n |= n >> k;
        }
        ++n;
        return n;
    }
protected:
    struct Loser {
        // flag, true if is a virtual maximum sentinel
        bool sup;
        Source source;
        ValueType key;
    };

    // The number of nodes in the tree.
    const Source ik_;
    // The next greater power of two of ik_.
    const Source k_;

    Vector<Loser> losers_;

    Comparator cmp_;

    bool first_insert_;
public:
    explicit LoserTreeBase(const Source& k,
                           const Comparator& cmp = Comparator())
         : ik_(k), k_(round_up_to_power_of_two(k)),
          losers_(2 * k_), cmp_(cmp), first_insert_(true) {
        /*
        for (Source i = ik_ - 1; i < k_; i++) {
            losers_[i + k_].sup = true;
            losers_[i + k_].source = invalid_;
        }
        */
        for (Source i = 0; i < 2 * k_; i++) {
            losers_[i].sup = true;
            losers_[i].source = invalid_;
        }
    }

    Source TopSource() {
        return losers_[0].source;
    }

    ValueType TopKey() {
        return losers_[0].key;
    }

    void InsertStart(const ValueType* keyp, const Source& source, bool sup) {
        Source pos = k_ + source;
        losers_[pos].source = source;
        losers_[pos].sup = sup;

        if (first_insert_) {
            for (Source i = 0; i < 2 * k_; ++i) {
                if (keyp) {
                    losers_[i].key = *keyp;
                } else {
                    losers_[i].key = ValueType();
                }
            }
            first_insert_ = false;
        } else {
            losers_[pos].key = (keyp ? *keyp : ValueType());
        }
    }

    // Recursively compute the winner of the competition at player root.
    Source InitWinner(const Source& root) {
        if (root >= k_) {
            return root;
        }
        Source left = InitWinner(2 * root);
        Source right = InitWinner(2 * root + 1);
        if (losers_[right].sup ||
            (!losers_[left].sup && !cmp_(losers_[right].key, losers_[left].key))) {
            losers_[root] = losers_[right];
            return left;
        } else {
            losers_[root] = losers_[left];
            return right;
        }
    }

    void Init() {
        if (k_ == 0) {
            return;
        }
        losers_[0] = losers_[InitWinner(1)];
    }
};

export template <typename ValueType, typename Comparator = std::greater<ValueType>>
class LoserTree : public LoserTreeBase<ValueType, Comparator> {
public:
    using Super = LoserTreeBase<ValueType, Comparator>;
    using Source = typename Super::Source;

public:
    explicit LoserTree(const Source& k,
                       const Comparator& cmp = Comparator())
        : Super(k, cmp) {}

    // Delete the current minimum and insert a new element.
    void DeleteTopInsert(const ValueType* keyp, bool sup) {
        assert(sup == (keyp == nullptr));
        Source source = Super::losers_[0].source;
        ValueType key = (keyp ? *keyp : ValueType());

        for (Source pos = (Super::k_ + source) / 2; pos > 0; pos /= 2) {
            if (sup) {
                std::swap(Super::losers_[pos].sup, sup);
                std::swap(Super::losers_[pos].source, source);
                std::swap(Super::losers_[pos].key, key);
            } else if (Super::losers_[pos].sup) {
                // do nothing
            } else if (Super::cmp_(Super::losers_[pos].key, key)) {
                std::swap(Super::losers_[pos].source, source);
                std::swap(Super::losers_[pos].key, key);
            } else {
                // do nothing
            }
        }
        if (sup) {
            Super::losers_[0].source = Super::invalid_;
        } else {
            Super::losers_[0].source = source;
        }
        Super::losers_[0].sup = sup;
        Super::losers_[0].key = key;
    }
};

} // namespace infinity