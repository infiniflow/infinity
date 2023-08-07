// Modified based on upscaledb by Christoph Rupp (chris@crupp.de)
#pragma once

#include "changeset.h"

namespace infinity {
struct Context {
    Context() {

    }

    ~Context() {
        changeset_.Clear();
    }
    ChangeSet changeset_;
};
}