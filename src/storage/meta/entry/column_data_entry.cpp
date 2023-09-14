//
// Created by jinhai on 23-8-12.
//

#include "column_data_entry.h"
#include "common/types/complex/embedding_type.h"
#include "common/types/data_type.h"
#include "common/types/internal_types.h"
#include "common/types/logical_type.h"
#include "common/utility/infinity_assert.h"
#include "segment_entry.h"
#include "storage/buffer/buffer_manager.h"
#include <cstddef>
#include <string>

namespace infinity {

SharedPtr<ColumnDataEntry>
ColumnDataEntry::MakeNewColumnDataEntry(const SegmentEntry *segment_entry,
                                        u64 column_id,
                                        u64 row_capacity,
                                        const SharedPtr<DataType>& data_type,
                                        BufferManager* buffer_mgr) {
    SharedPtr<ColumnDataEntry> column_data_entry = MakeShared<ColumnDataEntry>(segment_entry);
    const auto* segment_ptr = (const SegmentEntry*)segment_entry;
    column_data_entry->base_dir_ = segment_ptr->base_dir_;
    column_data_entry->row_capacity_ = row_capacity;
    column_data_entry->column_type_ = data_type;
    column_data_entry->column_id_ = column_id;
    column_data_entry->file_name_ = MakeShared<String>(std::to_string(column_id) + ".col");
    column_data_entry->buffer_handle_ = buffer_mgr->AllocateBufferHandle(column_data_entry->base_dir_,
                                                                         column_data_entry->file_name_,
                                                                         data_type->Size() * row_capacity);
    return column_data_entry;
}

ObjectHandle
ColumnDataEntry::GetColumnData(ColumnDataEntry* column_data_entry, BufferManager* buffer_mgr) {
    if(column_data_entry->buffer_handle_ == nullptr) {
        // Get buffer handle from buffer manager
        column_data_entry->buffer_handle_ = buffer_mgr->GetBufferHandle(column_data_entry->base_dir_,
                                                                        column_data_entry->file_name_,
                                                                        BufferType::kFile);
    }

//    ptr_t ptr = buffer_handle_->LoadData();
    return ObjectHandle(column_data_entry->buffer_handle_);
}

/**
 * @brief Convert data to coressponding type and append to column data entry
 * 
 * @param column_data_entry column data need to be inserted
 * @param data data to be inserted, represented by string
 * @param offset offset of column data
 */
void
ColumnDataEntry::Append(ColumnDataEntry* column_data_entry, const StringView& data, SizeT offset) {
    if (column_data_entry->buffer_handle_ == nullptr) {
        StorageError("Not initialize buffer handle");
    }
    ptr_t ptr = column_data_entry->buffer_handle_->LoadData();
    auto type_size = column_data_entry->column_type_->Size();
    ptr += offset * type_size;
    switch (column_data_entry->column_type_->type()) {
        case kBoolean:
            DataType::WriteData<BooleanT>(ptr, data);
            break;
        case kTinyInt:
            DataType::WriteData<TinyIntT>(ptr, data);
            break;
        case kSmallInt:
            DataType::WriteData<SmallIntT>(ptr, data);
            break;
        case kInteger:
            DataType::WriteData<IntegerT>(ptr, data);
            break;
        case kBigInt:
            DataType::WriteData<BigIntT>(ptr, data);
            break;
        case kFloat:
            DataType::WriteData<FloatT>(ptr, data);
            break;
        case kDouble: 
            DataType::WriteData<DoubleT>(ptr, data);
            break;
        default:
            NotImplementError("not support data type");
    }
}

void ColumnDataEntry::AppendEmbedding(ColumnDataEntry *column_data_entry, const StringView &data, SizeT offset, char delimiter) {
    if (column_data_entry->buffer_handle_ == nullptr) {
        StorageError("Not initialize buffer handle");
    }
    ptr_t ptr = column_data_entry->buffer_handle_->LoadData();
    auto type_size = column_data_entry->column_type_->Size();
    ptr += offset * type_size;

    auto type_info = column_data_entry->column_type_->type_info().get();
    auto embedding_info = dynamic_cast<EmbeddingInfo *>(type_info);
    auto dimension = embedding_info->Dimension();
    switch (embedding_info->Type()) {
        case kElemBit:
            DataType::WriteEmbedding<BooleanT>(ptr, data, dimension, delimiter);
            break;
        case kElemInt8:
            DataType::WriteEmbedding<TinyIntT>(ptr, data, dimension, delimiter);
            break;
        case kElemInt16:
            DataType::WriteEmbedding<SmallIntT>(ptr, data, dimension, delimiter);
            break;
        case kElemInt32:
            DataType::WriteEmbedding<IntegerT>(ptr, data, dimension, delimiter);
            break;
        case kElemInt64:
            DataType::WriteEmbedding<BigIntT>(ptr, data, dimension, delimiter);
            break;
        case kElemFloat:
            DataType::WriteEmbedding<FloatT>(ptr, data, dimension, delimiter);
            break;
        case kElemDouble:
            DataType::WriteEmbedding<DoubleT>(ptr, data, dimension, delimiter);
            break;
        case kElemInvalid:
            NotImplementError("not support data type");
    }
}

void
ColumnDataEntry::Append(ColumnDataEntry* column_data_entry,
       const SharedPtr<ColumnVector>& column_vector,
       SizeT block_start_offset,
       SizeT column_start_offset,
       SizeT rows) {
    if(column_data_entry->buffer_handle_ == nullptr) {
        StorageError("Not initialize buffer handle")
    }
    ptr_t ptr = column_data_entry->buffer_handle_->LoadData();
    switch(column_vector->data_type()->type()) {
        case kBoolean:
        case kTinyInt:
        case kSmallInt:
        case kInteger:
        case kBigInt:
        case kHugeInt:
        case kDecimal:
        case kFloat:
        case kDouble:
        case kDate:
        case kTime:
        case kDateTime:
        case kTimestamp:
        case kInterval:
        case kPoint:
        case kLine:
        case kLineSeg:
        case kBox:
        case kCircle:
        case kBitmap:
        case kUuid:
        case kEmbedding: {
            ptr_t src_ptr = column_vector->data() + block_start_offset * column_vector->data_type_size_;
            ptr_t dst_ptr = ptr + column_start_offset * column_vector->data_type_size_;
            memcpy(dst_ptr, src_ptr, rows *  column_vector->data_type_size_);
            break;
        }

        case kVarchar:
        case kArray:
        case kTuple:
        case kPath:
        case kPolygon:
        case kBlob:
        case kMixed:
        case kNull: {
            LOG_ERROR("{} isn't supported", column_vector->data_type()->ToString())
            NotImplementError("Not supported now in append data in column")
        }
        case kMissing:
        case kInvalid: {
            LOG_ERROR("Invalid data type {}", column_vector->data_type()->ToString())
            StorageError("Invalid data type")
        }
    }
}

void
ColumnDataEntry::Flush(ColumnDataEntry* column_data_entry,
                       SizeT row_count) {
    SizeT buffer_size{0};
    switch(column_data_entry->column_type_->type()) {
        case kBoolean:
        case kTinyInt:
        case kSmallInt:
        case kInteger:
        case kBigInt:
        case kHugeInt:
        case kDecimal:
        case kFloat:
        case kDouble:
        case kDate:
        case kTime:
        case kDateTime:
        case kTimestamp:
        case kInterval:
        case kPoint:
        case kLine:
        case kLineSeg:
        case kBox:
        case kCircle:
        case kBitmap:
        case kUuid:
        case kEmbedding: {
            buffer_size = row_count * column_data_entry->column_type_->Size();
            break;
        }

        case kVarchar:
        case kArray:
        case kTuple:
        case kPath:
        case kPolygon:
        case kBlob:
        case kMixed:
        case kNull: {
            LOG_ERROR("{} isn't supported", column_data_entry->column_type_->ToString())
            NotImplementError("Not supported now in append data in column")
        }
        case kMissing:
        case kInvalid: {
            LOG_ERROR("Invalid data type {}", column_data_entry->column_type_->ToString())
            StorageError("Invalid data type")
        }
    }

    column_data_entry->buffer_handle_->WriteFile(buffer_size);
    column_data_entry->buffer_handle_->SyncFile();
    column_data_entry->buffer_handle_->CloseFile();
}

nlohmann::json
ColumnDataEntry::Serialize(const ColumnDataEntry* column_data_entry) {
    nlohmann::json json_res;
    json_res["column_type"] = column_data_entry->column_type_->Serialize();
    json_res["base_dir"] = *column_data_entry->base_dir_;
    json_res["file_name"] = *column_data_entry->file_name_;
    json_res["column_id"] = column_data_entry->column_id_;
    json_res["row_capacity"] = column_data_entry->row_capacity_;
    json_res["begin_ts"] = column_data_entry->begin_ts_;
    json_res["commit_ts"] = column_data_entry->commit_ts_.load();
    json_res["txn_id"] = column_data_entry->txn_id_.load();
    json_res["deleted"] = column_data_entry->deleted_;
    return json_res;
}

SharedPtr<ColumnDataEntry>
ColumnDataEntry::Deserialize(const nlohmann::json& column_data_json, SegmentEntry* segment_entry, BufferManager* buffer_mgr) {
    SharedPtr<ColumnDataEntry> column_data_entry = MakeShared<ColumnDataEntry>(segment_entry);
    column_data_entry->column_type_ = DataType::Deserialize(column_data_json["column_type"]);
    column_data_entry->base_dir_ = MakeShared<String>(column_data_json["base_dir"]);
    column_data_entry->file_name_ = MakeShared<String>(column_data_json["file_name"]);
    column_data_entry->column_id_ = column_data_json["column_id"];
    column_data_entry->row_capacity_ = column_data_json["row_capacity"];
    column_data_entry->begin_ts_ = column_data_json["begin_ts"];
    column_data_entry->commit_ts_ = column_data_json["commit_ts"];
    column_data_entry->txn_id_ = column_data_json["txn_id"];
    column_data_entry->deleted_ = column_data_json["deleted"];

//    column_data_entry->buffer_handle_ = buffer_mgr->AllocateBufferHandle(column_data_entry->base_dir_,
//                                                                         column_data_entry->file_name_,
//                                                                         column_data_entry->column_type_ * column_data_entry->row_capacity_);

    return column_data_entry;
}

}