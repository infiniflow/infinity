module;

module infinity_core:hit.impl;

import :hit;
import :ik_dict_segment;

namespace infinity {

void Hit::SetMatchedDictSegment(DictSegment *matched_dict_segment) { matched_dict_segment_ = matched_dict_segment; }

} // namespace infinity