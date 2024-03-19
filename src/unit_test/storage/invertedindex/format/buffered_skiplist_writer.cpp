#include "unit_test/base_test.h"

import stl;
import memory_pool;
import buffered_skiplist_writer;
import buffered_byte_slice_reader;

using namespace infinity;

class BufferedSkipListWriterTest : public BaseTest {
public:
    BufferedSkipListWriterTest() {
        byte_slice_pool_ = new MemoryPool(1024);
        buffer_pool_ = new RecyclePool(1024);
    }
    ~BufferedSkipListWriterTest() {
        delete byte_slice_pool_;
        delete buffer_pool_;
    }
    void SetUp() override {}

    void TearDown() override { buffered_skiplist_writer_.reset(); }

protected:
    MemoryPool *byte_slice_pool_;
    RecyclePool *buffer_pool_;
    SharedPtr<BufferedSkipListWriter> buffered_skiplist_writer_;
};

TEST_F(BufferedSkipListWriterTest, test1) {
    using namespace infinity;

    // TODO yzc:
    // invokes AddItem(u32 key) SKIP_LIST_BUFFER_SIZE+2 times
    // Dump()
    // check EstimateDumpSize()
    // read back via BufferedByteSliceReader
    // check the values
}

TEST_F(BufferedSkipListWriterTest, test2) {
    using namespace infinity;

    // TODO yzc:
    // invokes AddItem(u32 key, u32 value1) SKIP_LIST_BUFFER_SIZE+2 times
    // Dump()
    // check EstimateDumpSize()
    // read back via BufferedByteSliceReader
    // check the values
}

TEST_F(BufferedSkipListWriterTest, test3) {
    using namespace infinity;

    // TODO yzc:
    // invokes AddItem(u32 key, u32 value1, u32 value2) SKIP_LIST_BUFFER_SIZE+2 times
    // Dump()
    // check EstimateDumpSize()
    // read back via BufferedByteSliceReader
    // check the values
}
