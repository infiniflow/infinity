#include "in_doc_pos_state.h"
#include "pos_list_decoder.h"
namespace infinity {

tf_t InDocPositionState::GetTermFreq() {
    if (tf_ == 0 && option_.HasTfBitmap()) {
        PositionBitmapReader *tf_bitmap = pos_decoder_->GetPositionBitmapReader();
        PositionCountInfo info = tf_bitmap->GetPosCountInfo(GetSeekedDocCount());
        tf_ = info.current_doc_pos_count_;
    }
    return tf_;
}

} // namespace infinity