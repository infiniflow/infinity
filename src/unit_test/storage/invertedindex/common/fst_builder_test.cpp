#include "unit_test/base_test.h"

#include "storage/invertedindex/common/openfst/fst_builder.h"
#include "storage/invertedindex/common/openfst/fst_decl.h"
#include "storage/invertedindex/common/openfst/fst_immutable.h"
#include "storage/invertedindex/common/openfst/fst_string_weight.h"
#include "storage/invertedindex/common/openfst/fst_stringref_weight.h"

#include <string>

class FSTBuilderTest : public BaseTest {
    void SetUp() override {}

    void TearDown() override {}
};

using namespace infinity;
using namespace fst;

TEST_F(FSTBuilderTest, test1) {}
