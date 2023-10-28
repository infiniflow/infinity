module;

import stl;
import pos_list_format_option;
import index_defines;
import position_bitmap_reader;
import pos_list_decoder;
module in_doc_pos_state;

namespace infinity {
void InDocPositionState::SetPositionListDecoder(PositionListDecoder *pos_decoder) { pos_decoder_ = pos_decoder; }

tf_t InDocPositionState::GetTermFreq() {
    if (tf_ == 0 && option_.HasTfBitmap()) {
        PositionBitmapReader *tf_bitmap = pos_decoder_->GetPositionBitmapReader();
        PositionCountInfo info = tf_bitmap->GetPosCountInfo(GetSeekedDocCount());
        tf_ = info.current_doc_pos_count_;
    }
    return tf_;
}

} // namespace infinity