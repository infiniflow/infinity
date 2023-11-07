//
// Created by yzq on 11/7/23.
//
module;
#include <algorithm>
#include <random>
export module extra_faiss_cmax_handler;

namespace infinity {
// traits of minheaps = heaps where the minimum value is stored on top
// useful to find the *max* values of an array
export template <typename T_, typename TI_>
struct CMax;
export template <typename T_, typename TI_>
struct CMin {
    typedef T_ T;
    typedef TI_ TI;
    // typedef CMax<T_, TI_> Crev; // reference to reverse comparison
    inline static bool cmp(T a, T b) { return a < b; }
    // Similar to cmp(), but also breaks ties
    // by comparing the second pair of arguments.
    inline static bool cmp2(T a1, T b1, TI a2, TI b2) { return (a1 < b1) || ((a1 == b1) && (a2 < b2)); }
    inline static T neutral() { return std::numeric_limits<T>::lowest(); }
    static const bool is_max = false;

    // inline static T nextafter(T x) { return cmin_nextafter(x); }
};

template <typename T_, typename TI_>
struct CMax {
    typedef T_ T;
    typedef TI_ TI;
    // typedef CMin<T_, TI_> Crev;
    inline static bool cmp(T a, T b) { return a > b; }
    // Similar to cmp(), but also breaks ties
    // by comparing the second pair of arguments.
    inline static bool cmp2(T a1, T b1, TI a2, TI b2) { return (a1 > b1) || ((a1 == b1) && (a2 > b2)); }
    inline static T neutral() { return std::numeric_limits<T>::max(); }
    static const bool is_max = true;
    // inline static T nextafter(T x) { return cmax_nextafter(x); }
};

export template <class C>
struct TopKSingleBestResultHandler {
    using T = typename C::T;
    using TI = typename C::TI;

    int nq;
    // contains exactly nq elements
    T *dis_tab;
    // contains exactly nq elements
    TI *ids_tab;

    TopKSingleBestResultHandler(size_t nq, T *dis_tab, TI *ids_tab) : nq(nq), dis_tab(dis_tab), ids_tab(ids_tab) {}

    struct SingleResultHandler {
        TopKSingleBestResultHandler &hr;

        T min_dis;
        TI min_idx;
        size_t current_idx = 0;

        SingleResultHandler(TopKSingleBestResultHandler &hr) : hr(hr) {}

        /// begin results for query # i
        void begin(const size_t current_idx) {
            this->current_idx = current_idx;
            min_dis = HUGE_VALF;
            min_idx = 0;
        }

        /// add one result for query i
        void add_result(T dis, TI idx) {
            if (C::cmp(min_dis, dis)) {
                min_dis = dis;
                min_idx = idx;
            }
        }

        /// series of results for query i is done
        void end() {
            hr.dis_tab[current_idx] = min_dis;
            hr.ids_tab[current_idx] = min_idx;
        }
    };

    size_t i0, i1;

    /// begin
    void begin_multiple(size_t i0, size_t i1) {
        this->i0 = i0;
        this->i1 = i1;

        for (size_t i = i0; i < i1; i++) {
            this->dis_tab[i] = HUGE_VALF;
        }
    }

    /// add results for query i0..i1 and j0..j1
    void add_results(size_t j0, size_t j1, const T *dis_tab) {
        for (int64_t i = i0; i < i1; i++) {
            const T *dis_tab_i = dis_tab + (j1 - j0) * (i - i0) - j0;

            auto &min_distance = this->dis_tab[i];
            auto &min_index = this->ids_tab[i];

            for (size_t j = j0; j < j1; j++) {
                const T distance = dis_tab_i[j];

                if (C::cmp(min_distance, distance)) {
                    min_distance = distance;
                    min_index = j;
                }
            }
        }
    }

    void add_result(const size_t i, const T dis, const TI idx) {
        auto &min_distance = this->dis_tab[i];
        auto &min_index = this->ids_tab[i];

        if (C::cmp(min_distance, dis)) {
            min_distance = dis;
            min_index = idx;
        }
    }

    /// series of results for queries i0..i1 is done
    void end_multiple() {}
};
} // namespace infinity