//
// Created by jinhai on 23-2-22.
//

#include "create_statement.h"

namespace infinity {

String
CreateStatement::ToString() const {
    return create_info_->ToString();
}

}
