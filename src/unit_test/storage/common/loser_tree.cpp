#include "unit_test/base_test.h"
import stl;
import random;
import third_party;
import loser_tree;

using namespace infinity;

class LoserTreeTest : public BaseTest {
public:
    LoserTreeTest() = default;
    ~LoserTreeTest() = default;

    void SetUp() override {
        numbers.clear();
        loser.clear();
    }
    u64 GetRandom(u64 max_val) {
        return static_cast<u64>(random() % max_val) * random() % max_val;
    }

    void GenerateData(SizeT num_size, SizeT loser_num, u64 max_val);

    void MultiWayMerge(SizeT num_size, SizeT loser_num, u64 max_val);

protected:
    Vector<u64> numbers;
    Vector<u64> num_idx;
    Vector<Vector<u64>> loser;
};

void LoserTreeTest::GenerateData(infinity::SizeT num_size, infinity::SizeT loser_num, infinity::u64 max_val) {
    numbers.clear();
    loser.clear();
    num_idx.clear();

    loser.resize(loser_num);
    num_idx.resize(loser_num, 0);

    for (SizeT i = 0; i < num_size; ++i) {
        auto val = GetRandom(max_val);
        numbers.emplace_back(val);
        loser[GetRandom(loser_num)].emplace_back(val);
    }
    for (SizeT i = 0; i < loser_num; ++i) {
        std::sort(loser[i].begin(), loser[i].end());
    }
}

void LoserTreeTest::MultiWayMerge(infinity::SizeT num_size, infinity::SizeT loser_num, infinity::u64 max_val) {
    auto loser_tree = MakeShared<LoserTree<u64, std::less<u64>>>(loser_num);
    for (SizeT i = 0; i < loser_num; ++i) {
        if (!loser[i].empty()) {
            loser_tree->InsertStart(&loser[i][num_idx[i]], static_cast<LoserTree<u64>::Source>(i), false);
            ++num_idx[i];
        } else {
            loser_tree->InsertStart(nullptr, static_cast<LoserTree<u64>::Source>(i), true);
            ++num_idx[i];
        }
    }
    loser_tree->Init();
    Vector<u64> merge_res;
    while (loser_tree->TopSource() != LoserTree<u64>::invalid_) {
        auto min_value = loser_tree->TopKey();
        auto min_source = loser_tree->TopSource();
        merge_res.push_back(min_value);
        auto& min_seq = num_idx[min_source];

        if (min_seq < loser[min_source].size()) {
            loser_tree->DeleteTopInsert(&(loser[min_source][min_seq]), false);
        } else {
            loser_tree->DeleteTopInsert(nullptr, true);
        }
        min_seq++;
    }
    sort(numbers.begin(), numbers.end());
    EXPECT_EQ(numbers.size(), merge_res.size());
    for (SizeT i = 0; i < merge_res.size(); ++i) {
        EXPECT_EQ(merge_res[i], numbers[i]);
    }
}

TEST_F(LoserTreeTest, BasicMerge1) {
    const SizeT num_size = 10;
    const SizeT loser_num = 3;
    const u64 max_val = 30;
    GenerateData(num_size, loser_num, max_val);
    MultiWayMerge(num_size, loser_num, max_val);
}

TEST_F(LoserTreeTest, BasicMerge2) {
    const SizeT num_size = 10;
    const SizeT loser_num = 12;
    const u64 max_val = 30;
    GenerateData(num_size, loser_num, max_val);
    MultiWayMerge(num_size, loser_num, max_val);
}

TEST_F(LoserTreeTest, BasicMerge3) {
    const SizeT num_size = 1000;
    const SizeT loser_num = 200;
    const u64 max_val = 10000;
    GenerateData(num_size, loser_num, max_val);
    MultiWayMerge(num_size, loser_num, max_val);
}

TEST_F(LoserTreeTest, BasicMerge4) {
    const SizeT num_size = 1000;
    const SizeT loser_num = 2000;
    const u64 max_val = 10000;
    GenerateData(num_size, loser_num, max_val);
    MultiWayMerge(num_size, loser_num, max_val);
}