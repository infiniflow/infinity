//
// Created by JinHai on 2022/11/6.
//

#pragma once
#include "common/types/internal_types.h"

namespace infinity {

constexpr i64 DEFAULT_VECTOR_SIZE = 8192;
constexpr i64 MAX_VARCHAR_SIZE = 65535;
constexpr i64 MAX_BLOB_SIZE = 65536L * 65536L;
constexpr i64 MAX_BITMAP_SIZE = 65536;
constexpr i64 EMBEDDING_LIMIT = 65536;

constexpr SizeT DEFAULT_SEGMENT_CAPACITY = 128;
constexpr SizeT DEFAULT_READER_PREPARE_QUEUE_SIZE = 1024;
constexpr SizeT DEFAULT_WRITER_PREPARE_QUEUE_SIZE = 1024;
constexpr SizeT DEFAULT_READER_COMMIT_QUEUE_SIZE = 1024;
constexpr SizeT DEFAULT_WRITER_COMMIT_QUEUE_SIZE = 1024;
}
