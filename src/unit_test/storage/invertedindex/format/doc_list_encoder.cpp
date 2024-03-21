#include "unit_test/base_test.h"

import stl;
import memory_pool;
import posting_byte_slice;
import posting_byte_slice_reader;
import doc_list_format_option;
import index_defines;
import flush_info;

using namespace infinity;

class DocListEncoderTest : public BaseTest {
public:
    DocListEncoderTest() {}
    ~DocListEncoderTest() {}
    void SetUp() override {}

    void TearDown() override {}

protected:
};

TEST_F(DocListEncoderTest, test1) {
    using namespace infinity;

    // TODO yzc:
    // invoke AddPosition(), EndDocument() multiple times
    // invoke GetInMemDocListDecoder(), check the values
    // invode Dump()
    // read back via Load() (currently absent), and check the values
}
