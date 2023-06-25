#include "storage/indexstore/bkd/bkd_writer.h"
#include "storage/indexstore/bkd/bkd_reader.h"
#include "storage/io/file_writer.h"
#include "storage/io/file_reader.h"
#include "storage/io/file_system.h"
#include "storage/io/local_file_system.h"

#include <iostream>

using namespace infinity;

void IntToSortableBytes(int value, std::vector<uint8_t> &result, int offset) {
    value ^= 0x80000000;
    result[offset] = static_cast<char>(value >> 24);
    result[offset + 1] = static_cast<char>(value >> 16);
    result[offset + 2] = static_cast<char>(value >> 8);
    result[offset + 3] = static_cast<char>(value);
}

int main(){

    LocalFileSystem local_file_system;
    String data_path = "./test_file1.data";
    String index_path = "./test_file1.idx";
    String meta_path = "./test_file1.meta";
    FileWriter data_writer(local_file_system, data_path, 1024);
	FileWriter index_writer(local_file_system, index_path, 1024);
	FileWriter meta_writer(local_file_system, meta_path, 1024);

	int32_t max_doc = 0;
	size_t bytes_per_dim = sizeof(int32_t);
	int32_t total_point_count = std::numeric_limits<std::int32_t>::max();
	const int DIMS = 1;
	const int32_t MAX_LEAF_COUNT = 1024;
	const float MAXMBSortInHeap = 512.0 * 8;
	const int32_t MAX_DEPTH_IN_BKD_TREE = 15;

	std::shared_ptr<BKDWriter> bkd_writer = std::make_shared<BKDWriter>(
                max_doc, DIMS, DIMS, bytes_per_dim, MAX_LEAF_COUNT, MAXMBSortInHeap,
                total_point_count, true, MAX_DEPTH_IN_BKD_TREE);;

	const int N = 1024 * 1024;
	for (int docID = 0; docID < N; docID++) {
        std::vector<uint8_t> scratch(4);

        if (docID > 500000) {
            IntToSortableBytes(200, scratch, 0);

        } else {
            IntToSortableBytes(100, scratch, 0);
        }
        bkd_writer->Add(scratch.data(), scratch.size(), docID);
    }
    int64_t index_fp = bkd_writer->Finish(&data_writer, &index_writer);
    bkd_writer->MetaFinish(&meta_writer, index_fp, 0);
    data_writer.Sync();
    index_writer.Sync();
    meta_writer.Sync();
    return 0;
}

