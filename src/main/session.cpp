//
// Created by JinHai on 2022/9/5.
//

#include "session.h"

namespace infinity {

Session::Session()
    : current_query_context_(nullptr),
      current_schema_("Default")
      {}

}
