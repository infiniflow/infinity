#include "index_entry.h"

#include "common/utility/infinity_assert.h"
#include "storage/buffer/buffer_manager.h"
#include "storage/index_def/ivfflat_index_def.h"

namespace infinity {
[[nodiscard]] const void IndexEntry::GetIndex(IndexEntry *index_entry, BufferManager *buffer_mgr) {
    if (index_entry->buffer_handle_ == nullptr) {
        auto &&index_dir = IndexEntry::IndexDirName(*index_entry->segment_entry_dir_);
        auto &&index_file = IndexEntry::IndexFileName(index_entry->index_def_->index_name_);
        index_entry->buffer_handle_ = buffer_mgr->GetBufferHandle(MakeShared<String>(index_dir), MakeShared<String>(index_dir), BufferType::kFile);
    }

    switch (index_entry->index_def_->method_type_) {
        case IndexMethod::kIVFFlat: {
            auto ivfflat_index_def =
                std::static_pointer_cast<IVFFlatIndexDef>(index_entry->index_def_); // Note shenyushi: use static_cast because type is known
            switch (ivfflat_index_def->metric_type_) {
                case MetricType::kMerticL2: {
                    // TODO shenyushi
                }
                case MetricType::kInvalid: {
                    StorageError("Invalid metric type")
                }
                default: {
                    NotImplementError("Not implemented.");
                }
            }

            break;
        }
        case IndexMethod::kInvalid: {
            StorageError("Invalid index method type")
        }
        default: {
            NotImplementError("Not implemented.");
        }
    }
}
} // namespace infinity