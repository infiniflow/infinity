#include "base_test.h"
#include "storage/invertedindex/common/fst/fst_builder.h"
#include "storage/invertedindex/common/fst/fst_decl.h"
#include "storage/invertedindex/common/fst/immutable_fst.h"
#include "storage/invertedindex/common/fst/string_weight.h"
#include "storage/invertedindex/common/fst/stringref_weight.h"

#include <string>

class FSTBuilderTest : public BaseTest {
    void SetUp() override {}

    void TearDown() override {}
};

using namespace infinity;
using namespace fst;

TEST_F(FSTBuilderTest, test1) {}
