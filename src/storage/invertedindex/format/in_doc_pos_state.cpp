module;

module infinity_core;

import :stl;
import :position_list_format_option;
import :index_defines;
import :position_list_decoder;

namespace infinity {
void InDocPositionState::SetPositionListDecoder(PositionListDecoder *pos_decoder) { pos_decoder_ = pos_decoder; }

tf_t InDocPositionState::GetTermFreq() { return tf_; }

} // namespace infinity