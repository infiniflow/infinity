//
// Created by jinhai on 23-10-1.
//

#include "common/types/complex/row_id.h"
#include "spdlog/logger.h"

namespace infinity {

String RowID::ToString() const { return fmt::format("{}:{}:{}", segment_id_, block_id_, block_offset_); }

} // namespace infinity