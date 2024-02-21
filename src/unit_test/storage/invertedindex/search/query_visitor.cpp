
#include "unit_test/base_test.h"

import stl;
import query_visitor;
import term_queries;

using namespace infinity;

class QueryVisitorTest : public BaseTest {
public:
    QueryVisitorTest() {}

    void SetUp() override {}

    void TearDown() override {}
};

template <typename Query>
void Optimize(UniquePtr<Query> &ref) {
    auto opt = TermQuery::Optimize(std::move(ref));
    ref.reset(dynamic_cast<Query *>(opt.get()));
    opt.release();
}

TEST_F(QueryVisitorTest, test1) {
    auto root = MakeUnique<AndQuery>();
    {
        root->AddChild(MakeUnique<TermQuery>());
        root->AddChild(MakeUnique<TermQuery>());
    }
    auto query = MakeUnique<AndQuery>();
    {
        query->AddChild(MakeUnique<TermQuery>());
        query->AddChild(MakeUnique<TermQuery>());
    }
    root->AddChild(UniquePtr<TermQuery>(query.release()));
    Optimize(root);
    ASSERT_EQ(root->GetChildrenCount(), 4);
}