// Copyright Vespa.ai. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#include "hdr_abort.h"
#include <cstdio>
#include <cstdlib>

namespace vespalib {

void hdr_abort(const char *message, const char *file, unsigned int line) { abort(); }

} // namespace vespalib
