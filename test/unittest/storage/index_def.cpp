#include "parser/definition/ivfflat_index_def.h"
#include <base_test.h>
#include <gtest/gtest.h>
class IndexDefTest : public BaseTest {};

TEST_F(IndexDefTest, ReadWrite) {
    using namespace infinity;

    Vector<String> columns{"col1", "col2"};
    Vector<InitParameter *> parameters;
    parameters.emplace_back(new InitParameter("centroids_count", "100"));
    parameters.emplace_back(new InitParameter("metric", "l2"));
    auto index_def = IVFFlatIndexDef::Make(MakeShared<String>("idx1"), IndexMethod::kIVFFlat, columns, parameters);
    std::cout << "index_def: " << index_def->ToString() << std::endl;

    int32_t exp_size = index_def->GetSizeInBytes();
    std::vector<char> buf(exp_size, char(0));
    char *buf_beg = buf.data();
    char *ptr = buf_beg;
    index_def->WriteAdv(ptr);
    EXPECT_EQ(ptr - buf_beg, exp_size);

    ptr = buf_beg;
    int32_t maxbytes = exp_size;
    SharedPtr<IndexDef> index_def1 = IndexDef::ReadAdv(ptr, maxbytes);
    std::cout << "index_def1: " << index_def1->ToString() << std::endl;
    EXPECT_EQ(ptr - buf_beg, exp_size);
    EXPECT_NE(index_def1.get(), nullptr);
    EXPECT_EQ(*index_def, *index_def1);
}