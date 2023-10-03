#pragma once

#include <memory>

#include "storage/indexstore/index_defines.h"

namespace infinity {

class PositionListFormatOption {
public:
    explicit PositionListFormatOption(optionflag_t option_flag = OPTION_FLAG_ALL) {
        Init(option_flag);
    }
    ~PositionListFormatOption() {}

    inline void
    Init(optionflag_t option_flag) {
        has_position_list_ = option_flag & of_position_list ? 1 : 0;
        if((option_flag & of_term_frequency) && (option_flag & of_tf_bitmap)) {
            has_tf_bitmap_ = 1;
        } else {
            has_tf_bitmap_ = 0;
        }
        unused_ = 0;
    }

    bool
    HasPositionList() const {
        return has_position_list_ == 1;
    }

    bool
    HasTfBitmap() const {
        return has_tf_bitmap_ == 1;
    }

    bool
    operator==(const PositionListFormatOption& right) const {
        return has_position_list_ == right.has_position_list_ &&
               has_tf_bitmap_ == right.has_tf_bitmap_;
    }

private:
    uint8_t has_position_list_ : 1;
    uint8_t has_tf_bitmap_ : 1;
    uint8_t unused_ : 6;
};


}// namespace infinity
