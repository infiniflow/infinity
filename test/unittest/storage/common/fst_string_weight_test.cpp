#include "base_test.h"
#include "storage/common/fst/string_weight.h"

#include <string>

class FSTStringWeightTest : public BaseTest {
    void SetUp() override {}

    void TearDown() override {}
};

using namespace infinity;
using namespace fst;

TEST_F(FSTStringWeightTest, test1) {
    ASSERT_EQ("left_string", StringLeftWeight<char>::Type());
    ASSERT_EQ(StringLeftWeight<char>(kStringInfinity), StringLeftWeight<char>::Zero());
    ASSERT_TRUE(StringLeftWeight<char>::Zero().Member());
    ASSERT_EQ(StringLeftWeight<char>(), StringLeftWeight<char>::One());
    ASSERT_TRUE(StringLeftWeight<char>::One().Member());
    ASSERT_EQ(StringLeftWeight<char>(kStringBad), StringLeftWeight<char>::NoWeight());
    ASSERT_FALSE(StringLeftWeight<char>::NoWeight().Member());
}

TEST_F(FSTStringWeightTest, test2) {
    StringLeftWeight<char> w;
    ASSERT_TRUE(w.Empty());
    ASSERT_EQ(0, w.Size());
}

TEST_F(FSTStringWeightTest, test3) {
    std::stringstream ss;

    StringLeftWeight<uint8_t> w0;
    w0.PushBack(1);
    w0.PushBack(char(255));
    w0.PushBack(3);
    w0.PushBack(3);
    w0.PushBack(char(129));
    w0.Write(ss);
    ASSERT_EQ(5, w0.Size());
    ASSERT_TRUE(w0.Member());

    ss.seekg(0);
    StringLeftWeight<uint8_t> w1;
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

        const StringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<char> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(lhs_weight, rhs_weight));
    }

    // weight + weight
    {
        const std::string lhs = "124";
        const std::string rhs = "123456";
        const std::string prefix = "12";

        const StringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<char> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(lhs_weight, rhs_weight));
    }

    // weight + Zero()
    {
        const std::string lhs = "123456";
        const std::string prefix = "";

        const StringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<char> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(lhs_weight, Plus(lhs_weight, StringLeftWeight<char>::Zero()));
    }

    // Zero() + weight
    {
        const std::string rhs = "123456";
        const std::string prefix = "";

        const StringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<char> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(rhs_weight, Plus(StringLeftWeight<char>::Zero(), rhs_weight));
    }

    // weight + NoWeight()
    {
        const std::string lhs = "123456";

        const StringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(StringLeftWeight<char>::NoWeight(), Plus(lhs_weight, StringLeftWeight<char>::NoWeight()));
    }

    // NoWeight() + weight
    {
        const std::string rhs = "123456";

        const StringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        ASSERT_EQ(StringLeftWeight<char>::NoWeight(), Plus(StringLeftWeight<char>::NoWeight(), rhs_weight));
    }

    // weight + One()
    {
        const std::string lhs = "123456";
        const std::string prefix = "";

        const StringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<char> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(lhs_weight, StringLeftWeight<char>::One()));
    }

    // One() + weight
    {
        const std::string rhs = "123456";
        const std::string prefix = "";

        const StringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<char> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(StringLeftWeight<char>::One(), rhs_weight));
    }
}

TEST_F(FSTStringWeightTest, test5) {
    // weight * weight
    {
        const std::string lhs = "123456";
        const std::string rhs = "124";
        const std::string result = "123456124";

        const StringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Times(lhs_weight, rhs_weight));
    }

    // weight * weight
    {
        const std::string lhs = "124";
        const std::string rhs = "123456";
        const std::string result = "124123456";

        const StringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Times(lhs_weight, rhs_weight));
    }

    // weight * Zero()
    {
        const std::string lhs = "123456";
        const std::string result = "";

        const StringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(StringLeftWeight<char>::Zero(), Times(lhs_weight, StringLeftWeight<char>::Zero()));
    }

    // Zero() * weight
    {
        const std::string rhs = "123456";
        const std::string result = "";

        const StringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(StringLeftWeight<char>::Zero(), Times(StringLeftWeight<char>::Zero(), rhs_weight));
    }

    // weight * NoWeight()
    {
        const std::string lhs = "123456";

        const StringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(StringLeftWeight<char>::NoWeight(), Times(lhs_weight, StringLeftWeight<char>::NoWeight()));
    }

    // NoWeight() * weight
    {
        const std::string rhs = "123456";

        const StringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        ASSERT_EQ(StringLeftWeight<char>::NoWeight(), Times(StringLeftWeight<char>::NoWeight(), rhs_weight));
    }

    // weight * One()
    {
        const std::string lhs = "123456";
        const std::string result = "123456";

        const StringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Times(lhs_weight, StringLeftWeight<char>::One()));
    }

    // One() + weight
    {
        const std::string rhs = "123456";
        const std::string result = "123456";

        const StringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Times(StringLeftWeight<char>::One(), rhs_weight));
    }
}

TEST_F(FSTStringWeightTest, test6) {
    // weight / weight
    {
        const std::string lhs = "123456";
        const std::string rhs = "12";
        const std::string result = "3456";

        const StringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(lhs_weight, rhs_weight, DIVIDE_LEFT));
    }

    // weight / weight
    {
        const std::string lhs = "124";
        const std::string rhs = "123456";
        const std::string result = "";

        const StringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(lhs_weight, rhs_weight, DIVIDE_LEFT));
    }

    // weight / Zero()
    {
        const std::string lhs = "123456";

        const StringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(StringLeftWeight<char>::NoWeight(), Divide(lhs_weight, StringLeftWeight<char>::Zero(), DIVIDE_LEFT));
    }

    // Zero() / weight
    {
        const std::string rhs = "123456";

        const StringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        ASSERT_EQ(StringLeftWeight<char>::Zero(), Divide(StringLeftWeight<char>::Zero(), rhs_weight, DIVIDE_LEFT));
    }

    // weight / NoWeight()
    {
        const std::string lhs = "123456";

        const StringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(StringLeftWeight<char>::NoWeight(), Divide(lhs_weight, StringLeftWeight<char>::NoWeight(), DIVIDE_LEFT));
    }

    // NoWeight() / weight
    {
        const std::string rhs = "123456";

        const StringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        ASSERT_EQ(StringLeftWeight<char>::NoWeight(), Divide(StringLeftWeight<char>::NoWeight(), rhs_weight, DIVIDE_LEFT));
    }

    // weight / One()
    {
        const std::string lhs = "123456";
        const std::string result = "123456";

        const StringLeftWeight<char> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(lhs_weight, StringLeftWeight<char>::One(), DIVIDE_LEFT));
    }

    // One() / weight
    {
        const std::string rhs = "123456";
        const std::string result = "";

        const StringLeftWeight<char> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<char> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(StringLeftWeight<char>::One(), rhs_weight, DIVIDE_LEFT));
    }
}

TEST_F(FSTStringWeightTest, test7) {
    ASSERT_EQ("left_string", StringLeftWeight<byte_type>::Type());
    ASSERT_TRUE(StringLeftWeight<byte_type>::Zero().Empty());
    ASSERT_TRUE(StringLeftWeight<byte_type>::Zero().Member());
    ASSERT_EQ(StringLeftWeight<byte_type>::One(), StringLeftWeight<byte_type>::Zero());
    ASSERT_EQ(StringLeftWeight<byte_type>::NoWeight(), StringLeftWeight<byte_type>::Zero());
}

TEST_F(FSTStringWeightTest, test8) {
    // weight + weight
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> rhs{1, 2, 4};
        const std::vector<byte_type> prefix{1, 2};

        const StringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<byte_type> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(lhs_weight, rhs_weight));
    }

    // weight + weight
    {
        const std::vector<byte_type> lhs{1, 2, 4};
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> prefix{1, 2};

        const StringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<byte_type> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(lhs_weight, rhs_weight));
    }

    // weight + Zero()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> prefix{};

        const StringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<byte_type> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(lhs_weight, StringLeftWeight<byte_type>::Zero()));
    }

    // Zero() + weight
    {
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> prefix{};

        const StringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<byte_type> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(StringLeftWeight<byte_type>::Zero(), rhs_weight));
    }

    // weight + NoWeight()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> prefix{};

        const StringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<byte_type> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(lhs_weight, StringLeftWeight<byte_type>::NoWeight()));
    }

    // NoWeight() + weight
    {
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> prefix{};

        const StringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<byte_type> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(StringLeftWeight<byte_type>::NoWeight(), rhs_weight));
    }

    // weight + One()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> prefix{};

        const StringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<byte_type> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(lhs_weight, StringLeftWeight<byte_type>::One()));
    }

    // One() + weight
    {
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> prefix{};

        const StringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<byte_type> prefix_weight(prefix.begin(), prefix.end());
        ASSERT_EQ(prefix_weight, Plus(StringLeftWeight<byte_type>::One(), rhs_weight));
    }
}

TEST_F(FSTStringWeightTest, test9) {
    // weight * weight
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> rhs{1, 2, 4};
        const std::vector<byte_type> result{1, 2, 3, 4, 5, 6, 1, 2, 4};

        const StringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<byte_type> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Times(lhs_weight, rhs_weight));
    }

    // weight * weight
    {
        const std::vector<byte_type> lhs{1, 2, 4};
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> result{1, 2, 4, 1, 2, 3, 4, 5, 6};

        const StringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<byte_type> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Times(lhs_weight, rhs_weight));
    }

    // weight * Zero()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};

        const StringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(lhs_weight, Times(lhs_weight, StringLeftWeight<byte_type>::Zero()));
    }

    // Zero() * weight
    {
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};

        const StringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        ASSERT_EQ(rhs_weight, Times(StringLeftWeight<byte_type>::Zero(), rhs_weight));
    }

    // weight * NoWeight()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};

        const StringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(lhs_weight, Times(lhs_weight, StringLeftWeight<byte_type>::NoWeight()));
    }

    // NoWeight() * weight
    {
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};

        const StringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        ASSERT_EQ(rhs_weight, Times(StringLeftWeight<byte_type>::NoWeight(), rhs_weight));
    }

    // weight * One()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};

        const StringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(lhs_weight, Times(lhs_weight, StringLeftWeight<byte_type>::One()));
    }

    // One() * weight
    {
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};

        const StringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        ASSERT_EQ(rhs_weight, Times(StringLeftWeight<byte_type>::One(), rhs_weight));
    }
}

TEST_F(FSTStringWeightTest, test10) {
    // weight / weight
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> rhs{1, 2};
        const std::vector<byte_type> result{3, 4, 5, 6};

        const StringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<byte_type> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(lhs_weight, rhs_weight, DIVIDE_LEFT));
    }

    // weight / weight
    {
        const std::vector<byte_type> lhs{1, 2};
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> result{};

        const StringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<byte_type> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(lhs_weight, rhs_weight, DIVIDE_LEFT));
    }

    // weight / Zero()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};

        const StringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(lhs_weight, Divide(lhs_weight, StringLeftWeight<byte_type>::Zero(), DIVIDE_LEFT));
    }

    // Zero() / weight
    {
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> result{};

        const StringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<byte_type> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(StringLeftWeight<byte_type>::Zero(), rhs_weight, DIVIDE_LEFT));
    }

    // weight / NoWeight()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};

        const StringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        ASSERT_EQ(lhs_weight, Divide(lhs_weight, StringLeftWeight<byte_type>::NoWeight(), DIVIDE_LEFT));
    }

    // NoWeight() / weight
    {
        const std::vector<byte_type> rhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> result{};

        const StringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<byte_type> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(StringLeftWeight<byte_type>::NoWeight(), rhs_weight, DIVIDE_LEFT));
    }

    // weight / One()
    {
        const std::vector<byte_type> lhs{1, 2, 3, 4, 5, 6};
        const std::vector<byte_type> result{1, 2, 3, 4, 5, 6};

        const StringLeftWeight<byte_type> lhs_weight(lhs.begin(), lhs.end());
        const StringLeftWeight<byte_type> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(lhs_weight, StringLeftWeight<byte_type>::One(), DIVIDE_LEFT));
    }

    // One() / weight
    {
        const std::string rhs = "123456";
        const std::vector<byte_type> result{};

        const StringLeftWeight<byte_type> rhs_weight(rhs.begin(), rhs.end());
        const StringLeftWeight<byte_type> result_weight(result.begin(), result.end());
        ASSERT_EQ(result_weight, Divide(StringLeftWeight<byte_type>::One(), rhs_weight, DIVIDE_LEFT));
    }
}
