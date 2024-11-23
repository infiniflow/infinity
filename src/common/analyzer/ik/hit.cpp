module;

import ik_dict_segment;

module hit;

namespace infinity {
const int Hit::UNMATCH = 0x00000000;
const int Hit::MATCH = 0x00000001;
const int Hit::PREFIX = 0x00000010;

void Hit::SetMatchedDictSegment(DictSegment *matched_dict_segment) { matched_dict_segment_ = matched_dict_segment; }

} // namespace infinity