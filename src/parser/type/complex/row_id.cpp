//
// Created by jinhai on 23-10-1.
//

#include "type/complex/row_id.h"
#include "spdlog/fmt/fmt.h"

#include <string>

namespace infinity {

std::string RowID::ToString() const { return fmt::format("{}:{}", segment_id_, segment_offset_); }

} // namespace infinity
