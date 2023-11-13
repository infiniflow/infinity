#include "unit_test/base_test.h"

#include "storage/invertedindex/common/openfst/fst_string_weight.h"

#include <string>

class FSTStringWeightTest : public BaseTest {
    void SetUp() override {}

    void TearDown() override {}
};

using namespace infinity;
using namespace fst;

TEST_F(FSTStringWeightTest, test1) {
    ASSERT_EQ("left_string", FSTStringLeftWeight<char>::Type());
    ASSERT_EQ(FSTStringLeftWeight<char>(kStringInfinity), FSTStringLeftWeight<char>::Zero());
    ASSERT_TRUE(FSTStringLeftWeight<char>::Zero().Member());
    ASSERT_EQ(FSTStringLeftWeight<char>(), FSTStringLeftWeight<char>::One());
    ASSERT_TRUE(FSTStringLeftWeight<char>::One().Member());
    ASSERT_EQ(FSTStringLeftWeight<char>(kStringBad), FSTStringLeftWeight<char>::NoWeight());
    ASSERT_FALSE(FSTStringLeftWeight<char>::NoWeight().Member());
}

TEST_F(FSTStringWeightTest, test2) {
    FSTStringLeftWeight<char> w;
    ASSERT_TRUE(w.Empty());
    ASSERT_EQ(0, w.Size());
}

TEST_F(FSTStringWeightTest, test3) {
    std::stringstream ss;

    FSTStringLeftWeight<byte_type> w0;
    w0.PushBack(1);
    w0.PushBack(char(255));
    w0.PushBack(3);
    w0.PushBack(3);
    w0.PushBack(char(129));
    w0.Write(ss);
    ASSERT_EQ(5, w0.Size());
    ASSERT_TRUE(w0.Member());

    ss.seekg(0);
    FSTStringLeftWeight<byte_type> w1;
    w1.Read(ss);
    // ASSERT_EQ(w0, w1);
    // ASSERT_EQ(w0.Hash(), w1.Hash());
}

TEST_F(FSTStringWeightTest, test4) {
    // weight + weight
    {
        const std::string lhs = "123456";
        const std::string rhs = "124";
        const std::string prefix = "12";

        const FSTStringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<char> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(lhs_weight, rhs_weight));
    }

    // weight + weight
    {
        const std::string lhs = "124";
        const std::string rhs = "123456";
        const std::string prefix = "12";

        const FSTStringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<char> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(lhs_weight, rhs_weight));
    }

    // weight + Zero()
    {
        const std::string lhs = "123456";
        const std::string prefix = "";

        const FSTStringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<char> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(lhs_weight, Plus(lhs_weight, FSTStringLeftWeight<char>::Zero()));
    }

    // Zero() + weight
    {
        const std::string rhs = "123456";
        const std::string prefix = "";

        const FSTStringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<char> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(rhs_weight, Plus(FSTStringLeftWeight<char>::Zero(), rhs_weight));
    }

    // weight + NoWeight()
    {
        const std::string lhs = "123456";

        const FSTStringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(FSTStringLeftWeight<char>::NoWeight(), Plus(lhs_weight, FSTStringLeftWeight<char>::NoWeight()));
    }

    // NoWeight() + weight
    {
        const std::string rhs = "123456";

        const FSTStringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        ASSERT_EQ(FSTStringLeftWeight<char>::NoWeight(), Plus(FSTStringLeftWeight<char>::NoWeight(), rhs_weight));
    }

    // weight + One()
    {
        const std::string lhs = "123456";
        const std::string prefix = "";

        const FSTStringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<char> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(lhs_weight, FSTStringLeftWeight<char>::One()));
    }

    // One() + weight
    {
        const std::string rhs = "123456";
        const std::string prefix = "";

        const FSTStringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<char> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(FSTStringLeftWeight<char>::One(), rhs_weight));
    }
}

TEST_F(FSTStringWeightTest, test5) {
    // weight * weight
    {
        const std::string lhs = "123456";
        const std::string rhs = "124";
        const std::string result = "123456124";

        const FSTStringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Times(lhs_weight, rhs_weight));
    }

    // weight * weight
    {
        const std::string lhs = "124";
        const std::string rhs = "123456";
        const std::string result = "124123456";

        const FSTStringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Times(lhs_weight, rhs_weight));
    }

    // weight * Zero()
    {
        const std::string lhs = "123456";
        const std::string result = "";

        const FSTStringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(FSTStringLeftWeight<char>::Zero(), Times(lhs_weight, FSTStringLeftWeight<char>::Zero()));
    }

    // Zero() * weight
    {
        const std::string rhs = "123456";
        const std::string result = "";

        const FSTStringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(FSTStringLeftWeight<char>::Zero(), Times(FSTStringLeftWeight<char>::Zero(), rhs_weight));
    }

    // weight * NoWeight()
    {
        const std::string lhs = "123456";

        const FSTStringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(FSTStringLeftWeight<char>::NoWeight(), Times(lhs_weight, FSTStringLeftWeight<char>::NoWeight()));
    }

    // NoWeight() * weight
    {
        const std::string rhs = "123456";

        const FSTStringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        ASSERT_EQ(FSTStringLeftWeight<char>::NoWeight(), Times(FSTStringLeftWeight<char>::NoWeight(), rhs_weight));
    }

    // weight * One()
    {
        const std::string lhs = "123456";
        const std::string result = "123456";

        const FSTStringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Times(lhs_weight, FSTStringLeftWeight<char>::One()));
    }

    // One() + weight
    {
        const std::string rhs = "123456";
        const std::string result = "123456";

        const FSTStringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Times(FSTStringLeftWeight<char>::One(), rhs_weight));
    }
}

TEST_F(FSTStringWeightTest, test6) {
    // weight / weight
    {
        const std::string lhs = "123456";
        const std::string rhs = "12";
        const std::string result = "3456";

        const FSTStringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(lhs_weight, rhs_weight, DIVIDE_LEFT));
    }

    // weight / weight
    {
        const std::string lhs = "124";
        const std::string rhs = "123456";
        const std::string result = "";

        const FSTStringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(lhs_weight, rhs_weight, DIVIDE_LEFT));
    }

    // weight / Zero()
    {
        const std::string lhs = "123456";

        const FSTStringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(FSTStringLeftWeight<char>::NoWeight(), Divide(lhs_weight, FSTStringLeftWeight<char>::Zero(), DIVIDE_LEFT));
    }

    // Zero() / weight
    {
        const std::string rhs = "123456";

        const FSTStringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        ASSERT_EQ(FSTStringLeftWeight<char>::Zero(), Divide(FSTStringLeftWeight<char>::Zero(), rhs_weight, DIVIDE_LEFT));
    }

    // weight / NoWeight()
    {
        const std::string lhs = "123456";

        const FSTStringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(FSTStringLeftWeight<char>::NoWeight(), Divide(lhs_weight, FSTStringLeftWeight<char>::NoWeight(), DIVIDE_LEFT));
    }

    // NoWeight() / weight
    {
        const std::string rhs = "123456";

        const FSTStringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        ASSERT_EQ(FSTStringLeftWeight<char>::NoWeight(), Divide(FSTStringLeftWeight<char>::NoWeight(), rhs_weight, DIVIDE_LEFT));
    }

    // weight / One()
    {
        const std::string lhs = "123456";
        const std::string result = "123456";

        const FSTStringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(lhs_weight, FSTStringLeftWeight<char>::One(), DIVIDE_LEFT));
    }

    // One() / weight
    {
        const std::string rhs = "123456";
        const std::string result = "";

        const FSTStringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(FSTStringLeftWeight<char>::One(), rhs_weight, DIVIDE_LEFT));
    }
}

TEST_F(FSTStringWeightTest, test7) {
    ASSERT_EQ("left_string", FSTStringLeftWeight<byte_type>::Type());
    ASSERT_TRUE(FSTStringLeftWeight<byte_type>::Zero().Empty());
    ASSERT_TRUE(FSTStringLeftWeight<byte_type>::Zero().Member());
    ASSERT_EQ(FSTStringLeftWeight<byte_type>::One(), FSTStringLeftWeight<byte_type>::Zero());
    ASSERT_EQ(FSTStringLeftWeight<byte_type>::NoWeight(), FSTStringLeftWeight<byte_type>::Zero());
}

TEST_F(FSTStringWeightTest, test8) {
    // weight + weight
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> rhs{1, 2, 4};
        const std::vector<byte_type> prefix{1, 2};

        const FSTStringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<byte_type> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(lhs_weight, rhs_weight));
    }

    // weight + weight
    {
        const std::vector<byte_type> lhs{1, 2, 4};
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> prefix{1, 2};

        const FSTStringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<byte_type> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(lhs_weight, rhs_weight));
    }

    // weight + Zero()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> prefix{};

        const FSTStringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<byte_type> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(lhs_weight, FSTStringLeftWeight<byte_type>::Zero()));
    }

    // Zero() + weight
    {
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> prefix{};

        const FSTStringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<byte_type> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(FSTStringLeftWeight<byte_type>::Zero(), rhs_weight));
    }

    // weight + NoWeight()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> prefix{};

        const FSTStringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<byte_type> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(lhs_weight, FSTStringLeftWeight<byte_type>::NoWeight()));
    }

    // NoWeight() + weight
    {
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> prefix{};

        const FSTStringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<byte_type> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(FSTStringLeftWeight<byte_type>::NoWeight(), rhs_weight));
    }

    // weight + One()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> prefix{};

        const FSTStringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<byte_type> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(lhs_weight, FSTStringLeftWeight<byte_type>::One()));
    }

    // One() + weight
    {
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> prefix{};

        const FSTStringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<byte_type> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(FSTStringLeftWeight<byte_type>::One(), rhs_weight));
    }
}

TEST_F(FSTStringWeightTest, test9) {
    // weight * weight
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> rhs{1, 2, 4};
        const std::vector<byte_type> result{1, 2, 3, 4, 5, 6, 1, 2, 4};

        const FSTStringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<byte_type> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Times(lhs_weight, rhs_weight));
    }

    // weight * weight
    {
        const std::vector<byte_type> lhs{1, 2, 4};
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> result{1, 2, 4, 1, 2, 3, 4, 5, 6};

        const FSTStringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<byte_type> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Times(lhs_weight, rhs_weight));
    }

    // weight * Zero()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};

        const FSTStringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(lhs_weight, Times(lhs_weight, FSTStringLeftWeight<byte_type>::Zero()));
    }

    // Zero() * weight
    {
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};

        const FSTStringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        ASSERT_EQ(rhs_weight, Times(FSTStringLeftWeight<byte_type>::Zero(), rhs_weight));
    }

    // weight * NoWeight()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};

        const FSTStringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(lhs_weight, Times(lhs_weight, FSTStringLeftWeight<byte_type>::NoWeight()));
    }

    // NoWeight() * weight
    {
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};

        const FSTStringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        ASSERT_EQ(rhs_weight, Times(FSTStringLeftWeight<byte_type>::NoWeight(), rhs_weight));
    }

    // weight * One()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};

        const FSTStringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(lhs_weight, Times(lhs_weight, FSTStringLeftWeight<byte_type>::One()));
    }

    // One() * weight
    {
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};

        const FSTStringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        ASSERT_EQ(rhs_weight, Times(FSTStringLeftWeight<byte_type>::One(), rhs_weight));
    }
}

TEST_F(FSTStringWeightTest, test10) {
    // weight / weight
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> rhs{1, 2};
        const std::vector<byte_type> result{3, 4, 5, 6};

        const FSTStringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<byte_type> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(lhs_weight, rhs_weight, DIVIDE_LEFT));
    }

    // weight / weight
    {
        const std::vector<byte_type> lhs{1, 2};
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> result{};

        const FSTStringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<byte_type> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(lhs_weight, rhs_weight, DIVIDE_LEFT));
    }

    // weight / Zero()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};

        const FSTStringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(lhs_weight, Divide(lhs_weight, FSTStringLeftWeight<byte_type>::Zero(), DIVIDE_LEFT));
    }

    // Zero() / weight
    {
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> result{};

        const FSTStringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<byte_type> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(FSTStringLeftWeight<byte_type>::Zero(), rhs_weight, DIVIDE_LEFT));
    }

    // weight / NoWeight()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};

        const FSTStringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(lhs_weight, Divide(lhs_weight, FSTStringLeftWeight<byte_type>::NoWeight(), DIVIDE_LEFT));
    }

    // NoWeight() / weight
    {
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> result{};

        const FSTStringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<byte_type> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(FSTStringLeftWeight<byte_type>::NoWeight(), rhs_weight, DIVIDE_LEFT));
    }

    // weight / One()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> result{1, 2, 3, 4, 5, 6};

        const FSTStringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const FSTStringLeftWeight<byte_type> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(lhs_weight, FSTStringLeftWeight<byte_type>::One(), DIVIDE_LEFT));
    }

    // One() / weight
    {
        const std::string rhs = "123456";
        const std::vector<byte_type> result{};

        const FSTStringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const FSTStringLeftWeight<byte_type> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(FSTStringLeftWeight<byte_type>::One(), rhs_weight, DIVIDE_LEFT));
    }
}
