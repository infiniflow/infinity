//
// Created by jinhai on 23-6-30.
//

#pragma once

#include "common/types/internal_types.h"
#include "storage/meta/entry/block_entry.h"

namespace infinity {

class BlockMeta {


private:
    RWMutex rw_locker_{};
    SizeT block_id_{};

    UniquePtr<BlockEntry> block_entry_{};
    List<UniquePtr<BlockEntry>> delete_entries_{};
};

}
