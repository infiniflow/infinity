
#ifdef CI
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.posting_merger;

import :ut.base_test;
import :posting_merger;
import :stl;
import :segment_term_posting;
import :memory_indexer;
import :column_vector;
import :value;
import :column_inverter;
import :index_defines;
import :column_index_reader;
import :posting_iterator;
import :virtual_store;
import :file_writer;
import :term_meta;
import :index_defines;
import :posting_list_format;
import :column_length_io;
import :posting_decoder;
import :column_index_merger;
import :infinity_exception;
import :vector_with_lock;
import :logger;
import :infinity_context;
import :persistence_manager;
import :persist_result_handler;
import :local_file_handle;
#endif

import data_type;
import internal_types;
import logical_type;

using namespace infinity;

class PostingMergerTest : public BaseTestParamStr {
public:
    PostingMergerTest() {}
    ~PostingMergerTest() {}

public:
    struct ExpectedPosting {
        String term;
        Vector<RowID> doc_ids;
        Vector<u32> tfs;
    };

protected:
    void CreateIndex();

protected:
    optionflag_t flag_{OPTION_FLAG_ALL};
    static constexpr SizeT BUFFER_SIZE_ = 1024;
    String config_path_{};
};

INSTANTIATE_TEST_SUITE_P(TestWithDifferentParams,
                         PostingMergerTest,
                         ::testing::Values(BaseTestParamStr::NULL_CONFIG_PATH, BaseTestParamStr::VFS_OFF_CONFIG_PATH));

void PostingMergerTest::CreateIndex() {

    const char *paragraphs[] = {
        R"#(A)#",
        R"#(A A)#",
    };
    const SizeT num_paragraph = sizeof(paragraphs) / sizeof(char *);

    SharedPtr<ColumnVector> column = ColumnVector::Make(MakeShared<DataType>(LogicalType::kVarchar));
    column->Initialize();
    for (SizeT i = 0; i < num_paragraph; ++i) {
        Value v = Value::MakeVarchar(String(paragraphs[i]));
        column->AppendValue(v);
    }

    MemoryIndexer indexer1(GetFullDataDir(), "chunk1", RowID(0U, 0U), flag_, "standard");
    indexer1.Insert(column, 0, 1);
    indexer1.Dump();

    MemoryIndexer indexer2(GetFullDataDir(), "chunk2", RowID(0U, 1U), flag_, "standard");
    indexer2.Insert(column, 1, 1);
    indexer2.Dump();
}

TEST_P(PostingMergerTest, Basic) {
    // using namespace infinity;
    CreateIndex();

    const String index_dir = GetFullDataDir();

    String dst_base_name = "merged_index";
    Path path = Path(index_dir) / dst_base_name;
    String index_prefix = path.string();
    String dict_file = index_prefix;
    dict_file.append(DICT_SUFFIX);

    String posting_file = path.string();
    posting_file.append(POSTING_SUFFIX);

    Vector<String> base_names = {"chunk1", "chunk2"};
    Vector<RowID> row_ids = {RowID{0U, 0U}, RowID{0U, 1U}};
    Vector<SegmentTermPosting *> segment_term_postings;

    Vector<Vector<u32>> expected_pos{{0}, {0, 1}};
    for (size_t i = 0; i < base_names.size(); ++i) {
        auto base_name = base_names[i];
        auto row_id = row_ids[i];
        {
            String expected_term("a");
            auto segment_term_posting1 = MakeUnique<SegmentTermPosting>(index_dir, base_name, row_id, flag_);
            auto column_index_iterator = segment_term_posting1->column_index_iterator_;
            PostingDecoder *decoder;
            String term_str;
            while (column_index_iterator->Next(term_str, decoder)) {
                EXPECT_EQ(term_str, expected_term);
                u32 pos_list_buf[1000];
                auto pos_num = decoder->DecodePosList(pos_list_buf, 1000);
                u32 pos = 0;
                for (u32 j = 0; j < pos_num; ++j) {
                    pos += pos_list_buf[j];
                    EXPECT_EQ(expected_pos[i][j], pos);
                }
            }
        }
        auto segment_term_posting = new SegmentTermPosting(index_dir, base_name, row_id, flag_);
        if (!segment_term_posting->HasNext()) {
            delete segment_term_posting;
        } else {
            segment_term_postings.push_back(segment_term_posting);
        }
    }

    auto merge_base_rowid = row_ids[0];
    for (auto &row_id : row_ids) {
        merge_base_rowid = std::min(merge_base_rowid, row_id);
    }

    VectorWithLock<u32> column_length_array;
    Vector<u32> &unsafe_column_length_array = column_length_array.UnsafeVec();
    {
        // prepare column length info
        // the indexes to be merged should be from the same segment
        // otherwise the range of row_id will be very large ( >= 2^32)
        PersistenceManager *pm = InfinityContext::instance().persistence_manager();
        PersistResultHandler handler(pm);
        unsafe_column_length_array.clear();
        for (u32 i = 0; i < base_names.size(); ++i) {
            String column_len_file = (Path(index_dir) / base_names[i]).string() + LENGTH_SUFFIX;
            String real_column_len_file = column_len_file;
            if (pm != nullptr) {
                PersistReadResult result = pm->GetObjCache(real_column_len_file);
                const ObjAddr &obj_addr = handler.HandleReadResult(result);
                real_column_len_file = pm->GetObjPath(obj_addr.obj_key_);
            }
            RowID base_row_id = row_ids[i];
            u32 id_offset = base_row_id - merge_base_rowid;
            auto [file_handle, status] = VirtualStore::Open(real_column_len_file, FileAccessMode::kRead);
            if (!status.ok()) {
                String error_message = status.message();
                UnrecoverableError(error_message);
            }
            const i64 file_size = file_handle->FileSize();
            u32 file_read_array_len = file_size / sizeof(u32);
            unsafe_column_length_array.resize(id_offset + file_read_array_len);
            auto [read_count, _] = file_handle->Read(unsafe_column_length_array.data() + id_offset, file_size);
            if (read_count != (SizeT)file_size) {
                String error_message = "ColumnIndexMerger: when loading column length file, read_count != file_size";
                UnrecoverableError(error_message);
            }
            if (pm != nullptr) {
                PersistWriteResult res = pm->PutObjCache(column_len_file);
                handler.HandleWriteResult(res);
            }
        }
    }

    auto posting_merger = MakeShared<PostingMerger>(flag_, column_length_array);

    posting_merger->Merge(segment_term_postings, merge_base_rowid);
    EXPECT_EQ(posting_merger->GetDF(), static_cast<u32>(2));
    EXPECT_EQ(posting_merger->GetTotalTF(), static_cast<u32>(3));
    for (auto segment_term_posting : segment_term_postings) {
        delete segment_term_posting;
    }
}
